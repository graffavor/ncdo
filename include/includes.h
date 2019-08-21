#ifndef CURSEDO_INCLUDES_H
#define CURSEDO_INCLUDES_H

#ifdef __APPLE__
#define _XOPEN_SOURCE_EXTENDED
#include <ncurses.h>
#else
#include <ncursesw/ncurses.h>
#endif

#include <string>

#endif //CURSEDO_INCLUDES_H
