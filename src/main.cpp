#include <core/screen.h>
#include <iostream>
#include <SDL2/SDL.h>

int main(int argc, char *argv[])
{
	mesh meshCube;
	meshCube.LoadFromObjectFile("res/spaceship.obj");
	Screen screen(meshCube);
	Uint32 startTime = SDL_GetTicks();
	const int FPS = 45;
	const int frameDelay = 1000 / FPS;
	while (true)
	{
		Uint32 frameStart = SDL_GetTicks();

		// Calculate elapsed time
		Uint32 currentTime = SDL_GetTicks();
		float elapsedTime = (currentTime - startTime) / 1000.0f; // Convert milliseconds to seconds

		screen.show(elapsedTime);
		screen.input();

		Uint32 frameTime = SDL_GetTicks() - frameStart;

		if (frameDelay > frameTime)
		{
			SDL_Delay(frameDelay - frameTime);
		}
	}
	SDL_Quit();
	return 0;
}