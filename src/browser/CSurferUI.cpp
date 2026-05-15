#include "CSurferUI.h"
#include "Tab.h"
#include "config/Config.h"
#include <iostream>

CSurferUI::CSurferUI(IBrowserCommands *browser) : browser_(browser) {
  // Story: Initialize bounding boxes for the browser shell UI components.

  // Back button (<)
  back_button_rect_ = {
      {config::UI_PADDING, config::TAB_HEIGHT + config::UI_PADDING / 2},
      40,
      config::ADDR_HEIGHT - config::UI_PADDING};

  // Address bar (URL input field)
  address_bar_rect_ = {{back_button_rect_.origin.x + back_button_rect_.width +
                            config::UI_PADDING,
                        config::TAB_HEIGHT + config::UI_PADDING / 2},
                       600, // Width
                       config::ADDR_HEIGHT - config::UI_PADDING};

  // New Tab (+) button (Far right)
  new_tab_rect_ = {
      {config::WINDOW_WIDTH - config::UI_PADDING - 40, config::UI_PADDING / 2},
      40,
      config::TAB_HEIGHT - config::UI_PADDING};
}

void CSurferUI::render(SDL_Renderer *renderer) const {
  TTF_Font *ui_font = browser_->ui_font();
  SDL_Color black_color = {0, 0, 0, 255};

  // 1. Draw UI Area Background
  SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
  SDL_Rect ui_background = {0, 0, config::WINDOW_WIDTH, config::UI_HEIGHT};
  SDL_RenderFillRect(renderer, &ui_background);

  // 2. Draw Tab Bar
  for (size_t i = 0; i < browser_->tab_count(); ++i) {
    SDL_Rect tab_rect = {config::UI_PADDING + (int)i * (config::TAB_WIDTH + 5),
                         config::UI_PADDING / 2, config::TAB_WIDTH,
                         config::TAB_HEIGHT - config::UI_PADDING};

    if (i == browser_->active_tab_index()) {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255,
                             255); // Active tab is white
    } else {
      SDL_SetRenderDrawColor(renderer, 210, 210, 210,
                             255); // Inactive tab is gray
    }
    SDL_RenderFillRect(renderer, &tab_rect);
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderDrawRect(renderer, &tab_rect);

    // Draw Tab Title Text
    std::string tab_title = browser_->get_tab(i)->title();
    SDL_Surface *title_surface =
        TTF_RenderText_Blended(ui_font, tab_title.c_str(), black_color);
    if (title_surface) {
      SDL_Texture *title_texture =
          SDL_CreateTextureFromSurface(renderer, title_surface);
      SDL_Rect title_rect = {tab_rect.x + 5, tab_rect.y + 5, title_surface->w,
                             title_surface->h};
      if (title_rect.w > config::TAB_WIDTH - 10)
        title_rect.w = config::TAB_WIDTH - 10;
      SDL_RenderCopy(renderer, title_texture, NULL, &title_rect);
      SDL_DestroyTexture(title_texture);
      SDL_FreeSurface(title_surface);
    }

    // Draw 'x' Close Icon
    SDL_Surface *close_surface =
        TTF_RenderText_Blended(ui_font, "x", black_color);
    if (close_surface) {
      SDL_Texture *close_texture =
          SDL_CreateTextureFromSurface(renderer, close_surface);
      SDL_Rect close_rect = {tab_rect.x + config::TAB_WIDTH - 20,
                             tab_rect.y + 5, close_surface->w,
                             close_surface->h};
      SDL_RenderCopy(renderer, close_texture, NULL, &close_rect);
      SDL_DestroyTexture(close_texture);
      SDL_FreeSurface(close_surface);
    }
  }

  // 3. Draw New Tab (+) Button
  SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
  SDL_Rect new_tab_button = {new_tab_rect_.origin.x, new_tab_rect_.origin.y,
                             new_tab_rect_.width, new_tab_rect_.height};
  SDL_RenderFillRect(renderer, &new_tab_button);
  SDL_Surface *plus_surface = TTF_RenderText_Blended(ui_font, "+", black_color);
  if (plus_surface) {
    SDL_Texture *plus_texture =
        SDL_CreateTextureFromSurface(renderer, plus_surface);
    SDL_Rect plus_rect = {new_tab_button.x + 13, new_tab_button.y + 5,
                          plus_surface->w, plus_surface->h};
    SDL_RenderCopy(renderer, plus_texture, NULL, &plus_rect);
    SDL_DestroyTexture(plus_texture);
    SDL_FreeSurface(plus_surface);
  }

  // 4. Draw Navigation Bar Background
  SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
  SDL_Rect nav_area = {0, config::TAB_HEIGHT, config::WINDOW_WIDTH,
                       config::ADDR_HEIGHT + 10};
  SDL_RenderFillRect(renderer, &nav_area);

  // 5. Draw Back Button
  SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
  SDL_Rect back_button = {back_button_rect_.origin.x,
                          back_button_rect_.origin.y, back_button_rect_.width,
                          back_button_rect_.height};
  SDL_RenderFillRect(renderer, &back_button);
  SDL_Surface *back_surface = TTF_RenderText_Blended(ui_font, "<", black_color);
  if (back_surface) {
    SDL_Texture *back_texture =
        SDL_CreateTextureFromSurface(renderer, back_surface);
    SDL_Rect back_rect = {back_button.x + 13, back_button.y + 5,
                          back_surface->w, back_surface->h};
    SDL_RenderCopy(renderer, back_texture, NULL, &back_rect);
    SDL_DestroyTexture(back_texture);
    SDL_FreeSurface(back_surface);
  }

  // 6. Draw Address Bar
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_Rect address_bar = {address_bar_rect_.origin.x,
                          address_bar_rect_.origin.y, address_bar_rect_.width,
                          address_bar_rect_.height};
  SDL_RenderFillRect(renderer, &address_bar);

  if (address_bar_focused_) {
    SDL_SetRenderDrawColor(renderer, 0, 120, 215, 255); // Highlight Blue
  } else {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
  }
  SDL_RenderDrawRect(renderer, &address_bar);

  // Render URL Text inside address bar
  int cursor_position_x = address_bar.x + 5;
  if (!address_bar_text_.empty()) {
    SDL_Surface *text_surface =
        TTF_RenderText_Blended(ui_font, address_bar_text_.c_str(), black_color);
    if (text_surface) {
      SDL_Texture *text_texture =
          SDL_CreateTextureFromSurface(renderer, text_surface);
      SDL_Rect text_rect = {address_bar.x + 5, address_bar.y + 5,
                            text_surface->w, text_surface->h};
      if (text_rect.w > address_bar.w - 10)
        text_rect.w = address_bar.w - 10;
      SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
      cursor_position_x += text_rect.w;
      SDL_DestroyTexture(text_texture);
      SDL_FreeSurface(text_surface);
    }
  }

  // Draw Blinking Caret
  if (address_bar_focused_ && (SDL_GetTicks() / 500) % 2 == 0) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect caret_rect = {cursor_position_x, address_bar.y + 5, 2,
                           address_bar.h - 10};
    SDL_RenderFillRect(renderer, &caret_rect);
  }

  // 7. Draw Separator Line
  SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
  SDL_RenderDrawLine(renderer, 0, config::UI_HEIGHT - 1, config::WINDOW_WIDTH,
                     config::UI_HEIGHT - 1);
}

void CSurferUI::click(utils::Point point) {
  address_bar_focused_ = false;

  // Check Tab Clicks
  for (size_t i = 0; i < browser_->tab_count(); ++i) {
    utils::Rect tab_hit_rect = {
        {config::UI_PADDING + (int)i * (config::TAB_WIDTH + 5),
         config::UI_PADDING / 2},
        config::TAB_WIDTH,
        config::TAB_HEIGHT - config::UI_PADDING};
    if (tab_hit_rect.contains(point)) {
      // Check if 'x' portion was clicked (Last 25 pixels)
      if (point.x > tab_hit_rect.origin.x + config::TAB_WIDTH - 25) {
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
    // Story: Auto-prepend protocol if missing (User convenience)
    std::string final_url_string = address_bar_text_;
    if (final_url_string.find("://") == std::string::npos) {
      final_url_string = "http://" + final_url_string;
    }
    browser_->load(final_url_string);
    address_bar_focused_ = false;
  }
}
