#include <SFML/Graphics.hpp>
#include <iostream>

#include "board.hpp"
#include "element.hpp"
using namespace sf;
using namespace std;

int main() {
    bool is_btn_left_released = true;
    bool is_btn_right_released = true;
    bool is_btn_rotate_released = true;
    // Delay between game loop iterations in ms.
    double time = 300;
    // Regular delay between game loop iterations in ms.
    double time_normal = 300;
    // Decreased delay between game loop iterations in ms.
    double time_fast = 100;
    RenderWindow window(VideoMode(700, 700), "Tetris");
    window.setFramerateLimit(30);
    Game game(&window);
    game.start_game();
    Clock clock;
    Texture back_texture;
    Sprite back_sprite;
    back_texture.loadFromFile("img/background.png");
    back_sprite.setTexture(back_texture);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                window.close();
                game.write_record();
                return 0;
            }
            // When user presses "down" key element falls three times faster.
            if (Keyboard::isKeyPressed(Keyboard::Down)) {
                time = time_fast;
            }
            if (event.type == Event::KeyReleased && event.key.code == Keyboard::Down) {
                time = time_normal;
            }
            if (Keyboard::isKeyPressed(Keyboard::Left)) {
                if (is_btn_left_released) {
                    game.left();
                    is_btn_left_released = false;
                }
            }
            if (event.type == Event::KeyReleased && event.key.code == Keyboard::Left) {
                is_btn_left_released = true;
            }

            if (Keyboard::isKeyPressed(Keyboard::Right)) {
                if (is_btn_right_released) {
                    game.right();
                    is_btn_right_released = false;
                }
            }
            if (event.type == Event::KeyReleased && event.key.code == Keyboard::Right) {
                is_btn_right_released = true;
            }

            if (Keyboard::isKeyPressed(Keyboard::Up)) {
                if (is_btn_rotate_released) {
                    game.rotate();
                    is_btn_rotate_released = false;
                }
            }
            if (event.type == Event::KeyReleased && event.key.code == Keyboard::Up) {
                is_btn_rotate_released = true;
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    game.mouse_clicked(event.mouseButton.x, event.mouseButton.y);
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
