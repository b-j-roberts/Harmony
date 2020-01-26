#include <SFML/Graphics.hpp>
#include <X11/Xlib.h> // XInitThreads ( MUST BE DECLARED IN MAIN THREAD )

#include <vector>
#include <map>
#include <string>

#include <thread>
#include <mutex>

#include <functional> // cref
#include <chrono>
#include <utility>
#include <memory> // unique_ptr
#include <type_traits> // enable_if, conjunction, is_same // TO DO : C++20 concepts

// TO DO : Change to just button class and allow different kinds of buttons?
//           ex : circular, rectangular, toggle, push, ...
//
// TO DO : Make buttons and things into template of things with draw, position, and size functions
// TO DO : Make option to allow label to be displayed outside of button (select position?)
// TO DO : Allow linking of toggle buttons in certain ways so activating one deactivates others?
// TO DO : Make XML format reader

class GUI; // Forward Declaration

// pair(active, deactive)
auto Black_White = std::make_pair(sf::Color::White, sf::Color::Black); 
auto Halloween   = std::make_pair(sf::Color::Black, sf::Color(235, 97, 35));
// TO DO : more color pairs


struct GUI_Scheme {

  sf::Color background_;
  sf::Color text_background_;
  sf::Color text_;
  sf::Color button_active_;
  sf::Color button_deactive_; 

};
// TO DO : Make some more color schemes

GUI_Scheme Halloween_Scheme = { sf::Color(136, 30, 228), sf::Color(136, 30, 228), 
                                sf::Color(133, 226, 31), sf::Color(235, 97, 35), sf::Color::Black };




class Text_Display_Impl {

protected:

  sf::RectangleShape background_;
  sf::Text text_;

  // setup size, position, and font after getting font from gui
  void GUI_sizing(const sf::Font& font) {
    text_.setFont(font);

    sf::Vector2f size = background_.getSize();
    
    size_t max_char_width = size.x / text_.getString().getSize(); // TO DO : line & letter spacing
    text_.setCharacterSize(max_char_width < size.y ? max_char_width : size.y);

    text_.setPosition(center_pos_TL(background_.getGlobalBounds(), text_.getGlobalBounds()));
    sf::Vector2f shift = sf::Vector2f(text_.getPosition().x - text_.getGlobalBounds().left,
                                      text_.getPosition().y - text_.getGlobalBounds().top);
    text_.setPosition(text_.getPosition() + shift);
  }

  // get shift vector for centering text
  static sf::Vector2f center_pos_TL(sf::FloatRect out, sf::FloatRect in) {
    sf::Vector2f unshifted = sf::Vector2f((out.width - in.width) / 2, (out.height - in.height) / 2);
    return unshifted + sf::Vector2f(out.left, out.top);
  }

public:

  // Remove copy

  Text_Display_Impl(size_t, size_t, size_t, size_t, const std::string&, sf::Color);

  void draw(sf::RenderWindow&) const;

  // TO DO : void set_text(std::string s) { text_.setString(s); } // Need to redo GUI_sizing
  // const sf::String& get_text() const { return pimpl_->text_.getString(); }

  friend GUI;

};

Text_Display_Impl::Text_Display_Impl(size_t width, size_t height, size_t pos_x,
                           size_t pos_y, const std::string& text, sf::Color bg = sf::Color::Blue):
  background_(sf::Vector2f(width, height)) {

  background_.setPosition(sf::Vector2f(pos_x, pos_y));
  background_.setFillColor(bg);

  text_.setString(text);
  text_.setFillColor(sf::Color::Black);
}

void Text_Display_Impl::draw(sf::RenderWindow& window) const {
  window.draw(background_);
  window.draw(text_);
}

class Text_Display {

  std::unique_ptr<Text_Display_Impl> pimpl_;

public:

  Text_Display(size_t w, size_t h, size_t x, size_t y, const std::string& t, sf::Color c = sf::Color::Blue):
    pimpl_(std::make_unique<Text_Display_Impl>(w, h, x, y, t, c)) { }

  friend GUI;

};

class Toggle_Button_Impl : Text_Display_Impl {
  
  bool state_;

  sf::Color active_color_;
  sf::Color deactive_color_;

  public:

