#include "2D_vector_height.h"
#include <cmath>

std::vector<std::vector<float>> get_Vector();

const sf::Vector2u window_size(1000, 1000);

int main() {

  // SFML Window setup
  sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "Visual 2D");
  window.setFramerateLimit(60);

  // Obtain visualized vector
  std::vector<std::vector<float>> visualize = get_Vector();

  sf::Event event;
  while(window.isOpen()) {
    while(window.pollEvent(event)) {
      switch(event.type) {
        case sf::Event::Closed:
          window.close();
          break;

        default: break;
      }
    }

    window.clear();

    // Call Visualization function
    // linear_Range_Shader(window, visualize, Jet_Shader());
    // linear_Range_Shader(window, visualize, Jet_Shader(), -.1f, .1f);
    // linear_Range_Shader(window, visualize, Jet_Shader(), -.25f, .25f);
    // linear_Range_Shader_Mesh(window, visualize, Grayscale_Shader(), 10, -.25f, .25f);
    // linear_Range_Shader_Dots(window, visualize, Purple_Shader(), 10, 0.f, 1.f);

    make_Or_Break(window, visualize, .15f, sf::Color::Red);
    window.display();
  }

}
/*
std::vector<std::vector<float>> get_Vector() {
  std::vector<std::vector<float>> ret(window_size.y, std::vector<float>(window_size.x));
  const float scale = (window_size.y - 1) * (window_size.x - 1);
  for(int y = 0; auto& row : ret) {
    for(int x = 0; auto& elem : row) {
      elem = (x++) * y / scale;
    }
    ++y;
  }
  return ret;
}
*/

/*
std::vector<std::vector<float>> get_Vector() {
  std::vector<std::vector<float>> ret(window_size.y, std::vector<float>(window_size.x));
  for(int y = 0; auto& row : ret) {
    for(int x = 0; auto& elem : row) {
      elem = std::sin(x * x + y * y ) / 10.f;
      ++x;
    }
    ++y;
  }
  return ret;
}
*/
/*
std::vector<std::vector<float>> get_Vector() {
  std::vector<std::vector<float>> ret(window_size.y, std::vector<float>(window_size.x));
  for(float y = 0.f; auto& row : ret) {
    for(float x = 0.f; auto& elem : row) {
      elem = std::sin(x + window_size.x / 2.f) * std::cos(y + window_size.y / 2.f) / 10.f;
      x+= .01;
    }
    y+=.01;
  }
  return ret;
}
*/
/*
std::vector<std::vector<float>> get_Vector() {
  std::vector<std::vector<float>> ret(window_size.y, std::vector<float>(window_size.x));
  for(float y = 0.f; auto& row : ret) {
    for(float x = 0.f; auto& elem : row) {
      elem = std::sin(x + window_size.x / 2.f) * std::cos(y + window_size.y / 2.f) / 10.f;
      x+= .01;
    }
    y+=.01;
  }
  return ret;
}
*/

/*
std::vector<std::vector<float>> get_Vector() {
  std::vector<std::vector<float>> ret(window_size.y, std::vector<float>(window_size.x));
  for(float y = 0.f; auto& row : ret) {
    for(float x = 0.f; auto& elem : row) {
      elem = std::cos(x) * std::cos(x) + std::cos(y) * std::cos(y);
      x+= .01;
    }
    y+=.01;
  }
  return ret;
}
*/

std::vector<std::vector<float>> get_Vector() {
  std::vector<std::vector<float>> ret(window_size.y, std::vector<float>(window_size.x));
  const float scale = (window_size.y - 1) * (window_size.x - 1);
  for(int y = 0; auto& row : ret) {
    for(int x = 0; auto& elem : row) {
      elem = (x++) * y / scale;
    }
    ++y;
  }
  return ret;
}

