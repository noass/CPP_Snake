#include "raylib.h"
#include "Window.h"
#include "Grid.h"
#include <iostream>
#include <random>
#include <ctime>
#include <queue>

std::queue<int> directionQueue;

const int COLS = 32;
const int ROWS = 32;

int grid[COLS][ROWS];

int playerX = ROWS / 2;
int playerY = COLS / 2;
float moveTimer = 100.0f;
float moveSpeed = 900.0f;
int direction = 0;
int snakeSize = 3;
Vector2 snakeTail[1000];
int score = 0;
bool dead = false;

int foodX;
int foodY;

bool spawnedFood = false;

bool showDebug = false;

Sound deathSound = LoadSound("resources/death.wav");
Sound eatSound = LoadSound("resources/eat.wav");

void printGrid() {
    if (moveTimer <= 0.0f) {
        for (int i = 0; i < COLS; i++) {
            for (int j = 0; j < ROWS; j++) {
                std::cout << grid[j][i];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

void foodSpawn() {
    if (!spawnedFood) {
        int rangeCOLS = COLS - 1;
        int rangeROWS = ROWS - 1;
        int randCOLS = std::rand() % rangeCOLS;
        int randROWS = std::rand() % rangeROWS;
        foodX = randCOLS;
        foodY = randROWS;
        if (grid[randCOLS][randROWS] == 0) {
            grid[randCOLS][randROWS] = 2;
            spawnedFood = true;
            std::cout << "Food spawned at: " << randCOLS << ", " << randROWS << std::endl;
        }
    }
}

void reset() {
	dead = false;
	snakeSize = 3;
	score = 0;
	playerX = ROWS / 2;
	playerY = COLS / 2;
	direction = 0;
	for (int i = 0; i < COLS; i++) {
		for (int j = 0; j < ROWS; j++) {
			grid[j][i] = 0;
		}
	}
	spawnedFood = false;
}


void Init() {
    InitWindow(Window::getWidth(), Window::getHeight(), "Snake");
    InitAudioDevice();
    //SetTargetFPS(60);
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // Fix for Problem 1
	SetMasterVolume(0.2f);
}

void Update() {
    static int prevDirection = direction; // Store the previous direction

    // Handle input immediately and add to the queue
    if (!dead) {
        if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && direction != 1) {
            directionQueue.push(0);
        }
        else if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && direction != 0) {
            directionQueue.push(1);
        }
        else if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && direction != 3) {
            directionQueue.push(2);
        }
        else if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && direction != 2) {
            directionQueue.push(3);
        }
    }

	if (IsKeyPressed(KEY_Z)) {
		showDebug = !showDebug;
	}

    if (IsKeyPressed(KEY_R)) {
        reset();
    }

    // Reduce the move timer interval for more frequent updates
    moveTimer -= moveSpeed * GetFrameTime();
    if (moveTimer <= 0.0f) {
        moveTimer = 100.0f;

        // Process the direction queue
        if (!directionQueue.empty()) {
            int newDirection = directionQueue.front();
            directionQueue.pop();
            // Ensure the new direction is not the opposite of the current direction
            if ((direction == 0 && newDirection != 1) ||
                (direction == 1 && newDirection != 0) ||
                (direction == 2 && newDirection != 3) ||
                (direction == 3 && newDirection != 2)) {
                direction = newDirection;
            }
        }

        // Move the snake's tail
        if (!dead) {
            for (int i = snakeSize - 1; i > 0; i--) {
                snakeTail[i] = snakeTail[i - 1];
            }
            snakeTail[0] = { (float)playerX, (float)playerY };
        }

        // Move the snake's head
        if (!dead) {
            if (direction == 0) {
                playerX++;
            }
            else if (direction == 1) {
                playerX--;
            }
            else if (direction == 2) {
                playerY--;
            }
            else if (direction == 3) {
                playerY++;
            }
        }

        // Check for food collision
        if (playerX == foodX && playerY == foodY && spawnedFood) {
            snakeSize++;
            score++;
            spawnedFood = false;
			PlaySound(eatSound);
        }

        // Player collides with tail
        if (snakeSize > 1) {
            for (int i = 0; i < snakeSize - 1; i++) {
                if (playerX == static_cast<int>(snakeTail[i].x) && playerY == static_cast<int>(snakeTail[i].y)) {
                    dead = true;
					PlaySound(deathSound);
                }
            }
        }

        // Player collides with walls, wrap around
        if (playerX >= COLS) {
            playerX = 0;
        }
        else if (playerX < 0) {
            playerX = COLS - 1;
        }
        if (playerY >= ROWS) {
            playerY = 0;
        }
        else if (playerY < 0) {
            playerY = ROWS - 1;
        }

        // Update the grid
        for (int i = 0; i < COLS; i++) {
            for (int j = 0; j < ROWS; j++) {
                if (grid[j][i] != 2) { // Preserve food position
                    grid[j][i] = 0;
                }
            }
        }
        grid[playerX][playerY] = 1;
        for (int i = 0; i < snakeSize - 1; i++) {
            grid[static_cast<int>(snakeTail[i].x)][static_cast<int>(snakeTail[i].y)] = 1; // Fix for Problem 2 and 3
        }

        foodSpawn();

        prevDirection = direction; // Update the previous direction
    }
}

