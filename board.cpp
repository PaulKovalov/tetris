#include "board.hpp"

#include "element.hpp"

#include <SFML/Graphics.hpp>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include <memory>

using namespace std;

Game::Game(sf::RenderWindow *window) {
    init_gui(window);
    load_record();
}

void Game::init_gui(sf::RenderWindow *window) {
    this->window = window;
    font.openFromFile("comic_sans.ttf");
    score_text = new sf::Text(font, "", 28);
    score_text->setPosition({540, 400});
    score_text->setFillColor(sf::Color::Blue);
    preview_sprite = new sf::Sprite(preview_texture);
    preview_sprite->setPosition({540, 85});
    final_sprite = new sf::Sprite(final_texture);
}

void Game::next() {
    bool check_down = true;
    // Check if element can move down.
    for (size_t i = 0; i < current_element->el.size(); ++i) {
        if (current_element->el[i].first < 13) {
            if (board[current_element->el[i].first + 1][current_element->el[i].second]) {
                check_down = false;
                break;
            }
        } else {
            check_down = false;
            break;
        }
    }
    // If there are no elements below, just keep moving.
    if (check_down) {
        for (size_t i = 0; i < current_element->el.size(); ++i) {
            current_element->el[i].first++;
        }
        current_element->pos_y++;
    } else {
        for (size_t i = 0; i < current_element->el.size(); ++i) {
            board[current_element->el[i].first][current_element->el[i].second] = current_element->type;
        }
        delete current_element;
        current_element = generate_element();
        check_win();
    }
}

void Game::check_win() {
    bool check_row;
    size_t count_blocks = 0;

    for (int i = 0; i < 14; ++i) {
        check_row = true;
        for (int j = 0; j < 10; ++j) {
            if (i == 0 && board[i][j]) {
                delete current_element;
                set_final_sprite("img/lost.png");
                is_final = true;
                write_record();
                return;
            }
            if (!board[i][j])
                check_row = false;
            else {
                count_blocks++;
            }
        }
        // Win case.
        if (count_blocks == current_element->el.size()) {
            set_final_sprite("img/win.png");
            write_record();
        }
        // Assembled row case.
        if (check_row) {
            current_score += 100;
            for (int k = 0; k < 10; k++) {
                board[i][k] = 0;
            }
            // Shift every element from above further to bottom of the board.
            move_down(i);
            // Call recursively to check if there is another assembled row
            // appeared after shifting.
            check_win();
        }
    }
}

void Game::move_down(int i) {
    for (int k = i; k >= 0; --k) {
        for (int t = 9; t >= 0; --t) {
            if (board[k][t]) {
                // While cell below is 0, move current and write 0 to the current.
                while (!board[k + 1][t]) {
                    board[k + 1][t] = board[k][t];
                    board[k][t] = 0;
                }
            }
        }
    }
}

bool Game::_valid_position(Element *element) {
    for (size_t i = 0; i < element->el.size(); ++i) {
        if (element->el[i].first > BOARD_HEIGHT || element->el[i].first < 0 || element->el[i].second > BOARD_WIDTH || element->el[i].second < 0)
            return false;
        if (board[element->el[i].first][element->el[i].second])
            return false;
    }
    return true;
}

void Game::_left(Element* element) {
    for (size_t i = 0; i < element->el.size(); ++i) {
        element->el[i].second--;
    }
    element->pos_x--;
}

void Game::left() {
    std::unique_ptr<Element> tmp = make_unique<Element>(current_element->el, current_element->type, current_element->pos_x, current_element->pos_y);
    _left(tmp.get());
    if (_valid_position(tmp.get())) {
        _left(current_element);
    }
}

void Game::_right(Element* element) {
    for (size_t i = 0; i < element->el.size(); ++i) {
        element->el[i].second++;
    }
    element->pos_x++;
}

void Game::right() {
    std::unique_ptr<Element> tmp = make_unique<Element>(current_element->el, current_element->type, current_element->pos_x, current_element->pos_y);
    _right(tmp.get());
    if (_valid_position(tmp.get())) {
        _right(current_element);
    }
}


void Game::_rotate(Element* element) {
    for (size_t i = 0; i < element->el.size(); ++i) {
        element->el[i].second -= element->pos_x;
        element->el[i].first -= element->pos_y;
        int t = element->el[i].second;
        element->el[i].second = element->el[i].first;
        element->el[i].first = 2 - t;  //[i][j] = e[j][2 - i];
        element->el[i].second += element->pos_x;
        element->el[i].first += element->pos_y;
    }
}

