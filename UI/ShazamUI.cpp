#include "ShazamUI.hpp"

#include <iostream>

ShazamUI::ShazamUI(int width, int height) {
  InitWindow(width, height);
  InitFont();
  InitUI();
}

void ShazamUI::InitWindow(int width, int height) {
  window_.create(sf::VideoMode(400, 700), "ShazamUI", sf::Style::Close);

}

void ShazamUI::InitFont() {
  std::cout << window_.isOpen() << '\n';
  font_ = std::make_unique<sf::Font>();
  if (font_->loadFromFile(FONT_PATH)) {
    std::cout << "Font loaded\n";
  } else {
    std::cerr << "Failed to load font\n";
    window_.close();
  }
}

void ShazamUI::InitUI() {
  label_.setFont(*font_.get());
  label_.setString("Music Finder");
  label_.setFillColor(sf::Color::Black);
  label_.setCharacterSize(24);
  label_.setOrigin(label_.getLocalBounds().width / 2,
                   label_.getLocalBounds().height / 2);
  label_.setPosition(window_.getSize().x / 2, 50.f);
  data_ = std::make_unique<PopUp>(window_.getSize().x, 200, window_.getSize().y);
  shape_ = std::make_unique<Button>(window_.getSize().x / 2,
                                    window_.getSize().y / 2, 100.f);
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
    } else if (event.type == sf::Event::KeyPressed) {
      HandleKeyPress(event.key.code);
    }
  }
}

void ShazamUI::HandleKeyPress(sf::Keyboard::Key key) {
  switch (key) {
    case sf::Keyboard::Enter:
      HandleEnterKeyPress();
      break;
    case sf::Keyboard::Space:
      data_->ChangeState();
      break;
    default:
      break;
  }
}

void ShazamUI::HandleEnterKeyPress() {
  // if (!recorder.IsRecording()) {
    shape_->ChangeState();
    recording_ = true;
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
  // std::pair<std::string, std::string> matches = GetMatches(recorder.GetData());
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