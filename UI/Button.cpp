#include "Button.hpp"

#include <iostream>

using namespace ButtonConstants;

Button::Button(float x, float y, float radius)
    : shape_(radius),
      layer1_(radius * LAYER1_SCALE),
      layer2_(radius * LAYER2_SCALE),
      is_pressed_(false),
      change_cnt_(0),
      dy_(RADIUS_DELTA) {
  shape_.setPosition(x - radius, y - radius);
  shape_.setFillColor(BUTTON_COLOR);

  layer1_.setPosition(x - radius * LAYER1_SCALE, y - radius * LAYER1_SCALE);
  layer1_.setFillColor(LAYER_COLOR);

  layer2_.setPosition(x - radius * LAYER2_SCALE, y - radius * LAYER2_SCALE);
  layer2_.setFillColor(LAYER_COLOR);

  font_ = std::make_unique<sf::Font>();
  if (!font_->loadFromFile(FONT_PATH)) {
    std::cout << "Couldn't load font\n";
  }

  label_.setFont(*font_.get());
  label_.setFillColor(sf::Color::Black);

  label_.setString(BUTTON_TEXT);
  label_.setPosition(x - label_.getLocalBounds().width / 2.f,
                     y - label_.getLocalBounds().height);
  label_.setCharacterSize(40);
}

Button::~Button() {}

void Button::ChangeState() {
  is_pressed_ = !is_pressed_;
  clock_.restart();
}

void Button::Update() {
  if (is_pressed_) {
    if (change_cnt_ == 7000) {
        dy_ *= -1;
        change_cnt_ = 0;
    }
    float dy2 = dy_ / 2;
    float x1 = layer1_.getPosition().x, y1 = layer1_.getPosition().y;
    float x2 = layer2_.getPosition().x, y2 = layer2_.getPosition().y;
    float radius1 = layer1_.getRadius(), radius2 = layer2_.getRadius();
    radius1 += dy_;
    radius2 += dy2;

    x1 -= dy_; x2 -= dy2;
    y1 -= dy_; y2 -= dy2;

    layer1_.setRadius(radius1);
    layer1_.setPosition(x1, y1);

    layer2_.setRadius(radius2);
    layer2_.setPosition(x2, y2);

    change_cnt_++;
  }
  if (clock_.getElapsedTime().asSeconds() >= ANIMATION_TIME) {
    is_pressed_ = false;
    clock_.restart();
  }
}

void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  if (is_pressed_) {
    target.draw(layer1_);
    target.draw(layer2_);
  }
  target.draw(shape_);
  target.draw(label_);
}