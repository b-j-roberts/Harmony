#include "2D_vector_height.h"

int main() {

  const sf::Vector2u window_size(1000, 1000);
  sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "Visual 2D");
  window.setFramerateLimit(60);

  std::vector<std::vector<float>> visualize;
  // Set visualize vector here

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

    window.display();
  }

}
