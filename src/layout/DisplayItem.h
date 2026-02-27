#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

// A single drawing command in the final display list.
//
// Coordinates are in page space (not yet scrolled). The execute method takes
// the current scroll offset and an SDL renderer to actually draw.
class DrawCommand {
public:
  virtual ~DrawCommand() = default;

  int top = 0;
  int left = 0;
  int bottom = 0;
  int right = 0;

  // Draw this command using the given scroll offset and SDL renderer.
  virtual void execute(int scroll, SDL_Renderer *renderer) const = 0;
};

// Draw a single piece of text at a fixed page position.
//
// Example:
//   auto cmd = std::make_unique<DrawText>(x, y, "Hello", font);
//   cmd->execute(scroll, renderer);
class DrawText final : public DrawCommand {
public:
  DrawText(int x1, int y1, std::string text, TTF_Font *font);

  void execute(int scroll, SDL_Renderer *renderer) const override;

private:
  std::string text_;
  TTF_Font *font_ = nullptr;
};

// Draw a filled rectangle, used for things like code block backgrounds.
class DrawRect final : public DrawCommand {
public:
  DrawRect(int x1, int y1, int x2, int y2, SDL_Color color);

  void execute(int scroll, SDL_Renderer *renderer) const override;

private:
  SDL_Color color_{0, 0, 0, 255};
};
