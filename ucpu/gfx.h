#ifndef GFX_H
#define GFX_H

#include <SDL.h>

typedef struct {
  struct SDL_Window* window;
  struct SDL_Renderer* renderer;

  size_t width;
  size_t height;
  char const* title;
} CPU_Window;


int window_create(CPU_Window* wnd);
int process_window_events(CPU_Window* wnd, uint32_t* mem);


#endif