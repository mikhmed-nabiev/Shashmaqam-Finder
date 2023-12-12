#include "PopUp.hpp"

PopUp::PopUp() : visible_(false) {
  // Set up the background_ rectangle
  background_.setSize(sf::Vector2f(400, 200));
  background_.setFillColor(sf::Color(15, 187, 255, 80));  // Semi-transparent black
  background_.setPosition(0, 500);

  font_ = std::make_unique<sf::Font>();
  if (!font_->loadFromFile(FONT_PATH)) {
    throw std::runtime_error("Couldn't load font");
  }
  song_name_text_.setFont(*font_.get());
  song_name_text_.setFillColor(sf::Color::Black);
  song_name_text_.setPosition(20, 540);

  song_artist_text_.setFont(*font_.get());
  song_artist_text_.setFillColor(sf::Color::Black);
  song_artist_text_.setPosition(20, 600);
}

void PopUp::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  if (visible_) {
    target.draw(background_);
    target.draw(song_name_text_);
    target.draw(song_artist_text_);
  }
}

void PopUp::ChangeState() { visible_ = !visible_; }

void PopUp::SetData(const std::string& song_name,
                    const std::string& song_artist) {
  song_name_text_.setString("Name : " + song_name);
  song_artist_text_.setString("Artist : " + song_artist);
}

bool PopUp::IsEmpty() const {
  return song_name_text_.getString().isEmpty() &&
         song_artist_text_.getString().isEmpty();
}

void PopUp::Clear() {
  song_name_text_.setString("");
  song_artist_text_.setString("");
}