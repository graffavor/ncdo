#ifndef NCDO_INCLUDE_UI_EDITOR_H_
#define NCDO_INCLUDE_UI_EDITOR_H_

#include <string>
#include "includes.h"
#include "component.h"
#include "hook.h"
#include "term.h"

using namespace std;

namespace cdo {
namespace ui {
class editor : component {
 public:
  editor() = delete;
  explicit editor(term *t) : component(L"editor", t) {};

  void start(wstring prefix = L"> ", int at_y = 0, int at_x = 0, int width = 20);
  void reset();
  editor& setBuffer(const wstring& v) { buffer = v; return *this; }

  void draw() override;

  wstring value();

  void handleInput(wint_t key);

  hook<void(wstring)> done;
 protected:
  void insert(wchar_t c);
  void remove_before();
  void remove_after();
  void curs_eol();
  void curs_beg();
 private:
  wstring buffer, pre;
  int wnd_width = 0;
  int cursor_position = 0;
  int frame_left = 0;
  int frame_right = 0;
};
}
}

#endif //NCDO_INCLUDE_UI_EDITOR_H_
