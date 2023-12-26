#ifndef SHASHMAQAMFINDER_UIMANAGER_HPP
#define SHASHMAQAMFINDER_UIMANAGER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "AudioConverter.hpp"
#include "Database.hpp"
#include "FingerPrint.hpp"
#include "Spectrogram.hpp"
#include "Button.hpp"
#include "PopUp.hpp"
#include "Logger.hpp"

class AudioIdentifier {
public:
    AudioIdentifier(int width = 400, int height = 700);
    void Run();

private:
    sf::RenderWindow window_;
    sf::Text label_;
    std::unique_ptr<PopUp> data_;
    std::unique_ptr<Button> shape_;

    sf::SoundBufferRecorder recorder_;
    sf::Clock timer_;

    DataBase db_;


    // AudioRecorder recorder_;
    bool recording_;

    void InitWindow(int width, int height);
    void InitFont();
    void InitUI();
    void HandleEvents();
    void HandleKeyPress(sf::Keyboard::Key key);
    void HandleEnterKeyPress();
    void Update();
    void FinishRecording();
    void HandleRecordedData();
    void Render();

    std::unique_ptr<sf::Font> font_;
};

#endif // SHASHMAQAMFINDER_UIMANAGER_HPP
