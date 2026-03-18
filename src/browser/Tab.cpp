#include "Tab.h"
#include "css/StyleEngine.h"
#include "html/HTMLParser.h"
#include "layout/LayoutTree.h"
#include <iostream>

Tab::Tab(std::shared_ptr<IRequest> http, int window_width,
         const FontMetrics &metrics)
    : http_(std::move(http)), window_width_(window_width), metrics_(metrics),
      url_("http://localhost/") {}

void Tab::load(const Url &url) {
  // Only push to history if it's not the exact same URL we're already on
  if (history_.empty() || history_.back().href() != url.href()) {
    history_.push_back(url);
  }

  url_ = url;
  std::cout << "[Tab] Navigating to: " << url_.href() << std::endl;

  std::string body;
  if (url_.href() == "about:welcome") {
    body = "<html><head><title>Welcome</title></head><body><div "
           "style=\"text-align:center; padding:50px;\"><h1>Welcome in "
           "CSurfer</h1><p>Your lightweight, SOLID-powered "
           "browser.</p><hr><p>Visit the project on GitHub:</p><a "
           "href=\"https://github.com/mohamedhany01/csurfer\">mohamedhany01/"
           "csurfer</a><br><br><p><i>Enjoy surfing the solid "
           "way!</i></p></div></body></html>";
  } else {
    // Fetch page body
    body = http_->request(url_);
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
  style_engine.apply(dynamic_cast<Element *>(root_.get()), url_);

  // Layout
  document_ =
      std::make_unique<DocumentLayout>(root_.get(), metrics_, window_width_);
  document_->layout();

  // Paint to internal display list
  display_list_.clear();
  paint_tree(*document_, display_list_);
  scroll_ = 0;
}

// Revised draw signature to take renderer directly
void Tab::render(SDL_Renderer *renderer, int y_offset) const {
  if (!document_)
    return;
  for (const auto &cmd : display_list_) {
    cmd->execute(scroll_, y_offset, renderer);
  }
}

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
    const Lexeme *curr = clicked_node;
    while (curr) {
      if (curr->type() == LexemeType::Element) {
        const auto *el = dynamic_cast<const Element *>(curr);
        if (el && el->tag() == "a" && el->attributes().contains("href")) {
          load(url_.resolve(el->attributes().at("href")));
          return;
        }
      }
      curr = curr->parent();
    }
  }
}

void Tab::scrolldown() {
  if (!document_)
    return;
  // Total height = content height + top padding (VSTEP) + bottom padding
  // (VSTEP)
  int total_height = (int)document_->height + 40;
  int max_scroll = std::max(0, total_height - 540);
  scroll_ = std::min(scroll_ + SCROLL_STEP, max_scroll);
}

void Tab::scrollup() { scroll_ = std::max(0, scroll_ - SCROLL_STEP); }

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
