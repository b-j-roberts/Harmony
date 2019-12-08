#include <SFML/Graphics.hpp>

#include <vector>
#include <map>
#include <string>

// TO DO : Change to just button class and allow different kinds of buttons?
//           ex : circular, rectangular, toggle, push, ...
//
// TO DO : Make buttons and things into template of things with draw, position, and size functions
// TO DO : Make option to allow label to be displayed outside of button (select position?)
// TO DO : Allow linking of toggle buttons in certain ways so activating one deactivates others?

class Toggle_Button {
  
  sf::RectangleShape button_;
  sf::Font font; // TO DO : Move to gui and use reference
  sf::Text label_;

  bool state_;

  sf::Color active_color_;
  sf::Color deactive_color_;

  public:

  Toggle_Button(size_t, size_t, size_t, size_t); // based on pixels
  Toggle_Button(float x, float y, float pos_x, float pos_y); // based on percentages

  bool is_clicked(int x, int y); // check if coordinates in rectangle
  void activate(); // set rectangle color to active_color and state to true
  void deactivate(); // set rectangle color to deactive_color and state to false

  bool get_state(); // get state

  void draw(sf::RenderWindow& window); // draw to window

  friend GUI;

};

Toggle_Button::Toggle_Button(size_t width, size_t height, 
                             size_t pos_x, size_t pos_y, std::string label) {
  
  /* TO DO :
   * active_color_ set
   * deactive_color_ set
   */

  button_.setSize(sf::Vector2f(width, height));
  button_.setPosition(sf::Vector2f(pos_x, pos_y));
  button_.setFillColor(active_color_);
  //button_.setOutlineColor(sf::Color(10, 10, 10));
  //button_.setOutlineThickness(3);

  if(!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf")) {
    throw std::runtime_error("No font file :""/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf"
                             " found!");
  }
  label_.setFont(font);
  label_.setString(label);
  label_.setCharacterSize(/* TO DO */);
  label_.setFillColor(sf::Color::Black);
  label_.setOrigin(/* TO DO */);
  label_.setPosition(/* TO DO */);

  state_ = false;
}

bool Toggle_Button::is_clicked(int x, int y) {
  sf::Vector2f position = button_.getPosition();
  sf::Vector2f size = button_.getSize();
  if(x > position.x && x < position.x + size.x &&
     y > position.y && y < position.y + size.y) return true;
  return false;
}

void Toggle_Button::activate() {
  state_ = true;
  button_.setFillColor(active_color_);
}

void Toggle_Button::deactivate() {
  state_ = false;
  button_.setFillColor(deactive_color_);
}

bool Toggle_Button::get_state() { return state_; }

void Toggle_Button::draw(sf::RenderWindow& window) {
  window.draw(button_);
  window.draw(label_);
}


class Push_Button {

  sf::RectangleShape button_;
  sf::Text label_;

  bool state_;

  sf::Color active_color_;
  sf::Color deactive_color_;

  time total_active_time; //(animation and clicking blocking)
  time start_active;      // for calculation of active time

  public:
  
  Push_Button(size_t x, size_t y, size_t pos_x, 
              size_t pos_y, time total_active_time = 1 sec); // based on pixels
  Push_Button(float x, float y, float pos_x, 
              float pos_y, time total_actv_time = 1 sec); // based on percentages

  bool is_clicked(int x, int y) // check if coordinates in rectangle
  void activate() // if not active already 
                  // (ie start_active + elapsed < total_active_time) => activate
                  //   set state to true

  bool get_state() // get state (set to false after this is called once
  
  //update? // deactivate based on time
  void draw(sf::RenderWindow& window); // draw to window and do update step?

  friend GUI;

};

bool Push_Button::is_clicked(int x, int y) {
  sf::Vector2f position = button_.getPosition();
  sf::Vector2f size = button_.getSize();
  if(x > position.x && x < position.x + size.x &&
     y > position.y && y < position.y + size.y) return true;
  return false;
}

void Push_Button::activate() {
  if(!state_) {
    start_active = now;
    state_ = true;
    button_.setFillColor(active_color_);
  }
}

bool Push_Button::get_state() { 
  if(state_) {
    state_ = false;
    return true; 
  } else {
    return false;
  }
}

void Push_Button::draw(sf::RenderWindow& window) {
  if(/*now - start_active > total_active_time*/) {
    button_.setFillColor(deactive_color_);
  }
  window.draw(button_);
  window.draw(label_);
}


class Text_Display {

  sf::RectangleShape background_;
  sf::Text text_;

public:

  void draw(sf::RenderWindow&);

  void set_text(std::string);
  
  // TO DO : as_string, as_int, ... functions (return text string as designated type)

};

/*
class Scrolling_Bar {

};
*/

// Do I want this?
class GUI {

  sf::Font font_;

  std::vector<sf::RectangleShape> backgrounds_;

  std::vector<Text_Display> text_displays_;

  std::vector<Toggle_Button> toggle_buttons_; // TO DO : Use unique ptr?
  std::vector<Push_Button> push_buttons_;

  public:

  GUI();

  void draw(sf::RenderWindow& window) {
    for(auto& back : backgrounds_) window.draw(back);
    for(auto& text : text_displays_) text.draw(window);
    for(auto& button : toggle_buttons_) button.draw(window);
    for(auto& button : push_buttons_) button.draw(window); 
  }

  void click(int x, int y);
  std::map<std::string, bool> get_state(); // map from button name to bool state
                                           // (true - active, false - inactive)

  void build_background(sf::RectangleShape rect, sf::RectangleShape... rects); // TO DO : move?

  void add_text(const Text_Display& text);

  void add_toggle_button(const Toggle_Button& toggle); // TO DO : Like above?? move?
  void add_push_button(const Push_Button& push);

};

void GUI::click(int x, int y) {
  for(auto& button : toggle_buttons_) {
    if(button.is_clicked(x, y)) button.get_state() ? button.deactivate() : button.activate();
  }
  for(auto& button : push_buttons_) {
    if(button.is_clicked(x, y)) button.activate();
  }
}

// TO DO : Error if buttons have same label..
std::map<std::string, bool> GUI::get_state() {
  std::map<std::string, bool> ret;
  for(auto& button : toggle_buttons) {
    ret[button.label_.getString()] = button.get_state();
  }
  for(auto& button : push_buttons) {
    ret[button.label_.getString()] = button.get_state();
  }
  return ret;
}

void GUI::build_background(sf::RectangleShape rect, sf::RectangleShape... rects) {
  backgrounds_.push_back(rect);
  build_background(rects);
}

void GUI::add_text(const Text_Display& text) { text_displays_.push_back(text); }

void GUI::add_toggle_button(const Toggle_Button& toggle) { toggle_buttons_.push_back(toggle); }

void GUI::add_push_button(const Push_Button& push) { push_buttons_.push_back(push); }

int main() {

  GUI gui;

}