  Toggle_Button_Impl(size_t, size_t, size_t, size_t, const std::string&, sf::Color, sf::Color); // pixel based
  Toggle_Button_Impl(size_t, size_t, size_t, size_t, const std::string&, std::pair<sf::Color, sf::Color>);
  //Toggle_Button(float x, float y, float pos_x, float pos_y); // based on percentages

  bool is_clicked(int x, int y) const; // check if coordinates in rectangle
  void activate(); // set rectangle color to active_color and state to true
  void deactivate(); // set rectangle color to deactive_color and state to false

  bool get_state(); // get state

  void draw(sf::RenderWindow& window) const; // draw to window

  friend GUI;

};

Toggle_Button_Impl::Toggle_Button_Impl(size_t width, size_t height, 
                             size_t pos_x, size_t pos_y, const std::string& label,
                             sf::Color active = sf::Color::Green, 
                             sf::Color deactive = sf::Color::Red):
  Text_Display_Impl(width, height, pos_x, pos_y, label, deactive),
  state_(false),
  active_color_(active),
  deactive_color_(deactive) { }

bool Toggle_Button_Impl::is_clicked(int x, int y) const {
  return background_.getGlobalBounds().contains(x, y);
}

void Toggle_Button_Impl::activate() {
  state_ = true;
  background_.setFillColor(active_color_);
}

void Toggle_Button_Impl::deactivate() {
  state_ = false;
  background_.setFillColor(deactive_color_);
}

bool Toggle_Button_Impl::get_state() { return state_; }

void Toggle_Button_Impl::draw(sf::RenderWindow& window) const {
  window.draw(background_);
  window.draw(text_);
}

class Toggle_Button {

  std::unique_ptr<Toggle_Button_Impl> pimpl_;

public:

  Toggle_Button(size_t w, size_t h, size_t x, size_t y, const std::string& t, sf::Color a = sf::Color::Green, sf::Color d = sf::Color::Red):
    pimpl_(std::make_unique<Toggle_Button_Impl>(w, h, x, y, t, a, d)) { }
  Toggle_Button(size_t w, size_t h, size_t x, size_t y, const std::string& t, std::pair<sf::Color, sf::Color> a_d_p):
    pimpl_(std::make_unique<Toggle_Button_Impl>(w, h, x, y, t, a_d_p.first, a_d_p.second)) { }

  friend GUI;

};


class Push_Button_Impl : Text_Display_Impl {

  bool state_;
  bool active_;

  sf::Color active_color_;
  sf::Color deactive_color_;

  std::chrono::milliseconds active_time_ = std::chrono::milliseconds(200);
  std::chrono::steady_clock::time_point start_time_;

  public:
  
  Push_Button_Impl(size_t, size_t, size_t, size_t, const std::string&, sf::Color, sf::Color);// based on pixels
  Push_Button_Impl(size_t, size_t, size_t, size_t, const std::string&, std::pair<sf::Color, sf::Color>);
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

Push_Button_Impl::Push_Button_Impl(size_t width, size_t height, 
                         size_t pos_x, size_t pos_y, const std::string& label,
                         sf::Color active = sf::Color::Green, 
                         sf::Color deactive = sf::Color::Red):
  Text_Display_Impl(width, height, pos_x, pos_y, label, deactive),
  state_(false),
  active_(false),
  active_color_(active),
  deactive_color_(deactive) { }

Push_Button_Impl::Push_Button_Impl(size_t width, size_t height, 
                         size_t pos_x, size_t pos_y, const std::string& label,
                         std::pair<sf::Color, sf::Color> active_deactive_pair):
  Text_Display_Impl(width, height, pos_x, pos_y, label, active_deactive_pair.second),
  state_(false),
  active_(false),
  active_color_(active_deactive_pair.first),
  deactive_color_(active_deactive_pair.second) { }

bool Push_Button_Impl::is_clicked(int x, int y) {
  return background_.getGlobalBounds().contains(x, y);
}

void Push_Button_Impl::activate() {
  if(!active_) {
    start_time_ = std::chrono::steady_clock::now();
    state_ = true;
    active_ = true;
    background_.setFillColor(active_color_);
  }
}

bool Push_Button_Impl::get_state() { 
  if(state_) {
    state_ = false;
    return true; 
  } else {
    return false;
  }
}

void Push_Button_Impl::update() {
  if(active_ && 
     std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - 
                                                           start_time_) > active_time_) {
    active_ = false;
    background_.setFillColor(deactive_color_);
  }
}

