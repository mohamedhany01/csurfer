#include "layout/BlockPainter.h"
#include "css/CSSUtils.h"
#include "dom/Element.h"
#include "layout/BlockLayout.h"
#include "utils/Logger.h"
#include <sstream>

namespace layout {

void BlockPainter::paint(
    const BlockLayout &layout,
    std::vector<std::unique_ptr<DrawCommand>> &display_list) {
  const auto *element = dynamic_cast<const Element *>(layout.node_);
  if (!element)
    return;

  auto styles = element->style();
  auto bounds = layout.bounds();

  float border_radius = 0.0f;
  if (styles.find("border-radius") != styles.end()) {
    std::string radius_string = styles.at("border-radius");
    if (radius_string.find("px") != std::string::npos) {
      radius_string = radius_string.substr(0, radius_string.find("px"));
    }
    try {
      border_radius = std::stof(radius_string);
    } catch (const std::exception &e) {
      CS_LOG_ERROR("Failed to parse border-radius: {}", e.what());
      border_radius = 0.0f;
    }
  }

  if (styles.find("box-shadow") != styles.end()) {
    std::string shadow_string = styles.at("box-shadow");
    std::stringstream ss(shadow_string);
    std::string dx_str, dy_str, blur_str, color_str;
    if (ss >> dx_str >> dy_str >> blur_str >> color_str) {
      try {
        int dx = std::stoi(dx_str);
        int dy = std::stoi(dy_str);
        int blur = std::stoi(blur_str);
        gfx::Color shadow_color = gfx::Color::from_name(color_str);
        display_list.push_back(std::make_unique<DrawBoxShadow>(
            utils::Rect{{bounds.origin.x, bounds.origin.y},
                        (int)bounds.width,
                        (int)bounds.height},
            (float)blur, dx, dy, shadow_color));
      } catch (const std::exception &e) {
        CS_LOG_ERROR("Failed to parse box-shadow: {}", e.what());
      }
    }
  }

  if (styles.find("background-color") != styles.end()) {
    std::string background_color_name = styles.at("background-color");
    if (background_color_name != "transparent" &&
        !background_color_name.empty()) {
      gfx::Color color = gfx::Color::from_name(background_color_name);

      if (border_radius > 0.0f) {
        display_list.push_back(std::make_unique<DrawRoundedRect>(
            utils::Rect{{bounds.origin.x, bounds.origin.y},
                        (int)bounds.width,
                        (int)bounds.height},
            border_radius, color));
      } else {
        display_list.push_back(std::make_unique<DrawRect>(
            bounds.origin.x, bounds.origin.y, bounds.origin.x + bounds.width,
            bounds.origin.y + bounds.height, color));
      }
    }
  }

  if (styles.find("background") != styles.end()) {
    std::string background_value = styles.at("background");
    std::string gradient_direction;
    gfx::Color color_start, color_end;
    if (CSS::parse_linear_gradient(background_value, gradient_direction,
                                   color_start, color_end)) {
      display_list.push_back(std::make_unique<DrawLinearGradient>(
          utils::Rect{{bounds.origin.x, bounds.origin.y},
                      (int)bounds.width,
                      (int)bounds.height},
          color_start, color_end, gradient_direction));
    }
  }
}

} // namespace layout
