## Tetris game

![Gameplay](https://github.com/PaulKovalov/tetris/blob/master/tetris.gif)

A very simple and robust implementation of Tetris with C++ and SFML


## Controls

`Left` and `Right` arrows move pieces left or right respectively. Pressing
`Down` arrow increases the speed of falling 3x times. When released speed
returns to normal. `Up` arrow rotates a piece.

## Implementation details

Game board is represented as a two-dimensional array of size 14x10. The dimensions
of each cell are 50x50 pixels. Each cell is represented with a number ranging from
0 to 6, where 0 means empty cell, and 1-6 means an element. There are corresponding
images to each of these numbers in the `img` folder.

`Element` class represents the currently falling element. It is not used to represent
the already fallen elements. When element falls - all coordinates from element are 
copied to the board array, and cells are rendered by finding a corresponding color
image. This helps to save memory, as texture data from `Element` is not stored after
element was placed.
