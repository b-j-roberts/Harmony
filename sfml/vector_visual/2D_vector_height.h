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
sf::Uint8 jet_shift(float in);

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
                              std::function<bool(size_t, size_t)> conditional);
// Draw entire rectangle 'vec' to 'window' using 'shader' on range 'range_min' to 'range_max'
void linear_Range_Shader(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                         const Shader& shader, float range_min = 0.f, float range_max = 1.f);
// Does same as above, but instead only draws mesh w/ spacing designated by 'mesh_size'
void linear_Range_Shader_Mesh(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                              const Shader& shader, size_t mesh_size = 5,
                              float range_min = 0.f, float range_max = 1.f);
// Does same as above, but only draws the corners of the mesh
void linear_Range_Shader_Dots(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                              const Shader& shader, size_t mesh_size = 5,
                              float range_min = 0.f, float range_max = 1.f);


// Draws point vec[i][j] of rectangle 'vec' to 'window' with 'color' 
//   only if 'conditional(vec[i][j]) == true)'
void make_Or_Break_Impl(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                        const sf::Color& color, std::function<bool(float)> conditional);
// Same as above w/ conditional == true iff vectors value is >= 'break_val'
void make_Or_Break(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                   float break_val, const sf::Color& color);
// Same as above w/ conditional == true iff vectors value >= 'break_min' && <= 'break_max'
void make_Or_Break_Range(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                         float break_min, float break_max, const sf::Color& color);

#endif
