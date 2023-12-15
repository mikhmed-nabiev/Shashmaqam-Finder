#ifndef SHASHMAQAMFINDER_BUTTON_HPP_
#define SHASHMAQAMFINDER_BUTTON_HPP_

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include <memory>

const std::string FONT_PATH = "../UI/assets/fonts/Roboto-Regular.ttf";

namespace ButtonConstants {
  const float RADIUS_DELTA = 0.006f;
  const int ANIMATION_TIME = 10;

  const sf::Color BUTTON_COLOR = sf::Color(15, 187, 255);
  const sf::Color LAYER_COLOR = sf::Color(15, 187, 255, 50);
  const float LAYER1_SCALE = 1.4f;
  const float LAYER2_SCALE = 1.2f;
  const std::string BUTTON_TEXT = "Surf";
}

class Button : public sf::Drawable {
 public:
  Button(float x, float y, float radius);
  ~Button();

  void ChangeState();

  void Update();

  void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

 private:
  sf::CircleShape shape_;
  sf::CircleShape layer1_;
  sf::CircleShape layer2_;

  bool is_pressed_;
  int change_cnt_;
  float dy_;

  std::unique_ptr<sf::Font> font_;
  sf::Text label_;

  sf::Clock clock_;
};

#endif  // SHASHMAQAMFINDER_BUTTON_HPP_