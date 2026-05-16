#include "Tab.h"
#include "config/Config.h"
#include "css/CSSParser.h"
#include "css/CSSSelector.h"
#include "css/StyleEngine.h"
#include "gfx/GraphicsContext.h"
#include "html/HTMLParser.h"
#include "js/JSContext.h"
#include "layout/LayoutTree.h"
#include "lexer/Text.h"
#include "utils/Parser.h"
#include <algorithm>
#include <iostream>
#include <sstream>

Tab::Tab(std::shared_ptr<IRequest> http, int window_width,
         gfx::FontManager &font_manager)
    : http_(std::move(http)), window_width_(window_width),
      font_manager_(font_manager), url_("http://localhost/") {}

Tab::~Tab() = default;

void Tab::parse_csp(const std::string &header_value) {
  csp_directives_.clear();
  std::stringstream ss(header_value);
  std::string directive;
  while (std::getline(ss, directive, ';')) {
    // Trim whitespace
    directive.erase(0, directive.find_first_not_of(" "));
    directive.erase(directive.find_last_not_of(" ") + 1);
    if (directive.empty())
      continue;

    std::stringstream dss(directive);
    std::string name;
    dss >> name; // First word is directive name (e.g. script-src)

    std::vector<std::string> origins;
    std::string origin;
    while (dss >> origin) {
      if (origin == "'self'") {
        origins.push_back(url_.origin());
      } else {
        origins.push_back(origin);
      }
    }
    csp_directives_[name] = origins;
  }
}

bool Tab::is_allowed(const Url &url, const std::string &directive) const {
  if (csp_directives_.empty())
    return true; // No policy = allow all

  std::vector<std::string> allowed;
  if (csp_directives_.count(directive)) {
    allowed = csp_directives_.at(directive);
  } else if (csp_directives_.count("default-src")) {
    allowed = csp_directives_.at("default-src");
  } else {
    return true; // Directive not specified and no default-src
  }

  std::string target_origin = url.origin();

  for (const auto &origin : allowed) {
    if (origin == target_origin)
      return true;
  }

  return false;
}

void Tab::load(const Url &url, const std::string &payload) {
  focus_ = nullptr; // Reset focus when navigating to a new page
  // Only push to history if it's not the exact same URL we're already on
  if (history_.empty() || history_.back().href() != url.href()) {
    history_.push_back(url);
  }

  Url referrer = url_;
  url_ = url;
  std::cout << "[Tab] Navigating to: " << url_.href()
            << " (Referrer: " << referrer.href() << ")" << std::endl;

  std::string body;
  if (url_.href() == "about:welcome") {
    body = "<html><head><title>Welcome</title></head><body><div "
           "style=\"text-align:center; padding:50px;\"><h1>Welcome in "
           "CSurfer</h1><p>Your lightweight, SOLID-powered "
           "browser.</p><hr><p>Visit the project on GitHub:</p><a "
           "href=\"https://github.com/mohamedhany01/csurfer\">mohamedhany01/"
           "csurfer</a><br><br><p>Local Test Pages:</p><a "
           "href=\"http://localhost:8000/\">http://localhost:8000/</a>"
           "<br><br><p><i>Enjoy surfing the solid "
           "way!</i></p></div></body></html>";
  } else {
    // Fetch page body
    auto response = http_->request(url_, payload, referrer);
    body = response.body;

    // Phase 4: Handle CSP
    if (response.headers.count("content-security-policy")) {
      parse_csp(response.headers.at("content-security-policy"));
    } else {
      csp_directives_.clear();
    }
  }

  if (body.empty()) {
    std::cout << "[Tab] Load failed. Showing error page." << std::endl;
    body = "<html><body><h1>Error Loading Page</h1><p>We couldn't reach <b>" +
           url_.href() +
           "</b>. Please check the URL and try again.</p></body></html>";
  }

  // Parse HTML
  HTMLParser parser(body);
  root_ = parser.parse();
  if (!root_) {
    std::cout << "[Tab] No root found. Showing parse error page." << std::endl;
    body = "<html><body><h1>Parsing Error</h1><p>The page at <b>" +
           url_.href() + "</b> could not be parsed.</p></body></html>";
    HTMLParser error_parser(body);
    root_ = error_parser.parse();
  }

  // Apply Styles
  StyleEngine style_engine(http_);
  style_engine.apply(
      dynamic_cast<Element *>(root_.get()), url_,
      [this](const Url &u, const std::string &d) { return is_allowed(u, d); },
      url_);

  // Layout
  document_ = std::make_unique<DocumentLayout>(root_.get(), font_manager_,
                                               window_width_);
  document_->layout();

  // Paint to internal display list
  display_list_.clear();
  paint_tree(*document_, display_list_);
  scroll_ = 0;

  // Initialize JavaScript Context
  js_ = std::make_unique<JSContext>(this);

  // Find and run all <script> tags
  auto selector = CSSParser::parse_selector("script");
  if (selector && root_) {
    std::vector<Element *> scripts;
    // Simple recursive finder
    auto find_scripts = [&](auto self, Element *node) -> void {
      if (selector->matches(node)) {
        scripts.push_back(node);
      }
      for (auto &child_lex : node->children()) {
        if (child_lex->type() == LexemeType::Element) {
          self(self, static_cast<Element *>(child_lex.get()));
        }
      }
    };
    find_scripts(find_scripts, dynamic_cast<Element *>(root_.get()));

    for (auto *script : scripts) {
      auto attrs = script->attributes();
      if (attrs.count("src")) {
        std::string script_url = attrs.at("src");
        Url resolved_url = url_.resolve(script_url);

        if (!is_allowed(resolved_url, "script-src")) {
          std::cout << "[SOP/CSP] Blocked script loading from: " << script_url
                    << " (CSP Violation)" << std::endl;
          continue;
        }

        std::cout << "[Tab] Loading external script: " << script_url
                  << std::endl;
        std::string content = http_->request(resolved_url, "", url_).body;
        if (!content.empty()) {
          js_->run(script_url, content);
        }
      } else {
        std::string content;
        for (auto &child_lex : script->children()) {
          if (child_lex->type() == LexemeType::Text) {
            content += child_lex->text();
          }
        }
        if (!content.empty()) {
          js_->run("inline", content);
        }
      }
    }
  }
}

