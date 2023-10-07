#include "gfx.h"
#include "keyboard.h"
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

int process_window_events(CPU_Window* wnd, uint32_t* mem)
{
  SDL_Event e;
  SDL_Keysym key;
  VirtualKey vkey;

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

      case SDL_KEYDOWN:
      key = ((SDL_KeyboardEvent*)&e)->keysym;
      vkey = (VirtualKey) key.sym;
      switch (vkey) {
        case VK_LEFT:
        mem[0xffc] = 1;
        break;
        case VK_RIGHT: 
        mem[0xffb] = 1;
        break;
        default:
        break;
        case VK_ESCAPE:
        return 0;
      }
      break;

      case SDL_KEYUP:
      key = ((SDL_KeyboardEvent*)&e)->keysym;
      vkey = (VirtualKey) key.sym;
      switch (vkey) {
        case VK_LEFT:
        mem[0xffc] = 0;
        break;
        case VK_RIGHT: 
        mem[0xffb] = 0;
        break;
        default:
        break;
      }
      break;
    }
  }
  return 1;
}