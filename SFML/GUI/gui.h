#include <SFML/Graphics.hpp>
#include <X11/Xlib.h> // XInitThreads ( MUST BE DECLARED IN MAIN THREAD )

#include <vector>
#include <map>
#include <string>
#include <thread>

#include <functional> // std::cref
#include <chrono> // duration_cast, milliseconds, steady_clock 
#include <utility> // pair
#include <memory> // unique_ptr
#include <type_traits> // enable_if, conjunction, is_same // TO DO : C++20 concepts

// TO DO : Make buttons into polymorphic class
// TO DO : Make option to allow label to be displayed outside of button (select position?)
// TO DO : Allow linking of toggle buttons in certain ways so activating one deactivates others?
// TO DO : Make XML format reader
// TO DO : Percent based size constuctors
// TO DO : Namespaces for colors and other gui things
// TO DO : Default label?
// TO DO : Prevent holding?
// TO DO : Font color in text display constructor
// TO DO : Think about inheritance levels of impl classes & protection of functions and members
// TO DO : Allow for setting all text to same ( minimum ) font
// TO DO : Multiline text labels
// TO DO : Default state (ie starting state_) other than false

/*
TO DO : FUTURE GUI ELEMENTS :
Scrolling_Bar
drop_down
Radio_Button
*/

class GUI; // Forward Declaration

// Premade Button Color Pairs - pair(active, deactive)
auto Black_White = std::make_pair(sf::Color::White, sf::Color::Black); 
auto Halloween   = std::make_pair(sf::Color(235, 97, 35), sf::Color::Black);


// GUI Color Schemes
struct GUI_Scheme {

  sf::Color background_;
  sf::Color text_background_;
  sf::Color text_;
  sf::Color button_active_;
  sf::Color button_deactive_; 

};

// Some premade schemes
GUI_Scheme Base_Scheme = { sf::Color(211, 211, 211), sf::Color(211, 211, 211),
                           sf::Color::Black, sf::Color(144, 238, 144), sf::Color(255, 135, 132) };
GUI_Scheme Halloween_Scheme = { sf::Color(136, 30, 228), sf::Color(136, 30, 228), 
                                sf::Color(133, 226, 31), sf::Color(235, 97, 35), sf::Color::Black };


// Function alias for chrono's duration_cast<milliseconds>
template <typename... Args>
auto duration_ms(Args&&... args) -> 
  decltype(std::chrono::duration_cast<std::chrono::milliseconds>(std::forward<Args>(args)...)) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::forward<Args>(args)...);
}

// Base class used to provide unique id's to gui elements(w/ state_) 
// Used for unique indexability in GUI::state_ map
class GUI_ID {
  // Return new id for each call
  static size_t get_gui_id() {
    static size_t calls = 0;
    return ++calls;
  }
  const size_t id_ = get_gui_id();
public:
  // id_ indexing functions
  const size_t id() const { return id_; }
  const size_t operator()() const { return id_; }
};


/*
 *
 * BACKGROUND
 *
 */

// Hold sf::RectangleShape of corresponding GUI background element w/ its GUI interface
class Background_Impl {

  sf::RectangleShape background_;

public:
  
  Background_Impl(const Background_Impl&) = delete;
  Background_Impl& operator=(const Background_Impl&) = delete;

  // params : width, height, pos_x, pos_y (from top left), color
  Background_Impl(size_t, size_t, size_t, size_t, sf::Color);

  void draw(sf::RenderWindow& window) const { window.draw(background_); }
  friend GUI;
};

Background_Impl::Background_Impl(size_t width, size_t height, size_t pos_x,
                                 size_t pos_y, sf::Color color):
  background_(sf::Vector2f(width, height)) {
  background_.setPosition(sf::Vector2f(pos_x, pos_y));
  background_.setFillColor(color);
}

// Class used to create background elements, unique_ptr to implimentation class
class Background {

  std::unique_ptr<Background_Impl> pimpl_;

public:

  // params : width, height, pos_x (from top left), pos_y (from top left), color
  Background(size_t w, size_t h, size_t x, size_t y, sf::Color c = Base_Scheme.background_):
    pimpl_(std::make_unique<Background_Impl>(w, h, x, y,c)) { }

  friend GUI;
};


