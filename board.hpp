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
    Element *current_element;
    RenderWindow *window;
    int current_score, next_element, best_score;
    // Font and text objects for scores and messages.
    Font font;
    Text *score_text;
    Texture preview_texture, final_texture;
    Sprite *preview_sprite, *final_sprite;

    const static int BOARD_HEIGHT = 13;
    const static int BOARD_WIDTH = 9;
   public:
    bool is_final;

    // Initialized GUI, loads record data.
    Game(RenderWindow *window);

    // Initializes GUI elements.
    void init_gui(RenderWindow *window);

    // Makes the next iteration of the game loop, which is either move current element down, or generate
    // a new one.
    void next();

    // Checks if there are assembled rows. Removes assembled rows,
    // shifts elements laying on the removed rows down. If all rows
    // were removed, sets win flag.
    void check_win();

    // Shifts current element 1 position below.
    void move_down(int i);

    // Checks & moves the current element, if the move is possible.
    void left();

    // Moves the element 1 position left, without checking if this is a
    // valid move.
    void _left(Element*);
    
    // Checks & moves the current element, if the move is possible.
    void right();

    // Moves the element 1 position right, without checking if this is a
    // valid move.
    void _right(Element*);

    void rotate();

    // Performs clockwise rotation of the current element. 
    void _rotate(Element*);

    // Checks if the position of the element is valid, e.g. is withing the
    // boundaries of the board and is not intersecting with any other element
    // on the board.
    bool _valid_position(Element*);

    // Draws all board with all elements present on it.
    void draw();

    // Click handler for "play again" and "exit" buttons.
    void mouse_clicked(int x, int y);

    // Initializes game variables. Generates initial element.
    void start_game();

    // Generates next element, and generates preview for the next element.
    Element *generate_element();

    // Formula for generatin the id of the next element.
    int gen_next_element_id();
    int get_score();

    // Saves current score to record.txt file if current score
    // is higher than record.
    void write_record();

    void load_record();
    
    void set_final_sprite(const string& s);
};
