#ifndef COLORS_H
#define COLORS_H

/*
	\033[38;2;<r>;<g>;<b>m     #Select RGB foreground color
	\033[48;2;<r>;<g>;<b>m     #Select RGB background color
*/

#define RESET_ALL  0
#define BRIGHT     1
#define DIM        2
#define UNDERSCORE 4
#define BLINK      5
#define REVERSE    7
#define HIDDEN     8

#define FG_BLACK   30
#define FG_RED     31
#define FG_GREEN   32
#define FG_YELLOW  33
#define FG_BLUE    34
#define FG_MAGENTA 35
#define FG_CYAN    36
#define FG_WHITE   37

#define BG_BLACK   40
#define BG_RED     41
#define BG_GREEN   42
#define BG_YELLOW  43
#define BG_BLUE    44
#define BG_MAGENTA 45
#define BG_CYAN    46
#define BG_WHITE   47

#endif
