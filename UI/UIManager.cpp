#include "UIManager.hpp"
#include "UIConstants.hpp"
#include <iostream>

using namespace ui_constants;

#define FILENAME "RecordedData.wav"
#define DBNAME "Database.db"
#define RECORD_DURATION 10

AudioIdentifier::AudioIdentifier(int width, int height) : recording_(false), db_(DBNAME) {
  LOG_INFO("Runnig UI");
  InitWindow(width, height);
  LOG_INFO("Window initialized");
  InitFont();
  LOG_INFO("Font initialized");
  InitUI();
  LOG_INFO("UI initialized");
}

void AudioIdentifier::InitWindow(int width, int height) {
  window_.create(sf::VideoMode(kMainWindowWidth, kMainWindowHeight),
                 MAIN_WINDOW_NAME, sf::Style::Close);
}

void AudioIdentifier::InitFont() {
  font_ = std::make_unique<sf::Font>();
  if (font_->loadFromFile(FONT_PATH)) {
    LOG_DEBUG("Font loaded");
  } else {
    LOG_ERROR("Failed to load font");
    window_.close();
  }
}

void AudioIdentifier::InitUI() {
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

void AudioIdentifier::Run() {
  while (window_.isOpen()) {
    HandleEvents();
    Update();
    Render();
  }
}

void AudioIdentifier::HandleEvents() {
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

void AudioIdentifier::HandleKeyPress(sf::Keyboard::Key key) {
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

void AudioIdentifier::HandleEnterKeyPress() {
  if (!recording_) {
    shape_->ChangeState();
    recorder_.start();
    timer_.restart();
    recording_ = true;
    LOG_INFO("Recording started.");
  }
}

void AudioIdentifier::Update() {
  shape_->Update();
  if (recording_ && timer_.getElapsedTime().asSeconds() > RECORD_DURATION) {
    FinishRecording();
    HandleRecordedData();
  }
}

void AudioIdentifier::FinishRecording() {
  recorder_.stop();
  recording_ = false;
  LOG_INFO("Done recording.");
  // Get the recorded sound buffer
  const sf::SoundBuffer& buffer = recorder_.getBuffer();

  // Save the recorded audio to a WAV file
  if (buffer.saveToFile(FILENAME)) {
      std::cout << "Audio recorded and saved successfully." << std::endl;
  } else {
      std::cerr << "Failed to save recorded audio." << std::endl;
  }
  shape_->ChangeState();
}

void AudioIdentifier::HandleRecordedData() {
  LOG_INFO("HandleRecordedData:");
  AudioConverter converter(FILENAME);
  LOG_INFO("Converted:");
  auto spectrogram = GenerateSpectrogram(converter.GetData());
  LOG_INFO("Generated Spectrogram:");
  auto hashes = GenerateFingerPrints(spectrogram);
  LOG_INFO("Generated hashes:");
  auto match = db_.getBestMatch(hashes);
  std::string artist = "Unknown artist";
  std::string title = "Unknown";
  if (match != std::nullopt) {
    artist = match->getArtist();
    title = match->getTitle();
    LOG_INFO("Best Match :", artist, title);
  } else {
    LOG_WARNING("No match found");
  }
  data_->SetData(artist, title);
  data_->ChangeState();
}

void AudioIdentifier::Render() {
  window_.clear(sf::Color::White);
  window_.draw(*shape_);
  window_.draw(label_);
  window_.draw(*data_);
  window_.display();
}