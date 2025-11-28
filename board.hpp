#pragma once

#define SFML_NO_DEPRECATED_WARNINGS

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

#include "element.hpp"

using namespace std;
using namespace sf;

class Game {
    // Coordinates array.
    vector<vector<int>> board;
    // Pointer to the element currently controlled by player.
    std::unique_ptr<Element> current_element;
    RenderWindow *window;
    // Font and text objects for scores and messages.
    Font score_font;
    std::unique_ptr<Text> score_text;
    Texture preview_texture, won_texture, lost_texture;

    const static int BOARD_HEIGHT = 14;
    const static int BOARD_WIDTH = 10;

    int current_score, next_element, best_score;

    std::unique_ptr<Sprite> preview_sprite, final_sprite;

    // Textures used to draw preview elements.
    vector<Texture> preview_textures;
    // Textures used to draw elements on the board.
    vector<Texture> textures;
    // Performs clockwise rotation of the current element, without checking if
    // this is a valid move.
    void _rotate(Element*);
    // Moves the element 1 position left, without checking if this is a
    // valid move.
    void _left(Element*);
    // Moves the element 1 position right, without checking if this is a
    // valid move.
    void _right(Element*);
    // Moves the element 1 position down, without checking if this is a
    // valid move.
    void _down(Element*);
    // Checks if the position of the element is valid, e.g. is withing the
    // boundaries of the board and is not intersecting with any other element
    // on the board.
    bool _valid_position(const Element*);

   public:
    bool is_final = false;

    // Initialized GUI, loads record data.
    Game(RenderWindow *window);

    // Makes the next iteration of the game loop, which is either move current element down, or generate
    // a new one.
    void next();

    // Checks if the game is won or lost, updates final sprite accordingly.
    void check_end_of_game();

    // Removes filled rows.
    void eliminate_rows();

    // Checks & moves the current element, if the move is possible.
    void left();
    
    // Checks & moves the current element, if the move is possible.
    void right();

    void rotate();

    // Draws all board with all elements present on it.
    void draw();

    // Click handler for "play again" and "exit" buttons.
    void mouse_clicked(int x, int y);

    // Initializes game variables. Generates initial element.
    void start_game();

    // Updates current element to the next element.
    void update_current_element();

    void create_next_element();

    int get_score();

    // Saves current score to record.txt file if current score
    // is higher than record.
    void write_record();
};
