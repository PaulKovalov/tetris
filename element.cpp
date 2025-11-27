#include "element.hpp"
#include <vector>

Element::Element(std::vector<std::pair<int, int>> el, int type, int pos_x, int pos_y) {
    this->el = el;
    this->type = type;
    texture.loadFromFile("img/el" + std::to_string(type) + ".jpg");
    sprite = new sf::Sprite(texture);
    this->pos_x = pos_x;
    this->pos_y = pos_y;
}
