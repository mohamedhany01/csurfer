#include "CSurferUI.h"
#include "Browser.h"
#include "config/Config.h"
#include <iostream>

CSurferUI::CSurferUI(Browser *browser) : browser_(browser) {
  // Initialize bounding boxes (coordinates relative to window 0,0)

  // Back button (<)
  back_button_rect_ = {
      {config::UI_PADDING, config::TAB_HEIGHT + config::UI_PADDING / 2},
      40,
      config::ADDR_HEIGHT - config::UI_PADDING};

  // Address bar (Input field)
  address_bar_rect_ = {{back_button_rect_.origin.x + back_button_rect_.width +
                            config::UI_PADDING,
                        config::TAB_HEIGHT + config::UI_PADDING / 2},
                       600, // Width
                       config::ADDR_HEIGHT - config::UI_PADDING};

  // New Tab (+) button placeholder (Far right)
  new_tab_rect_ = {
      {config::WINDOW_WIDTH - config::UI_PADDING - 40, config::UI_PADDING / 2},
      40,
      config::TAB_HEIGHT - config::UI_PADDING};
}

void CSurferUI::render(SDL_Renderer *renderer) const {
  TTF_Font *font = browser_->get_font();
  SDL_Color black = {0, 0, 0, 255};

  // 1. Draw UI Background
  SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
  SDL_Rect ui_bg = {0, 0, config::WINDOW_WIDTH, config::UI_HEIGHT};
  SDL_RenderFillRect(renderer, &ui_bg);

  // 2. Draw Tab Bar
  for (size_t i = 0; i < browser_->tab_count(); ++i) {
    SDL_Rect tab_rect = {config::UI_PADDING + (int)i * (config::TAB_WIDTH + 5),
                         config::UI_PADDING / 2, config::TAB_WIDTH,
                         config::TAB_HEIGHT - config::UI_PADDING};

    if (i == browser_->active_tab_index()) {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    } else {
      SDL_SetRenderDrawColor(renderer, 210, 210, 210, 255);
    }
    SDL_RenderFillRect(renderer, &tab_rect);
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderDrawRect(renderer, &tab_rect);

    // Draw Tab Title
    std::string title = browser_->get_tab(i)->title();
    SDL_Surface *s = TTF_RenderText_Blended(font, title.c_str(), black);
    if (s) {
      SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
      SDL_Rect tr = {tab_rect.x + 5, tab_rect.y + 5, s->w, s->h};
      if (tr.w > config::TAB_WIDTH - 10)
        tr.w = config::TAB_WIDTH - 10;
      SDL_RenderCopy(renderer, t, NULL, &tr);
      SDL_DestroyTexture(t);
      SDL_FreeSurface(s);
    }

    // Draw 'x' Close Button
    SDL_Surface *xs = TTF_RenderText_Blended(font, "x", black);
    if (xs) {
      SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, xs);
      SDL_Rect xr = {tab_rect.x + config::TAB_WIDTH - 20, tab_rect.y + 5, xs->w,
                     xs->h};
      SDL_RenderCopy(renderer, t, NULL, &xr);
      SDL_DestroyTexture(t);
      SDL_FreeSurface(xs);
    }
  }

  // 3. Draw New Tab (+) button
  SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
  SDL_Rect nt = {new_tab_rect_.origin.x, new_tab_rect_.origin.y,
                 new_tab_rect_.width, new_tab_rect_.height};
  SDL_RenderFillRect(renderer, &nt);
  SDL_Surface *ns = TTF_RenderText_Blended(font, "+", black);
  if (ns) {
    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, ns);
    SDL_Rect tr = {nt.x + 13, nt.y + 5, ns->w, ns->h};
    SDL_RenderCopy(renderer, t, NULL, &tr);
    SDL_DestroyTexture(t);
    SDL_FreeSurface(ns);
  }

  // 4. Draw Address Bar Area Background
  SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
  SDL_Rect addr_area = {0, config::TAB_HEIGHT, config::WINDOW_WIDTH,
                        config::ADDR_HEIGHT + 10};
  SDL_RenderFillRect(renderer, &addr_area);

  // 5. Draw Back Button
  SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
  SDL_Rect bb = {back_button_rect_.origin.x, back_button_rect_.origin.y,
                 back_button_rect_.width, back_button_rect_.height};
  SDL_RenderFillRect(renderer, &bb);
  SDL_Surface *bs = TTF_RenderText_Blended(font, "<", black);
  if (bs) {
    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, bs);
    SDL_Rect tr = {bb.x + 13, bb.y + 5, bs->w, bs->h};
    SDL_RenderCopy(renderer, t, NULL, &tr);
    SDL_DestroyTexture(t);
    SDL_FreeSurface(bs);
  }

  // 6. Draw Address Bar
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_Rect ab = {address_bar_rect_.origin.x, address_bar_rect_.origin.y,
                 address_bar_rect_.width, address_bar_rect_.height};
  SDL_RenderFillRect(renderer, &ab);

  if (address_bar_focused_) {
    SDL_SetRenderDrawColor(renderer, 0, 120, 215, 255); // Blue
  } else {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
  }
  SDL_RenderDrawRect(renderer, &ab);

  // Render Address Bar Text
  int cursor_x = ab.x + 5;
  if (!address_bar_text_.empty()) {
    SDL_Surface *s =
        TTF_RenderText_Blended(font, address_bar_text_.c_str(), black);
    if (s) {
      SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
      SDL_Rect tr = {ab.x + 5, ab.y + 5, s->w, s->h};
      if (tr.w > ab.w - 10)
        tr.w = ab.w - 10;
      SDL_RenderCopy(renderer, t, NULL, &tr);
      cursor_x += tr.w;
      SDL_DestroyTexture(t);
      SDL_FreeSurface(s);
    }
  }

  // Draw Blinking Cursor
  if (address_bar_focused_ && (SDL_GetTicks() / 500) % 2 == 0) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect cursor = {cursor_x, ab.y + 5, 2, ab.h - 10};
    SDL_RenderFillRect(renderer, &cursor);
  }

  // 7. Draw Bottom Border
  SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
  SDL_RenderDrawLine(renderer, 0, config::UI_HEIGHT - 1, config::WINDOW_WIDTH,
                     config::UI_HEIGHT - 1);
}

