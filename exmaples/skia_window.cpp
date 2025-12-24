#include <SDL2/SDL.h>
#include <core/SkBitmap.h>
#include <core/SkCanvas.h>
#include <core/SkFont.h>
#include <core/SkPaint.h>
#include <iostream>

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
    return 1;
  }

  SDL_Window *window =
      SDL_CreateWindow("SDL2 + Skia Hello World", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);

  if (!window)
    return 1;
  SDL_Surface *sdlSurface = SDL_GetWindowSurface(window);

  // Create SkBitmap as raster buffer
  SkBitmap bitmap;
  bitmap.allocN32Pixels(sdlSurface->w, sdlSurface->h);
  bitmap.eraseColor(SK_ColorWHITE); // clear

  SkCanvas canvas(bitmap);

  SkPaint paint;
  paint.setAntiAlias(true);
  paint.setColor(SK_ColorBLUE);
  canvas.drawRect({50, 50, 300, 200}, paint);

  paint.setColor(SK_ColorRED);
  SkFont font(nullptr, 32);
  canvas.drawString("Hello Skia + SDL2!", 60, 150, font, paint);

  // Copy pixels to SDL surface
  uint32_t *dst = (uint32_t *)sdlSurface->pixels;
  const SkPixmap &pixmap = bitmap.pixmap();
  for (int y = 0; y < sdlSurface->h; ++y) {
    memcpy(&dst[y * sdlSurface->w], pixmap.addr32(0, y),
           sdlSurface->w * sizeof(uint32_t));
  }

  SDL_UpdateWindowSurface(window);

  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN)
        quit = true;
    }
    SDL_Delay(16);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
