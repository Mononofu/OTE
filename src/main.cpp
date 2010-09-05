
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <taskengine/taskengine.h>
#include "physics.h"
#include "graphics.h"
#include "game.h"
#include "input.h"

int main(int argc, char *argv[])
{
	initDebug();

	Threadmanager myManager;
	Graphics graphics;;
	Physics physics;
	Input input;
	Game game;

#ifdef SINGLE_THREADED
	myManager.setThreadingMode(THREADING_SEQUENCIAL);
#endif

	myManager.registerTask(&game);
	myManager.registerTask(&graphics);
	myManager.registerTask(&physics);
	myManager.registerTask(&input);

	myManager.run();

	myManager.waitForThreadsToFinish();


	return EXIT_SUCCESS;
}
