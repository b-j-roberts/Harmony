#ifndef HARMONY_2D_VECTOR_VISUAL_H
#define HARMONY_2D_VECTOR_VISUAL_H

#include <SFML/Graphics.hpp>

#include <vector>

#include <functional> // function<>

// TO DO : Create custom color maps based on matrix like matlab?
// TO DO : Check from outside range values?
// TO DO : Fix issue with using with double instead of float vector
// TO DO : Create function for streaming a vector to a window, like std::copy
// TO DO : Specify draw box, so it doesn't take whole window ?

// Virtual Parent Shader Class
class Shader {
public:
  // Return color based on shift from 0.f - 1.f
  virtual sf::Color color(float shift) const = 0;
  virtual ~Shader() = default;
};


// BASIC SHADERS

class Red_Shader : public Shader {
public:
  sf::Color color(float shift) const override { 
    return sf::Color(static_cast<unsigned char>(shift * 255), 0, 0);
  }
};

class Green_Shader : public Shader {
public:
  sf::Color color(float shift) const override {
    return sf::Color(0, static_cast<unsigned char>(shift * 255), 0);
  }
};

class Blue_Shader : public Shader {
public:
  sf::Color color(float shift) const override {
    return sf::Color(0, 0, static_cast<unsigned char>(shift * 255));
  }
};

class Yellow_Shader : public Shader {
public:
  sf::Color color(float shift) const override {
    return sf::Color(static_cast<unsigned char>(shift * 255), 
                     static_cast<unsigned char>(shift * 255), 0);
  }
};

class Purple_Shader : public Shader {
public:
  sf::Color color(float shift) const override {
    return sf::Color(static_cast<unsigned char>(shift * 255), 0, 
                     static_cast<unsigned char>(shift * 255));
  }
};

class Cyan_Shader : public Shader {
public:
  sf::Color color(float shift) const override {
    return sf::Color(0, static_cast<unsigned char>(shift * 255), 
                     static_cast<unsigned char>(shift * 255));
  }
};

class Grayscale_Shader : public Shader {
public:
  sf::Color color(float shift) const override { 
    return sf::Color(static_cast<unsigned char>(shift * 255), 
                     static_cast<unsigned char>(shift * 255), 
                     static_cast<unsigned char>(shift * 255));
  }
};


// ADVANCED SHADERS

// Jet Shaders ( based on Jet Matlab colormap )

// Jet shader helper function
// transforms float 0.f - 1.f passed in to sf::Uint8 value 0 - 255 for color ( and returns )
//   to give to rgb color based on "Jet Curve" used by Matlab
static sf::Uint8 jet_shift(float in) {
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

class Jet_BTR_Shader : public Shader { // 1
public:
  sf::Color color(float shift) const override { 
    return sf::Color(jet_shift(shift - .25f), jet_shift(shift), jet_shift(shift + .25f)); 
  }
};

class Jet_RTB_Shader : public Shader { // 1 FLIP
public:
  sf::Color color(float shift) const override { 
    return sf::Color(jet_shift(shift + .25f), jet_shift(shift), jet_shift(shift - .25f)); 
  }
};

class Jet_BTG_Shader : public Shader { // 2
public:
  sf::Color color(float shift) const override { 
    return sf::Color(jet_shift(shift), jet_shift(shift - .25f), jet_shift(shift + .25f)); 
  }
};

class Jet_GTB_Shader : public Shader { // 2 FLIP
public:
  sf::Color color(float shift) const override { 
    return sf::Color(jet_shift(shift), jet_shift(shift + .25f), jet_shift(shift - .25f)); 
  }
};

class Jet_GTR_Shader : public Shader { // 3
public:
  sf::Color color(float shift) const override { 
    return sf::Color(jet_shift(shift - .25f), jet_shift(shift + .25f), jet_shift(shift)); 
  }
};

class Jet_RTG_Shader : public Shader { // 3 FLIP
public:
  sf::Color color(float shift) const override { 
    return sf::Color(jet_shift(shift + .25f), jet_shift(shift - .25f), jet_shift(shift));
  }
};

using Jet_Shader = Jet_BTR_Shader;


// DRAWING FUNCTIONS

// Draws rectangle 'vec' to 'window' using 'shader' such that 'range_min' corresponds to 0.f for
//   the shader and 'range_max' corresponds to 1.f for the shader ( linear interpolation )
// 'conditional' returns true for (y, x) to be drawn, st (y, x) -> vec[y][x]
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
// Draw entire rectangle 'vec' to 'window' using 'shader' on range 'range_min' to 'range_max'
void linear_Range_Shader(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                         const Shader& shader, float range_min = 0.f, float range_max = 1.f) {
  linear_Range_Shader_Impl(window, vec, shader, range_min, range_max, 
                           [](size_t, size_t){ return true; });
}
// Does same as above, but instead only draws mesh w/ spacing designated by 'mesh_size'
void linear_Range_Shader_Mesh(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                              const Shader& shader, size_t mesh_size = 5,
                              float range_min = 0.f, float range_max = 1.f) {
  auto meshify = [mesh_size](size_t i, size_t j){ return i % mesh_size == 0 || j % mesh_size == 0; };
  linear_Range_Shader_Impl(window, vec, shader, range_min, range_max, meshify);
}
// Does same as above, but only draws the corners of the mesh
void linear_Range_Shader_Dots(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                              const Shader& shader, size_t mesh_size = 5,
                              float range_min = 0.f, float range_max = 1.f) {
  auto dotify = [mesh_size](size_t i, size_t j){ return i % mesh_size == 0 && j % mesh_size == 0; };
  linear_Range_Shader_Impl(window, vec, shader, range_min, range_max, dotify);
}


// Draws point vec[i][j] of rectangle 'vec' to 'window' with 'color' 
//   only if 'conditional(vec[i][j]) == true)'
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
// Same as above w/ conditional == true iff vectors value is >= 'break_val'
void make_Or_Break(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                   float break_val, const sf::Color& color) {
  make_Or_Break_Impl(window, vec, color, [break_val](float val){ return val >= break_val; });
}
// Same as above w/ conditional == true iff vectors value >= 'break_min' && <= 'break_max'
void make_Or_Break_Range(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                         float break_min, float break_max, const sf::Color& color) {
  auto rangify = [break_min, break_max](float val){ return val >= break_min && val <= break_max; };
  make_Or_Break_Impl(window, vec, color, rangify);
}

#endif
