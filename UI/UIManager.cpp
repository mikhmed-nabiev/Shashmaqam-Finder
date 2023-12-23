#include "UIManager.hpp"
#include "UIConstants.hpp"
#include <iostream>

using namespace ui_constants;

ShazamUI::ShazamUI(int width, int height) {
  LOG_INFO("Runnig UI");
  InitWindow(width, height);
  LOG_INFO("Window initialized");
  InitFont();
  LOG_INFO("Font initialized");
  InitUI();
  LOG_INFO("UI initialized");
}

void ShazamUI::InitWindow(int width, int height) {
  window_.create(sf::VideoMode(kMainWindowWidth, kMainWindowHeight),
                 MAIN_WINDOW_NAME, sf::Style::Close);
}

void ShazamUI::InitFont() {
  font_ = std::make_unique<sf::Font>();
  if (font_->loadFromFile(FONT_PATH)) {
    LOG_DEBUG("Font loaded");
  } else {
    LOG_ERROR("Failed to load font");
    window_.close();
  }
}

void ShazamUI::InitUI() {
  label_.setFont(*font_.get());
  label_.setString(APP_NAME);
  label_.setFillColor(sf::Color::Black);
  label_.setCharacterSize(24);
  label_.setOrigin(label_.getLocalBounds().width / 2,
                   label_.getLocalBounds().height / 2);
  label_.setPosition(window_.getSize().x / 2, 50.f);
  data_ = std::make_unique<PopUp>(window_.getSize().x, kPopUpWidth,
                                  window_.getSize().y);
  shape_ = std::make_unique<Button>(window_.getSize().x / 2,
                                    window_.getSize().y / 2, kButtonRadius);
}

void ShazamUI::Run() {
  while (window_.isOpen()) {
    HandleEvents();
    Update();
    Render();
  }
}

void ShazamUI::HandleEvents() {
  sf::Event event;
  while (window_.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window_.close();
      LOG_INFO("Window closed");
    } else if (event.type == sf::Event::KeyPressed) {
      HandleKeyPress(event.key.code);
    }
  }
}

void ShazamUI::HandleKeyPress(sf::Keyboard::Key key) {
  switch (key) {
    case sf::Keyboard::Enter:
      HandleEnterKeyPress();
      LOG_INFO("Pressed enter key");
      break;
    case sf::Keyboard::Space:
      data_->ChangeState();
      LOG_INFO("Pressed space key");
      break;
    default:
      break;
  }
}

void ShazamUI::HandleEnterKeyPress() {
  // if (!recorder.IsRecording()) {
  shape_->ChangeState();
  recording_ = true;
  LOG_INFO("Recording started.");
  // }
}

void ShazamUI::Update() {
  shape_->Update();
  // if (recording) {
  //   handleRecording();
  // }
}

void ShazamUI::HandleRecording() {
  // recorder.StartRecording(1);
  // std::pair<std::string, std::string> matches =
  // GetMatches(recorder.GetData());
  data_->SetData("Unknow", "Unknown");
  // recording = false;
  shape_->ChangeState();
  data_->ChangeState();
}

void ShazamUI::Render() {
  window_.clear(sf::Color::White);
  window_.draw(*shape_);
  window_.draw(label_);
  window_.draw(*data_);
  window_.display();
}