void Tab::rebuild_layout() {
  if (!root_)
    return;
  StyleEngine style_engine(http_);
  style_engine.apply(
      dynamic_cast<Element *>(root_.get()), url_,
      [this](const Url &u, const std::string &d) { return is_allowed(u, d); },
      url_);
  document_ = std::make_unique<DocumentLayout>(root_.get(), font_manager_,
                                               window_width_);
  document_->layout();
  display_list_.clear();
  paint_tree(*document_, display_list_);
}

// Revised draw signature to take GraphicsContext
void Tab::render(gfx::GraphicsContext &ctx, int y_offset) const {
  if (!document_)
    return;
  for (const auto &cmd : display_list_) {
    cmd->execute(scroll_, y_offset, ctx);
  }
  render_scrollbar(ctx, y_offset);
}

void Tab::render_scrollbar(gfx::GraphicsContext &ctx, int y_offset) const {
  if (!document_)
    return;

  int viewport_height = config::WINDOW_HEIGHT - y_offset;
  int doc_height = (int)document_->height + 60; // Increased padding

  if (doc_height <= viewport_height)
    return; // No need to scroll

  // Bar dimensions
  int bar_width = config::SCROLLBAR_WIDTH;
  int bar_x = window_width_ - bar_width;

  // Track (Background)
  ctx.draw_rect({bar_x, y_offset, bar_width, viewport_height},
                gfx::Color::FromRGB(240, 240, 240));

  // Thumb (Draggable part)
  double thumb_ratio = (double)viewport_height / doc_height;
  int thumb_height = (int)(viewport_height * thumb_ratio);
  if (thumb_height < 20)
    thumb_height = 20; // Minimum size

  double scroll_ratio = (double)scroll_ / (doc_height - viewport_height);
  int thumb_y =
      y_offset + (int)(scroll_ratio * (viewport_height - thumb_height));

  ctx.draw_rect({bar_x + 2, thumb_y, bar_width - 4, thumb_height},
                gfx::Color::FromRGB(160, 160, 160));
}

void Tab::handle_mousedown(int x, int y) {
  if (!document_)
    return;

  int ui_height = config::UI_HEIGHT;
  int v_height = config::WINDOW_HEIGHT - ui_height;
  int d_height = (int)document_->height + 100;

  if (d_height > v_height && x >= window_width_ - config::SCROLLBAR_WIDTH) {
    is_dragging_scrollbar_ = true;
    handle_mousemove(x, y); // Initial jump to position
    return;
  }

  // If not scrollbar, perform normal click
  click(x, y);
}

void Tab::handle_mousemove(int x, int y) {
  if (!is_dragging_scrollbar_ || !document_)
    return;

  int ui_height = config::UI_HEIGHT;
  int v_height = config::WINDOW_HEIGHT - ui_height;
  int d_height = (int)document_->height + 100;

  double scroll_ratio = (double)y / v_height;
  int max_scroll = std::max(0, d_height - v_height);
  scroll_ = std::min(
      max_scroll, std::max(0, (int)(scroll_ratio * d_height - v_height / 2)));
}

void Tab::handle_mouseup(int x, int y) { is_dragging_scrollbar_ = false; }

