#include "SDL3/SDL_opengl.h"
#include "SDL3/SDL_video.h"
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <core/Application.hpp>

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
  SDL_SetAppMetadata("Voxel Engine", "1.0", "me.perzero.voxel-engine");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_Window *window = SDL_CreateWindow("Voxel Engine", 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (!window) {
    SDL_Log("Couldn't create window: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_GLContext context = SDL_GL_CreateContext(window);
  if (!context) {
    SDL_Log("Couldn't create OpenGL context: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    return SDL_APP_FAILURE;
  }
  
  glEnable(GL_DEPTH_TEST);

  *appstate = new Core::Application(window, context);
  return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
  Core::Application* app = (Core::Application*)appstate;
  if (event->type == SDL_EVENT_QUIT) {
    app->appQuit = SDL_APP_SUCCESS;  /* signal to quit the program. */
  }
  return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
  Core::Application* app = (Core::Application*)appstate;
  const double now = ((double)SDL_GetTicks()) / 1000.0;  /* convert from milliseconds to seconds. */
  glViewport(0, 0, 640, 480);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  return app->appQuit;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  Core::Application* app = (Core::Application*)appstate;
  if (app) {
    SDL_DestroyWindow(app->getWindow());
    delete app;
  }
  SDL_Log("Exiting with result %d", result);
  SDL_Quit();
}


