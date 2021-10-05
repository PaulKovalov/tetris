#pragma once
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <vector>
using namespace std;
using namespace sf;

// Element is an array of 2D points representing a one single square
// of an element, a texture and a sprite.
class Element {
   public:
    vector<pair<int, int>> el;
    Texture texture;
    Sprite sprite;
    int type;
    int pos_x, pos_y;
    Element(vector<pair<int, int>> el, int type, int pos_x, int pos_y) {
        this->el = el;
        this->type = type;
        texture.loadFromFile("img/el" + to_string(type) + ".jpg");
        sprite.setTexture(texture);
        this->pos_x = pos_x;
        this->pos_y = pos_y;
    }
};
