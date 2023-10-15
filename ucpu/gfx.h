#ifndef GFX_H
#define GFX_H

#include <SDL.h>

struct CPU_Window 
{
  struct SDL_Window* window;
  struct SDL_Renderer* renderer;

  size_t width;
  size_t height;
  char const* title;
};

int window_create(struct CPU_Window* wnd);
void window_draw(struct CPU_Window* wnd, uint32_t* mem);
int window_process_events(struct CPU_Window* wnd, uint32_t* mem);


#endif