void Push_Button_Impl::draw(sf::RenderWindow& window) const {
  window.draw(background_);
  window.draw(text_);
}

class Push_Button {

  std::unique_ptr<Push_Button_Impl> pimpl_;

public:
  Push_Button(size_t w, size_t h, size_t x, size_t y, const std::string& t, sf::Color a = sf::Color::Green, sf::Color d = sf::Color::Red):
    pimpl_(std::make_unique<Push_Button_Impl>(w, h, x, y, t, a, d)) { }

  Push_Button(size_t w, size_t h, size_t x, size_t y, const std::string& t, std::pair<sf::Color, sf::Color> a_d_p):
    pimpl_(std::make_unique<Push_Button_Impl>(w, h, x, y, t, a_d_p.first, a_d_p.second)) { }

  friend GUI;

};

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

Radio_Button
*/

class GUI {

  std::thread gui_thread_;
 
  std::chrono::milliseconds click_sensitive_dur_ = std::chrono::milliseconds(300);
  std::chrono::steady_clock::time_point last_click_;
  bool clicked_;

  bool new_state_;
  std::map<std::string, bool> state_; // map from button name to bool state of button
                                      // (true - active, false - inactive)

  sf::Font font_;

  std::vector<sf::RectangleShape> backgrounds_;

  std::vector<std::unique_ptr<Text_Display_Impl>> text_displays_;

  std::vector<std::unique_ptr<Toggle_Button_Impl>> toggle_buttons_;
  std::vector<std::unique_ptr<Push_Button_Impl>> push_buttons_;

  void GUI_Loop(sf::RenderWindow& window) {
    while(window.isOpen()) {
      if(clicked_) {
        if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() -
                                                           last_click_) > click_sensitive_dur_) {
          clicked_ = false;
        }
      } else {
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
          sf::Vector2i position = sf::Mouse::getPosition(window);
          for(auto& button : toggle_buttons_) {
            if(button->is_clicked(position.x, position.y)) {
              button->get_state() ? button->deactivate() : button->activate();
              new_state_ = true;
            } 
          }
          for(auto& button : push_buttons_) {
            if(button->is_clicked(position.x, position.y)) {
              button->activate();
              new_state_ = true;
            }
          }
          clicked_ = true;
          last_click_ = std::chrono::steady_clock::now();
          // TO DO : Prevent holding?
        }
      }

      for(auto& button : push_buttons_) { button->update(); }
    }
  }

  public:

  GUI(const GUI&) = delete;
  GUI& operator=(const GUI&) = delete;

  GUI(sf::RenderWindow& window, const sf::Font& font = sf::Font()):
    clicked_(false),
    new_state_(true) {

    gui_thread_ = std::thread(&GUI::GUI_Loop, this, std::ref(window));
    
    if(font.getInfo().family.empty()) {
      // Load default
      if(!font_.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf")) {
        throw std::runtime_error("No def file :""/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf"
                                 " found!");
      }
    } else {
      // Load passed
      font_ = font;
    }

  }

  ~GUI() { gui_thread_.join(); }

  void draw(sf::RenderWindow& window) const {
    for(auto& back : backgrounds_) window.draw(back);
    for(auto& text : text_displays_) text->draw(window);
    for(auto& button : toggle_buttons_) button->draw(window);
    for(auto& button : push_buttons_) button->draw(window); 
  }

  const std::map<std::string, bool>& get_state();

   void set_background_color(sf::Color bg_c, sf::Color text_bg_c) {
     for(auto& bg : backgrounds_) {
       bg.setFillColor(bg_c);
     }
     for(auto& bg : text_displays_) {
       bg->background_.setFillColor(text_bg_c);
     }
   }

   void set_background_color(sf::Color bg_c) {
     set_background_color(bg_c, bg_c);
   }

   void set_text_color(sf::Color t) {
     for(auto& text : text_displays_) {
       text->text_.setFillColor(t);
     }
     for(auto& text : push_buttons_) {
       text->text_.setFillColor(t);
     }
     for(auto& text : toggle_buttons_) {
       text->text_.setFillColor(t);
     }
   }

   void set_button_color(sf::Color active, sf::Color deactive) {
     for(auto& button : push_buttons_) {
       button->active_color_ = active;
       button->deactive_color_ = deactive;
       button->state_ ? button->background_.setFillColor(active) : 
                        button->background_.setFillColor(deactive);
     }
     for(auto& button : toggle_buttons_) {
       button->active_color_ = active;
       button->deactive_color_ = deactive;
       button->state_ ? button->background_.setFillColor(active) : 
                        button->background_.setFillColor(deactive);
     }
   }

   void set_button_color(std::pair<sf::Color, sf::Color> act_deact_pair) {
     for(auto& button : push_buttons_) {
       button->active_color_ = act_deact_pair.first;
       button->deactive_color_ = act_deact_pair.second;
       button->state_ ? button->background_.setFillColor(act_deact_pair.first) : 
                        button->background_.setFillColor(act_deact_pair.second);
     }
     for(auto& button : toggle_buttons_) {
       button->active_color_ = act_deact_pair.first;
       button->deactive_color_ = act_deact_pair.second;
       button->state_ ? button->background_.setFillColor(act_deact_pair.first) : 
                        button->background_.setFillColor(act_deact_pair.second);
     }
   }

   void set_color_scheme(const GUI_Scheme& gs) {
     set_background_color(gs.background_, gs.text_background_);
     set_text_color(gs.text_);
     set_button_color(gs.button_active_, gs.button_deactive_);
   }

  // Add Parts of GUI ( background , text display , push & toggle buttons )

   // TO DO : Background class?
  template<typename... Args,
           typename = std::enable_if_t<
                        std::conjunction_v<std::is_same<sf::RectangleShape&, Args>...>
                      > >
  void add_background(Args&&... args) {
    (void)std::initializer_list<int>{ (backgrounds_.emplace_back(args), 0)... };
  }

