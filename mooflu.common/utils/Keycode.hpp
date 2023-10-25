#pragma once
//
// Based on SDL_scancode and SDL_keycode
//

namespace InputEvent
{
#define KEY_SCANCODE_MASK (1<<30)
#define ScancodeToKeycode(X) (X | KEY_SCANCODE_MASK)

    enum Scancode
    {
        Scancode_UNKNOWN = 0,

        /** 
         *  \name Usage page 0x07
         *  
         *  These values are from usage page 0x07 (USB keyboard page).
         */
        /*@{*/

        Scancode_A = 4,
        Scancode_B = 5,
        Scancode_C = 6,
        Scancode_D = 7,
        Scancode_E = 8,
        Scancode_F = 9,
        Scancode_G = 10,
        Scancode_H = 11,
        Scancode_I = 12,
        Scancode_J = 13,
        Scancode_K = 14,
        Scancode_L = 15,
        Scancode_M = 16,
        Scancode_N = 17,
        Scancode_O = 18,
        Scancode_P = 19,
        Scancode_Q = 20,
        Scancode_R = 21,
        Scancode_S = 22,
        Scancode_T = 23,
        Scancode_U = 24,
        Scancode_V = 25,
        Scancode_W = 26,
        Scancode_X = 27,
        Scancode_Y = 28,
        Scancode_Z = 29,

        Scancode_1 = 30,
        Scancode_2 = 31,
        Scancode_3 = 32,
        Scancode_4 = 33,
        Scancode_5 = 34,
        Scancode_6 = 35,
        Scancode_7 = 36,
        Scancode_8 = 37,
        Scancode_9 = 38,
        Scancode_0 = 39,

        Scancode_RETURN = 40,
        Scancode_ESCAPE = 41,
        Scancode_BACKSPACE = 42,
        Scancode_TAB = 43,
        Scancode_SPACE = 44,

        Scancode_MINUS = 45,
        Scancode_EQUALS = 46,
        Scancode_LEFTBRACKET = 47,
        Scancode_RIGHTBRACKET = 48,
        Scancode_BACKSLASH = 49, /**< Located at the lower left of the return 
                                      *   key on ISO keyboards and at the right end 
                                      *   of the QWERTY row on ANSI keyboards. 
                                      *   Produces REVERSE SOLIDUS (backslash) and 
                                      *   VERTICAL LINE in a US layout, REVERSE 
                                      *   SOLIDUS and VERTICAL LINE in a UK Mac 
                                      *   layout, NUMBER SIGN and TILDE in a UK 
                                      *   Windows layout, DOLLAR SIGN and POUND SIGN
                                      *   in a Swiss German layout, NUMBER SIGN and 
                                      *   APOSTROPHE in a German layout, GRAVE 
                                      *   ACCENT and POUND SIGN in a French Mac 
                                      *   layout, and ASTERISK and MICRO SIGN in a 
                                      *   French Windows layout.
                                      */
        Scancode_NONUSHASH = 50, /**< ISO USB keyboards actually use this code 
                                      *   instead of 49 for the same key, but all 
                                      *   OSes I've seen treat the two codes 
                                      *   identically. So, as an implementor, unless
                                      *   your keyboard generates both of those 
                                      *   codes and your OS treats them differently,
                                      *   you should generate Scancode_BACKSLASH
                                      *   instead of this code. As a user, you 
                                      *   should not rely on this code because SDL 
                                      *   will never generate it with most (all?) 
                                      *   keyboards. 
                                      */
        Scancode_SEMICOLON = 51,
        Scancode_APOSTROPHE = 52,
        Scancode_GRAVE = 53, /**< Located in the top left corner (on both ANSI 
                                  *   and ISO keyboards). Produces GRAVE ACCENT and 
                                  *   TILDE in a US Windows layout and in US and UK 
                                  *   Mac layouts on ANSI keyboards, GRAVE ACCENT 
                                  *   and NOT SIGN in a UK Windows layout, SECTION 
                                  *   SIGN and PLUS-MINUS SIGN in US and UK Mac 
                                  *   layouts on ISO keyboards, SECTION SIGN and 
                                  *   DEGREE SIGN in a Swiss German layout (Mac: 
                                  *   only on ISO keyboards), CIRCUMFLEX ACCENT and 
                                  *   DEGREE SIGN in a German layout (Mac: only on 
                                  *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                                  *   French Windows layout, COMMERCIAL AT and 
                                  *   NUMBER SIGN in a French Mac layout on ISO 
                                  *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                                  *   SIGN in a Swiss German, German, or French Mac 
                                  *   layout on ANSI keyboards.
                                  */
        Scancode_COMMA = 54,
        Scancode_PERIOD = 55,
        Scancode_SLASH = 56,

