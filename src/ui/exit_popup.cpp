#include "ui/exit_popup.h"
#include "ui/textutil.h"

namespace cdo {
namespace ui {
exit_popup::exit_popup(cdo::term *t, cdo::td::todo *td):
  component(L"exit_popup", t), rawpress_handler(t), todo_ref_(td) {
  t->preventExit();
  t->setMode(MODE_SKIP);

  int w = t->getWidth() / 3;
  int h = 6;
  init(w, h, t->getWidth() / 2 - w / 2, t->getHeight() / 2 - h / 2);
}

void exit_popup::draw() {
  auto color = term_ref_->getColorId("exit_popup");
  auto btn_color = term_ref_->getColorId("exit_popup_btn");
  wbkgd(wnd_, COLOR_PAIR(color));

  wwrite_wrap(wnd_, L"You have unsaved changes. Save before exit?", 2, 1);

  wmove(wnd_, 4, 2);
  if (selected_item_ == 0) wattron(wnd_, COLOR_PAIR(btn_color));
  waddwstr(wnd_, L"  NO  ");
  wattroff(wnd_, COLOR_PAIR(btn_color));

  wmove(wnd_, 4, getmaxx(wnd_) - 9);
  if (selected_item_ == 1) wattron(wnd_, COLOR_PAIR(btn_color));
  waddwstr(wnd_, L"  YES  ");
  wattroff(wnd_, COLOR_PAIR(btn_color));

  term_ref_->update(wnd_);
}

void exit_popup::on_key_pressed(wint_t key) {
  switch(key) {
    case KEY_LEFT: {
      if (selected_item_ == 1)
        selected_item_ = 0;
      draw();
      break;
    }
    case KEY_RIGHT: {
      if (selected_item_ == 0)
        selected_item_ = 1;
      draw();
      break;
    }
    case '\n': {
      if (selected_item_ == 1) {
        todo_ref_->save();
      }

      term_ref_->stop();
      break;
    }
    default: return;
  }
}
}
}