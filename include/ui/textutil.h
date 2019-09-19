#ifndef NCDO_INCLUDE_UI_TEXTUTIL_H_
#define NCDO_INCLUDE_UI_TEXTUTIL_H_

#include "includes.h"

namespace cdo {
namespace ui {
// write text wrapping into multiple lines if text length exceeds window width
// return number of written lines
int wwrite_wrap(WINDOW* w, const std::wstring& text, int pad_x = 0, int pad_y = 0);
}
}

#endif //NCDO_INCLUDE_UI_TEXTUTIL_H_
