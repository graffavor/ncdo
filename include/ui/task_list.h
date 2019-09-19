#ifndef NCDO_TASK_LIST_H
#define NCDO_TASK_LIST_H

#include "includes.h"
#include <vector>
#include <string>
#include "component.h"
#include "td/todo.h"
#include "term.h"
#include "editor.h"
#include "data_list.h"
#include "util.h"
#include "menu.h"
#include "help.h"
#include "exit_popup.h"

using namespace cdo::td;

namespace cdo {
namespace ui {
enum _focused_window {
  TASK_LIST = 0,
  PROJECT_LIST,
  CONTEXT_LIST
};

class project_list : public data_list {
 public:
  project_list(todo* td, term* t) : data_list(L"project-list", t, td) {}

  std::set<std::wstring> getData() override  { return todo_ref_->projects(); }
  std::wstring getTitle() override { return L" Projects "; }
};

class context_list : public data_list {
 public:
  context_list(todo* td, term* t) : data_list(L"context-list", t, td) {}

  std::set<std::wstring> getData() override { return todo_ref_->contexts(); }
  std::wstring getTitle() override { return L" Contexts "; }
};

class task_list : public component, public keypress_handler {
 public:
  task_list(todo *, term *);

  void draw() override;
  void on_key_pressed(wint_t) override;

 protected:
  void update_focus(int);
  void update_data();

 private:
  todo *todo_ref_;

  int selected_item_ = -1;
  _focused_window focus_ = TASK_LIST;

  bool is_editing_ = false;
  bool is_searching_ = false;
  bool is_help_open_ = false;
  bool should_revert_menu_state_ = false;
  int editing_at_ = 0;

  std::vector<task> data_copy_;

  std::vector<std::wstring> project_filter_;
  std::vector<std::wstring> context_filter_;
  std::wstring search_filter_;

  editor editor_;
  project_list project_list_;
  context_list context_list_;
  menu menu_;
  help_window help_;

  std::shared_ptr<exit_popup> exit_popup_;

  bool should_display(const task&);
};
}
}

#endif //NCDO_TASK_LIST_H
