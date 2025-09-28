#include <glad/glad.h>
#include <SDL3/SDL_opengl.h>
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <core/Application.hpp>
#include <windows.h>

void AttachConsoleForLogging() {
    AllocConsole(); // create a new console
    freopen("CONOUT$", "w", stdout);  // redirect stdout to console
    freopen("CONOUT$", "w", stderr);  // redirect stderr to console
}
/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
  AttachConsoleForLogging();
  SDL_SetAppMetadata("Voxel Engine", "1.0", "me.perzero.voxel-engine");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_Window *window = SDL_CreateWindow("Voxel Engine", 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    SDL_Log("Failed to initialize GLAD");
    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    return SDL_APP_FAILURE;
  }

  SDL_GL_MakeCurrent(window, context);
  SDL_GL_SetSwapInterval(1);
  
  *appstate = new Core::Application(window, context);
  Core::Application* app = (Core::Application*)(*appstate);
  app->start();
  return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
  Core::Application* app = (Core::Application*)appstate;
  switch (event->type) {
    case SDL_EVENT_WINDOW_RESIZED:
      app->onWindowResize();
      break;
    case SDL_EVENT_QUIT:
      app->appQuit = SDL_APP_SUCCESS;  /* signal to quit the program. */
      break;
    default:
      break;
  }
  app->handleEvents(event);
  return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
  Core::Application* app = (Core::Application*)appstate;
  // const double now = ((double)SDL_GetTicks()) / 1000.0;  /* convert from milliseconds to seconds. */
  app->update();
  app->render();
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


