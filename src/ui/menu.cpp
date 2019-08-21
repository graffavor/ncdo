#include "ui/menu.h"

namespace cdo {
namespace ui {
menu::menu(cdo::term *t, cdo::ui::menu_state p) : component(L"menu", t), persistent_(std::move(p)) {
  for (auto &i : persistent_) {
    pa_length_ += (int)i.length() + 1;
  }
}

void menu::draw() {
  term_ref_->clear(wnd_);

  int w = getmaxx(wnd_);

  if (!states_.empty()) {
    for (auto &i: states_[states_.size() - 1]) {
      waddwstr(wnd_, i.c_str());
      waddch(wnd_, L' ');
    }
  }

  int pa_x = w - pa_length_;
  wmove(wnd_, 0, pa_x);
  for (auto &i : persistent_) {
    waddwstr(wnd_, i.c_str());
    waddch(wnd_, L' ');
  }

  term_ref_->update(wnd_);
}

void menu::push_state(cdo::ui::menu_state s) {
  states_.emplace_back(std::move(s));
  draw();
}

void menu::pop_state() {
  states_.pop_back();
  draw();
}
}
}