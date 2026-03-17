#include "Browser.h"
#include "css/StyleEngine.h"
#include "html/HTMLParser.h"
#include "layout/LayoutConstants.h"
#include "layout/LayoutTree.h"
#include "request/HttpRequest.h"
#include <algorithm>
#include <iostream>
#include <string>

Browser::Browser() : Browser(std::make_shared<HttpRequest>()) {}

Browser::Browser(std::shared_ptr<IRequest> http) : http_(std::move(http)) {

  initSDL();
  initTTF();
  loadFont();
  createWindow();
  createRenderer();
}

Browser::~Browser() { shutdown(); }

void Browser::initSDL() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    running = false;
  }
}

void Browser::initTTF() {
  if (TTF_Init() != 0) {
    std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
    running = false;
  }
}

void Browser::loadFont() {
  std::string font_path =
      std::string(ASSETS_DIR) + "/fonts/NotoSansCJK-Regular.ttc";

  font = TTF_OpenFont(font_path.c_str(), 16);
  if (!font) {
    std::cerr << "Font error: " << TTF_GetError() << std::endl;
    running = false;
  }
}

void Browser::createWindow() {
  window =
      SDL_CreateWindow("C Surfer 🌊", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

  if (!window) {
    std::cerr << "Window error: " << SDL_GetError() << std::endl;
    running = false;
  }
}

void Browser::createRenderer() {
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "Renderer error: " << SDL_GetError() << std::endl;
    running = false;
  }
}

void Browser::shutdown() {
  if (renderer)
    SDL_DestroyRenderer(renderer);
  if (window)
    SDL_DestroyWindow(window);
  if (font)
    TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();
}

void Browser::load(const Url &url) {
  if (!running)
    return;

  // Only push to history if it's not the exact same URL we're already on
  if (history_.empty() || history_.back().href() != url.href()) {
    history_.push_back(url);
  }

  // Fetch page body via request abstraction
  std::string body = http_->request(url);

  HTMLParser parser(body);
  root_ = parser.parse();
  if (!root_) {
    std::cout << "LOG :: no root found!" << std::endl;
    return;
  }

  HTMLParser::print_tree(*root_);

  // Extract font metrics
  FontMetrics metrics{TTF_FontAscent(font), abs(TTF_FontDescent(font)),
                      TTF_FontLineSkip(font)};

  // Apply all CSS: default styles, external stylesheets, inline styles.
  // Specificity-based cascade is handled inside StyleEngine.
  StyleEngine style_engine(http_);
  style_engine.apply(dynamic_cast<Element *>(root_.get()), url);

  // Build a layout tree for the parsed HTML and compute positions.
  document_ = std::make_unique<DocumentLayout>(root_.get(), metrics, WIDTH);
  document_->layout();

  display_list.clear();
  paint_tree(*document_, display_list);
  scroll = 0;

  debug_print_layout_tree(*document_);
  exit(0); // Exit right after printing so we don't need UI

  // Start SDL loop
  mainLoop();
}

// Main SDL loop
void Browser::mainLoop() {
  while (running) {
    handleEvents();
    draw();
    SDL_Delay(16); // ~60 FPS
  }
}

void Browser::handleEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT)
      running = false;

    if (e.type == SDL_MOUSEBUTTONDOWN) {
      if (e.button.button == SDL_BUTTON_LEFT) {
        click(e.button.x, e.button.y + scroll);
      }
    }

    if (e.type == SDL_KEYDOWN) {
      if (e.key.keysym.sym == SDLK_DOWN)
        scroll += 50;

      if (e.key.keysym.sym == SDLK_UP)
        scroll -= 50;

      if (e.key.keysym.sym == SDLK_BACKSPACE)
        go_back();

      int max_scroll = 0;
      if (document_) {
        max_scroll = std::max(document_->height + 2 * VSTEP - HEIGHT, 0);
      }
      scroll = std::clamp(scroll, 0, max_scroll);
    }
  }
}

void Browser::click(int x, int y) {
  if (!document_) return;

  auto list = tree_to_list(*document_);
  const LayoutObject* clicked_node = nullptr;

  // Find the deepest layout node that contains the point
  for (auto it = list.rbegin(); it != list.rend(); ++it) {
    const LayoutObject* obj = *it;
    if (x >= obj->x && x < obj->x + obj->width &&
        y >= obj->y && y < obj->y + obj->height) {
      clicked_node = obj;
      break;
    }
  }

  if (!clicked_node) return;

  const Lexeme* node = clicked_node->node();
  if (!node) return;

  // Walk up the DOM to find an <a> tag with an href
  const Lexeme* current = node;
  while (current) {
    if (current->type() == LexemeType::Element) {
      const Element* el = static_cast<const Element*>(current);
      if (el->tag() == "a") {
        auto attrs = el->attributes();
        if (attrs.find("href") != attrs.end()) {
          std::string href = attrs.at("href");
          
          // Resolve relative URL using current base URL
          Url current_url = history_.back();
          Url next_url = current_url.resolve(href);
          
          load(next_url);
          return;
        }
      }
    }
    current = current->parent();
  }
}

void Browser::go_back() {
  if (history_.size() <= 1) return; // Cannot go back further than first page

  // Pop current
  history_.pop_back();

  // Get previous
  Url previous = history_.back();
  history_.pop_back(); // load() will push it back

  load(previous);
}

void Browser::draw() {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
  for (const auto &cmd : display_list) {
    if (!cmd)
      continue;
    if (cmd->top > scroll + HEIGHT)
      continue;
    if (cmd->bottom < scroll)
      continue;
    cmd->execute(scroll, renderer);
  }
  SDL_RenderPresent(renderer);
}
