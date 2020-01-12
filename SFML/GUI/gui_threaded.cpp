#include <SFML/Graphics.hpp>
#include <X11/Xlib.h> // XInitThreads

#include <vector>
#include <map>
#include <string>

#include <thread>
#include <mutex>
#include <functional> // cref

#include <iostream> // TO DO

// TO DO : Change to just button class and allow different kinds of buttons?
//           ex : circular, rectangular, toggle, push, ...
//
// TO DO : Make buttons and things into template of things with draw, position, and size functions
// TO DO : Make option to allow label to be displayed outside of button (select position?)
// TO DO : Allow linking of toggle buttons in certain ways so activating one deactivates others?
// TO DO : GUI should have its own thread to work properly

class GUI; // Forward Declaration

sf::Vector2f center_pos_TL(sf::FloatRect out, sf::FloatRect in) {

  sf::Vector2f unshifted = sf::Vector2f((out.width - in.width) / 2, (out.height - in.height) / 2);
  return unshifted + sf::Vector2f(out.left, out.top);

}

class Toggle_Button {
  
  sf::RectangleShape button_;
  sf::Text label_;

  bool state_;

  sf::Color active_color_;
  sf::Color deactive_color_;

  void GUI_sizing(const sf::Font& font) {

    label_.setFont(font);

    sf::Vector2f size = button_.getSize();

    size_t max_char_width = (float)size.x / (float)label_.getString().getSize(); // TO DO : Letter Spacing?
    // TO DO : Check new line characters for max_char_height? // TO DO : Line spacing?
    label_.setCharacterSize(max_char_width < size.y ? max_char_width : size.y);

    label_.setPosition(center_pos_TL(button_.getGlobalBounds(), label_.getGlobalBounds()));
    sf::Vector2f shift = sf::Vector2f(label_.getPosition().x - label_.getGlobalBounds().left,
                                    label_.getPosition().y - label_.getGlobalBounds().top);
    label_.setPosition(label_.getPosition() + shift);
  }

  public:

  Toggle_Button(size_t, size_t, size_t, size_t, std::string); // based on pixels
  //Toggle_Button(float x, float y, float pos_x, float pos_y); // based on percentages

  bool is_clicked(int x, int y); // check if coordinates in rectangle
  void activate(); // set rectangle color to active_color and state to true
  void deactivate(); // set rectangle color to deactive_color and state to false

  bool get_state(); // get state

  void draw(sf::RenderWindow& window) const; // draw to window

  friend GUI;

};

Toggle_Button::Toggle_Button(size_t width, size_t height, 
                             size_t pos_x, size_t pos_y, std::string label = "") {
  
  /* TO DO :
   * active_color_ set
   * deactive_color_ set
   */

  // TEMP
  active_color_ = sf::Color::Green;
  deactive_color_ = sf::Color::Yellow;

  button_.setSize(sf::Vector2f(width, height));
  button_.setPosition(sf::Vector2f(pos_x, pos_y));
  button_.setFillColor(deactive_color_);
  //button_.setOutlineColor(sf::Color(10, 10, 10));
  //button_.setOutlineThickness(3);

  //label_.setFont(font);
  label_.setString(label);
  label_.setFillColor(sf::Color::Black);
/*
  size_t max_char_width = (float)width / (float)label.length(); // TO DO : Letter Spacing?
  // TO DO : Check new line characters for max_char_height? // TO DO : Line spacing?
  label_.setCharacterSize(max_char_width < height ? max_char_width : height);

  label_.setPosition(center_pos_TL(button_.getGlobalBounds(), label_.getGlobalBounds()));
  sf::Vector2f shift = sf::Vector2f(label_.getPosition().x - label_.getGlobalBounds().left, 
                                    label_.getPosition().y - label_.getGlobalBounds().top);
  label_.setPosition(label_.getPosition() + shift);
*/
  state_ = false;
}

