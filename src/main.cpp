#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
#include <glad/glad.h>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include "tracy/Tracy.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <core/Application.hpp>
#include <windows.h>

void AttachConsoleForLogging() {
  AllocConsole();                  // create a new console
  freopen("CONOUT$", "w", stdout); // redirect stdout to console
  freopen("CONOUT$", "w", stderr); // redirect stderr to console
}
/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  AttachConsoleForLogging();

  *appstate = new Core::Application();
  Core::Application *app = (Core::Application *)(*appstate);

  FrameMark;

  return app->appQuit; /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  Core::Application *app = (Core::Application *)appstate;
  app->handleEvents(event);
  return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
  ZoneScopedN("SDL_AppIterate");
  Core::Application *app = (Core::Application *)appstate;
  // const double now = ((double)SDL_GetTicks()) / 1000.0;  /* convert from
  // milliseconds to seconds. */
  app->update();
  app->render();
  FrameMark;
  return app->appQuit; /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  Core::Application *app = (Core::Application *)appstate;
  if (app) {
    // SDL_DestroyWindow(app->getWindow());
    delete app;
  }
  SDL_Log("Exiting with result %d", result);
  SDL_Quit();
}
