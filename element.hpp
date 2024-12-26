#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// Element is an array of 2D points representing a one single square
// of an element, a texture and a sprite.
class Element {
   public:
    std::vector<std::pair<int, int>> el;
    sf::Texture texture;
    sf::Sprite sprite;
    int type;
    int pos_x, pos_y;
    Element(std::vector<std::pair<int, int>> el, int type, int pos_x, int pos_y);
};
