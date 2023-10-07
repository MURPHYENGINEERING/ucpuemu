#include "gfx.h"
#include <SDL.h>


int window_create(CPU_Window* wnd)
{
  wnd->window = SDL_CreateWindow(
    wnd->title,
    0,
    0,
    wnd->width,
    wnd->height,
    SDL_WINDOW_SHOWN);

  if (!wnd->window) {
    printf("Error creating SDL window: %s\n", SDL_GetError());
    return 0;
  }

  wnd->renderer = SDL_CreateRenderer(wnd->window, -1, SDL_RENDERER_ACCELERATED);
  if (!wnd->renderer) {
    printf("Error creating SDL renderer: %s\n", SDL_GetError());
    return 0;
  }

  return 1;
}

int process_window_events(CPU_Window* wnd)
{
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_QUIT:
      SDL_ShowCursor(1);
      return 0;

      case SDL_WINDOWEVENT:
      switch (((SDL_WindowEvent*)&e)->event) {
        case SDL_WINDOWEVENT_ENTER: SDL_ShowCursor(0); break;
        case SDL_WINDOWEVENT_LEAVE: SDL_ShowCursor(1); break;
      }
      break;
    }
  }
  return 1;
}