/*
 *
 *  TEXT DISPLAY
 *
 */

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

  friend GUI;
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
  sf::Vector2f size = background_.getSize();
  
  text_.setFont(font);
  if(text_.getString().getSize() != 0) {
    size_t max_char_width = size.x / text_.getString().getSize();
    text_.setCharacterSize(max_char_width < size.y ? max_char_width : size.y);

    // Helper lambda to get offset vector for first position set
    static auto center_pos_TL = [](sf::FloatRect out, sf::FloatRect in){
                                    return sf::Vector2f((out.width - in.width) / 2 + out.left,
                                                        (out.height - in.height) / 2 + out.top); }; 
    text_.setPosition(center_pos_TL(background_.getGlobalBounds(), text_.getGlobalBounds()));
    // Move based on text bounds
    text_.move(text_.getPosition() - 
               sf::Vector2f(text_.getGlobalBounds().left, text_.getGlobalBounds().top));
  }
}

// Class used to create Text Display elements, unique_ptr to implimentation class
class Text_Display : public GUI_ID {

  std::unique_ptr<Text_Display_Impl> pimpl_;

public:

  // params : width, height, pos_x, pos_y (from top left), label, background color
  Text_Display(size_t w, size_t h, size_t x, size_t y, const std::string& t, 
               sf::Color c = Base_Scheme.text_background_):
    pimpl_(std::make_unique<Text_Display_Impl>(w, h, x, y, t, c)) { }

  friend GUI;
};


/*
 *
 *  TOGGLE BUTTON
 *
 */

// Holds Text_Display_Impl & active/deactive colors for graphics & state_
class Toggle_Button_Impl : Text_Display_Impl {
  
  bool state_;
  sf::Color active_color_;
  sf::Color deactive_color_;

  public:

  Toggle_Button_Impl(const Toggle_Button_Impl&) = delete;
  Toggle_Button_Impl& operator=(const Toggle_Button_Impl&) = delete;

  // params : width, height, pos_x, pos_y (from top left), label, active color, deactive color
  Toggle_Button_Impl(size_t, size_t, size_t, size_t, const std::string&, 
                     sf::Color, sf::Color); // pixel based

  // Returns if passed coordinates are inside button range
  bool is_clicked(int x, int y) const { return background_.getGlobalBounds().contains(x, y); }
  bool get_state() { return state_; }

  // Sets state_ & corresonding color
  void activate();
  void deactivate();

  friend GUI;
};

Toggle_Button_Impl::Toggle_Button_Impl(size_t width, size_t height, size_t pos_x, size_t pos_y, 
                                       const std::string& label, 
                                       sf::Color active, sf::Color deactive):
  Text_Display_Impl(width, height, pos_x, pos_y, label, deactive),
  state_(false),
  active_color_(active),
  deactive_color_(deactive) { }

void Toggle_Button_Impl::activate() {
  state_ = true;
  background_.setFillColor(active_color_);
}

void Toggle_Button_Impl::deactivate() {
  state_ = false;
  background_.setFillColor(deactive_color_);
}

// Class used to create Toggle Button elements, unique_ptr to implimentation class
class Toggle_Button : public GUI_ID {

  std::unique_ptr<Toggle_Button_Impl> pimpl_;

public:

  // params : width, height, pos_x, pos_y (from top left), label, active color, deactive color
  Toggle_Button(size_t w, size_t h, size_t x, size_t y, const std::string& t, 
                sf::Color a = Base_Scheme.button_active_, 
                sf::Color d = Base_Scheme.button_deactive_):
    pimpl_(std::make_unique<Toggle_Button_Impl>(w, h, x, y, t, a, d)) { }

  // params : width, height, pos_x, pos_y (from top left), label, active-deactive color pair
  Toggle_Button(size_t w, size_t h, size_t x, size_t y, const std::string& t, 
                std::pair<sf::Color, sf::Color> a_d_p):
    pimpl_(std::make_unique<Toggle_Button_Impl>(w, h, x, y, t, a_d_p.first, a_d_p.second)) { }

  friend GUI;
};


/*
 *
 *  PUSH BUTTON
 *
 */

// Holds Text_Display_Impl & active/deactive colors for graphics & state_
// Also holds timing info for activation/deactivation of button
class Push_Button_Impl : Text_Display_Impl {