        Scancode_CAPSLOCK = 57,

        Scancode_F1 = 58,
        Scancode_F2 = 59,
        Scancode_F3 = 60,
        Scancode_F4 = 61,
        Scancode_F5 = 62,
        Scancode_F6 = 63,
        Scancode_F7 = 64,
        Scancode_F8 = 65,
        Scancode_F9 = 66,
        Scancode_F10 = 67,
        Scancode_F11 = 68,
        Scancode_F12 = 69,

        Scancode_PRINTSCREEN = 70,
        Scancode_SCROLLLOCK = 71,
        Scancode_PAUSE = 72,
        Scancode_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                       does send code 73, not 117) */
        Scancode_HOME = 74,
        Scancode_PAGEUP = 75,
        Scancode_DELETE = 76,
        Scancode_END = 77,
        Scancode_PAGEDOWN = 78,
        Scancode_RIGHT = 79,
        Scancode_LEFT = 80,
        Scancode_DOWN = 81,
        Scancode_UP = 82,

        Scancode_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards 
                                         */
        Scancode_KP_DIVIDE = 84,
        Scancode_KP_MULTIPLY = 85,
        Scancode_KP_MINUS = 86,
        Scancode_KP_PLUS = 87,
        Scancode_KP_ENTER = 88,
        Scancode_KP_1 = 89,
        Scancode_KP_2 = 90,
        Scancode_KP_3 = 91,
        Scancode_KP_4 = 92,
        Scancode_KP_5 = 93,
        Scancode_KP_6 = 94,
        Scancode_KP_7 = 95,
        Scancode_KP_8 = 96,
        Scancode_KP_9 = 97,
        Scancode_KP_0 = 98,
        Scancode_KP_PERIOD = 99,

        Scancode_NONUSBACKSLASH = 100, /**< This is the additional key that ISO 
                                            *   keyboards have over ANSI ones, 
                                            *   located between left shift and Y. 
                                            *   Produces GRAVE ACCENT and TILDE in a
                                            *   US or UK Mac layout, REVERSE SOLIDUS
                                            *   (backslash) and VERTICAL LINE in a 
                                            *   US or UK Windows layout, and 
                                            *   LESS-THAN SIGN and GREATER-THAN SIGN
                                            *   in a Swiss German, German, or French
                                            *   layout. */
        Scancode_APPLICATION = 101, /**< windows contextual menu, compose */
        Scancode_POWER = 102, /**< The USB document says this is a status flag, 
                                   *   not a physical key - but some Mac keyboards 
                                   *   do have a power key. */
        Scancode_KP_EQUALS = 103,
        Scancode_F13 = 104,
        Scancode_F14 = 105,
        Scancode_F15 = 106,
        Scancode_F16 = 107,
        Scancode_F17 = 108,
        Scancode_F18 = 109,
        Scancode_F19 = 110,
        Scancode_F20 = 111,
        Scancode_F21 = 112,
        Scancode_F22 = 113,
        Scancode_F23 = 114,
        Scancode_F24 = 115,
        Scancode_EXECUTE = 116,
        Scancode_HELP = 117,
        Scancode_MENU = 118,
        Scancode_SELECT = 119,
        Scancode_STOP = 120,
        Scancode_AGAIN = 121,   /**< redo */
        Scancode_UNDO = 122,
        Scancode_CUT = 123,
        Scancode_COPY = 124,
        Scancode_PASTE = 125,
        Scancode_FIND = 126,
        Scancode_MUTE = 127,
        Scancode_VOLUMEUP = 128,
        Scancode_VOLUMEDOWN = 129,
    /* not sure whether there's a reason to enable these */
    /*     Scancode_LOCKINGCAPSLOCK = 130,  */
    /*     Scancode_LOCKINGNUMLOCK = 131, */
    /*     Scancode_LOCKINGSCROLLLOCK = 132, */
        Scancode_KP_COMMA = 133,
        Scancode_KP_EQUALSAS400 = 134,

