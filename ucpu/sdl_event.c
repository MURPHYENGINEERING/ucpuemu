#include "sdl_event.h"
#include <SDL.h>


char const*
lm_sdl_event_type_name(SDL_EventType type)
{
  switch (type) { // clang-format off
case SDL_QUIT:                     return "SDL_QUIT (user-requested quit)";
case SDL_APP_TERMINATING:          return "SDL_APP_TERMINATING (OS is terminating the application)";
case SDL_APP_LOWMEMORY:            return "SDL_APP_LOWMEMORY (OS is low on memory; free some)";
case SDL_APP_WILLENTERBACKGROUND:  return "SDL_APP_WILLENTERBACKGROUND (application is entering background)";
case SDL_APP_DIDENTERBACKGROUND:   return "SDL_APP_DIDENTERBACKGROUND (application entered background)";
case SDL_APP_WILLENTERFOREGROUND:  return "SDL_APP_WILLENTERFOREGROUND (application is entering foreground)";
case SDL_APP_DIDENTERFOREGROUND:   return "SDL_APP_DIDENTERFOREGROUND (application entered foreground)";
case SDL_WINDOWEVENT:              return "SDL_WINDOWEVENT (window state change)";
case SDL_SYSWMEVENT:               return "SDL_SYSWMEVENT (system specific event)";
case SDL_KEYDOWN:                  return "SDL_KEYDOWN (key pressed)";
case SDL_KEYUP:                    return "SDL_KEYUP (key released)";
case SDL_TEXTEDITING:              return "SDL_TEXTEDITING (keyboard text editing (composition))";
case SDL_TEXTINPUT:                return "SDL_TEXTINPUT (keyboard text input)";
case SDL_KEYMAPCHANGED:            return "SDL_KEYMAPCHANGED (keymap changed due to a system event such as an input language or keyboard layout change (>= SDL 2.0.4))";
case SDL_MOUSEMOTION:              return "SDL_MOUSEMOTION (mouse moved)";
case SDL_MOUSEBUTTONDOWN:          return "SDL_MOUSEBUTTONDOWN (mouse button pressed)";
case SDL_MOUSEBUTTONUP:            return "SDL_MOUSEBUTTONUP (mouse button released)";
case SDL_MOUSEWHEEL:               return "SDL_MOUSEWHEEL (mouse wheel motion)";
case SDL_JOYAXISMOTION:            return "SDL_JOYAXISMOTION (joystick axis motion)";
case SDL_JOYBALLMOTION:            return "SDL_JOYBALLMOTION (joystick trackball motion)";
case SDL_JOYHATMOTION:             return "SDL_JOYHATMOTION (joystick hat position change)";
case SDL_JOYBUTTONDOWN:            return "SDL_JOYBUTTONDOWN (joystick button pressed)";
case SDL_JOYBUTTONUP:              return "SDL_JOYBUTTONUP (joystick button released)";
case SDL_JOYDEVICEADDED:           return "SDL_JOYDEVICEADDED (joystick connected)";
case SDL_JOYDEVICEREMOVED:         return "SDL_JOYDEVICEREMOVED (joystick disconnected)";
case SDL_CONTROLLERAXISMOTION:     return "SDL_CONTROLLERAXISMOTION (controller axis motion)";
case SDL_CONTROLLERBUTTONDOWN:     return "SDL_CONTROLLERBUTTONDOWN (controller button pressed)";
case SDL_CONTROLLERBUTTONUP:       return "SDL_CONTROLLERBUTTONUP (controller button released)";
case SDL_CONTROLLERDEVICEADDED:    return "SDL_CONTROLLERDEVICEADDED (controller connected)";
case SDL_CONTROLLERDEVICEREMOVED:  return "SDL_CONTROLLERDEVICEREMOVED (controller disconnected)";
case SDL_CONTROLLERDEVICEREMAPPED: return "SDL_CONTROLLERDEVICEREMAPPED (controller mapping updated)";
case SDL_FINGERDOWN:               return "SDL_FINGERDOWN (user has touched input device)";
case SDL_FINGERUP:                 return "SDL_FINGERUP (user stopped touching input device)";
case SDL_FINGERMOTION:             return "SDL_FINGERMOTION (user is dragging finger on input device)";
case SDL_DOLLARGESTURE:            return "SDL_DOLLARGESTURE ()";
case SDL_DOLLARRECORD:             return "SDL_DOLLARRECORD ()";
case SDL_MULTIGESTURE:             return "SDL_MULTIGESTURE ()";
case SDL_CLIPBOARDUPDATE:          return "SDL_CLIPBOARDUPDATE (the clipboard changed)";
case SDL_DROPFILE:                 return "SDL_DROPFILE (the system requests a file open)";
case SDL_DROPTEXT:                 return "SDL_DROPTEXT (text/plain drag-and-drop event)";
case SDL_DROPBEGIN:                return "SDL_DROPBEGIN (a new set of drops is beginning (>= SDL 2.0.5))";
case SDL_DROPCOMPLETE:             return "SDL_DROPCOMPLETE (current set of drops is now complete (>= SDL 2.0.5))";
case SDL_AUDIODEVICEADDED:         return "SDL_AUDIODEVICEADDED (a new audio device is available (>= SDL 2.0.4))";
case SDL_AUDIODEVICEREMOVED:       return "SDL_AUDIODEVICEREMOVED (an audio device has been removed (>= SDL 2.0.4))";
case SDL_RENDER_TARGETS_RESET:     return "SDL_RENDER_TARGETS_RESET (the render targets have been reset and their contents need to be updated (>= SDL 2.0.2))";
case SDL_RENDER_DEVICE_RESET:      return "SDL_RENDER_DEVICE_RESET (the device has been reset and all textures need to be recreated (>= SDL 2.0.4))";
case SDL_USEREVENT:                return "SDL_USEREVENT (a user-specified event)";
case SDL_LASTEVENT:                return "SDL_LASTEVENT (only for bounding internal arrays)";
default: return "<unknown>";
  } // clang-format on
}