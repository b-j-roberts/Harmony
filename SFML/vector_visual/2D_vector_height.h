#include <SFML/Graphics.hpp>

#include <vector>

// TO DO : Create custom color maps based on matrix like matlab?
// TO DO : Check from outside range values?
// TO DO : Resolution drawing ( only draws blocks every designated size and colors whole space
//                              based on top left value )
// TO DO : Fix issue with using with double instead of float vector

// Polymorphic shader class
class Shader {

public:

  // Return color based on shift from 0.f - 1.f
  virtual sf::Color color(float shift) const = 0;

};


/*
 *
 *  BASIC SHADERS
 *
 */

class Red_Shader : public Shader {

public:
  sf::Color color(float shift) const override { return sf::Color(shift * 255, 0, 0); }

};

class Green_Shader : public Shader {

public:
  sf::Color color(float shift) const override { return sf::Color(0, shift * 255, 0); }

};

class Blue_Shader : public Shader {

public:
  sf::Color color(float shift) const override { return sf::Color(0, 0, shift * 255); }

};

class Yellow_Shader : public Shader {

public:
  sf::Color color(float shift) const override { return sf::Color(shift * 255, shift * 255, 0); }

};

class Purple_Shader : public Shader {

public:
  sf::Color color(float shift) const override { return sf::Color(shift * 255, 0, shift * 255); }

};

class Cyan_Shader : public Shader {

public:
  sf::Color color(float shift) const override { return sf::Color(0, shift * 255, shift * 255); }

};

class Grayscale_Shader : public Shader {

public:
  sf::Color color(float shift) const override { return sf::Color(shift * 255, shift * 255, shift * 255); }

};



/*
 *
 *  ADVANCED SHADERS
 *
 */


// Jet Shaders ( based on Jet Matlab colormap )
const float interpolate_jet(float in) {
  if(in < .125) {
    return 0.f;
  } else if(in < .375) {
    return 4.f * in - .5; 
  } else if(in < .625) {
    return 1.f;
  } else if(in < .875) {
    return -4.f * in + 3.5; 
  } else if(in < 1.f) {
    return 0;
  } else {
    return 0;
  }
}

class Jet_BTR_Shader : public Shader { // 1

public:
  sf::Color color(float shift) const override { 
    return sf::Color((int)(interpolate_jet(shift - .25) * 255), 
                     (int)(interpolate_jet(shift) * 255),
                     (int)(interpolate_jet(shift + .25) * 255)); 
  }

};

class Jet_RTB_Shader : public Shader { // 1 FLIP

public:
  sf::Color color(float shift) const override { 
    return sf::Color((int)(interpolate_jet(shift + .25) * 255), 
                     (int)(interpolate_jet(shift) * 255),
                     (int)(interpolate_jet(shift - .25) * 255)); 
  }

};

class Jet_BTG_Shader : public Shader { // 2

public:
  sf::Color color(float shift) const override { 
    return sf::Color((int)(interpolate_jet(shift) * 255), 
                     (int)(interpolate_jet(shift - .25) * 255),
                     (int)(interpolate_jet(shift + .25) * 255)); 
  }

};

class Jet_GTB_Shader : public Shader { // 2 Flip

public:
  sf::Color color(float shift) const override { 
    return sf::Color((int)(interpolate_jet(shift) * 255), 
                     (int)(interpolate_jet(shift + .25) * 255),
                     (int)(interpolate_jet(shift - .25) * 255)); 
  }

};

class Jet_GTR_Shader : public Shader { // 3

public:
  sf::Color color(float shift) const override { 
    return sf::Color((int)(interpolate_jet(shift - .25) * 255), 
                     (int)(interpolate_jet(shift + .25) * 255),
                     (int)(interpolate_jet(shift) * 255)); 
  }

};

class Jet_RTG_Shader : public Shader { // 3 FLIP

public:
  sf::Color color(float shift) const override { 
    return sf::Color((int)(interpolate_jet(shift + .25) * 255), 
                     (int)(interpolate_jet(shift - .25) * 255),
                     (int)(interpolate_jet(shift) * 255)); 
  }

};

using Jet_Shader = Jet_BTR_Shader;


/*
 *
 * Drawing Functions
 *
 */


