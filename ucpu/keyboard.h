#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef enum {
  VK_UNKNOWN            = 0x00,
  VK_BACKSPACE          = 0x08,
  VK_TAB                = 0x09,
  VK_RETURN             = 0x0D,
  VK_ESCAPE             = 0x1B,
  VK_SPACE              = 0x20,
  VK_EXCLAIM            = 0x21,
  VK_QUOTEDBL           = 0x22,
  VK_HASH               = 0x23,
  VK_DOLLAR             = 0x24,
  VK_PERCENT            = 0x25,
  VK_AMPERSAND          = 0x26,
  VK_QUOTE              = 0x27,
  VK_LEFTPAREN          = 0x28,
  VK_RIGHTPAREN         = 0x29,
  VK_ASTERISK           = 0x2A,
  VK_PLUS               = 0x2B,
  VK_COMMA              = 0x2C,
  VK_MINUS              = 0x2D,
  VK_PERIOD             = 0x2E,
  VK_SLASH              = 0x2F,
  VK_0                  = 0x30,
  VK_1                  = 0x31,
  VK_2                  = 0x32,
  VK_3                  = 0x33,
  VK_4                  = 0x34,
  VK_5                  = 0x35,
  VK_6                  = 0x36,
  VK_7                  = 0x37,
  VK_8                  = 0x38,
  VK_9                  = 0x39,
  VK_COLON              = 0x3A,
  VK_SEMICOLON          = 0x3B,
  VK_LESS               = 0x3C,
  VK_EQUALS             = 0x3D,
  VK_GREATER            = 0x3E,
  VK_QUESTION           = 0x3F,
  VK_AT                 = 0x40,
  VK_LEFTBRACKET        = 0x5B,
  VK_BACKSLASH          = 0x5C,
  VK_RIGHTBRACKET       = 0x5D,
  VK_CARET              = 0x5E,
  VK_UNDERSCORE         = 0x5F,
  VK_BACKQUOTE          = 0x60,
  VK_A                  = 0x61,
  VK_B                  = 0x62,
  VK_C                  = 0x63,
  VK_D                  = 0x64,
  VK_E                  = 0x65,
  VK_F                  = 0x66,
  VK_G                  = 0x67,
  VK_H                  = 0x68,
  VK_I                  = 0x69,
  VK_J                  = 0x6A,
  VK_K                  = 0x6B,
  VK_L                  = 0x6C,
  VK_M                  = 0x6D,
  VK_N                  = 0x6E,
  VK_O                  = 0x6F,
  VK_P                  = 0x70,
  VK_Q                  = 0x71,
  VK_R                  = 0x72,
  VK_S                  = 0x73,
  VK_T                  = 0x74,
  VK_U                  = 0x75,
  VK_V                  = 0x76,
  VK_W                  = 0x77,
  VK_X                  = 0x78,
  VK_Y                  = 0x79,
  VK_Z                  = 0x7A,
  VK_DELETE             = 0x7F,
  VK_CAPSLOCK           = 0x40000039,
  VK_F1                 = 0x4000003A,
  VK_F2                 = 0x4000003B,
  VK_F3                 = 0x4000003C,
  VK_F4                 = 0x4000003D,
  VK_F5                 = 0x4000003E,
  VK_F6                 = 0x4000003F,
  VK_F7                 = 0x40000040,
  VK_F8                 = 0x40000041,
  VK_F9                 = 0x40000042,
  VK_F10                = 0x40000043,
  VK_F11                = 0x40000044,
  VK_F12                = 0x40000045,
  VK_PRINTSCREEN        = 0x40000046,
  VK_SCROLLLOCK         = 0x40000047,
  VK_PAUSE              = 0x40000048,
  VK_INSERT             = 0x40000049,
  VK_HOME               = 0x4000004A,
  VK_PAGEUP             = 0x4000004B,
  VK_END                = 0x4000004D,
  VK_PAGEDOWN           = 0x4000004E,
  VK_RIGHT              = 0x4000004F,
  VK_LEFT               = 0x40000050,
  VK_DOWN               = 0x40000051,
  VK_UP                 = 0x40000052,
  VK_NUMLOCKCLEAR       = 0x40000053,
  VK_KP_DIVIDE          = 0x40000054,
  VK_KP_MULTIPLY        = 0x40000055,
  VK_KP_MINUS           = 0x40000056,
  VK_KP_PLUS            = 0x40000057,
  VK_KP_ENTER           = 0x40000058,
  VK_KP_1               = 0x40000059,
  VK_KP_2               = 0x4000005A,
  VK_KP_3               = 0x4000005B,
  VK_KP_4               = 0x4000005C,
  VK_KP_5               = 0x4000005D,
  VK_KP_6               = 0x4000005E,
  VK_KP_7               = 0x4000005F,
  VK_KP_8               = 0x40000060,
  VK_KP_9               = 0x40000061,
  VK_KP_0               = 0x40000062,
  VK_KP_PERIOD          = 0x40000063,
  VK_APPLICATION        = 0x40000065,
  VK_POWER              = 0x40000066,
  VK_KP_EQUALS          = 0x40000067,
  VK_F13                = 0x40000068,
  VK_F14                = 0x40000069,
  VK_F15                = 0x4000006A,
  VK_F16                = 0x4000006B,
  VK_F17                = 0x4000006C,
  VK_F18                = 0x4000006D,
  VK_F19                = 0x4000006E,
  VK_F20                = 0x4000006F,
  VK_F21                = 0x40000070,
  VK_F22                = 0x40000071,
  VK_F23                = 0x40000072,
  VK_F24                = 0x40000073,
  VK_EXECUTE            = 0x40000074,
  VK_HELP               = 0x40000075,
  VK_MENU               = 0x40000076,
  VK_SELECT             = 0x40000077,
  VK_STOP               = 0x40000078,
  VK_AGAIN              = 0x40000079,
  VK_UNDO               = 0x4000007A,
  VK_CUT                = 0x4000007B,
  VK_COPY               = 0x4000007C,
  VK_PASTE              = 0x4000007D,
  VK_FIND               = 0x4000007E,
  VK_MUTE               = 0x4000007F,
  VK_VOLUMEUP           = 0x40000080,
  VK_VOLUMEDOWN         = 0x40000081,
  VK_KP_COMMA           = 0x40000085,
  VK_KP_EQUALSAS400     = 0x40000086,
  VK_ALTERASE           = 0x40000099,
  VK_SYSREQ             = 0x4000009A,
  VK_CANCEL             = 0x4000009B,
  VK_CLEAR              = 0x4000009C,
  VK_PRIOR              = 0x4000009D,
  VK_RETURN2            = 0x4000009E,
  VK_SEPARATOR          = 0x4000009F,
  VK_OUT                = 0x400000A0,
  VK_OPER               = 0x400000A1,
  VK_CLEARAGAIN         = 0x400000A2,
  VK_CRSEL              = 0x400000A3,
  VK_EXSEL              = 0x400000A4,
  VK_KP_00              = 0x400000B0,
  VK_KP_000             = 0x400000B1,
  VK_THOUSANDSSEPARATOR = 0x400000B2,
  VK_DECIMALSEPARATOR   = 0x400000B3,
  VK_CURRENCYUNIT       = 0x400000B4,
  VK_CURRENCYSUBUNIT    = 0x400000B5,
  VK_KP_LEFTPAREN       = 0x400000B6,
  VK_KP_RIGHTPAREN      = 0x400000B7,
  VK_KP_LEFTBRACE       = 0x400000B8,
  VK_KP_RIGHTBRACE      = 0x400000B9,
  VK_KP_TAB             = 0x400000BA,
  VK_KP_BACKSPACE       = 0x400000BB,
  VK_KP_A               = 0x400000BC,
  VK_KP_B               = 0x400000BD,
  VK_KP_C               = 0x400000BE,
  VK_KP_D               = 0x400000BF,
  VK_KP_E               = 0x400000C0,
  VK_KP_F               = 0x400000C1,
  VK_KP_XOR             = 0x400000C2,
  VK_KP_POWER           = 0x400000C3,
  VK_KP_PERCENT         = 0x400000C4,
  VK_KP_LESS            = 0x400000C5,
  VK_KP_GREATER         = 0x400000C6,
  VK_KP_AMPERSAND       = 0x400000C7,
  VK_KP_DBLAMPERSAND    = 0x400000C8,
  VK_KP_VERTICALBAR     = 0x400000C9,
  VK_KP_DBLVERTICALBAR  = 0x400000CA,
  VK_KP_COLON           = 0x400000CB,
  VK_KP_HASH            = 0x400000CC,
  VK_KP_SPACE           = 0x400000CD,
  VK_KP_AT              = 0x400000CE,
  VK_KP_EXCLAM          = 0x400000CF,
  VK_KP_MEMSTORE        = 0x400000D0,
  VK_KP_MEMRECALL       = 0x400000D1,
  VK_KP_MEMCLEAR        = 0x400000D2,
  VK_KP_MEMADD          = 0x400000D3,
  VK_KP_MEMSUBTRACT     = 0x400000D4,
  VK_KP_MEMMULTIPLY     = 0x400000D5,
  VK_KP_MEMDIVIDE       = 0x400000D6,
  VK_KP_PLUSMINUS       = 0x400000D7,
  VK_KP_CLEAR           = 0x400000D8,
  VK_KP_CLEARENTRY      = 0x400000D9,
  VK_KP_BINARY          = 0x400000DA,
  VK_KP_OCTAL           = 0x400000DB,
  VK_KP_DECIMAL         = 0x400000DC,
  VK_KP_HEXADECIMAL     = 0x400000DD,
  VK_LCTRL              = 0x400000E0,
  VK_LSHIFT             = 0x400000E1,
  VK_LALT               = 0x400000E2,
  VK_LGUI               = 0x400000E3,
  VK_RCTRL              = 0x400000E4,
  VK_RSHIFT             = 0x400000E5,
  VK_RALT               = 0x400000E6,
  VK_RGUI               = 0x400000E7,
  VK_MODE               = 0x40000101,
  VK_AUDIONEXT          = 0x40000102,
  VK_AUDIOPREV          = 0x40000103,
  VK_AUDIOSTOP          = 0x40000104,
  VK_AUDIOPLAY          = 0x40000105,
  VK_AUDIOMUTE          = 0x40000106,
  VK_MEDIASELECT        = 0x40000107,
  VK_WWW                = 0x40000108,
  VK_MAIL               = 0x40000109,
  VK_CALCULATOR         = 0x4000010A,
  VK_COMPUTER           = 0x4000010B,
  VK_AC_SEARCH          = 0x4000010C,
  VK_AC_HOME            = 0x4000010D,
  VK_AC_BACK            = 0x4000010E,
  VK_AC_FORWARD         = 0x4000010F,
  VK_AC_STOP            = 0x40000110,
  VK_AC_REFRESH         = 0x40000111,
  VK_AC_BOOKMARKS       = 0x40000112,
  VK_BRIGHTNESSDOWN     = 0x40000113,
  VK_BRIGHTNESSUP       = 0x40000114,
  VK_DISPLAYSWITCH      = 0x40000115,
  VK_KBDILLUMTOGGLE     = 0x40000116,
  VK_KBDILLUMDOWN       = 0x40000117,
  VK_KBDILLUMUP         = 0x40000118,
  VK_EJECT              = 0x40000119,
  VK_SLEEP              = 0x4000011A
} VirtualKey;

#endif