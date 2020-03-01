#ifndef HARMONY_GUI_SFML_H
#define HARMONY_GUI_SFML_H

#include <SFML/Graphics.hpp>
#include <X11/Xlib.h> // XInitThreads ( MUST BE DECLARED IN MAIN THREAD )

#include <map>
#include <vector>
#include <string>
#include <thread>

#include <functional> // std::cref
#include <chrono> // duration_cast, milliseconds, steady_clock 
#include <utility> // pair, forward, move
#include <memory> // unique_ptr
#include <type_traits> // enable_if, conjunction, is_same // TO DO : C++20 concepts
#include <stdexcept> // runtime_error

// TO DO : Make buttons into polymorphic class
// TO DO : Make option to allow label to be displayed outside of button (select position?)
// TO DO : Make XML format reader
// TO DO : Percent based size constuctors
// TO DO : Namespaces for colors, ... ( remove global constructors -Wglobal-constructors )
// TO DO : Default label?
// TO DO : Prevent holding?
// TO DO : Font color in text display constructor
// TO DO : Think about inheritance levels of impl classes & protection of functions and members
// TO DO : Allow for setting all text to same ( minimum ) font
// TO DO : Multiline text labels
// TO DO : Allow selection of min & max # of active linked toggles
// TO DO : Default state checking on linked toggles
// TO DO : Locking buttons into states, or from being clicked

/*
TO DO : FUTURE GUI ELEMENTS :
Scrolling_Bar
drop_down
Radio_Button
*/

class Gui; // Forward Declaration

// COLORING

// Premade Button Color Pairs - pair(active, deactive)
const auto Black_White = std::make_pair(sf::Color::White, sf::Color::Black); 
const auto Halloween   = std::make_pair(sf::Color(235, 97, 35), sf::Color::Black);


// GUI Color Schemes
struct Gui_Scheme {
  sf::Color background_;
  sf::Color text_background_;
  sf::Color text_;
  sf::Color button_active_;
  sf::Color button_deactive_; 
};
// Some premade schemes
const Gui_Scheme Base_Scheme = { sf::Color(211, 211, 211), sf::Color(211, 211, 211),
                                 sf::Color::Black, sf::Color(144, 238, 144), 
                                 sf::Color(255, 135, 132) };
const Gui_Scheme Halloween_Scheme = { sf::Color(136, 30, 228), sf::Color(136, 30, 228), 
                                      sf::Color(133, 226, 31), sf::Color(235, 97, 35),
                                      sf::Color::Black };


// HELPERS

// Function alias for chrono's duration_cast<milliseconds>
template <typename... Args>
auto duration_ms(Args&&... args) -> 
  decltype(std::chrono::duration_cast<std::chrono::milliseconds>(std::forward<Args>(args)...)) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::forward<Args>(args)...);
}

// Base class used to provide unique id's to gui elements 
// Used for unique indexability in Gui::state_ map
class GUI_ID {
  // Return new id for each call
  static size_t get_gui_id() {
    static size_t calls = 0;
    return ++calls;
  }
  const size_t id_ = get_gui_id();
public:
  // id_ indexing functions
  size_t id() const { return id_; }
  size_t operator()() const { return id_; }
};


// BACKGROUND

// Hold sf::RectangleShape of corresponding GUI background element w/ its GUI interface
class Background_Impl {
  
  sf::RectangleShape background_;

public:
  Background_Impl(const Background_Impl&) = delete;
  Background_Impl& operator=(const Background_Impl&) = delete;
  // params : width, height, pos_x, pos_y (from top left), color
  Background_Impl(size_t, size_t, size_t, size_t, sf::Color);

  void draw(sf::RenderWindow& window) const { window.draw(background_); }
  friend Gui;
};

Background_Impl::Background_Impl(size_t width, size_t height, size_t pos_x,
                                 size_t pos_y, sf::Color color):
  background_(sf::Vector2f(width, height)) {
  background_.setPosition(sf::Vector2f(pos_x, pos_y));
  background_.setFillColor(color);
}

// Background_Impl Factory w/ unique_ptr to impl & unique id
class Background : GUI_ID {
  std::unique_ptr<Background_Impl> pimpl_;
public:
  // params : width, height, pos_x (from top left), pos_y (from top left), color
  Background(size_t w, size_t h, size_t x, size_t y, sf::Color c = Base_Scheme.background_):
    pimpl_(std::make_unique<Background_Impl>(w, h, x, y, c)) { }
  friend Gui;
};


// TEXT DISPLAY

