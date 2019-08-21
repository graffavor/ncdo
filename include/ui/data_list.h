#ifndef CURSEDO_INCLUDE_UI_DATA_LIST_H_
#define CURSEDO_INCLUDE_UI_DATA_LIST_H_

#include "includes.h"
#include <vector>
#include <string>
#include <set>
#include "component.h"
#include "hook.h"
#include "term.h"
#include "td/todo.h"
#include "charcodes.h"

using namespace cdo::td;

namespace cdo {
namespace ui {
class data_list : public component, public keypress_handler {
 public:
  data_list() = delete;
  data_list(std::wstring name, term* tm, todo* td);

  virtual std::wstring getTitle() = 0;
  virtual std::set<std::wstring> getData() = 0;

  std::vector<std::wstring> getSelected() { return checked_rows_; }

  void update();
  void draw() override;
  void on_key_pressed(wint_t) override;

  hook<void()> on_change;
 protected:
  todo* todo_ref_;
 private:
  int selected_row_ = -1;

  std::set<std::wstring> data_copy_;
  std::vector<std::wstring> checked_rows_;

  void cleanup_checks();
};
}
}

#endif //CURSEDO_INCLUDE_UI_DATA_LIST_H_
