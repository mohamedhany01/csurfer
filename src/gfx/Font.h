#pragma once

#include <string>
#include <memory>

namespace gfx {

/**
 * Font represents a specific typeface and style (size, weight, etc.)
 * that can be used to measure text dimensions.
 *
 * Theory: Decoupling measurement from rendering allows the layout engine
 * to calculate geometry without knowing about the underlying graphics library.
 */
class Font {
public:
    virtual ~Font() = default;

    /**
     * Measures the width and height of the given text when rendered with this font.
     */
    virtual void measure_text(const std::string& text, int& width, int& height) = 0;
};

/**
 * FontManager is responsible for loading and caching Font instances.
 */
class FontManager {
public:
    virtual ~FontManager() = default;

    /**
     * Returns a Font instance matching the requested style.
     */
    virtual std::shared_ptr<Font> get_font(const std::string& family, int size, bool bold, bool italic) = 0;
};

} // namespace gfx