// Hold sf::RectangleShape for background & sf::Text object of GUI element w/ its GUI interfacing
class Text_Display_Impl {
protected:
  sf::RectangleShape background_;
  sf::Text text_;
  // setup function for text_ : size, position, and font after being given to a GUI obj
  void GUI_sizing(const sf::Font&);

public:
  Text_Display_Impl(const Text_Display_Impl&) = delete;
  Text_Display_Impl& operator=(const Text_Display_Impl&) = delete;
  // params : width, height, pos_x, pos_y (from top left), label, color
  Text_Display_Impl(size_t, size_t, size_t, size_t, const std::string&, sf::Color);

  void draw(sf::RenderWindow& window) const { window.draw(background_); window.draw(text_); }
  friend Gui;
};

Text_Display_Impl::Text_Display_Impl(size_t width, size_t height, size_t pos_x,
                                     size_t pos_y, const std::string& text, sf::Color bg):
  background_(sf::Vector2f(width, height)) {
  background_.setPosition(sf::Vector2f(pos_x, pos_y));
  background_.setFillColor(bg);
  text_.setString(text);
  text_.setFillColor(Base_Scheme.text_);
}

void Text_Display_Impl::GUI_sizing(const sf::Font& font) {
  text_.setFont(font);
  if(text_.getString().getSize() != 0) {
    sf::Vector2f size = background_.getSize();
    // Select max font size
    unsigned int max_char_width = static_cast<unsigned int>(size.x) / text_.getString().getSize();
    text_.setCharacterSize(max_char_width < size.y ? max_char_width : 
                                                     static_cast<unsigned int>(size.y));

    // Helper lambda to get offset vector for first position set
    auto center_pos_TL = [](sf::FloatRect out, sf::FloatRect in){
                                    return sf::Vector2f((out.width - in.width) / 2 + out.left,
                                                        (out.height - in.height) / 2 + out.top); }; 
    text_.setPosition(center_pos_TL(background_.getGlobalBounds(), text_.getGlobalBounds()));
    // Move based on text bounds
    text_.move(text_.getPosition() - 
               sf::Vector2f(text_.getGlobalBounds().left, text_.getGlobalBounds().top));
  }
}

// Text_Display_Impl Factory w/ unique_ptr to impl & unique id
class Text_Display : public GUI_ID {
  std::unique_ptr<Text_Display_Impl> pimpl_;
public:
  // params : width, height, pos_x, pos_y (from top left), label, background color
  Text_Display(size_t w, size_t h, size_t x, size_t y, const std::string& t, 
               sf::Color c = Base_Scheme.text_background_):
    pimpl_(std::make_unique<Text_Display_Impl>(w, h, x, y, t, c)) { }
  friend Gui;
};


// TOGGLE BUTTON

// Is a Text_Display_Impl w/ active/deactive colors for graphics
// state_ toggles each time clicked
class Toggle_Button_Impl : Text_Display_Impl {
  
  bool state_;
  sf::Color active_color_;
  sf::Color deactive_color_;

public:
  Toggle_Button_Impl(const Toggle_Button_Impl&) = delete;
  Toggle_Button_Impl& operator=(const Toggle_Button_Impl&) = delete;
  // params : width, height, pos_x, pos_y (from top left), label, state, active color, deactive color
  Toggle_Button_Impl(size_t, size_t, size_t, size_t, const std::string&, bool, sf::Color, sf::Color);

  bool get_state() const { return state_; }
  // Returns if passed coordinates are inside button range
  bool is_clicked(int x, int y) const { return background_.getGlobalBounds().contains(x, y); }
  // Toggles state_ & corresonding color
  void toggle();
  friend Gui;
};

Toggle_Button_Impl::Toggle_Button_Impl(size_t width, size_t height, size_t pos_x, size_t pos_y, 
                                       const std::string& label, bool state,
                                       sf::Color active, sf::Color deactive):
  Text_Display_Impl(width, height, pos_x, pos_y, label, state ? active : deactive),
  state_(state),
  active_color_(active),
  deactive_color_(deactive) { }

void Toggle_Button_Impl::toggle() {
  state_ = !state_;
  background_.setFillColor(state_ ? active_color_ : deactive_color_);
}