        Scancode_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see 
                                                footnotes in USB doc */
        Scancode_INTERNATIONAL2 = 136,
        Scancode_INTERNATIONAL3 = 137, /**< Yen */
        Scancode_INTERNATIONAL4 = 138,
        Scancode_INTERNATIONAL5 = 139,
        Scancode_INTERNATIONAL6 = 140,
        Scancode_INTERNATIONAL7 = 141,
        Scancode_INTERNATIONAL8 = 142,
        Scancode_INTERNATIONAL9 = 143,
        Scancode_LANG1 = 144, /**< Hangul/English toggle */
        Scancode_LANG2 = 145, /**< Hanja conversion */
        Scancode_LANG3 = 146, /**< Katakana */
        Scancode_LANG4 = 147, /**< Hiragana */
        Scancode_LANG5 = 148, /**< Zenkaku/Hankaku */
        Scancode_LANG6 = 149, /**< reserved */
        Scancode_LANG7 = 150, /**< reserved */
        Scancode_LANG8 = 151, /**< reserved */
        Scancode_LANG9 = 152, /**< reserved */

        Scancode_ALTERASE = 153, /**< Erase-Eaze */
        Scancode_SYSREQ = 154,
        Scancode_CANCEL = 155,
        Scancode_CLEAR = 156,
        Scancode_PRIOR = 157,
        Scancode_RETURN2 = 158,
        Scancode_SEPARATOR = 159,
        Scancode_OUT = 160,
        Scancode_OPER = 161,
        Scancode_CLEARAGAIN = 162,
        Scancode_CRSEL = 163,
        Scancode_EXSEL = 164,

        Scancode_KP_00 = 176,
        Scancode_KP_000 = 177,
        Scancode_THOUSANDSSEPARATOR = 178,
        Scancode_DECIMALSEPARATOR = 179,
        Scancode_CURRENCYUNIT = 180,
        Scancode_CURRENCYSUBUNIT = 181,
        Scancode_KP_LEFTPAREN = 182,
        Scancode_KP_RIGHTPAREN = 183,
        Scancode_KP_LEFTBRACE = 184,
        Scancode_KP_RIGHTBRACE = 185,
        Scancode_KP_TAB = 186,
        Scancode_KP_BACKSPACE = 187,
        Scancode_KP_A = 188,
        Scancode_KP_B = 189,
        Scancode_KP_C = 190,
        Scancode_KP_D = 191,
        Scancode_KP_E = 192,
        Scancode_KP_F = 193,
        Scancode_KP_XOR = 194,
        Scancode_KP_POWER = 195,
        Scancode_KP_PERCENT = 196,
        Scancode_KP_LESS = 197,
        Scancode_KP_GREATER = 198,
        Scancode_KP_AMPERSAND = 199,
        Scancode_KP_DBLAMPERSAND = 200,
        Scancode_KP_VERTICALBAR = 201,
        Scancode_KP_DBLVERTICALBAR = 202,
        Scancode_KP_COLON = 203,
        Scancode_KP_HASH = 204,
        Scancode_KP_SPACE = 205,
        Scancode_KP_AT = 206,
        Scancode_KP_EXCLAM = 207,
        Scancode_KP_MEMSTORE = 208,
        Scancode_KP_MEMRECALL = 209,
        Scancode_KP_MEMCLEAR = 210,
        Scancode_KP_MEMADD = 211,
        Scancode_KP_MEMSUBTRACT = 212,
        Scancode_KP_MEMMULTIPLY = 213,
        Scancode_KP_MEMDIVIDE = 214,
        Scancode_KP_PLUSMINUS = 215,
        Scancode_KP_CLEAR = 216,
        Scancode_KP_CLEARENTRY = 217,
        Scancode_KP_BINARY = 218,
        Scancode_KP_OCTAL = 219,
        Scancode_KP_DECIMAL = 220,
        Scancode_KP_HEXADECIMAL = 221,

        Scancode_LCTRL = 224,
        Scancode_LSHIFT = 225,
        Scancode_LALT = 226, /**< alt, option */
        Scancode_LGUI = 227, /**< windows, command (apple), meta */
        Scancode_RCTRL = 228,
        Scancode_RSHIFT = 229,
        Scancode_RALT = 230, /**< alt gr, option */
        Scancode_RGUI = 231, /**< windows, command (apple), meta */

