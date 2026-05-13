#pragma once

#include "Font.h"
#include <core/SkFont.h>
#include <core/SkFontMgr.h>
#include <core/SkTypeface.h>
#include <unordered_map>

namespace gfx {

class SkiaFont final : public Font {
public:
    explicit SkiaFont(sk_sp<SkTypeface> typeface, float size);
    ~SkiaFont() override = default;

    void measure_text(const std::string& text, int& width, int& height) override;

    const SkFont& sk_font() const { return font_; }

private:
    SkFont font_;
};

class SkiaFontManager final : public FontManager {
public:
    SkiaFontManager();
    ~SkiaFontManager() override = default;

    std::shared_ptr<Font> get_font(const std::string& family, int size, bool bold, bool italic) override;

private:
    sk_sp<SkFontMgr> font_mgr_;
    
    struct FontKey {
        std::string family;
        int size;
        bool bold;
        bool italic;

        bool operator==(const FontKey& other) const {
            return family == other.family && size == other.size && bold == other.bold && italic == other.italic;
        }
    };

    struct FontKeyHash {
        std::size_t operator()(const FontKey& k) const {
            return std::hash<std::string>()(k.family) ^ std::hash<int>()(k.size) ^ (std::hash<bool>()(k.bold) << 1);
        }
    };

    std::unordered_map<FontKey, std::shared_ptr<Font>, FontKeyHash> cache_;
};

} // namespace gfx
