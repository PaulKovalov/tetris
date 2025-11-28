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
    this->window = window;

    for (int preview_texture_id = 1; preview_texture_id < 7; ++preview_texture_id) {
        preview_textures.push_back(Texture("img/preview_" + to_string(preview_texture_id) + ".png"));
    }
    for (int element_texture_id = 1; element_texture_id < 7; ++element_texture_id) {
        textures.push_back(Texture("img/el" + to_string(element_texture_id) + ".jpg"));
    }

    next_element = (rand() % 6) + 1;
    preview_sprite = make_unique<Sprite>(preview_textures[next_element - 1]);
    preview_sprite->setPosition({540, 85});

    score_font.openFromFile("comic_sans.ttf");
    score_text = make_unique<sf::Text>(score_font, "", 28);
    score_text->setPosition({540, 400});
    score_text->setFillColor(sf::Color::Blue);

    won_texture.loadFromFile("img/win.png");
    lost_texture.loadFromFile("img/lost.png");

    ifstream fin("record.txt");
    fin >> best_score;
    fin.close();

}

void Game::_down(Element* element) {
    for (size_t i = 0; i < element->el.size(); ++i) {
        element->el[i].first++;
    }
    element->pos_y++;
}

void Game::next() {
    std::unique_ptr<Element> tmp = make_unique<Element>(current_element->el, current_element->type, current_element->pos_x, current_element->pos_y);
    _down(tmp.get());
    // If there are no elements below, just keep moving.
    if (_valid_position(tmp.get())) {
        _down(current_element.get());
    } else {
        for (size_t i = 0; i < current_element->el.size(); ++i) {
            board[current_element->el[i].first][current_element->el[i].second] = current_element->type;
        }
        update_current_element();
        create_next_element();
        eliminate_rows();
        check_end_of_game();
    }
}

void Game::check_end_of_game() {
    bool last_row_empty = true;
    for (int j = 0; j < BOARD_WIDTH; ++j) {
        if (board[BOARD_HEIGHT - 1][j])
            last_row_empty = false;
    }
    // If the last row is empty and current score is not 0, then the game is won
    if (last_row_empty && current_score > 0) {
        final_sprite = make_unique<Sprite>(won_texture);
        is_final = true;
        write_record();
    }
    // Alternatively, check if the top-most row has a permanent element
    for (int i = 0; i < BOARD_WIDTH; ++i) {
        if (board[0][i]) {
            final_sprite = make_unique<Sprite>(lost_texture);
            is_final = true;
            write_record();
            return;
        }
    }
}

void Game::eliminate_rows() {
    vector<int> rows;
    for (int i = 0; i < BOARD_HEIGHT; ++i) {
        bool row_full = true;
        for (int j = 0; j < BOARD_WIDTH; ++j) {
            if (board[i][j] == 0)
                row_full = false;
        }
        if (row_full)
            rows.push_back(i);
    }
    for (const int &r : rows) {
        for (int k = r; k >= 1; --k) {
            for (int t = 0; t <= BOARD_WIDTH; ++t) {
                // Overwrite current row with the row from above.
                board[k][t] = board[k - 1][t];
            }
        }
    }
}

bool Game::_valid_position(const Element *element) {
    for (size_t i = 0; i < element->el.size(); ++i) {
        if (element->el[i].first > BOARD_HEIGHT - 1 || element->el[i].first < 0 || element->el[i].second > BOARD_WIDTH - 1 || element->el[i].second < 0)
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
        _left(current_element.get());
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
        _right(current_element.get());
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
        _rotate(current_element.get());
    }
}

void Game::draw() {
    if (is_final) {
        window->draw(*final_sprite);
    } else {
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
        for (int i = 0; i < BOARD_HEIGHT; ++i) {
            for (int j = 0; j < BOARD_WIDTH; ++j) {
                if (board[i][j]) {
                    int x = j * 50;
                    int y = i * 50;
                    Sprite s(textures[board[i][j] - 1]);
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
    board.resize(BOARD_HEIGHT, vector<int>(BOARD_WIDTH, 0));
    current_score = 0;
    update_current_element();
    create_next_element();
}

void Game::update_current_element() {
    // Generate random X position of the element.
    int pos_x = rand() % 7;
    vector<pair<int, int>> element;
    switch (next_element) {
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
    current_element = make_unique<Element>(element, next_element, pos_x, 0);
}

void Game::create_next_element() {
    next_element = (rand() % 6) + 1;
    preview_sprite->setTexture(preview_textures[next_element - 1], true);
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
