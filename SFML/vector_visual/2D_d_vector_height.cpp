#include <SFML/Graphics.hpp>

#include <vector>

#include <iostream> // TO DO : Debug

// vector must be scaled to have max value = 1 & min value = 0
// vector must be square (or only will size based on vec[0]'s size)
// TO DO : Think about these constraints
void display_vector(const std::vector<std::vector<double>>& vec, sf::RenderWindow& window) {

  const sf::Vector2u window_size(window.getView().getSize());
  const float pixel_x = window_size.x * 1.f / vec[0].size();
  const float pixel_y = window_size.y * 1.f / vec.size();

  std::cout << "p_x: " << pixel_x << " | p_y:" << pixel_y << std::endl;

  sf::RectangleShape pixel;
  pixel.setSize(sf::Vector2f(pixel_x, pixel_y));

  // TO DO : Think of ways to choose color shading patterns?
  // TO DO : Check for incorrect diagonal flipping
  for(int i = 0; i < vec.size(); ++i) {
    for(int j = 0; j < vec[i].size(); ++j) {
      pixel.setPosition(sf::Vector2f((float)j * pixel_x, (float)i * pixel_y));
      int blue_shift = 255 * vec[i][j];
      pixel.setFillColor(sf::Color(0, 0, blue_shift));
      window.draw(pixel);
    } 
  }

}

int main() {

  sf::VideoMode desktop = sf::VideoMode().getDesktopMode();
  sf::RenderWindow window(desktop, "vector visual test");

  window.setFramerateLimit(30);

  std::vector<std::vector<double>> test(500);
  for(float i = 0; i < 500; ++i) {

    for(float j = 0; j < 500; ++j) {
      test[i].push_back((i + j) / 1000.f);
    }

  }

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
    display_vector(test, window);
    window.display();

  }

  return 0;

}