void Draw() {
    ClearBackground(BLACK);

    // Draw the snake's head
    DrawRectangle(playerX * Window::getWidth() / COLS, playerY * Window::getHeight() / ROWS, Window::getWidth() / COLS, Window::getHeight() / ROWS, GREEN);

    // Draw the snake's tail
    for (int i = 0; i < snakeSize - 1; i++) {
        DrawRectangle(static_cast<int>(snakeTail[i].x) * Window::getWidth() / COLS, static_cast<int>(snakeTail[i].y) * Window::getHeight() / ROWS, Window::getWidth() / COLS, Window::getHeight() / ROWS, GREEN); // Fix for Problem 2 and 3
    }

    // Draw the food
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < ROWS; j++) {
            if (grid[j][i] == 2) {
                DrawRectangle(j * Window::getWidth() / COLS, i * Window::getHeight() / ROWS, Window::getWidth() / COLS, Window::getHeight() / ROWS, RED);
            }
        }
    }

    createGrid(COLS, ROWS);

    DrawText(TextFormat("SCORE: %i", score), 0, 0, 40, RAYWHITE);

    if (dead) {
		const char* death_text = "You died!";
		DrawText(death_text, Window::getWidth() / 2 - (MeasureText(death_text, 40)/2), Window::getHeight() / 2 - 20, 40, RED);
        const char* death_under_text = "Press 'R' to restart.";
		DrawText(death_under_text, Window::getWidth() / 2 - (MeasureText(death_under_text, 20) / 2), Window::getHeight() / 2 + 20, 20, RED);
    }

    if (showDebug) {
        DrawText(TextFormat("SIZE: %i", snakeSize), 0, Window::getHeight() - 80, 20, DARKGREEN);
        DrawText(TextFormat("SNAKE POS: (%i, %i)", playerX, playerY), 0, Window::getHeight() - 60, 20, DARKGREEN);
        DrawText(TextFormat("FOOD POS: (%i, %i)", foodX, foodY), 0, Window::getHeight() - 40, 20, DARKGREEN);
        DrawText(TextFormat("FPS: %i", GetFPS()), 0, Window::getHeight() - 20, 20, DARKGREEN);
    }
    else {
		DrawText("Press 'Z' to toggle debug info.", 0, Window::getHeight() - 11, 11, DARKGRAY);
    }
}

void Unload() {
	UnloadSound(deathSound);
	UnloadSound(eatSound);
	CloseAudioDevice();
}
