#include "SkiaFont.h"
#include <core/SkFontMetrics.h>
#include <ports/SkFontMgr_directory.h>
#include <cmath>

namespace gfx {

SkiaFont::SkiaFont(sk_sp<SkTypeface> typeface, float size) : font_(std::move(typeface), size) {
    font_.setEdging(SkFont::Edging::kSubpixelAntiAlias);
    font_.setSubpixel(true);
}

void SkiaFont::measure_text(const std::string& text, int& width, int& height) {
    SkRect bounds;
    font_.measureText(text.c_str(), text.size(), SkTextEncoding::kUTF8, &bounds);
    
    width = std::ceil(bounds.width());
    
    SkFontMetrics metrics;
    font_.getMetrics(&metrics);
    height = std::ceil(metrics.fDescent - metrics.fAscent);
}

SkiaFontManager::SkiaFontManager() {
    std::string fonts_dir = std::string(ASSETS_DIR) + "/fonts";
    font_mgr_ = SkFontMgr_New_Custom_Directory(fonts_dir.c_str());
}

std::shared_ptr<Font> SkiaFontManager::get_font(const std::string& family, int size, bool bold, bool italic) {
    FontKey key{family, size, bold, italic};
    if (cache_.contains(key)) {
        return cache_[key];
    }

    // For now, we only have one font (Ubuntu-Regular.ttf)
    // In a real browser, we would use the family name to find the typeface.
    std::string font_path = std::string(ASSETS_DIR) + "/fonts/Ubuntu-Regular.ttf";
    sk_sp<SkTypeface> typeface = font_mgr_->makeFromFile(font_path.c_str());

    if (!typeface) {
        typeface = SkTypeface::MakeEmpty();
    }

    auto font = std::make_shared<SkiaFont>(std::move(typeface), (float)size);
    cache_[key] = font;
    return font;
}

} // namespace gfx