// Toggle_Button_Impl Factory w/ unique_ptr to impl & unique id
class Toggle_Button : public GUI_ID {
  std::unique_ptr<Toggle_Button_Impl> pimpl_;
public:
  // params : width, height, pos_x, pos_y (from top left), label, state, active color, deactive color
  Toggle_Button(size_t w, size_t h, size_t x, size_t y, const std::string& t, bool s = false, 
                sf::Color a = Base_Scheme.button_active_, 
                sf::Color d = Base_Scheme.button_deactive_):
    pimpl_(std::make_unique<Toggle_Button_Impl>(w, h, x, y, t, s, a, d)) { }
  // params : width, height, pos_x, pos_y (from top left), label, active color, deactive color
  Toggle_Button(size_t w, size_t h, size_t x, size_t y, const std::string& t,
                sf::Color a, sf::Color d):
    pimpl_(std::make_unique<Toggle_Button_Impl>(w, h, x, y, t, false, a, d)) { }
  // params : width, height, pos_x, pos_y (from top left), label, state, active-deactive color pair
  Toggle_Button(size_t w, size_t h, size_t x, size_t y, const std::string& t, bool s,
                std::pair<sf::Color, sf::Color> a_d_p):
    pimpl_(std::make_unique<Toggle_Button_Impl>(w, h, x, y, t, s, a_d_p.first, a_d_p.second)) { }
  // params : width, height, pos_x, pos_y (from top left), label, active-deactive color pair
  Toggle_Button(size_t w, size_t h, size_t x, size_t y, const std::string& t,
                std::pair<sf::Color, sf::Color> a_d_p):
    pimpl_(std::make_unique<Toggle_Button_Impl>(w, h, x, y, t, false, a_d_p.first, a_d_p.second)) { }
  friend Gui;
};


// PUSH BUTTON

// Is a Text_Display_Impl w/ active/deactive colors for graphics
// state_ is true for one get_state() call after click
// active_ is true for active_time_ ( in order to do active_color & click timing )
class Push_Button_Impl : Text_Display_Impl {

  bool state_;
  bool active_;
  sf::Color active_color_;
  sf::Color deactive_color_;
  std::chrono::milliseconds active_time_ = std::chrono::milliseconds(200);
  std::chrono::steady_clock::time_point start_time_;

public:
  Push_Button_Impl(const Push_Button_Impl&) = delete;
  Push_Button_Impl& operator=(const Push_Button_Impl&) = delete;
  // params : width, height, pos_x, pos_y (from top left), label, active color, deactive color
  Push_Button_Impl(size_t, size_t, size_t, size_t, const std::string&, sf::Color, sf::Color);

  // Returns state_ ( & sets state_ to false after being called, so only get one true per click )
  bool get_state();
  // Returns if passed coordinates are inside button range
  bool is_clicked(int x, int y) const { return background_.getGlobalBounds().contains(x, y); }
  // Set state_, active characteristics, & start timer - if !active_
  void activate(); 
  // Deactivate after timer is up
  void update();
  friend Gui;
};

Push_Button_Impl::Push_Button_Impl(size_t width, size_t height, 
                                   size_t pos_x, size_t pos_y, const std::string& label,
                                   sf::Color active, sf::Color deactive):
  Text_Display_Impl(width, height, pos_x, pos_y, label, deactive),
  state_(false),
  active_(false),
  active_color_(active),
  deactive_color_(deactive) { }

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
  if(active_ && duration_ms(std::chrono::steady_clock::now() - start_time_) > active_time_) {
    active_ = false;
    background_.setFillColor(deactive_color_);
  }
}

// Push_Button_Impl Factory w/ unique_ptr to impl & unique id
class Push_Button : public GUI_ID {
  std::unique_ptr<Push_Button_Impl> pimpl_;
public:
  // params : width, height, pos_x, pos_y (from top left), label, active color, deactive color
  Push_Button(size_t w, size_t h, size_t x, size_t y, const std::string& t, 
              sf::Color a = Base_Scheme.button_active_, sf::Color d = Base_Scheme.button_deactive_):
    pimpl_(std::make_unique<Push_Button_Impl>(w, h, x, y, t, a, d)) { }
  // params : width, height, pos_x, pos_y (from top left), label, active-deactive color pair
  Push_Button(size_t w, size_t h, size_t x, size_t y, const std::string& t, 
              std::pair<sf::Color, sf::Color> a_d_p):
    pimpl_(std::make_unique<Push_Button_Impl>(w, h, x, y, t, a_d_p.first, a_d_p.second)) { }
  friend Gui;
};


// GUI

// Interface class for drawing and getting state of the GUI for the user; also stores GUI elements 
// Has its own designated thread ( gui_thread_ ) for user input and updating states
class Gui {

  bool clicked_;
  // Time GUI disables clicks after a click ( how long clicked_ stays true )
  std::chrono::milliseconds click_sensitive_dur_ = std::chrono::milliseconds(300);
  std::chrono::steady_clock::time_point last_click_;

  // If need to calculate state_ again ( after an updated state )
  bool new_state_;
  // button id_ to bool state_ ( true - active, false - inactive )
  std::map<size_t, bool> state_;

