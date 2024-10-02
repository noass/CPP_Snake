#include "raylib.h"
#include "Game.h"

int main()
{
	Init();

	while (!WindowShouldClose())
	{
		Update();

		BeginDrawing();
			Draw();
		EndDrawing();
	}

	Unload();
	CloseWindow();
}