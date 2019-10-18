#include <sstream>
#include "ui/task_list.h"

using namespace std;

namespace cdo {
namespace ui {
task_list::task_list(todo *td, cdo::term *tm) :
    component(L"task-list", tm), keypress_handler(tm), todo_ref_(td),
    editor_(tm), project_list_(td, tm), context_list_(td, tm),
    menu_(tm, {L"?: Help", L"F4: Quit"}), help_(tm) {
  this->is_scrollable_ = false;
  tm->on_init += [this]() {
    auto term_width = this->term_ref_->getWidth();
    auto term_height = this->term_ref_->getHeight();
    auto w = (int) ((float) term_width * 0.7f);
    this->init(w, term_height - 1);

    this->project_list_.init(term_width - w, (term_height) / 2, w, 0);
    this->project_list_.update();

    this->context_list_.init(term_width - w, (term_height - 1) / 2, w, (term_height) / 2);
    this->context_list_.update();

    this->menu_.init(term_width - 1, 2, 1, term_height - 1);
    this->menu_.push_state({L"F: Filter", L"S: Sort", L"+,-: Arrange", L"E: Edit", L"X: Remove"});
    this->menu_.draw();

    this->focus();
    this->update_data();
  };

  tm->on_exit += [this]() {
    if (this->todo_ref_->isChanged() && this->exit_popup_ == nullptr) {
      // dont use std::make_shared to avoid deleted copy constructor
      this->blur();
      this->exit_popup_ = std::shared_ptr<exit_popup>(new exit_popup(this->term_ref_, this->todo_ref_));
      this->exit_popup_->draw();
    }
  };

  tm->on_resize += [this]() {
    // todo: currently its broken
    auto term_width = this->term_ref_->getWidth();
    auto term_height = this->term_ref_->getHeight();
    auto w = (int) ((float) term_width * 0.7f);

    this->resize(w, term_height - 1);
    this->project_list_.resize(term_width - w, (term_height) / 2, w, 0);
    this->context_list_.resize(term_width - w, (term_height - 1) / 2, w, (term_height) / 2);
    this->menu_.resize(term_width, 2, 0, term_height - 1);

    this->term_ref_->update();
  };

  help_.on_close += [this]() {
    this->is_help_open_ = false;
    term_ref_->setMode(term::mode::MODE_NORMAL);
    this->draw();
    this->project_list_.draw();
    this->context_list_.draw();
    this->menu_.draw();
  };

  project_list_.on_change += [this]() {
    this->project_filter_ = this->project_list_.getSelected();
    this->selected_item_ = -1;
    this->update_data();
  };

  context_list_.on_change += [this]() {
    this->context_filter_ = this->context_list_.getSelected();
    this->selected_item_ = -1;
    this->update_data();
  };

  editor_.done += [this](const wstring &buf) {
    if (this->is_editing_) {
      this->is_editing_ = false;

      if (!buf.empty()) {
        if (is_creating_) {
          this->todo_ref_->addTask(buf, this->editing_at_ - 1);
        } else {
          this->todo_ref_->updateTask(buf, this->editing_at_ - 1);
        }
      }

      this->is_creating_ = false;
      this->editor_.reset();
      this->update_data();
      this->project_list_.update();
      this->context_list_.update();
    } else if (this->is_searching_) {
      this->is_searching_ = false;
      this->search_filter_ = buf;

      this->editor_.reset();
      this->update_data();
      this->menu_.draw();
    }
  };
}

void task_list::draw() {
  term_ref_->clear(wnd_);

  auto focused_item_color = term_ref_->getColorId("highlighted_item");
  auto focused_window_color = term_ref_->getColorId("focused_window");

  box(wnd_, 0, 0);
  if (is_focused_) wattron(wnd_, COLOR_PAIR(focused_window_color));

  if (todo_ref_->isChanged()) {
    mvwaddwstr(wnd_, 0, 1, L" *");
  }

  mvwaddwstr(wnd_, 0, 3, L" Tasks: ");
  wattroff(wnd_, COLOR_PAIR(focused_window_color));

  if (!search_filter_.empty()) {
    waddch(wnd_, L'[');
    waddwstr(wnd_, search_filter_.c_str());
    waddch(wnd_, L']');
  }

  wmove(wnd_, 1, 1);

  int row = 0, data_index = 0;
  for (auto &task : data_copy_) {
    if (data_index == selected_item_ && !is_editing_) wattron(wnd_, COLOR_PAIR(focused_item_color));
    wprintw(wnd_, "%d. ", data_index + 1);

    waddch(wnd_, '[');
    if (task.done) {
      waddch(wnd_, 'x');
    } else {
      waddch(wnd_, ' ');
    }
    waddwstr(wnd_, L"] ");

    int pref_length = getcurx(wnd_);

    if (task.priority != 0) {
      wattron(wnd_, A_ITALIC);
      waddch(wnd_, '(');
      waddch(wnd_, task.priority);
      waddch(wnd_, ')');
      wattroff(wnd_, A_ITALIC);
      waddch(wnd_, ' ');
    }

    int max_length = getmaxx(wnd_) - getcurx(wnd_) - /*borders*/2;
    int sum_length = 0;

    wstringstream ss(task.desc);
    wstring tmp;

    while (ss >> tmp) {
      if (((int) (tmp.length() + 1) + sum_length) > max_length - 1) {
        wmove(wnd_, ++row + 1, pref_length);
        sum_length = 0;
      }

      if (tmp[0] == '+' || tmp[0] == '@') wattron(wnd_, A_DIM);

      waddwstr(wnd_, tmp.c_str());
      wattroff(wnd_, A_DIM);
      waddch(wnd_, ' ');

      sum_length += (int) tmp.length() + 1;
    }

    wattroff(wnd_, COLOR_PAIR(focused_item_color));

    row += (is_editing_ && is_creating_ && row == (editing_at_ - 2) ? 2 : 1);
    data_index++;
    wmove(wnd_, row + 1, 1);
  }

  term_ref_->update(wnd_);
}

void task_list::on_key_pressed(wint_t key) {
  if (is_help_open_) {
    return;
  }

  // as this component controls focus of all child components we have to capture
  // focus switching events first
  if (term_ref_->getMode() == term::mode::MODE_NORMAL) {
    if (key == KEY_LEFT) {
      update_focus(-1);
    } else if (key == KEY_RIGHT || key == '\t') {
      update_focus(1);
    }
  }

  if (!is_focused_) {
    return;
  }

  if (is_editing_ || is_searching_) {
    editor_.handleInput(key);
    return;
  }

  if (should_revert_menu_state_) {
    menu_.pop_state();
    should_revert_menu_state_ = false;
  }

  switch (key) {
    case KEY_UP: {
      if (selected_item_ > -1) {
        selected_item_--;
      } else {
        selected_item_ = (int) data_copy_.size() - 1;
      }

      draw();
      break;
    }
    case KEY_DOWN: {
      if (selected_item_ == (int) data_copy_.size() - 1) {
        selected_item_ = -1;
      } else {
        selected_item_++;
      }

      draw();
      break;
    }
    case ' ': {
      if (selected_item_ >= 0 && (size_t) selected_item_ < data_copy_.size()) {
        todo_ref_->setComplete(data_copy_[selected_item_], !(*todo_ref_)[selected_item_].done);
        update_data();
      }

      break;
    }
    case KEY_ENTER:
    case '\n': {
      if (is_focused_ && !is_editing_) {
        // position and number of element that being edited (created)
        editing_at_ = selected_item_ > -1 ? (selected_item_ + 2) : ((int) data_copy_.size() + 1);

        is_editing_ = is_creating_ = true;

        // first draw
        draw();
        // then start editing from current position
        wstringstream wss;
        wss << editing_at_ << L". [ ] ";
        editor_.start(wss.str(), editing_at_, 1, getmaxx(wnd_) - 3);
      }

      break;
    }
    case '?':
    case '/': {
      is_help_open_ = true;
      help_.init(term_ref_->getWidth(), term_ref_->getHeight());
      help_.focus();
      term_ref_->setMode(term::mode::MODE_SKIP);

      break;
    }
    case '+':
    case '=': {
      if (selected_item_ > 0) {
        todo_ref_->placeBefore(data_copy_[selected_item_], data_copy_[selected_item_ - 1]);
        selected_item_--;
        update_data();
      }

      break;
    }
    case '-':
    case '_': {
      if (selected_item_ != -1 && selected_item_ < (int) data_copy_.size() - 1) {
        todo_ref_->placeAfter(data_copy_[selected_item_], data_copy_[selected_item_ + 1]);
        selected_item_++;
        update_data();
      }

      break;
    }
    case 'e':
    case 'E': {
      if (is_focused_ && !is_editing_ && selected_item_ > -1) {
        // position and number of element that being edited (created)
        editing_at_ = selected_item_ + 1;

        is_editing_ = true;

        // first draw
        draw();
        // then start editing from current position
        wstringstream wss;
        wss << editing_at_ << L". [" << (data_copy_[selected_item_].done ? L"x" : L" ") << "] ";
        editor_.setBuffer(data_copy_[selected_item_].desc)
            .start(wss.str(), editing_at_, 1, getmaxx(wnd_) - 3);
      }

      break;
    }
    case 'w':
    case 'W': {
      try {
        todo_ref_->save();
        menu_.push_state({L"Data saved!"});
        draw();
      } catch (std::system_error &e) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
        menu_.push_state({L"Write failed: " + conv.from_bytes(e.what())});
      }

      should_revert_menu_state_ = true;
      break;
    }
    case 'x':
    case 'X': {
      if (selected_item_ > -1) {
        todo_ref_->remove(data_copy_[selected_item_]);

        if (selected_item_ >= (int) todo_ref_->size()) {
          selected_item_ = (int) todo_ref_->size() - 1;
        }

        update_data();
      }
      break;
    }
    case 'f':
    case 'F': {
      if (!is_searching_) {
        is_searching_ = true;

        editor_.setBuffer(search_filter_).start(L"Search: ", term_ref_->getHeight() - 1, 1, getmaxx(wnd_) - 11);
      }

      break;
    }
    case 's':
    case 'S': {
      sort_data();
      break;
    }
    case '1':
    case 'a':
    case 'A': {
      if (selected_item_ > -1) {
        auto &item = data_copy_[selected_item_];
        todo_ref_->setPriority(item, item.priority != 'A' ? 'A' : 0);
        update_data();
      }
      break;
    }
    case '2':
    case 'b':
    case 'B': {
      if (selected_item_ > -1) {
        auto &item = data_copy_[selected_item_];
        todo_ref_->setPriority(item, item.priority != 'B' ? 'B' : 0);
        update_data();
      }
      break;
    }
    case '3':
    case 'c':
    case 'C': {
      if (selected_item_ > -1) {
        auto &item = data_copy_[selected_item_];
        todo_ref_->setPriority(item, item.priority != 'C' ? 'C' : 0);
        update_data();
      }
      break;
    }
    case '<': {
      if (selected_item_ > -1) {
        auto &item = data_copy_[selected_item_];
        todo_ref_->setPriority(item,
                               (char) (item.priority == 0 ? 'A' : (item.priority == 'C' ? 0 : item.priority + 1)));
        update_data();
      }
      break;
    }
    case '>': {
      if (selected_item_ > -1) {
        auto &item = data_copy_[selected_item_];
        todo_ref_->setPriority(item,
                               (char) (item.priority == 0 ? 'C' : (item.priority == 'A' ? 0 : item.priority - 1)));
        update_data();
      }
      break;
    }
    default:return;
  }
}

void task_list::update_focus(int value) {
  switch (focus_) {
    case TASK_LIST: {
      blur();

      if (value > 0) {
        this->project_list_.focus();
        focus_ = PROJECT_LIST;
      } else {
        this->context_list_.focus();
        focus_ = CONTEXT_LIST;
      }

      break;
    }
    case PROJECT_LIST: {
      this->project_list_.blur();

      if (value > 0) {
        this->context_list_.focus();
        focus_ = CONTEXT_LIST;
      } else {
        focus();
        focus_ = TASK_LIST;
      }

      break;
    }
    case CONTEXT_LIST: {
      this->context_list_.blur();

      if (value > 0) {
        focus();
        focus_ = TASK_LIST;
      } else {
        this->project_list_.focus();
        focus_ = PROJECT_LIST;
      }

      break;
    }
  }
}

bool task_list::should_display(const task &t) {
  bool project_any_ok = project_filter_.empty();
  bool context_any_ok = context_filter_.empty();

  for (auto &i: project_filter_) {
    auto tmp = L"+" + i;

    if (t.desc.find(tmp) != std::wstring::npos) {
      project_any_ok = true;
      break;
    }
  }

  if (!project_any_ok) {
    return false;
  }

  for (auto &i: context_filter_) {
    auto tmp = L"@" + i;

    if (t.desc.find(tmp) != std::wstring::npos) {
      context_any_ok = true;
      break;
    }
  }

  if (!context_any_ok) {
    return false;
  }

  if (!search_filter_.empty()) {
    return t.desc.find(search_filter_) != std::wstring::npos;
  }

  return true;
}

void task_list::update_data() {
  data_copy_.clear();
  std::copy_if(todo_ref_->begin(), todo_ref_->end(), std::back_inserter(data_copy_), [this](const task &t) -> bool {
    return this->should_display(t);
  });

  if (selected_item_ >= (int) data_copy_.size()) {
    selected_item_ = -1;
  }

  draw();
}

void task_list::sort_data() {
  todo_ref_->sort();
  update_data();
  draw();
}
}
}