  bool state_;
  sf::Color active_color_;
  sf::Color deactive_color_;

  bool active_;
  std::chrono::milliseconds active_time_ = std::chrono::milliseconds(200);
  std::chrono::steady_clock::time_point start_time_;

public:
  
  Push_Button_Impl(const Push_Button_Impl&) = delete;
  Push_Button_Impl& operator=(const Push_Button_Impl&) = delete;

  // params : width, height, pos_x, pos_y (from top left), label, active color, deactive color
  Push_Button_Impl(size_t, size_t, size_t, size_t, const std::string&, sf::Color, sf::Color);

  // Returns if passed coordinates are inside button range
  bool is_clicked(int x, int y) const { return background_.getGlobalBounds().contains(x, y); }
  // Returns state_ ( & sets state_ to false after being called, so only get one true per click )
  bool get_state();

  // Set state_ & active characteristics & start timer if not already active
  void activate(); 
  // Deactivate after timer is up
  void update();

  friend GUI;
};

Push_Button_Impl::Push_Button_Impl(size_t width, size_t height, 
                                   size_t pos_x, size_t pos_y, const std::string& label,
                                   sf::Color active, sf::Color deactive):
  Text_Display_Impl(width, height, pos_x, pos_y, label, deactive),
  state_(false),
  active_color_(active),
  deactive_color_(deactive),
  active_(false) { }

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

// Class used to create Push Button elements, unique_ptr to implimentation class
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

  friend GUI;
};


/*
 *
 * GUI
 *
 */

// GUI class which stores created GUI elements, does clicking & updating in its own thread,
// and allows obtaining the state of all the GUI elements & drawing of elements
class GUI {

  // Runs GUI_Loop function
  std::thread gui_thread_;
 
  // How long GUI waits between clicks
  std::chrono::milliseconds click_sensitive_dur_ = std::chrono::milliseconds(300);
  // Helpers to calculate when clickable ( ie so one click doesnt register as a ton )
  bool clicked_; // TO DO : Store last click?
  std::chrono::steady_clock::time_point last_click_;

  // If need to calculate state_ again
  bool new_state_;
  // button id_ to bool state_ ( true - active, false - inactive )
  std::map<size_t, bool> state_;

  sf::Font font_;

  // GUI elements
  std::vector<std::unique_ptr<Background_Impl>> backgrounds_;
  std::map<size_t, std::unique_ptr<Text_Display_Impl>> text_displays_;
  std::map<size_t, std::unique_ptr<Toggle_Button_Impl>> toggle_buttons_;
  std::map<size_t, std::unique_ptr<Push_Button_Impl>> push_buttons_;

  // Locks push_button update if push_button being inserted
  bool update_lock_ = false;
  // window loop that the gui_thread_ runs
  void GUI_Loop(const sf::RenderWindow&);

public:

  GUI(const GUI&) = delete;
  GUI& operator=(const GUI&) = delete;

  // params : window, font
  GUI(const sf::RenderWindow&, const sf::Font& = sf::Font());
  ~GUI() { gui_thread_.join(); }

  // Draw GUI to the window
  void draw(sf::RenderWindow&) const;
  // Return const reference to the state_ map, recalculates state_ only if new_state_=true
  // This reference will have a chance of invalidating other refs from get_state made if it updates
  const std::map<size_t, bool>& get_state();

  // Get & Set text in text_displays_
  std::string get_text(size_t id) const;
  void set_text(size_t id, std::string text);

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
  // params : GUI_Scheme class
  void set_color_scheme(const GUI_Scheme&);

  // Add Parts of GUI ( background , text display , push & toggle buttons )

  // Inserts underlying Background_Impl's into background_
  // params : Background, ...
  template<typename... Args,
           typename = std::enable_if_t<std::conjunction_v<std::is_same<Background&, Args>...>> >
  void add_background(Args&&... args) {
    (backgrounds_.emplace_back(std::move(args.pimpl_)), ...);
  }

  // Inserts into text_displays_ & sets text sizes for underlying Text_Display_Impl's
  // params : Text_Display, ...
  template<typename... Args, 
           typename = std::enable_if_t<std::conjunction_v<std::is_same<Text_Display&, Args>...>> >
  void add_text(Args&&... args) {
    (args.pimpl_->GUI_sizing(font_), ...);
    (text_displays_.emplace(args(), std::move(args.pimpl_)), ...);
  }


