#ifndef SHASHMAQAMFINDER_POPUP_HPP_
#define SHASHMAQAMFINDER_POPUP_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <memory>
#include <stdexcept>

#include "Button.hpp"

namespace PopUpConstants {
  const std::string ARTIST = "Artist";
  const std::string TITLE = "Title";
  const int Y_MARGIN = 60;
  const int X_MARGIN = 20;
  const int LINE_SPACING = 100;
}
class PopUp : public sf::Drawable {
 public:
  PopUp(int width, int height, int window_height);

  void ChangeState();

  void SetData(const std::string& song_name, const std::string& song_artist);

  void Clear();
  bool IsEmpty() const;

 private:
  std::unique_ptr<sf::Font> font_;
  sf::Text song_name_text_;
  sf::Text song_artist_text_;
  sf::RectangleShape background_;

  bool visible_;

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif  // SHASHMAQAMFINDER_POPUP_HPP_