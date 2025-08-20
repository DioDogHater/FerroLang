#ifndef FERRO_TEXT_COLOR_H
#define FERRO_TEXT_COLOR_H

// Header to implement

#if defined(__linux__) || defined(__unix__)

#define SET_ATTR(x) "\x1b[" #x "m"
#define BLACK_FG SET_ATTR(30)
#define BLACK_BG SET_ATTR(40)
#define RED_FG SET_ATTR(31)
#define RED_BG SET_ATTR(41)
#define GREEN_FG SET_ATTR(32)
#define GREEN_BG SET_ATTR(42)
#define YELLOW_FG SET_ATTR(33)
#define YELLOW_BG SET_ATTR(43)
#define BLUE_FG SET_ATTR(34)
#define BLUE_BG SET_ATTR(44)
#define MAGENTA_FG SET_ATTR(35)
#define MAGENTA_BG SET_ATTR(45)
#define CYAN_FG SET_ATTR(36)
#define CYAN_BG SET_ATTR(46)
#define WHITE_FG SET_ATTR(37)
#define WHITE_BG SET_ATTR(47)
#define BOLD SET_ATTR(1)
#define ITALIC SET_ATTR(2)
#define DEFAULT_FG SET_ATTR(39)
#define DEFAULT_BG SET_ATTR(49)
#define RESET_ATTR SET_ATTR(0)

#else

#define SET_ATTR(x)
#define BLACK_FG
#define BLACK_BG
#define RED_FG
#define RED_BG
#define GREEN_FG
#define GREEN_BG
#define YELLOW_FG
#define YELLOW_BG
#define BLUE_FG
#define BLUE_BG
#define MAGENTA_FG
#define MAGENTA_BG
#define CYAN_FG
#define CYAN_BG
#define WHITE_FG
#define WHITE_BG
#define BOLD
#define ITALIC
#define DEFAULT_FG
#define DEFAULT_BG
#define RESET_ATTR

#endif

#endif