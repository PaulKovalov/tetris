#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>

#include "board.hpp"
#include "element.hpp"

using namespace sf;
using namespace std;

int main() {
    srand(time(NULL));
    bool is_btn_left_released = true;
    bool is_btn_right_released = true;
    bool is_btn_rotate_released = true;
    // Delay between game loop iterations in ms.
    double time = 300;
    // Regular delay between game loop iterations in ms.
    double time_normal = 300;
    // Decreased delay between game loop iterations in ms.
    double time_fast = 100;
    RenderWindow window(VideoMode({700, 700}), "Tetris");
    window.setFramerateLimit(30);
    Game game(&window);
    game.start_game();
    Clock clock;
    Texture back_texture("img/background.png");
    Sprite back_sprite(back_texture);

    while (window.isOpen()) {
        while (optional event = window.pollEvent()) {
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    window.close();
                    game.write_record();
                    return 0;
                }
                // When user presses "down" key element falls three times faster.
                if (keyPressed->scancode == sf::Keyboard::Scancode::Down) {
                    time = time_fast;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Left && is_btn_left_released) {
                    game.left();
                    is_btn_left_released = false;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Right) {
                    if (is_btn_right_released) {
                        game.right();
                        is_btn_right_released = false;
                    }
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Up) {
                    if (is_btn_rotate_released) {
                        game.rotate();
                        is_btn_rotate_released = false;
                    }
                }
            }

            if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                if (keyReleased->scancode == sf::Keyboard::Scancode::Down) {
                    time = time_normal;
                }
                if (keyReleased->scancode == sf::Keyboard::Scancode::Left) {
                    is_btn_left_released = true;
                }
                if (keyReleased->scancode == sf::Keyboard::Scancode::Right) {
                    is_btn_right_released = true;
                }
                if (keyReleased->scancode == sf::Keyboard::Scancode::Up) {
                    is_btn_rotate_released = true;
                }
            }
            
            if (const auto* buttonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (buttonPressed->button == sf::Mouse::Button::Left) {

                    game.mouse_clicked(buttonPressed->position.x, buttonPressed->position.y);
                }
            }
        }

        window.clear();
        window.draw(back_sprite);
        if (clock.getElapsedTime().asMilliseconds() > time) {
            clock.restart();
            if (!game.is_final)
                game.next();
        }
        game.draw();
        window.display();
    }
    return 0;
}