  // Inserts into toggle_buttons_ & new_state_=true & text sizes for underlying Text_Display_Impl's
  // params : Toggle_Button, ...
  template<typename... Args,
           typename = std::enable_if_t<std::conjunction_v<std::is_same<Toggle_Button&, Args>...>> >
  void add_toggle_button(Args&&... args) {
    (args.pimpl_->GUI_sizing(font_), ...);
    (toggle_buttons_.emplace(args(), std::move(args.pimpl_)), ...);
    new_state_ = true;
  }

  // Inserts into push_buttons_ & new_state_=true & text sizes for underlying Text_Display_Impl's
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

GUI::GUI(const sf::RenderWindow& window, const sf::Font& font):
  clicked_(false),
  new_state_(true) {

  // Launch Thread
  gui_thread_ = std::thread(&GUI::GUI_Loop, this, std::cref(window));
  
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

void GUI::GUI_Loop(const sf::RenderWindow& window) {
  while(window.isOpen()) {
    if(clicked_) {
      // Wait till it has been over click_sensitive_dur_ milliseconds & make clickable again
      if(duration_ms(std::chrono::steady_clock::now() - last_click_) > click_sensitive_dur_) {
        clicked_ = false;
      }
    } else {
      // If clicked check if any button was clicked & set clicked_ / timer
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i position = sf::Mouse::getPosition(window);
        for(auto& [key, button] : toggle_buttons_) {
          if(button->is_clicked(position.x, position.y)) {
            button->get_state() ? button->deactivate() : button->activate();
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

void GUI::draw(sf::RenderWindow& window) const {
  for(auto& back : backgrounds_) back->draw(window);
  for(const auto& [key, text] : text_displays_) text->draw(window);
  for(const auto& [key, button] : toggle_buttons_) button->draw(window);
  for(const auto& [key, button] : push_buttons_) button->draw(window); 
}

// NOTE : This reference gets invalidated as soon as this function is called again because it may
//        change the map each call
const std::map<size_t, bool>& GUI::get_state() {
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

// TO DO : Document these new functions
std::string GUI::get_text(size_t id) const {
  if(text_displays_.count(id)) {
    return text_displays_.at(id)->text_.getString();
  } else {
    // TO DO : Send error
    return "";
  }
}

void GUI::set_text(size_t id, std::string text) {
  if(text_displays_.count(id)) {
    text_displays_[id]->text_.setString(text);
    text_displays_[id]->GUI_sizing(font_); // TO DO : Think about not setting font again
  } else {
    // TO DO : Send error
  }
}

void GUI::set_background_color(sf::Color bg_c) {
  set_background_color(bg_c, bg_c);
}

void GUI::set_background_color(sf::Color bg_c, sf::Color text_bg_c) {
  for(auto& bg : backgrounds_) bg->background_.setFillColor(bg_c);
  for(auto& [key, bg] : text_displays_) bg->background_.setFillColor(text_bg_c);
}

void GUI::set_text_color(sf::Color t) {
  for(auto& [key, text] : text_displays_) text->text_.setFillColor(t);
  for(auto& [key, button] : push_buttons_) button->text_.setFillColor(t);
  for(auto& [key, button] : toggle_buttons_) button->text_.setFillColor(t);
}

void GUI::set_button_color(sf::Color active, sf::Color deactive) {
  for(auto& [key, button] : push_buttons_) {
    button->active_color_ = active;
    button->deactive_color_ = deactive;
    button->state_ ? button->background_.setFillColor(active) : 
                     button->background_.setFillColor(deactive);
  }
  for(auto& [key, button] : toggle_buttons_) {
    button->active_color_ = active;
    button->deactive_color_ = deactive;
    button->state_ ? button->background_.setFillColor(active) : 
                     button->background_.setFillColor(deactive);
  }
}

void GUI::set_button_color(std::pair<sf::Color, sf::Color> act_deact_pair) {
  set_button_color(act_deact_pair.first, act_deact_pair.second);
}

void GUI::set_color_scheme(const GUI_Scheme& gs) {
  set_background_color(gs.background_, gs.text_background_);
  set_text_color(gs.text_);
  set_button_color(gs.button_active_, gs.button_deactive_);
}

// Going Forward : 
// release