// draw vec to window with shader on range of values 
// Vector must be rectangular or could have indexing error
void linear_range_shader(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                         const Shader& shader, float range_min = 0.f, float range_max = 1.f) {

  const sf::Vector2u window_size(window.getView().getSize());
  const float pixel_x = window_size.x * 1.f / vec[0].size();
  const float pixel_y = window_size.y * 1.f / vec.size();

  sf::RectangleShape pixel;
  pixel.setSize(sf::Vector2f(pixel_x, pixel_y));

  const float slope = 1.f / (range_max - range_min); 
  for(int i = 0; i < vec.size(); ++i) {
    for(int j = 0; j < vec[i].size(); ++j) {
      pixel.setPosition(sf::Vector2f((float)j * pixel_x, (float)i * pixel_y));
      float shift = slope * vec[i][j] - slope * range_min;
      if(shift > 1.f) shift = 1.f;
      else if(shift < 0) shift = 0;
      pixel.setFillColor(shader.color(shift));
      window.draw(pixel);
    } 
  }

}

// Same as above but only draws mesh with size specified
void linear_range_shader_mesh(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                         const Shader& shader, float range_min = 0.f, float range_max = 1.f, 
                         size_t mesh_size = 5) {

  const sf::Vector2u window_size(window.getView().getSize());
  const float pixel_x = window_size.x * 1.f / vec[0].size();
  const float pixel_y = window_size.y * 1.f / vec.size();

  sf::RectangleShape pixel;
  pixel.setSize(sf::Vector2f(pixel_x, pixel_y));

  const float slope = 1.f / (range_max - range_min); 
  for(int i = 0; i < vec.size(); ++i) {
    for(int j = 0; j < vec[i].size(); ++j) {
      if(i % mesh_size == 0 || j % mesh_size == 0) {
        pixel.setPosition(sf::Vector2f((float)j * pixel_x, (float)i * pixel_y));
        float shift = slope * vec[i][j] - slope * range_min;
        if(shift > 1.f) shift = 1.f;
        else if(shift < 0) shift = 0;
        pixel.setFillColor(shader.color(shift));
        window.draw(pixel);
      }
    } 
  }

}

// Same as above but only draws corners of mesh
void linear_range_shader_dots(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                         const Shader& shader, float range_min = 0.f, float range_max = 1.f, 
                         size_t mesh_size = 5) {

  const sf::Vector2u window_size(window.getView().getSize());
  const float pixel_x = window_size.x * 1.f / vec[0].size();
  const float pixel_y = window_size.y * 1.f / vec.size();

  sf::RectangleShape pixel;
  pixel.setSize(sf::Vector2f(pixel_x, pixel_y));

  const float slope = 1.f / (range_max - range_min); 
  for(int i = 0; i < vec.size(); ++i) {
    for(int j = 0; j < vec[i].size(); ++j) {
      if(i % mesh_size == 0 && j % mesh_size == 0) {
        pixel.setPosition(sf::Vector2f((float)j * pixel_x, (float)i * pixel_y));
        float shift = slope * vec[i][j] - slope * range_min;
        if(shift > 1.f) shift = 1.f;
        else if(shift < 0) shift = 0;
        pixel.setFillColor(shader.color(shift));
        window.draw(pixel);
      }
    } 
  }

}


// Draws vec to window only on points above break_val , draws as color
void make_or_break(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                   float break_val, const sf::Color& color) {

  const sf::Vector2u window_size(window.getView().getSize());
  const float pixel_x = window_size.x * 1.f / vec[0].size();
  const float pixel_y = window_size.y * 1.f / vec.size();

  sf::RectangleShape pixel;
  pixel.setSize(sf::Vector2f(pixel_x, pixel_y));
  pixel.setFillColor(color);

  for(int i = 0; i < vec.size(); ++i) {
    for(int j = 0; j < vec[i].size(); ++j) {
      if(vec[i][j] >= break_val) {
        pixel.setPosition(sf::Vector2f((float)j * pixel_x, (float)i * pixel_y));
        window.draw(pixel);
      }
    }
  }

}

// Draws vec to window only on points in range , draw as color
void make_or_break_range(sf::RenderWindow& window, const std::vector<std::vector<float>>& vec,
                   float break_min, float break_max, const sf::Color& color) {

  const sf::Vector2u window_size(window.getView().getSize());
  const float pixel_x = window_size.x * 1.f / vec[0].size();
  const float pixel_y = window_size.y * 1.f / vec.size();

  sf::RectangleShape pixel;
  pixel.setSize(sf::Vector2f(pixel_x, pixel_y));
  pixel.setFillColor(color);

  for(int i = 0; i < vec.size(); ++i) {
    for(int j = 0; j < vec[i].size(); ++j) {
      if(vec[i][j] >= break_min && vec[i][j] <= break_max) {
        pixel.setPosition(sf::Vector2f((float)j * pixel_x, (float)i * pixel_y));
        window.draw(pixel);
      }
    }
  }

}
