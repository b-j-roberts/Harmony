#include "2D_vector_height.h"

// Jet shader helper function
sf::Uint8 jet_shift(float in) {
  if(in < .125f) {
    return 0;
  } else if(in < .375f) {
    return static_cast<unsigned char>((4.f * in - .5f) * 255);
  } else if(in < .625f) {
    return 255;
  } else if(in < .875f) {
    return static_cast<unsigned char>((-4.f * in + 3.5f) * 255);
  } else if(in < 1.f) {
    return 0;
  } else {
    return 0;
  }
}

void linear_Range_Shader_Impl(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                         const Shader& shader, float range_min, float range_max,
                         std::function<bool(size_t, size_t)> conditional) {
  const sf::Vector2u window_size(window.getView().getSize());
  const float pixel_x = window_size.x * 1.f / vec[0].size();
  const float pixel_y = window_size.y * 1.f / vec.size();

  sf::RectangleShape pixel;
  pixel.setSize(sf::Vector2f(pixel_x, pixel_y));

  const float slope = 1.f / (range_max - range_min);
  for(size_t i = 0; i < vec.size(); ++i) {
    for(size_t j = 0; j < vec[i].size(); ++j) {
      if(conditional(i, j)) {
        pixel.setPosition(sf::Vector2f(static_cast<float>(j) * pixel_x,
                                       static_cast<float>(i) * pixel_y));
        float shift = slope * vec[i][j] - slope * range_min;
        if(shift > 1.f) shift = 1.f;
        else if(shift < 0) shift = 0;
        pixel.setFillColor(shader.color(shift));
        window.draw(pixel);
      }
    }
  }
}

void linear_Range_Shader(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                         const Shader& shader, float range_min, float range_max) {
  linear_Range_Shader_Impl(window, vec, shader, range_min, range_max,
                           [](size_t, size_t){ return true; });
}

void linear_Range_Shader_Mesh(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                              const Shader& shader, size_t mesh_size,
                              float range_min, float range_max) {
  auto meshify = [mesh_size](size_t i, size_t j){ return i % mesh_size == 0 || j % mesh_size == 0; };
  linear_Range_Shader_Impl(window, vec, shader, range_min, range_max, meshify);
}

void linear_Range_Shader_Dots(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                              const Shader& shader, size_t mesh_size,
                              float range_min, float range_max) {
  auto dotify = [mesh_size](size_t i, size_t j){ return i % mesh_size == 0 && j % mesh_size == 0; };
  linear_Range_Shader_Impl(window, vec, shader, range_min, range_max, dotify);
}


void make_Or_Break_Impl(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                        const sf::Color& color, std::function<bool(float)> conditional) {
  const sf::Vector2u window_size(window.getView().getSize());
  const float pixel_x = window_size.x * 1.f / vec[0].size();
  const float pixel_y = window_size.y * 1.f / vec.size();

  sf::RectangleShape pixel;
  pixel.setSize(sf::Vector2f(pixel_x, pixel_y));
  pixel.setFillColor(color);

  for(size_t i = 0; i < vec.size(); ++i) {
    for(size_t j = 0; j < vec[i].size(); ++j) {
      if(conditional(vec[i][j])) {
        pixel.setPosition(sf::Vector2f(static_cast<float>(j) * pixel_x,
                                       static_cast<float>(i) * pixel_y));
        window.draw(pixel);
      }
    }
  }
}

void make_Or_Break(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                   float break_val, const sf::Color& color) {
  make_Or_Break_Impl(window, vec, color, [break_val](float val){ return val >= break_val; });
}

void make_Or_Break_Range(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                         float break_min, float break_max, const sf::Color& color) {
  auto rangify = [break_min, break_max](float val){ return val >= break_min && val <= break_max; };
  make_Or_Break_Impl(window, vec, color, rangify);
}