        Scancode_MODE = 257,    /**< I'm not sure if this is really not covered 
                                     *   by any of the above, but since there's a 
                                     *   special KMOD_MODE for it I'm adding it here
                                     */
        
        /*@}*//*Usage page 0x07*/

        /**
         *  \name Usage page 0x0C
         *  
         *  These values are mapped from usage page 0x0C (USB consumer page).
         */
        /*@{*/

        Scancode_AUDIONEXT = 258,
        Scancode_AUDIOPREV = 259,
        Scancode_AUDIOSTOP = 260,
        Scancode_AUDIOPLAY = 261,
        Scancode_AUDIOMUTE = 262,
        Scancode_MEDIASELECT = 263,
        Scancode_WWW = 264,
        Scancode_MAIL = 265,
        Scancode_CALCULATOR = 266,
        Scancode_COMPUTER = 267,
        Scancode_AC_SEARCH = 268,
        Scancode_AC_HOME = 269,
        Scancode_AC_BACK = 270,
        Scancode_AC_FORWARD = 271,
        Scancode_AC_STOP = 272,
        Scancode_AC_REFRESH = 273,
        Scancode_AC_BOOKMARKS = 274,
        
        /*@}*//*Usage page 0x0C*/

        /**
         *  \name Walther keys
         *  
         *  These are values that Christian Walther added (for mac keyboard?).
         */
        /*@{*/

        Scancode_BRIGHTNESSDOWN = 275,
        Scancode_BRIGHTNESSUP = 276,
        Scancode_DISPLAYSWITCH = 277, /**< display mirroring/dual display 
                                               switch, video mode switch */
        Scancode_KBDILLUMTOGGLE = 278,
        Scancode_KBDILLUMDOWN = 279,
        Scancode_KBDILLUMUP = 280,
        Scancode_EJECT = 281,
        Scancode_SLEEP = 282,
        
        /*@}*//*Walther keys*/

        /* Add any other keys here. */

