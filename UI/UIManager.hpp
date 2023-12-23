#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include <SFML/Graphics.hpp>
#include "Button.hpp"
#include "PopUp.hpp"
#include "Logger.hpp"

class ShazamUI {
public:
    ShazamUI(int width = 400, int height = 700);
    void Run();

private:
    sf::RenderWindow window_;
    sf::Text label_;
    std::unique_ptr<PopUp> data_;
    std::unique_ptr<Button> shape_;

    // AudioRecorder recorder_;
    bool recording_ = false;

    void InitWindow(int width, int height);
    void InitFont();
    void InitUI();
    void HandleEvents();
    void HandleKeyPress(sf::Keyboard::Key key);
    void HandleEnterKeyPress();
    void Update();
    void HandleRecording();
    void Render();

    std::unique_ptr<sf::Font> font_;
};

#endif // UIMANAGER_HPP