bool Toggle_Button::is_clicked(int x, int y) {
  return button_.getGlobalBounds().contains(x, y);
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

void Toggle_Button::draw(sf::RenderWindow& window) const {
  window.draw(button_);
/*
  auto rect = label_.getGlobalBounds();
  auto d_rect = sf::RectangleShape(sf::Vector2f(rect.width, rect.height));
  d_rect.setPosition(sf::Vector2f(rect.left, rect.top));
  d_rect.setFillColor(sf::Color::Blue);
  window.draw(d_rect);
*/
  window.draw(label_);
}


class Push_Button {

  sf::RectangleShape button_;
  sf::Text label_;

  bool state_;

  sf::Color active_color_;
  sf::Color deactive_color_;

  //time total_active_time; //(animation and clicking blocking)
  //time start_active;      // for calculation of active time


  void GUI_sizing(const sf::Font& font) {

    label_.setFont(font);

    sf::Vector2f size = button_.getSize();

    size_t max_char_width = (float)size.x / (float)label_.getString().getSize(); // TO DO : Letter Spacing?
    // TO DO : Check new line characters for max_char_height? // TO DO : Line spacing?
    label_.setCharacterSize(max_char_width < size.y ? max_char_width : size.y);

    label_.setPosition(center_pos_TL(button_.getGlobalBounds(), label_.getGlobalBounds()));
    sf::Vector2f shift = sf::Vector2f(label_.getPosition().x - label_.getGlobalBounds().left,
                                    label_.getPosition().y - label_.getGlobalBounds().top);
    label_.setPosition(label_.getPosition() + shift);
  }

  public:
  
  Push_Button(size_t, size_t, size_t, 
              size_t, std::string);//, time total_active_time = 1 sec); // based on pixels
  //Push_Button(float x, float y, float pos_x, 
  //            float pos_y, time total_actv_time = 1 sec); // based on percentages

  bool is_clicked(int x, int y); // check if coordinates in rectangle
  void activate(); // if not active already 
                  // (ie start_active + elapsed < total_active_time) => activate
                  //   set state to true

  bool get_state(); // get state (set to false after this is called once
  
  void update(); // deactivate based on time
  void draw(sf::RenderWindow&) const; // draw to window and do update step?

  friend GUI;

};

Push_Button::Push_Button(size_t width, size_t height, size_t pos_x, size_t pos_y,
                         std::string label = "") {

  /* TO DO :
   * active_color_ set
   * deactive_color_ set
   */

  // TEMP
  active_color_ = sf::Color::Green;
  deactive_color_ = sf::Color::Yellow;

  button_.setSize(sf::Vector2f(width, height));
  button_.setPosition(sf::Vector2f(pos_x, pos_y));
  button_.setFillColor(deactive_color_);

  //label_.setFont(font);
  label_.setString(label);
  label_.setFillColor(sf::Color::Black);
/*
  size_t max_char_width = (float)width / (float)label.length(); // TO DO : Letter Spacing?
  // TO DO : Check new line characters for max_char_height? // TO DO : Line spacing?
  label_.setCharacterSize(max_char_width < height ? max_char_width : height);

  label_.setPosition(center_pos_TL(button_.getGlobalBounds(), label_.getGlobalBounds()));
  sf::Vector2f shift = sf::Vector2f(label_.getPosition().x - label_.getGlobalBounds().left, 
                                    label_.getPosition().y - label_.getGlobalBounds().top);
  label_.setPosition(label_.getPosition() + shift);
*/
  state_ = false;
}

bool Push_Button::is_clicked(int x, int y) {
  return button_.getGlobalBounds().contains(x, y);
}

void Push_Button::activate() {
  if(!state_) {
    //start_active = now;
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

void Push_Button::update() {
  //if(!state_ && now - start_active > total_active_time) {
  //  button_.setFillColor(deactive_color_);
  //}

}

void Push_Button::draw(sf::RenderWindow& window) const {
  window.draw(button_);
/*
  auto rect = label_.getGlobalBounds();
  auto d_rect = sf::RectangleShape(sf::Vector2f(rect.width, rect.height));
  d_rect.setPosition(sf::Vector2f(rect.left, rect.top));
  d_rect.setFillColor(sf::Color::Blue);
  window.draw(d_rect);
*/
  window.draw(label_);
}



class Text_Display {

  sf::RectangleShape background_;
  sf::Text text_;

  // TO DO : Fix & move?
  // setup size, position, and font after getting font from gui
  void GUI_sizing(const sf::Font& font) {
    text_.setFont(font);

    sf::Vector2f size = background_.getSize();
    
    size_t max_char_width = size.x / text_.getString().getSize();
    text_.setCharacterSize(max_char_width < size.y ? max_char_width : size.y);

    text_.setPosition(center_pos_TL(background_.getGlobalBounds(), text_.getGlobalBounds()));
    sf::Vector2f shift = sf::Vector2f(text_.getPosition().x - text_.getGlobalBounds().left,
                                    text_.getPosition().y - text_.getGlobalBounds().top);
    text_.setPosition(text_.getPosition() + shift);
  }

public:

  Text_Display(size_t, size_t, size_t, size_t, std::string);

  void draw(sf::RenderWindow&) const;

  void set_text(std::string s) { text_.setString(s); }
  std::string get_text() { return text_.getString(); }

  // TO DO : as_string, as_int, ... functions (return text string as designated type)

  friend GUI;

};

Text_Display::Text_Display(size_t width, size_t height, size_t pos_x,
                           size_t pos_y, std::string text):
  background_(sf::Vector2f(width, height)) {

  // TO DO : Background color?

  background_.setPosition(sf::Vector2f(pos_x, pos_y));
  background_.setFillColor(sf::Color(255, 255, 255)); // TO DO : TEMP

  text_.setString(text);
  text_.setFillColor(sf::Color::Black);
}

void Text_Display::draw(sf::RenderWindow& window) const {
  window.draw(background_);
  window.draw(text_);
}

/*
class Scrolling_Bar {

  scroll bar
  scroll horizontal ( or vertical)
  scroll position
  range
  scroller ball ( or box ? )

  draw()
  is_clicked() ( wait till not clicked and scroll to either x or y pos based on scroll horizontal )
  get position() ( return as a value )

};


class drop_down {

};
*/

class GUI {

  std::thread gui_thread_;
  //std::mutex lock_;

  bool new_state_;
  std::map<std::string, bool> state_; // map from button name to bool state of button
                                      // (true - active, false - inactive)

  sf::Font font_;

  std::vector<sf::RectangleShape> backgrounds_;

  std::vector<Text_Display> text_displays_;

  std::vector<Toggle_Button> toggle_buttons_; // TO DO : Use unique ptr?
  std::vector<Push_Button> push_buttons_;

  void GUI_Loop(sf::RenderWindow& window) {
    while(window.isOpen()) {
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i position = sf::Mouse::getPosition(window);
        for(auto& button : toggle_buttons_) {
          if(button.is_clicked(position.x, position.y)) {
            button.get_state() ? button.deactivate() : button.activate();
            new_state_ = true;
          } 
        }
        for(auto& button : push_buttons_) {
          if(button.is_clicked(position.x, position.y)) {
            button.activate();
            new_state_ = true;
          }
        }
      // TO DO : Disable clicks for x time
      }

      for(auto& button : push_buttons_) { button.update(); /* TO DO : set new state if changed ? */}
    }
  }

  public:

  GUI(const GUI&) = delete;
  GUI& operator=(const GUI&) = delete;

  GUI(sf::RenderWindow& window):
    new_state_(true) {

    gui_thread_ = std::thread(&GUI::GUI_Loop, this, std::ref(window));
    
    if(!font_.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf")) {
      throw std::runtime_error("No font file :""/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf"
                               " found!");
    }

  }

  ~GUI() {
    gui_thread_.join();
    // TO DO
  }

  void draw(sf::RenderWindow& window) const {
    for(auto& back : backgrounds_) window.draw(back);
    for(auto& text : text_displays_) text.draw(window);
    for(auto& button : toggle_buttons_) button.draw(window);
    for(auto& button : push_buttons_) button.draw(window); 
  }

  const std::map<std::string, bool>& get_state();

  // Add Parts of GUI ( background , text display , push & toggle buttons )

  // TO DO : Check move and such
  template<typename... Args>
  void add_background(Args&&... args) {
    (void)std::initializer_list<int>{ (backgrounds_.emplace_back(args), 0)... };
  }

/*	
template<class F, class...Args>
F for_each_arg(F f, Args&&...args) {
  (f(std::forward<Args>(args)),...);
  return f;
}*/

  template<class F, class...Args>
  void for_each_arg(F f, Args&&...args) {
    (void)std::initializer_list<int>{ (f(std::forward<Args>(args)), 0)... };
  }

// TO DO : Prevent other object calls
  template<typename... Args>
  void add_text(Args&&... args) {
    auto set_stuff = [&](Text_Display& t) { t.GUI_sizing(font_); };
    for_each_arg(set_stuff, args...);
    (void)std::initializer_list<int>{ (text_displays_.emplace_back(args), 0)... };
  }


  template<typename... Args>
  void add_toggle_button(Args&&... args) {
    auto set_stuff = [&](Toggle_Button& t){ t.GUI_sizing(font_);};
    for_each_arg(set_stuff, args...);
    (void)std::initializer_list<int>{ (toggle_buttons_.emplace_back(args), 0)... };
    new_state_ = true;
  }

  template<typename... Args>
  void add_push_button(Args&&... args) {
    auto set_stuff = [&](Push_Button& t){ t.GUI_sizing(font_); };
    for_each_arg(set_stuff, args...);
    (void)std::initializer_list<int>{ (push_buttons_.emplace_back(args), 0)... };
    new_state_ = true;
  }

};

// TO DO : Error if buttons have same label..
const std::map<std::string, bool>& GUI::get_state() {
  if(new_state_) {
    new_state_ = false;
    for(auto& button : toggle_buttons_) {
      state_[button.label_.getString()] = button.get_state();
    }
    for(auto& button : push_buttons_) {
      if(button.get_state()) {
        state_[button.label_.getString()] = true;
        new_state_ = true; // because we need to update new state since this is now false
      } else {
        state_[button.label_.getString()] = false;
      }
    }
  }
  return state_;
}

// Going Forward : 
//   Create gui thread
//   add colors
//   fonts
//   time on push button
//   scroll bar
//   ...

int main() {

  XInitThreads();

  sf::VideoMode desktop = sf::VideoMode().getDesktopMode();
  sf::RenderWindow window(desktop, "GUI Test");
  window.setFramerateLimit(30);
  const sf::Vector2u window_size(window.getSize());

  GUI gui(window);

  sf::RectangleShape back_1(sf::Vector2f(window_size.x - 50, window_size.y - 50));
  sf::RectangleShape back_2(sf::Vector2f(window_size.x - 200, window_size.y - 200));
  back_1.setFillColor(sf::Color(0, 0, 255));
  back_2.setFillColor(sf::Color(255, 0, 0));
  back_1.setPosition(20, 20);
  back_2.setPosition(0, 30);
  gui.add_background(back_1, back_2);

  Toggle_Button tog_1(600, 80, 320, 220, "Tog1");
  Toggle_Button tog_2(600, 40, 900, 900, "Tog2");
  gui.add_toggle_button(tog_1, tog_2);

  Push_Button push_1(100, 180, 1020, 20, "Push1");
  Push_Button push_2(200, 240, 1090, 90, "Push2");
  gui.add_push_button(push_1, push_2);

  Text_Display t1(100, 200, 0, 0, "1");
  Text_Display t2(100, 200, 300, 0, "2");
  gui.add_text(t1, t2);

  sf::Event event;

  while(window.isOpen()) {

    // Input
    while(window.pollEvent(event)) {
      switch(event.type) {

        case sf::Event::KeyPressed:
          switch(event.key.code) {
            case sf::Keyboard::Escape:
              window.close();
              break;
            default: break;
          }
          break; // key pressed

          case sf::Event::Closed:
            window.close();
            break;

        default: break;
      }
    }

    auto gui_state = gui.get_state();
    // use state

    window.clear();

    gui.draw(window);

    window.display();

  }

  return 0;

}
