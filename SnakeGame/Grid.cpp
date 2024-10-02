#include "Window.h"
#include "Grid.h"
#include "raylib.h"

void createGrid(int rows, int cols) {
    const int CELL_HEIGHT = Window::getHeight() / rows;
    const int CELL_WIDTH = Window::getWidth() / cols;
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
			DrawRectangleLines(i * CELL_WIDTH, j * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, BLACK);
        }
    }
}
