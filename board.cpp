#include "board.hpp"

#include "element.hpp"

#include <SFML/Graphics.hpp>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

Game::Game(sf::RenderWindow *window) {
    init_gui(window);
    load_record();
}

void Game::init_gui(sf::RenderWindow *window) {
    this->window = window;
    font.loadFromFile("comic_sans.ttf");
    score_text.setFont(font);
    score_text.setCharacterSize(28);
    score_text.setPosition(540, 400);
    score_text.setColor(sf::Color::Blue);
    preview_sprite.setPosition(540, 85);
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

void Game::left() {
    // Checks if it is possible to move to the left.
    bool check_left = true;
    for (size_t i = 0; i < current_element->el.size(); ++i) {
        if (current_element->el[i].second - 1 < 0)
            check_left = false;
        if (board[current_element->el[i].first][current_element->el[i].second - 1]) {
            check_left = false;
        }
    }

    if (check_left) {
        for (size_t i = 0; i < current_element->el.size(); ++i) {
            current_element->el[i].second--;
        }
        current_element->pos_x--;
    }
}

void Game::right() {
    // Checks if it is possible to move to the right.
    bool check_right = true;
    for (size_t i = 0; i < current_element->el.size(); ++i) {
        if (current_element->el[i].second + 1 > 9)
            check_right = false;
        if (board[current_element->el[i].first][current_element->el[i].second + 1]) {
            check_right = false;
        }
    }
    if (check_right) {
        for (size_t i = 0; i < current_element->el.size(); ++i) {
            current_element->el[i].second++;
        }
        current_element->pos_x++;
    }
}

void Game::rotate() {
    Element *tmp = new Element(current_element->el, current_element->type, current_element->pos_x, current_element->pos_y);
    bool check_rotate = true;
    for (size_t i = 0; i < tmp->el.size(); ++i) {
        tmp->el[i].second -= tmp->pos_x;
        tmp->el[i].first -= tmp->pos_y;
        int t = tmp->el[i].second;
        tmp->el[i].second = tmp->el[i].first;
        tmp->el[i].first = 2 - t;  //[i][j] = e[j][2 - i];
        tmp->el[i].second += tmp->pos_x;
        tmp->el[i].first += tmp->pos_y;
        if (tmp->el[i].first > 13 || tmp->el[i].first < 0 || tmp->el[i].second > 9 || tmp->el[i].second < 0)
            check_rotate = false;
        if (!check_rotate && board[tmp->el[i].first][tmp->el[i].second])
            check_rotate = false;
    }
    delete tmp;
    if (check_rotate) {
        for (size_t i = 0; i < current_element->el.size(); ++i) {
            current_element->el[i].second -= current_element->pos_x;
            current_element->el[i].first -= current_element->pos_y;
            int t = current_element->el[i].second;
            current_element->el[i].second = current_element->el[i].first;
            current_element->el[i].first = 2 - t;  //[i][j] = e[j][2 - i];
            current_element->el[i].second += current_element->pos_x;
            current_element->el[i].first += current_element->pos_y;
        }
    }
}

void Game::draw() {
    // If the game has ended, depending on the player's state
    // either winning or losing sprite will be drawn.
    if (is_final) {
        window->draw(final_sprite);
    }
    // Game has not ended yet case.
    else {
        for (size_t i = 0; i < current_element->el.size(); ++i) {
            int x = current_element->el[i].second;
            int y = current_element->el[i].first;
            x *= 50;
            y *= 50;
            current_element->sprite.setPosition(Vector2f(x, y));
            window->draw(current_element->sprite);
        }
        string score = to_string(current_score);
        if (current_score > best_score) {
            score = to_string(current_score) + "\nNew high score!";
        }
        score_text.setString(score);
        preview_texture.loadFromFile("img/preview_" + to_string(next_element) + ".png");
        preview_sprite.setTexture(preview_texture, true);
        for (int i = 0; i < 14; ++i) {
            for (int j = 0; j < 10; ++j) {
                if (board[i][j]) {
                    int x = j * 50;
                    int y = i * 50;
                    texture.loadFromFile("img/el" + to_string(board[i][j]) + ".jpg");
                    sprite.setTexture(texture);
                    sprite.setPosition(Vector2f(x, y));
                    window->draw(sprite);
                }
            }
        }
        window->draw(score_text);
        window->draw(preview_sprite);
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
    final_sprite.setTexture(final_texture);
}