void Game::rotate() {
    std::unique_ptr<Element> tmp = make_unique<Element>(current_element->el, current_element->type, current_element->pos_x, current_element->pos_y);
    _rotate(tmp.get());
    if (_valid_position(tmp.get())) {
        _rotate(current_element);
    }
}

void Game::draw() {
    // If the game has ended, depending on the player's state
    // either winning or losing sprite will be drawn.
    if (is_final) {
        window->draw(*final_sprite);
    }
    // Game has not ended yet case.
    else {
        for (size_t i = 0; i < current_element->el.size(); ++i) {
            int x = current_element->el[i].second;
            int y = current_element->el[i].first;
            x *= 50;
            y *= 50;
            current_element->sprite->setPosition(Vector2f(x, y));
            window->draw(*current_element->sprite);
        }
        string score = to_string(current_score);
        if (current_score > best_score) {
            score = to_string(current_score) + "\nNew high score!";
        }
        score_text->setString(score);
        preview_texture.loadFromFile("img/preview_" + to_string(next_element) + ".png");
        preview_sprite->setTexture(preview_texture, true);
        for (int i = 0; i < 14; ++i) {
            for (int j = 0; j < 10; ++j) {
                if (board[i][j]) {
                    int x = j * 50;
                    int y = i * 50;
                    Texture t;
                    t.loadFromFile("img/el" + to_string(board[i][j]) + ".jpg");
                    Sprite s(t);
                    s.setPosition(Vector2f(x, y));
                    window->draw(s);
                }
            }
        }
        window->draw(*score_text);
        window->draw(*preview_sprite);
    }
}

void Game::mouse_clicked(int x, int y) {
    if (x > 50 && y > 400 && x < 280 && y < 500) {
        // Play again case.
        start_game();
    } else if ((x > 420 && y > 400 && x < 660 && y < 500)) {
        // Exit game case.
        window->close();
        exit(0);
    }
}

void Game::start_game() {
    srand(time(NULL));
    board.clear();
    board.resize(14, vector<int>(10, 0));
    current_score = 0;
    is_final = false;
    next_element = gen_next_element_id();
    current_element = generate_element();
}

Element* Game::generate_element() {
    // Generate random X position of the element.
    int pos_x = rand() % 7;
    int element_type = next_element;
    vector<pair<int, int>> element;
    switch (element_type) {
        case 1: {
            element.push_back(make_pair(0, pos_x));
            element.push_back(make_pair(0, pos_x + 1));
            element.push_back(make_pair(1, pos_x + 1));
            element.push_back(make_pair(1, pos_x));
            break;
        }
        case 2: {
            element.push_back(make_pair(0, pos_x + 1));
            element.push_back(make_pair(1, pos_x));
            element.push_back(make_pair(1, pos_x + 1));
            element.push_back(make_pair(1, pos_x + 2));
            break;
        }
        case 3: {
            element.push_back(make_pair(0, pos_x + 2));
            element.push_back(make_pair(1, pos_x));
            element.push_back(make_pair(1, pos_x + 1));
            element.push_back(make_pair(1, pos_x + 2));
            break;
        }
        case 4: {
            element.push_back(make_pair(0, pos_x));
            element.push_back(make_pair(1, pos_x));
            element.push_back(make_pair(2, pos_x));
            element.push_back(make_pair(3, pos_x));
            break;
        }
        case 5: {
            element.push_back(make_pair(0, pos_x + 1));
            element.push_back(make_pair(1, pos_x));
            element.push_back(make_pair(1, pos_x + 1));
            element.push_back(make_pair(2, pos_x));
            break;
        }
        case 6: {
            element.push_back(make_pair(0, pos_x));
            element.push_back(make_pair(1, pos_x));
            element.push_back(make_pair(1, pos_x + 1));
            element.push_back(make_pair(1, pos_x + 2));
            break;
        }
    }
    // Create index of the next element to show in preview.
    next_element = gen_next_element_id();
    return new Element(element, element_type, pos_x, 0);
}

int Game::gen_next_element_id() {
    // Ids are indexed from 1, because 0 means empty cell on the board.
    return (rand() % 6) + 1;
}

int Game::get_score() {
    return current_score;
}

void Game::write_record() {
    if (current_score > best_score) {
        ofstream fout("record.txt");
        fout << current_score;
        fout.close();
    }
}

void Game::load_record() {
    ifstream fin("record.txt");
    fin >> best_score;
    fin.close();
}

void Game::set_final_sprite(const string& s) {
    final_texture.loadFromFile(s);
    final_sprite->setTexture(final_texture);
}
