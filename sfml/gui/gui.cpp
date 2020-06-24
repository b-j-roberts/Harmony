#include "gui.h"

#include <stdexcept> // runtime_error
#include <functional> // std::cref

// HELPERS

// Function alias for chrono's duration_cast<milliseconds>
template <typename... Args>
auto duration_ms(Args&&... args) ->
  decltype(std::chrono::duration_cast<std::chrono::milliseconds>(std::forward<Args>(args)...)) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::forward<Args>(args)...);
}

Background_Impl::Background_Impl(size_t width, size_t height, size_t pos_x,
                                 size_t pos_y, sf::Color color):
  background_(sf::Vector2f(static_cast<float>(width), static_cast<float>(height))) {
  background_.setPosition(sf::Vector2f(static_cast<float>(pos_x), static_cast<float>(pos_y)));
  background_.setFillColor(color);
}

Text_Display_Impl::Text_Display_Impl(size_t width, size_t height, size_t pos_x,
                                     size_t pos_y, const std::string& text, sf::Color bg):
  background_(sf::Vector2f(static_cast<float>(width), static_cast<float>(height))) {
  background_.setPosition(sf::Vector2f(static_cast<float>(pos_x), static_cast<float>(pos_y)));
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
    text_.setCharacterSize(static_cast<float>(max_char_width) < size.y ? max_char_width :
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

Gui::Gui(const sf::RenderWindow& window, const sf::Font& font):
  clicked_(false),
  new_state_(true) {
  // Launch Thread
  gui_thread_ = std::thread(&Gui::gui_loop, this, std::cref(window));
  // Load default or passed font
  if(font.getInfo().family.empty()) {
    if(!font_.loadFromFile("FreeMonoBold.ttf")) {
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
          if(button->is_clicked(static_cast<float>(position.x), static_cast<float>(position.y))) {
            button->toggle();
            deactivate_links(key);
            new_state_ = true;
          }
        }
        for(auto& [key, button] : push_buttons_) {
          if(button->is_clicked(static_cast<float>(position.x), static_cast<float>(position.y))) {
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
