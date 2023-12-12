#ifndef SHASHMAQAMFINDER_POPUP_HPP_
#define SHASHMAQAMFINDER_POPUP_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <memory>
#include <stdexcept>

#include "Button.hpp"

class PopUp : public sf::Drawable {
 public:
  PopUp();

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