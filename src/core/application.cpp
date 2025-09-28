#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_timer.h"
#include "engine/Camera.hpp"
#include "engine/Input.hpp"
#include "engine/Transform.hpp"
#include "engine/chunk/ChunkData.hpp"
#include "core/Common.hpp"
#include <core/Application.hpp>
#include "engine/chunk/ChunkMeshBuilder.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "engine/utils/Utils.hpp"

namespace Core {
Application::Application(SDL_Window* win, SDL_GLContext &context)  {
  m_window = win;
  m_context = context;
  appQuit = SDL_APP_CONTINUE;
}

void Application::start() {
  onWindowResize();

  SDL_SetWindowRelativeMouseMode(m_window, true);
  m_windowFocused = true;
  Engine::Input::instance().init(this);

  Engine::Chunk::ChunkData chunk;
  for (int i = 0; i < CHUNK_SIZE; i++) {
    chunk.voxels[i].type = 1;
  }

  const auto chunkEntity = m_registry.create();
  m_registry.emplace<Engine::Chunk::ChunkData>(chunkEntity, chunk);
  m_registry.emplace<Engine::Chunk::ChunkMeshBuilder>(chunkEntity);

  Engine::Camera camComponent = Engine::Utils::defaultCamera(60.0f);
  Engine::Transform camTransform = Engine::Utils::defaultTransform();

  Engine::Utils::cameraWithControllerEntity(m_registry);

  m_now = SDL_GetPerformanceCounter();
}

void Application::update() {
  m_last = m_now;
  m_now = SDL_GetPerformanceCounter();
  float dt = static_cast<double>(m_now - m_last) / SDL_GetPerformanceFrequency();
  Engine::CameraControllerSystem::update(m_registry, dt);
}

void Application::handleEvents(SDL_Event* event) {
  switch (event->type) {
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
      m_windowFocused = true;
      SDL_SetWindowRelativeMouseMode(m_window, true);
      break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
      m_windowFocused = false;
      SDL_SetWindowRelativeMouseMode(m_window, false);
      break;
    default:
      break;
  }
}

void Application::render() {
  clearScreen();
  glm::mat4 viewProjection = Engine::CameraSystem::update(m_registry, (float)m_width / (float)m_height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glLoadMatrixf(glm::value_ptr(viewProjection));
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0f, 0.0f, -5.0f);
  glRotatef((float)SDL_GetTicks() * 0.05f, 0.0f, 1.0f, 0.0f);

  glBegin(GL_QUADS);

    // Front
    glColor3f(1, 0, 0);
    glVertex3f(-1, -1,  1);
    glVertex3f( 1, -1,  1);
    glVertex3f( 1,  1,  1);
    glVertex3f(-1,  1,  1);

    // Back
    glColor3f(0, 1, 0);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1,  1, -1);
    glVertex3f( 1,  1, -1);
    glVertex3f( 1, -1, -1);

    // Top
    glColor3f(0, 0, 1);
    glVertex3f(-1, 1, -1);
    glVertex3f(-1, 1,  1);
    glVertex3f( 1, 1,  1);
    glVertex3f( 1, 1, -1);

    // Bottom
    glColor3f(1, 1, 0);
    glVertex3f(-1, -1, -1);
    glVertex3f( 1, -1, -1);
    glVertex3f( 1, -1,  1);
    glVertex3f(-1, -1,  1);

    // Right
    glColor3f(1, 0, 1);
    glVertex3f(1, -1, -1);
    glVertex3f(1,  1, -1);
    glVertex3f(1,  1,  1);
    glVertex3f(1, -1,  1);

    // Left
    glColor3f(0, 1, 1);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1,  1);
    glVertex3f(-1,  1,  1);
    glVertex3f(-1,  1, -1);

    glEnd();
  SDL_GL_SwapWindow(m_window);
}

void Application::onWindowResize() {
  int w, h;
  SDL_GetWindowSize(m_window, &w, &h);
  m_width = w;
  m_height = h;
}

Application::~Application() {
}

void Application::clearScreen() {
  glViewport(0, 0, m_width, m_height);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void  Application::perspective(float fovY, float aspect, float zNear, float zFar, float* matrix) {
  const float f = 1.0f / tanf(fovY * 0.5f * (3.14159265358979323846f / 180.0f));
  matrix[0] = f / aspect;
  matrix[1] = 0.0f;
  matrix[2] = 0.0f;
  matrix[3] = 0.0f;

  matrix[4] = 0.0f;
  matrix[5] = f;
  matrix[6] = 0.0f;
  matrix[7] = 0.0f;

  matrix[8] = 0.0f;
  matrix[9] = 0.0f;
  matrix[10] = (zFar + zNear) / (zNear - zFar);
  matrix[11] = -1.0f;

  matrix[12] = 0.0f;
  matrix[13] = 0.0f;
  matrix[14] = (2.0f * zFar * zNear) / (zNear - zFar);
  matrix[15] = 0.0f;
}
}
