main: main.cpp board.o element.o
	g++ main.cpp board.o element.o -o main -std=c++17 -Wall -lsfml-system -lsfml-graphics -lsfml-window

board.o: board.hpp board.cpp element.hpp
	g++ board.cpp -c -Wall -std=c++17 -lsfml-system -lsfml-graphics -lsfml-window

element.o: element.hpp element.cpp
	g++ element.cpp -c -Wall -std=c++17 -lsfml-system -lsfml-graphics -lsfml-window

clean:
	rm *.o;