void CSurferUI::click(utils::Point point) {
  address_bar_focused_ = false;

  // Check Tabs
  for (size_t i = 0; i < browser_->tab_count(); ++i) {
    utils::Rect tr = {{config::UI_PADDING + (int)i * (config::TAB_WIDTH + 5),
                       config::UI_PADDING / 2},
                      config::TAB_WIDTH,
                      config::TAB_HEIGHT - config::UI_PADDING};
    if (tr.contains(point)) {
      // Check if 'x' button was portion of the tab clicked
      if (point.x > tr.origin.x + config::TAB_WIDTH - 25) {
        browser_->close_tab(i);
      } else {
        browser_->switch_to_tab(i);
      }
      return;
    }
  }

  if (back_button_rect_.contains(point)) {
    browser_->go_back();
  } else if (address_bar_rect_.contains(point)) {
    address_bar_focused_ = true;
    address_bar_text_ = "";
  } else if (new_tab_rect_.contains(point)) {
    browser_->new_tab(Url("about:welcome"));
  }
}

void CSurferUI::keypress(SDL_Keycode key, const std::string &text) {
  if (!address_bar_focused_)
    return;

  if (key == SDLK_BACKSPACE) {
    if (!address_bar_text_.empty())
      address_bar_text_.pop_back();
  } else if (!text.empty()) {
    address_bar_text_ += text;
  }

  std::cout << "[UI] Typing URL: " << address_bar_text_ << std::endl;
}

void CSurferUI::enter() {
  if (address_bar_focused_ && !address_bar_text_.empty()) {
    std::cout << "[UI] Navigating to: " << address_bar_text_ << std::endl;
    // Auto-prepend http:// if missing
    std::string final_url = address_bar_text_;
    if (final_url.find("://") == std::string::npos) {
      final_url = "http://" + final_url;
    }
    browser_->load(final_url);
    address_bar_focused_ = false;
  }
}
