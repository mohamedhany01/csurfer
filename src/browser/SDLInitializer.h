#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdexcept>
#include <string>

/**
 * Story: Manages the lifecycle of SDL and SDL_ttf libraries.
 * Why: To ensure proper initialization and cleanup using RAII.
 */
class SDLInitializer {
public:
  SDLInitializer() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      throw std::runtime_error("SDL_Init Error: " +
                               std::string(SDL_GetError()));
    }
    if (TTF_Init() != 0) {
      SDL_Quit();
      throw std::runtime_error("TTF_Init Error: " +
                               std::string(TTF_GetError()));
    }
  }

  ~SDLInitializer() {
    TTF_Quit();
    SDL_Quit();
  }

  // Disable copy and move to prevent double cleanup
  SDLInitializer(const SDLInitializer &) = delete;
  SDLInitializer &operator=(const SDLInitializer &) = delete;
  SDLInitializer(SDLInitializer &&) = delete;
  SDLInitializer &operator=(SDLInitializer &&) = delete;
};
