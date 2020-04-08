#include "gui.h"

int main() {
  XInitThreads();

  // SFML Window setup
  const sf::Vector2u window_size(1000, 1000);
  sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "Gui DEMO");
  window.setFramerateLimit(60);

  // SETUP GUI

  // Color Scheme
  sf::Color back = sf::Color(143, 224, 254);
  sf::Color active = sf::Color(3, 137, 242);
  sf::Color deactive = sf::Color(102, 146, 119);
  sf::Color alt =  sf::Color(58, 133, 191);
  
  // load font
  sf::Font font;
  if(!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf")) {
    throw std::runtime_error("No font file :""/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf"
                             " found!");
  }

  // Gui
  Gui gui(window, font);

  // Background
  gui.add_background(Background(window_size.x, window_size.y, 0, 0, back));
  
  // Push ( Temporary ) Buttons
  Push_Button button_1(200, 200, 100, 100, "button_1", active, deactive);
  Push_Button button_2(200, 200, 400, 100, "button_2", active, deactive);
  gui.add_push_button(button_1, button_2);

  // Toggle Button
  Toggle_Button toggler(500, 200, 100, 400, "toggle", active, deactive);
  gui.add_toggle_button(toggler);

  // Linked Toggle Buttons
  Toggle_Button linked_toggle_1(200, 200, 100, 700, "linked_1", true, active, deactive);
  Toggle_Button linked_toggle_2(200, 200, 400, 700, "linked_2", active, deactive);
  gui.add_linked_toggle(linked_toggle_1, linked_toggle_2);

  // Text Displays
  gui.add_text(Text_Display(200, 100, 700, 100, "Last Pressed", alt),
               Text_Display(200, 100, 700, 400, "Toggles State", alt),
               Text_Display(200, 100, 700, 700, "Linked One", alt));
  Text_Display last_pressed(200, 100, 700, 200, "--", back);
  Text_Display toggles_state(200, 100, 700, 500, "--", back);
  Text_Display linked_one(200, 100, 700, 800, "1", back);
  gui.add_text(last_pressed, toggles_state, linked_one);

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

    // USE GUI
    auto state = gui.get_state();
    
    if(state[button_1.id()]) { gui.set_text(last_pressed(), "1"); }
    else if(state[button_2.id()]) { gui.set_text(last_pressed(), "2"); }

    gui.set_text(toggles_state.id(), state.at(toggler.id()) ? "ON" : "OFF");

    if(state[linked_toggle_1()]) { gui.set_text(linked_one(), "1"); }
    else if(state[linked_toggle_2()]) { gui.set_text(linked_one(), "2"); }

    window.clear();

    // DRAW GUI
    gui.draw(window);

    window.display();
  }
}
