#include "ui/data_list.h"

namespace cdo {
namespace ui {
data_list::data_list(std::wstring name, term *tm, todo *td) :
    component(std::move(name), tm), keypress_handler(tm), todo_ref_(td) {
}

void data_list::update() {
  data_copy_ = getData();
  cleanup_checks();

  draw();
}

void data_list::draw() {
  term_ref_->clear(wnd_);

  auto focused_window_color = term_ref_->getColorId("focused_window");
  auto focused_item_color = term_ref_->getColorId("highlighted_item");

  box(wnd_, 0, 0);
  if (is_focused_) wattron(wnd_, COLOR_PAIR(focused_window_color));
  mvwaddwstr(wnd_, 0, 3, getTitle().c_str());
  if (is_focused_) wattroff(wnd_, COLOR_PAIR(focused_window_color));
  wmove(wnd_, 1, 1);

  int row = 0;
  for (auto &i : data_copy_) {
    if (row == selected_row_) wattron(wnd_, COLOR_PAIR(focused_item_color));
    auto check_ok = std::find(checked_rows_.begin(), checked_rows_.end(), i);
    waddwstr(wnd_, (check_ok != checked_rows_.end() ? CHECK_MARK : L" "));
    waddwstr(wnd_, L" ");
    waddwstr(wnd_, i.c_str());
    if (row == selected_row_) wattroff(wnd_, COLOR_PAIR(focused_item_color));
    wmove(wnd_, ++row + 1, 1);
  }

  term_ref_->update(wnd_);
}

void data_list::on_key_pressed(wint_t key) {
  if (!is_focused_) {
    return;
  }

  switch (key) {
    case KEY_UP: {
      if (selected_row_ > -1) {
        selected_row_--;
      } else {
        selected_row_ = (int) data_copy_.size() - 1;
      }

      draw();
      break;
    }
    case KEY_DOWN: {
      if (selected_row_ < (int) data_copy_.size() - 1) {
        selected_row_++;
      } else {
        selected_row_ = -1;
      }

      draw();
      break;
    }
    case ' ': {
      if (selected_row_ != -1) {
        auto di = data_copy_.begin();
        for (int i = 0; i < selected_row_ && di != data_copy_.end(); i++, di++) {}
        auto ci = std::find(checked_rows_.begin(), checked_rows_.end(), *di);

        if (ci == checked_rows_.end()) {
          checked_rows_.emplace_back(*di);
        } else {
          checked_rows_.erase(ci);
        }

        on_change();

        draw();
      }

      break;
    }
    default: return;
  }
}

void data_list::cleanup_checks() {
  bool list_changed = false;

  for (auto i = checked_rows_.begin(); i != checked_rows_.end(); i++) {
    if (std::find(data_copy_.begin(), data_copy_.end(), *i) == data_copy_.end()) {
      checked_rows_.erase(i);
      list_changed = true;
    }
  }

  if (list_changed) on_change();
}
}
}