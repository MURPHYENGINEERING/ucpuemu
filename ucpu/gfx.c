#include "gfx.h"
#include "keyboard.h"
#include <SDL.h>


int window_create(CPU_Window* wnd)
{
  SDL_Init(SDL_INIT_VIDEO);

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

int window_process_events(CPU_Window* wnd, uint32_t* mem)
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
        case VK_SPACE:
        mem[0xffd] = 1;
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
        case VK_SPACE:
        mem[0xffd] = 0;
        default:
        break;
      }
      break;
    }
  }

  return 1;
}


void window_draw(CPU_Window* wnd, uint32_t* mem)
{
  SDL_SetRenderDrawColor(wnd->renderer, 0, 0, 0, 0);
  SDL_RenderClear(wnd->renderer);
  SDL_SetRenderDrawColor(wnd->renderer, 255, 255, 255, 255);

  /*
  for (size_t y = 0; y < 480; ++y) {
    for (size_t x = 0; x < 80; ++x) {
      size_t iword = y * x;
      uint32_t word = mem[0x400 + iword];
      for (size_t ipx = 0; ipx < 8; ++ipx) {
        if ((word >> ipx) & 1)
          SDL_RenderDrawPoint(wnd->renderer, x*8 + ipx, y);
      }
    }
  }
  */
  uint32_t x = 0, y = 0;
  for (size_t i = 0; i < 38400; ++i) {
    x += 8;
    if (x >= 640) {
      x = 0;
      ++y;
    }
    uint32_t word = mem[0x400 + i];
    for (size_t ipx = 0; ipx < 8; ++ipx) {
      if ((word >> ipx) & 1) {
        SDL_RenderDrawPoint(wnd->renderer, x+ipx, y);
      }
    }
  }

  SDL_RenderPresent(wnd->renderer);
}