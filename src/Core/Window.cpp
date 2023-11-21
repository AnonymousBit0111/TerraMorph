#include "Core/Window.h"
#include "Core/Event.h"
#include "Core/EventHandlers.h"
#include <SDL_events.h>
#include <SDL_video.h>
#include <iostream>
#include <sys/types.h>
#include <utility>

using namespace TerraMorph::Core;

Window::Window(const std::string &name, uint width, uint height)
    : m_width(width), m_height(height) {

  if (!SDL_CreateWindow(m_name.c_str(), 100, 100, width, height,
                        SDL_WINDOW_SHOWN)) {
    std::cerr << "Unable to create window\n";
    exit(-1);
  };
}

void Window::subscribeToEvent(EventType event, EventHandler handler) {
  m_eventHandlers.emplace(std::make_pair(event, handler));
}

void Window::pollEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      auto range = m_eventHandlers.equal_range(EventType::Quit);

      for (auto it = range.first; it != range.second; ++it) {
        it->second({EventType::Quit});
      }
    }
  }
}

Window::~Window() { SDL_DestroyWindow(m_window); }