void Tab::click(int x, int y) {
  if (!document_)
    return;

  // y already comes in as relative to tab top (from Browser routing)
  auto total_y = y + scroll_;

  auto all = tree_to_list(*document_);
  const Lexeme *clicked_node = nullptr;

  for (auto it = all.rbegin(); it != all.rend(); ++it) {
    const LayoutObject *obj = *it;
    if (x >= obj->x && x < obj->x + obj->width && total_y >= obj->y &&
        total_y < obj->y + obj->height) {
      clicked_node = obj->node();
      if (clicked_node)
        break;
    }
  }

  if (clicked_node) {
    if (focus_) {
      focus_->set_focused(false);
    }
    focus_ = nullptr;

    const Lexeme *curr = clicked_node;
    while (curr) {
      if (curr->type() == LexemeType::Element) {
        const auto *el = dynamic_cast<const Element *>(curr);
        if (el) {
          if (js_ && js_->dispatch_event("click", const_cast<Element *>(el))) {
            return;
          }
          if (el->tag() == "a" && el->attributes().contains("href")) {
            load(url_.resolve(el->attributes().at("href")));
            return;
          } else if (el->tag() == "input") {
            focus_ = const_cast<Element *>(el);
            focus_->set_focused(true);
            focus_->setAttribute("value", ""); // Clear on click (Ch8)
            // Refresh display list to show caret
            display_list_.clear();
            paint_tree(*document_, display_list_);
            return;
          } else if (el->tag() == "button") {
            // Find parent form
            const Element *f_curr = el;
            while (f_curr) {
              if (f_curr->tag() == "form" &&
                  f_curr->attributes().contains("action")) {
                submit_form(f_curr);
                return;
              }
              f_curr = dynamic_cast<const Element *>(f_curr->parent());
            }
          }
        }
      }
      curr = curr->parent();
    }
  }

  // Refresh if focus changed to null
  display_list_.clear();
  paint_tree(*document_, display_list_);
}

void Tab::handle_keypress(SDL_Keycode key, const std::string &text) {
  if (!focus_)
    return;

  auto *el = dynamic_cast<Element *>(focus_);
  if (!el)
    return;

  if (js_ && js_->dispatch_event("keydown", el)) {
    return;
  }

  auto attrs = el->attributes();
  std::string value = attrs.count("value") ? attrs.at("value") : "";

  if (key == SDLK_BACKSPACE) {
    if (!value.empty()) {
      value.pop_back();
    }
  } else if (!text.empty()) {
    value += text;
  }

  el->setAttribute("value", value);

  // Re-layout and re-paint to update text width/caret position
  // In a full browser, we might only re-layout the changed element.
  // Here we re-run layout for the whole document for simplicity.
  document_->layout();
  display_list_.clear();
  paint_tree(*document_, display_list_);
}

void Tab::submit_form(const Element *form) {
  if (!form)
    return;

  if (js_ && js_->dispatch_event("submit", const_cast<Element *>(form))) {
    return;
  }

  std::string payload;

  // Find all <input> children that have a 'name'
  std::vector<const Lexeme *> queue = {form};
  while (!queue.empty()) {
    const Lexeme *node = queue.front();
    queue.erase(queue.begin());

    if (node->type() == LexemeType::Element) {
      const auto *el = dynamic_cast<const Element *>(node);
      if (el->tag() == "input" && el->attributes().contains("name")) {
        std::string name = el->attributes().at("name");
        std::string value =
            el->attributes().count("value") ? el->attributes().at("value") : "";
        if (!payload.empty())
          payload += "&";
        payload += utils::urlEncode(name) + "=" + utils::urlEncode(value);
      }
      for (const auto &child : el->children()) {
        queue.push_back(child.get());
      }
    }
  }

  std::string action = form->attributes().at("action");
  std::cout << "[Tab] Submitting form to: " << action
            << " with payload: " << payload << std::endl;
  load(url_.resolve(action), payload);
}

void Tab::scrolldown() {
  if (!document_)
    return;
  // Total height = content height + bottom padding
  int total_height = (int)document_->height + 60;
  int max_scroll =
      std::max(0, total_height - (config::WINDOW_HEIGHT - config::UI_HEIGHT));
  scroll_ = std::min(scroll_ + config::SCROLL_STEP, max_scroll);
}

void Tab::scrollup() { scroll_ = std::max(0, scroll_ - config::SCROLL_STEP); }

void Tab::go_back() {
  if (history_.size() > 1) {
    history_.pop_back(); // Remove current
    Url previous = history_.back();
    history_.pop_back(); // load() will push it back
    std::cout << "[Tab] Going back to: " << previous.href() << std::endl;
    load(previous);
  }
}

const std::string Tab::title() const {
  if (!root_)
    return url_.host();

  // Find <title> tag
  std::vector<const Lexeme *> queue = {root_.get()};
  while (!queue.empty()) {
    const Lexeme *node = queue.front();
    queue.erase(queue.begin());

    if (node->type() == LexemeType::Element) {
      const auto *el = dynamic_cast<const Element *>(node);
      if (el->tag() == "title" && !el->children().empty()) {
        const auto *text_node =
            dynamic_cast<const Text *>(el->children().front().get());
        if (text_node)
          return text_node->text();
      }
      for (const auto &child : el->children()) {
        queue.push_back(child.get());
      }
    }
  }

  return url_.host();
}
