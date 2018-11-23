#pragma once
#include "element.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>

using namespace std;
using namespace sf;

class Game
{
    vector<vector<int>> board;
    Element *current_element;
    RenderWindow *window;
    int current_score = 0, next_element;
    int best_score;
    Font font;
    Text score_text;
    Texture texture, preview_texture, final_texture;
    Sprite sprite, preview_sprite, final_sprite;
    
  public:
    bool is_final = false;
    Game(RenderWindow *window)
    {
        ifstream fin("record.txt");
        fin >> best_score;
        fin.close();
        this->window = window;
        srand(time(NULL));
        for (int i = 0; i < 14; i++)
        {
            vector<int> tmp;
            for (int j = 0; j < 10; j++)
            {
                tmp.push_back(0);
            }
            board.push_back(tmp);
        }
        font.loadFromFile("comic_sans.ttf");
        score_text.setFont(font);
        score_text.setCharacterSize(28);
        score_text.setPosition(540, 400);
        score_text.setColor(Color::Blue);
        next_element = rand() % 6;
        preview_sprite.setPosition(540, 85);
    }
    void start()
    {
        current_element = generate_element();
    }
    void next()
    {
        bool check_down = true;
        bool check_line = true;
        //check if there is an element below
        //check if element is on the bottom
        for (int i = 0; i < current_element->el.size(); i++)
        {
            if (current_element->el[i].first < 13)
            {
                if (board[current_element->el[i].first + 1][current_element->el[i].second])
                    check_down = false;
            }
            else
            {
                check_down = false;
            }
        }
        //if there's no elements below, just keep moving
        if (check_down)
        {
            for (int i = 0; i < current_element->el.size(); i++)
            {
                current_element->el[i].first++;
            }
            current_element->pos_y++;
        }
        else
        {
            for (int i = 0; i < current_element->el.size(); i++)
            {
                board[current_element->el[i].first][current_element->el[i].second] = current_element->type;
            }
            delete current_element;
            current_element = generate_element();
            check_win();
        }
    }
    void check_win()
    {
        bool check_row;
        int count_blocks = 0;
        //checks row by row the matrix
        //if finds a row which is full removes it
        for (int i = 0; i < 14; i++)
        {
            check_row = true;
            for (int j = 0; j < 10; j++)
            {
                if (i == 0 && board[i][j])
                {
                    delete current_element;
                    final_texture.loadFromFile("img/lost.png");
                    final_sprite.setTexture(final_texture);
                    is_final = true;
                    save();
                    return;
                }
                if (!board[i][j])
                    check_row = false;
                else
                {
                    count_blocks++;
                }
            }
            //if user won
            if (count_blocks == current_element->el.size())
            {
                final_texture.loadFromFile("img/win.png");
                final_sprite.setTexture(final_texture);
                save();
            }
            //found a row
            if (check_row)
            {
                current_score += 100;
                for (int k = 0; k < 10; k++)
                {
                    board[i][k] = 0;
                }
                //shifts all elements above row number i down
                move_down(i);
                check_win();
            }
        }
    }
    void move_down(int i)
    {
        for (int k = i; k >= 0; k--)
        {
            for (int t = 9; t >= 0; t--)
            {
                if (board[k][t])
                {
                    //while cell below is 0, move current and write 0 to the current
                    while (!board[k + 1][t])
                    {
                        board[k + 1][t] = board[k][t];
                        board[k][t] = 0;
                    }
                }
            }
        }
    }
    void left()
    {
        bool check_left = true;
        for (int i = 0; i < current_element->el.size(); i++)
        {
            if (current_element->el[i].second - 1 < 0)
                check_left = false;
            if (board[current_element->el[i].first][current_element->el[i].second - 1])
            {
                check_left = false;
            }
        }
        if (check_left)
        {
            for (int i = 0; i < current_element->el.size(); i++)
            {
                current_element->el[i].second--;
            }
            current_element->pos_x--;
        }
    }
    void right()
    {
        bool check_right = true;
        for (int i = 0; i < current_element->el.size(); i++)
        {
            if (current_element->el[i].second + 1 > 9)
                check_right = false;
            if (board[current_element->el[i].first][current_element->el[i].second + 1])
            {
                check_right = false;
            }
        }
        if (check_right)
        {
            for (int i = 0; i < current_element->el.size(); i++)
            {
                current_element->el[i].second++;
            }
            current_element->pos_x++;
        }
    }
    void rotate()
    {
        Element *tmp = new Element(current_element->el, current_element->type, current_element->pos_x, current_element->pos_y);
        bool check_rotate = true;
        for (int i = 0; i < tmp->el.size(); i++)
        {
            tmp->el[i].second -= tmp->pos_x;
            tmp->el[i].first -= tmp->pos_y;
            int t = tmp->el[i].second;
            tmp->el[i].second = tmp->el[i].first;
            tmp->el[i].first = 2 - t; //[i][j] = e[j][2 - i];
            tmp->el[i].second += tmp->pos_x;
            tmp->el[i].first += tmp->pos_y;
            if (tmp->el[i].first > 13 || tmp->el[i].first < 0 || tmp->el[i].second > 9 || tmp->el[i].second < 0)
                check_rotate = false;
            if (!check_rotate && board[tmp->el[i].first][tmp->el[i].second])
                check_rotate = false;
        }
        delete tmp;
        if (check_rotate)
        {
            for (int i = 0; i < current_element->el.size(); i++)
            {
                current_element->el[i].second -= current_element->pos_x;
                current_element->el[i].first -= current_element->pos_y;
                int t = current_element->el[i].second;
                current_element->el[i].second = current_element->el[i].first;
                current_element->el[i].first = 2 - t; //[i][j] = e[j][2 - i];
                current_element->el[i].second += current_element->pos_x;
                current_element->el[i].first += current_element->pos_y;
            }
        }
    }
    void draw()
    {
        if (is_final)
        {
            window->draw(final_sprite);
        }
        else
        {
            for (int i = 0; i < current_element->el.size(); i++)
            {
                int x = current_element->el[i].second;
                int y = current_element->el[i].first;
                x *= 50;
                y *= 50;
                current_element->sprite.setPosition(Vector2f(x, y));
                window->draw(current_element->sprite);
            }
            string score = to_string(current_score);
            if (current_score > best_score)
            {
                score = to_string(current_score) + "\nNew high score!";
            }
            score_text.setString(score);
            preview_texture.loadFromFile("img/preview_" + to_string(next_element + 1) + ".png");
            preview_sprite.setTexture(preview_texture, true);
            for (int i = 0; i < 14; i++)
            {
                for (int j = 0; j < 10; j++)
                {
                    if (board[i][j])
                    {
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
    void mouse_clicked(int x, int y)
    {
        if (x > 50 && y > 400 && x < 280 && y < 500)
        {
            reset_game();
        }
        else if ((x > 420 && y > 400 && x < 660 && y < 500))
        {
            window->close();
            exit(0);
        }
    }
    void reset_game()
    {
        srand(time(NULL));
        vector<vector<int>>().swap(board);
        ifstream fin("record.txt");
        fin >> best_score;
        fin.close();
        for (int i = 0; i < 14; i++)
        {
            vector<int> tmp;
            for (int j = 0; j < 10; j++)
            {
                tmp.push_back(0);
            }
            board.push_back(tmp);
        }
        current_score = 0;
        is_final = false;
        start();
        next();
    }
    Element *generate_element()
    {
        int pos_x = rand() % 10;
        int element_type = next_element;
        vector<pair<int, int>> element;
        if ((pos_x + 3) > 9)
        {
            pos_x -= 3;
        }
        element_type++;
        switch (element_type)
        {
        case 1:
        {
            element.push_back(make_pair(0, pos_x));
            element.push_back(make_pair(0, 1 + pos_x));
            element.push_back(make_pair(1, 1 + pos_x));
            element.push_back(make_pair(1, pos_x));
            break;
        }
        case 2:
        {
            element.push_back(make_pair(0, pos_x + 1));
            element.push_back(make_pair(1, pos_x));
            element.push_back(make_pair(1, pos_x + 1));
            element.push_back(make_pair(1, pos_x + 2));
            break;
        }
        case 3:
        {
            element.push_back(make_pair(0, pos_x + 2));
            element.push_back(make_pair(1, pos_x));
            element.push_back(make_pair(1, pos_x + 1));
            element.push_back(make_pair(1, pos_x + 2));
            break;
        }
        case 4:
        {
            element.push_back(make_pair(0, pos_x));
            element.push_back(make_pair(1, pos_x));
            element.push_back(make_pair(2, pos_x));
            element.push_back(make_pair(3, pos_x));
            break;
        }
        case 5:
        {
            element.push_back(make_pair(0, pos_x + 1));
            element.push_back(make_pair(1, pos_x));
            element.push_back(make_pair(1, pos_x + 1));
            element.push_back(make_pair(2, pos_x));
            break;
        }
        case 6:
        {
            element.push_back(make_pair(0, pos_x));
            element.push_back(make_pair(1, pos_x));
            element.push_back(make_pair(1, pos_x + 1));
            element.push_back(make_pair(1, pos_x + 2));
            break;
        }
        }
        next_element = rand() % 6;
        return new Element(element, element_type, pos_x, 0);
    }

    int get_score(){
        return current_score;
    }
    void save()
    {
        if (current_score > best_score)
        {
            ofstream fout("record.txt");
            fout << current_score;
            fout.close();
        }
    }
};
