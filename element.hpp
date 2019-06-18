#pragma once
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

/**
 * Каждый элемент представляет собой массив квадратных элементов, 
 * у каждого есть свои координаты. Каждый такой квадратный элемент представлен 
 * общим для фигуры спрайтом и текстурой
 * 
 * У каждой фигуры существует индекс, определяющий ее тип (форму).
 * 
 * **/
class Element
{
    public:
    vector<pair<int, int>> el;
    Texture texture;
    Sprite sprite;
    int type;
    int pos_x, pos_y; // координаты спрайта на доске
    Element(vector<pair<int, int>> el, int type, int pos_x, int pos_y)
    {
        this->el = el;
        this->type = type;
        texture.loadFromFile("img/el"+to_string(type)+".jpg");
        sprite.setTexture(texture);
        this->pos_x=pos_x;
        this->pos_y=pos_y;

    }  
};