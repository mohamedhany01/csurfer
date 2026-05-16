#pragma once
#include <SDL2/SDL_ttf.h>
#include <string>

class Tab;
class Url;

/**
 * Story: An interface for browser-level commands and state inspection.
 *
 * Use-case: This breaks the circular dependency between CSurferUI and Browser,
 * allowing the UI to interact with the browser without knowing its concrete
 * implementation.
 */
class IBrowserCommands {
public:
  virtual ~IBrowserCommands() = default;

  // State Inspection
  virtual TTF_Font *ui_font() const = 0;
  virtual size_t tab_count() const = 0;
  virtual size_t active_tab_index() const = 0;
  virtual Tab *get_tab(size_t index) const = 0;

  // Commands
  virtual void close_tab(size_t index) = 0;
  virtual void switch_to_tab(size_t index) = 0;
  virtual void go_back() = 0;
  virtual void new_tab(const Url &url) = 0;
  virtual void load(const std::string &raw_url) = 0;
};
