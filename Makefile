main: main.cpp board.hpp element.hpp
	g++ main.cpp -o main -std=c++17 -Wall -lsfml-system -lsfml-graphics -lsfml-window