  sf::Font font_;

  // GUI elements
  std::map<size_t, std::unique_ptr<Background_Impl>> backgrounds_;
  std::map<size_t, std::unique_ptr<Text_Display_Impl>> text_displays_;
  std::map<size_t, std::unique_ptr<Toggle_Button_Impl>> toggle_buttons_;
  std::map<size_t, std::unique_ptr<Push_Button_Impl>> push_buttons_;

  // Maps Toggle_Button id to vector of ids to linked toggle_buttons_ ( only if there is linkage )
  std::map<size_t, std::shared_ptr<std::vector<size_t>>> linked_toggles_;
  // Deactivate the toggle_buttons_ linked to id passed in
  void deactivate_links(size_t);

  std::thread gui_thread_;
  // Locks push_button update while push_button inserted ( prevents thread accessing unready ptr )
  bool update_lock_ = false;
  // window loop that the gui_thread_ runs
  void gui_loop(const sf::RenderWindow&);

public:
  Gui(const Gui&) = delete;
  Gui& operator=(const Gui&) = delete;
  // params : window, font
  Gui(const sf::RenderWindow&, const sf::Font& = sf::Font());
  ~Gui() { gui_thread_.join(); }

  // Draws in order : backgrounds_, text_displays_, toggle_buttons_, push_buttons_
  void draw(sf::RenderWindow&) const;
  // Returns state_ map, recalculates state_ only if new_state_==true (ie use invalidates last ref)
  const std::map<size_t, bool>& get_state();
  // Get & Set text in text_displays_
  std::string get_text(size_t id) const;
  void set_text(size_t id, const std::string& text);

  // SET COLORS

  // params : background & text_display background color
  void set_background_color(sf::Color);
  // params : background color, text_display background color
  void set_background_color(sf::Color, sf::Color);
  // params : Text color
  void set_text_color(sf::Color);
  // params : active color , deactive color
  void set_button_color(sf::Color, sf::Color);
  // params : active-deactive color pair
  void set_button_color(std::pair<sf::Color, sf::Color>);
  // params : Gui_Scheme class
  void set_color_scheme(const Gui_Scheme&);

  // Add Parts of GUI ( background , text display , push & toggle buttons )

  // params : Background, ...
  template<typename... Args,
           typename = std::enable_if_t<std::conjunction_v<std::is_same<Background&, Args>...>> >
  void add_background(Args&&... args) {
    (backgrounds_.emplace(args(), std::move(args.pimpl_)), ...);
  }
  // params : Text_Display, ...
  template<typename... Args, 
           typename = std::enable_if_t<std::conjunction_v<std::is_same<Text_Display&, Args>...>> >
  void add_text(Args&&... args) {
    (args.pimpl_->GUI_sizing(font_), ...);
    (text_displays_.emplace(args(), std::move(args.pimpl_)), ...);
  }
  // params : Toggle_Button, ...
  template<typename... Args,
           typename = std::enable_if_t<std::conjunction_v<std::is_same<Toggle_Button&, Args>...>> >
  void add_toggle_button(Args&&... args) {
    (args.pimpl_->GUI_sizing(font_), ...);
    (toggle_buttons_.emplace(args(), std::move(args.pimpl_)), ...);
    new_state_ = true;
  }
  // params : Toggle_Button, ... ( to be linked )
  template<typename... Args,
           typename = std::enable_if_t<std::conjunction_v<std::is_same<Toggle_Button&, Args>...>> >
  void add_linked_toggle(Args&&... args) {
    (args.pimpl_->GUI_sizing(font_), ...);
    (toggle_buttons_.emplace(args(), std::move(args.pimpl_)), ...);
    auto linked_vec = std::make_shared<std::vector<size_t>>();
    (linked_vec->push_back(args()), ...);
    (linked_toggles_.emplace(args(), linked_vec), ...);
    new_state_ = true;
  }
  // params : Push_Button, ...
  template<typename... Args,
           typename = std::enable_if_t<std::conjunction_v<std::is_same<Push_Button&, Args>...>> >
  void add_push_button(Args&&... args) {
    update_lock_ = true;
    (args.pimpl_->GUI_sizing(font_), ...);
    (push_buttons_.emplace(args(), std::move(args.pimpl_)), ...);
    new_state_ = true;
    update_lock_ = false;
  }
};

Gui::Gui(const sf::RenderWindow& window, const sf::Font& font):
  clicked_(false),
  new_state_(true) {
  // Launch Thread
  gui_thread_ = std::thread(&Gui::gui_loop, this, std::cref(window));
  // Load default or passed font
  if(font.getInfo().family.empty()) {
    if(!font_.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf")) {
      throw std::runtime_error("No def file :""/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf"
                               " found!");
    }
  } else {
    font_ = font;
  }
}