/*TO DO	
template<class F, class...Args>
F for_each_arg(F f, Args&&...args) {
  (f(std::forward<Args>(args)),...);
  return f;
}*/

  template<class F, class...Args>
  void for_each_arg(F f, Args&&...args) {
    (void)std::initializer_list<int>{ (f(std::forward<Args>(args)), 0)... };
  }

  template<typename... Args, 
           typename = std::enable_if_t<std::conjunction_v<std::is_same<Text_Display&, Args>...>> >
  void add_text(Args&&... args) {
    auto set_stuff = [&](Text_Display& t) { t.pimpl_->GUI_sizing(font_); };
    for_each_arg(set_stuff, args...);
    (void)std::initializer_list<int>{ (text_displays_.emplace_back(std::move(args.pimpl_)), 0)... };
  }


  template<typename... Args,
           typename = std::enable_if_t<std::conjunction_v<std::is_same<Toggle_Button&, Args>...>> >
  void add_toggle_button(Args&&... args) {
    auto set_stuff = [&](Toggle_Button& t){ t.pimpl_->GUI_sizing(font_);};
    for_each_arg(set_stuff, args...);
    (void)std::initializer_list<int>{ (toggle_buttons_.emplace_back(std::move(args.pimpl_)), 0)... };
    new_state_ = true;
  }

  template<typename... Args,
           typename = std::enable_if_t<std::conjunction_v<std::is_same<Push_Button&, Args>...>> >
  void add_push_button(Args&&... args) {
    auto set_stuff = [&](Push_Button& t){ t.pimpl_->GUI_sizing(font_); };
    for_each_arg(set_stuff, args...);
    (void)std::initializer_list<int>{ (push_buttons_.emplace_back(std::move(args.pimpl_)), 0)... };
    new_state_ = true;
  }

};

// TO DO : Error if buttons have same label..
// NOTE : This reference gets invalidated as soon as this function is called again because it may
//        change the map each call
const std::map<std::string, bool>& GUI::get_state() {
  if(new_state_) {
    new_state_ = false;
    for(auto& button : toggle_buttons_) {
      state_[button->text_.getString()] = button->get_state();
    }
    for(auto& button : push_buttons_) {
      if(button->get_state()) {
        state_[button->text_.getString()] = true;
        new_state_ = true; // because we need to update new state since this is now false
      } else {
        state_[button->text_.getString()] = false;
      }
    }
  }
  return state_;
}

// Going Forward : 
// TO DO : Error if buttons have same label..
// fold expression
// colors
// clean
// release
//
//   fonts lines spacing