        NUM_SCANCODES = 512 /**< not a key, just marks the number of scancodes 
                                     for array bounds */
    };

    enum KeyCode
    {
        Keycode_UNKNOWN = 0,

        Keycode_RETURN = '\r',
        Keycode_ESCAPE = '\033',
        Keycode_BACKSPACE = '\b',
        Keycode_TAB = '\t',
        Keycode_SPACE = ' ',
        Keycode_EXCLAIM = '!',
        Keycode_QUOTEDBL = '"',
        Keycode_HASH = '#',
        Keycode_PERCENT = '%',
        Keycode_DOLLAR = '$',
        Keycode_AMPERSAND = '&',
        Keycode_QUOTE = '\'',
        Keycode_LEFTPAREN = '(',
        Keycode_RIGHTPAREN = ')',
        Keycode_ASTERISK = '*',
        Keycode_PLUS = '+',
        Keycode_COMMA = ',',
        Keycode_MINUS = '-',
        Keycode_PERIOD = '.',
        Keycode_SLASH = '/',
        Keycode_0 = '0',
        Keycode_1 = '1',
        Keycode_2 = '2',
        Keycode_3 = '3',
        Keycode_4 = '4',
        Keycode_5 = '5',
        Keycode_6 = '6',
        Keycode_7 = '7',
        Keycode_8 = '8',
        Keycode_9 = '9',
        Keycode_COLON = ':',
        Keycode_SEMICOLON = ';',
        Keycode_LESS = '<',
        Keycode_EQUALS = '=',
        Keycode_GREATER = '>',
        Keycode_QUESTION = '?',
        Keycode_AT = '@',
        /* 
           Skip uppercase letters
         */
        Keycode_LEFTBRACKET = '[',
        Keycode_BACKSLASH = '\\',
        Keycode_RIGHTBRACKET = ']',
        Keycode_CARET = '^',
        Keycode_UNDERSCORE = '_',
        Keycode_BACKQUOTE = '`',
        Keycode_a = 'a',
        Keycode_b = 'b',
        Keycode_c = 'c',
        Keycode_d = 'd',
        Keycode_e = 'e',
        Keycode_f = 'f',
        Keycode_g = 'g',
        Keycode_h = 'h',
        Keycode_i = 'i',
        Keycode_j = 'j',
        Keycode_k = 'k',
        Keycode_l = 'l',
        Keycode_m = 'm',
        Keycode_n = 'n',
        Keycode_o = 'o',
        Keycode_p = 'p',
        Keycode_q = 'q',
        Keycode_r = 'r',
        Keycode_s = 's',
        Keycode_t = 't',
        Keycode_u = 'u',
        Keycode_v = 'v',
        Keycode_w = 'w',
        Keycode_x = 'x',
        Keycode_y = 'y',
        Keycode_z = 'z',

        Keycode_CAPSLOCK = ScancodeToKeycode(Scancode_CAPSLOCK),

        Keycode_F1 = ScancodeToKeycode(Scancode_F1),
        Keycode_F2 = ScancodeToKeycode(Scancode_F2),
        Keycode_F3 = ScancodeToKeycode(Scancode_F3),
        Keycode_F4 = ScancodeToKeycode(Scancode_F4),
        Keycode_F5 = ScancodeToKeycode(Scancode_F5),
        Keycode_F6 = ScancodeToKeycode(Scancode_F6),
        Keycode_F7 = ScancodeToKeycode(Scancode_F7),
        Keycode_F8 = ScancodeToKeycode(Scancode_F8),
        Keycode_F9 = ScancodeToKeycode(Scancode_F9),
        Keycode_F10 = ScancodeToKeycode(Scancode_F10),
        Keycode_F11 = ScancodeToKeycode(Scancode_F11),
        Keycode_F12 = ScancodeToKeycode(Scancode_F12),

        Keycode_PRINTSCREEN = ScancodeToKeycode(Scancode_PRINTSCREEN),
        Keycode_SCROLLLOCK = ScancodeToKeycode(Scancode_SCROLLLOCK),
        Keycode_PAUSE = ScancodeToKeycode(Scancode_PAUSE),
        Keycode_INSERT = ScancodeToKeycode(Scancode_INSERT),
        Keycode_HOME = ScancodeToKeycode(Scancode_HOME),
        Keycode_PAGEUP = ScancodeToKeycode(Scancode_PAGEUP),
        Keycode_DELETE = '\177',
        Keycode_END = ScancodeToKeycode(Scancode_END),
        Keycode_PAGEDOWN = ScancodeToKeycode(Scancode_PAGEDOWN),
        Keycode_RIGHT = ScancodeToKeycode(Scancode_RIGHT),
        Keycode_LEFT = ScancodeToKeycode(Scancode_LEFT),
        Keycode_DOWN = ScancodeToKeycode(Scancode_DOWN),
        Keycode_UP = ScancodeToKeycode(Scancode_UP),

        Keycode_NUMLOCKCLEAR = ScancodeToKeycode(Scancode_NUMLOCKCLEAR),
        Keycode_KP_DIVIDE = ScancodeToKeycode(Scancode_KP_DIVIDE),
        Keycode_KP_MULTIPLY = ScancodeToKeycode(Scancode_KP_MULTIPLY),
        Keycode_KP_MINUS = ScancodeToKeycode(Scancode_KP_MINUS),
        Keycode_KP_PLUS = ScancodeToKeycode(Scancode_KP_PLUS),
        Keycode_KP_ENTER = ScancodeToKeycode(Scancode_KP_ENTER),
        Keycode_KP_1 = ScancodeToKeycode(Scancode_KP_1),
        Keycode_KP_2 = ScancodeToKeycode(Scancode_KP_2),
        Keycode_KP_3 = ScancodeToKeycode(Scancode_KP_3),
        Keycode_KP_4 = ScancodeToKeycode(Scancode_KP_4),
        Keycode_KP_5 = ScancodeToKeycode(Scancode_KP_5),
        Keycode_KP_6 = ScancodeToKeycode(Scancode_KP_6),
        Keycode_KP_7 = ScancodeToKeycode(Scancode_KP_7),
        Keycode_KP_8 = ScancodeToKeycode(Scancode_KP_8),
        Keycode_KP_9 = ScancodeToKeycode(Scancode_KP_9),
        Keycode_KP_0 = ScancodeToKeycode(Scancode_KP_0),
        Keycode_KP_PERIOD = ScancodeToKeycode(Scancode_KP_PERIOD),

        Keycode_APPLICATION = ScancodeToKeycode(Scancode_APPLICATION),
        Keycode_POWER = ScancodeToKeycode(Scancode_POWER),
        Keycode_KP_EQUALS = ScancodeToKeycode(Scancode_KP_EQUALS),
        Keycode_F13 = ScancodeToKeycode(Scancode_F13),
        Keycode_F14 = ScancodeToKeycode(Scancode_F14),
        Keycode_F15 = ScancodeToKeycode(Scancode_F15),
        Keycode_F16 = ScancodeToKeycode(Scancode_F16),
        Keycode_F17 = ScancodeToKeycode(Scancode_F17),
        Keycode_F18 = ScancodeToKeycode(Scancode_F18),
        Keycode_F19 = ScancodeToKeycode(Scancode_F19),
        Keycode_F20 = ScancodeToKeycode(Scancode_F20),
        Keycode_F21 = ScancodeToKeycode(Scancode_F21),
        Keycode_F22 = ScancodeToKeycode(Scancode_F22),
        Keycode_F23 = ScancodeToKeycode(Scancode_F23),
        Keycode_F24 = ScancodeToKeycode(Scancode_F24),
        Keycode_EXECUTE = ScancodeToKeycode(Scancode_EXECUTE),
        Keycode_HELP = ScancodeToKeycode(Scancode_HELP),
        Keycode_MENU = ScancodeToKeycode(Scancode_MENU),
        Keycode_SELECT = ScancodeToKeycode(Scancode_SELECT),
        Keycode_STOP = ScancodeToKeycode(Scancode_STOP),
        Keycode_AGAIN = ScancodeToKeycode(Scancode_AGAIN),
        Keycode_UNDO = ScancodeToKeycode(Scancode_UNDO),
        Keycode_CUT = ScancodeToKeycode(Scancode_CUT),
        Keycode_COPY = ScancodeToKeycode(Scancode_COPY),
        Keycode_PASTE = ScancodeToKeycode(Scancode_PASTE),
        Keycode_FIND = ScancodeToKeycode(Scancode_FIND),
        Keycode_MUTE = ScancodeToKeycode(Scancode_MUTE),
        Keycode_VOLUMEUP = ScancodeToKeycode(Scancode_VOLUMEUP),
        Keycode_VOLUMEDOWN = ScancodeToKeycode(Scancode_VOLUMEDOWN),
        Keycode_KP_COMMA = ScancodeToKeycode(Scancode_KP_COMMA),
        Keycode_KP_EQUALSAS400 =
            ScancodeToKeycode(Scancode_KP_EQUALSAS400),

        Keycode_ALTERASE = ScancodeToKeycode(Scancode_ALTERASE),
        Keycode_SYSREQ = ScancodeToKeycode(Scancode_SYSREQ),
        Keycode_CANCEL = ScancodeToKeycode(Scancode_CANCEL),
        Keycode_CLEAR = ScancodeToKeycode(Scancode_CLEAR),
        Keycode_PRIOR = ScancodeToKeycode(Scancode_PRIOR),
        Keycode_RETURN2 = ScancodeToKeycode(Scancode_RETURN2),
        Keycode_SEPARATOR = ScancodeToKeycode(Scancode_SEPARATOR),
        Keycode_OUT = ScancodeToKeycode(Scancode_OUT),
        Keycode_OPER = ScancodeToKeycode(Scancode_OPER),
        Keycode_CLEARAGAIN = ScancodeToKeycode(Scancode_CLEARAGAIN),
        Keycode_CRSEL = ScancodeToKeycode(Scancode_CRSEL),
        Keycode_EXSEL = ScancodeToKeycode(Scancode_EXSEL),

        Keycode_KP_00 = ScancodeToKeycode(Scancode_KP_00),
        Keycode_KP_000 = ScancodeToKeycode(Scancode_KP_000),
        Keycode_THOUSANDSSEPARATOR =
            ScancodeToKeycode(Scancode_THOUSANDSSEPARATOR),
        Keycode_DECIMALSEPARATOR =
            ScancodeToKeycode(Scancode_DECIMALSEPARATOR),
        Keycode_CURRENCYUNIT = ScancodeToKeycode(Scancode_CURRENCYUNIT),
        Keycode_CURRENCYSUBUNIT =
            ScancodeToKeycode(Scancode_CURRENCYSUBUNIT),
        Keycode_KP_LEFTPAREN = ScancodeToKeycode(Scancode_KP_LEFTPAREN),
        Keycode_KP_RIGHTPAREN = ScancodeToKeycode(Scancode_KP_RIGHTPAREN),
        Keycode_KP_LEFTBRACE = ScancodeToKeycode(Scancode_KP_LEFTBRACE),
        Keycode_KP_RIGHTBRACE = ScancodeToKeycode(Scancode_KP_RIGHTBRACE),
        Keycode_KP_TAB = ScancodeToKeycode(Scancode_KP_TAB),
        Keycode_KP_BACKSPACE = ScancodeToKeycode(Scancode_KP_BACKSPACE),
        Keycode_KP_A = ScancodeToKeycode(Scancode_KP_A),
        Keycode_KP_B = ScancodeToKeycode(Scancode_KP_B),
        Keycode_KP_C = ScancodeToKeycode(Scancode_KP_C),
        Keycode_KP_D = ScancodeToKeycode(Scancode_KP_D),
        Keycode_KP_E = ScancodeToKeycode(Scancode_KP_E),
        Keycode_KP_F = ScancodeToKeycode(Scancode_KP_F),
        Keycode_KP_XOR = ScancodeToKeycode(Scancode_KP_XOR),
        Keycode_KP_POWER = ScancodeToKeycode(Scancode_KP_POWER),
        Keycode_KP_PERCENT = ScancodeToKeycode(Scancode_KP_PERCENT),
        Keycode_KP_LESS = ScancodeToKeycode(Scancode_KP_LESS),
        Keycode_KP_GREATER = ScancodeToKeycode(Scancode_KP_GREATER),
        Keycode_KP_AMPERSAND = ScancodeToKeycode(Scancode_KP_AMPERSAND),
        Keycode_KP_DBLAMPERSAND =
            ScancodeToKeycode(Scancode_KP_DBLAMPERSAND),
        Keycode_KP_VERTICALBAR =
            ScancodeToKeycode(Scancode_KP_VERTICALBAR),
        Keycode_KP_DBLVERTICALBAR =
            ScancodeToKeycode(Scancode_KP_DBLVERTICALBAR),
        Keycode_KP_COLON = ScancodeToKeycode(Scancode_KP_COLON),
        Keycode_KP_HASH = ScancodeToKeycode(Scancode_KP_HASH),
        Keycode_KP_SPACE = ScancodeToKeycode(Scancode_KP_SPACE),
        Keycode_KP_AT = ScancodeToKeycode(Scancode_KP_AT),
        Keycode_KP_EXCLAM = ScancodeToKeycode(Scancode_KP_EXCLAM),
        Keycode_KP_MEMSTORE = ScancodeToKeycode(Scancode_KP_MEMSTORE),
        Keycode_KP_MEMRECALL = ScancodeToKeycode(Scancode_KP_MEMRECALL),
        Keycode_KP_MEMCLEAR = ScancodeToKeycode(Scancode_KP_MEMCLEAR),
        Keycode_KP_MEMADD = ScancodeToKeycode(Scancode_KP_MEMADD),
        Keycode_KP_MEMSUBTRACT =
            ScancodeToKeycode(Scancode_KP_MEMSUBTRACT),
        Keycode_KP_MEMMULTIPLY =
            ScancodeToKeycode(Scancode_KP_MEMMULTIPLY),
        Keycode_KP_MEMDIVIDE = ScancodeToKeycode(Scancode_KP_MEMDIVIDE),
        Keycode_KP_PLUSMINUS = ScancodeToKeycode(Scancode_KP_PLUSMINUS),
        Keycode_KP_CLEAR = ScancodeToKeycode(Scancode_KP_CLEAR),
        Keycode_KP_CLEARENTRY = ScancodeToKeycode(Scancode_KP_CLEARENTRY),
        Keycode_KP_BINARY = ScancodeToKeycode(Scancode_KP_BINARY),
        Keycode_KP_OCTAL = ScancodeToKeycode(Scancode_KP_OCTAL),
        Keycode_KP_DECIMAL = ScancodeToKeycode(Scancode_KP_DECIMAL),
        Keycode_KP_HEXADECIMAL =
            ScancodeToKeycode(Scancode_KP_HEXADECIMAL),

        Keycode_LCTRL = ScancodeToKeycode(Scancode_LCTRL),
        Keycode_LSHIFT = ScancodeToKeycode(Scancode_LSHIFT),
        Keycode_LALT = ScancodeToKeycode(Scancode_LALT),
        Keycode_LGUI = ScancodeToKeycode(Scancode_LGUI),
        Keycode_RCTRL = ScancodeToKeycode(Scancode_RCTRL),
        Keycode_RSHIFT = ScancodeToKeycode(Scancode_RSHIFT),
        Keycode_RALT = ScancodeToKeycode(Scancode_RALT),
        Keycode_RGUI = ScancodeToKeycode(Scancode_RGUI),

        Keycode_MODE = ScancodeToKeycode(Scancode_MODE),

        Keycode_AUDIONEXT = ScancodeToKeycode(Scancode_AUDIONEXT),
        Keycode_AUDIOPREV = ScancodeToKeycode(Scancode_AUDIOPREV),
        Keycode_AUDIOSTOP = ScancodeToKeycode(Scancode_AUDIOSTOP),
        Keycode_AUDIOPLAY = ScancodeToKeycode(Scancode_AUDIOPLAY),
        Keycode_AUDIOMUTE = ScancodeToKeycode(Scancode_AUDIOMUTE),
        Keycode_MEDIASELECT = ScancodeToKeycode(Scancode_MEDIASELECT),
        Keycode_WWW = ScancodeToKeycode(Scancode_WWW),
        Keycode_MAIL = ScancodeToKeycode(Scancode_MAIL),
        Keycode_CALCULATOR = ScancodeToKeycode(Scancode_CALCULATOR),
        Keycode_COMPUTER = ScancodeToKeycode(Scancode_COMPUTER),
        Keycode_AC_SEARCH = ScancodeToKeycode(Scancode_AC_SEARCH),
        Keycode_AC_HOME = ScancodeToKeycode(Scancode_AC_HOME),
        Keycode_AC_BACK = ScancodeToKeycode(Scancode_AC_BACK),
        Keycode_AC_FORWARD = ScancodeToKeycode(Scancode_AC_FORWARD),
        Keycode_AC_STOP = ScancodeToKeycode(Scancode_AC_STOP),
        Keycode_AC_REFRESH = ScancodeToKeycode(Scancode_AC_REFRESH),
        Keycode_AC_BOOKMARKS = ScancodeToKeycode(Scancode_AC_BOOKMARKS),

        Keycode_BRIGHTNESSDOWN =
            ScancodeToKeycode(Scancode_BRIGHTNESSDOWN),
        Keycode_BRIGHTNESSUP = ScancodeToKeycode(Scancode_BRIGHTNESSUP),
        Keycode_DISPLAYSWITCH = ScancodeToKeycode(Scancode_DISPLAYSWITCH),
        Keycode_KBDILLUMTOGGLE =
            ScancodeToKeycode(Scancode_KBDILLUMTOGGLE),
        Keycode_KBDILLUMDOWN = ScancodeToKeycode(Scancode_KBDILLUMDOWN),
        Keycode_KBDILLUMUP = ScancodeToKeycode(Scancode_KBDILLUMUP),
        Keycode_EJECT = ScancodeToKeycode(Scancode_EJECT),
        Keycode_SLEEP = ScancodeToKeycode(Scancode_SLEEP)
    };

    enum Modifier
    {
        Modifier_NONE = 0x0000,
        Modifier_LSHIFT = 0x0001,
        Modifier_RSHIFT = 0x0002,
        Modifier_LCTRL = 0x0040,
        Modifier_RCTRL = 0x0080,
        Modifier_LALT = 0x0100,
        Modifier_RALT = 0x0200,
        Modifier_LGUI = 0x0400,
        Modifier_RGUI = 0x0800,
        Modifier_NUM = 0x1000,
        Modifier_CAPS = 0x2000,
        Modifier_MODE = 0x4000,
        Modifier_RESERVED = 0x8000
    };

const int Modifier_CTRL  = (Modifier_LCTRL|Modifier_RCTRL);
const int Modifier_SHIFT = (Modifier_LSHIFT|Modifier_RSHIFT);
const int Modifier_ALT   = (Modifier_LALT|Modifier_RALT);
const int Modifier_GUI   = (Modifier_LGUI|Modifier_RGUI);

#ifdef __APPLE__
const int Modifier_MAIN = Modifier_GUI;
#else
const int Modifier_MAIN = Modifier_CTRL;
#endif

} // namespace Input