void Gui::deactivate_links(size_t id) {
  if(linked_toggles_.count(id) != 0) {
    for(size_t idx : *linked_toggles_.at(id) )
      if(idx != id && toggle_buttons_[idx]->get_state()) toggle_buttons_[idx]->toggle();
  }
}

void Gui::gui_loop(const sf::RenderWindow& window) {
  while(window.isOpen()) {
    if(clicked_) {
      // Wait till it has been over click_sensitive_dur_ milliseconds & make clickable again
      if(duration_ms(std::chrono::steady_clock::now() - last_click_) > click_sensitive_dur_) {
        clicked_ = false;
      }
    } else {
      // If clicked check if any button was clicked, set state accordingly, & set clicked_ + timer
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i position = sf::Mouse::getPosition(window);
        for(auto& [key, button] : toggle_buttons_) {
          if(button->is_clicked(position.x, position.y)) {
            button->toggle();
            deactivate_links(key);
            new_state_ = true;
          } 
        }
        for(auto& [key, button] : push_buttons_) {
          if(button->is_clicked(position.x, position.y)) {
            button->activate();
            new_state_ = true;
          }
        }
        clicked_ = true;
        last_click_ = std::chrono::steady_clock::now();
      }
    }

    // Update push_buttons_ so they will deactivate if needed
    for(auto& [key, button] : push_buttons_) { if(!update_lock_) button->update(); }
  }
}

void Gui::draw(sf::RenderWindow& window) const {
  for(const auto& [key, back] : backgrounds_) back->draw(window);
  for(const auto& [key, text] : text_displays_) text->draw(window);
  for(const auto& [key, button] : toggle_buttons_) button->draw(window);
  for(const auto& [key, button] : push_buttons_) button->draw(window); 
}

// NOTE : Last reference gets invalidated when this function is called bc state_ map may change
const std::map<size_t, bool>& Gui::get_state() {
  if(new_state_) {
    new_state_ = false;
    for(auto& [key, button] : toggle_buttons_) {
      state_[key] = button->get_state();
    }
    for(auto& [key, button] : push_buttons_) {
      if(button->get_state()) {
        state_[key] = true;
        new_state_ = true; // To prevent multiple true states on push_button
      } else {
        state_[key] = false;
      }
    }
  }
  return state_;
}

std::string Gui::get_text(size_t id) const {
  if(text_displays_.count(id)) {
    return text_displays_.at(id)->text_.getString();
  } else {
    throw std::runtime_error("Invalid Text_Display id given in get_text: id=" + std::to_string(id));
  }
}

void Gui::set_text(size_t id, const std::string& text) {
  if(text_displays_.count(id)) {
    text_displays_[id]->text_.setString(text);
    text_displays_[id]->GUI_sizing(font_); // TO DO : Think about not setting font again
  } else {
    throw std::runtime_error("Invalid Text_Display id given in set_text: id=" + std::to_string(id));
  }
}

void Gui::set_background_color(sf::Color bg_c) {
  set_background_color(bg_c, bg_c);
}
void Gui::set_background_color(sf::Color bg_c, sf::Color text_bg_c) {
  for(auto& [key, bg] : backgrounds_) bg->background_.setFillColor(bg_c);
  for(auto& [key, bg] : text_displays_) bg->background_.setFillColor(text_bg_c);
}
void Gui::set_text_color(sf::Color t) {
  for(auto& [key, text] : text_displays_) text->text_.setFillColor(t);
  for(auto& [key, button] : push_buttons_) button->text_.setFillColor(t);
  for(auto& [key, button] : toggle_buttons_) button->text_.setFillColor(t);
}
void Gui::set_button_color(sf::Color active, sf::Color deactive) {
  for(auto& [key, button] : push_buttons_) {
    button->active_color_ = active;
    button->deactive_color_ = deactive;
    button->background_.setFillColor(button->state_ ? active : deactive);
  }
  for(auto& [key, button] : toggle_buttons_) {
    button->active_color_ = active;
    button->deactive_color_ = deactive;
    button->background_.setFillColor(button->state_ ? active : deactive);
  }
}
void Gui::set_button_color(std::pair<sf::Color, sf::Color> act_deact_pair) {
  set_button_color(act_deact_pair.first, act_deact_pair.second);
}
void Gui::set_color_scheme(const Gui_Scheme& gs) {
  set_background_color(gs.background_, gs.text_background_);
  set_text_color(gs.text_);
  set_button_color(gs.button_active_, gs.button_deactive_);
}

#endif
