#ifndef NCDO_INCLUDE_UI_EXIT_POPUP_H_
#define NCDO_INCLUDE_UI_EXIT_POPUP_H_

#include "component.h"
#include "td/todo.h"

using namespace cdo::td;

namespace cdo {
namespace ui {
class exit_popup : public component, rawpress_handler {
 public:
  explicit exit_popup(term* t, todo* td);

  void draw() override;

  void on_key_pressed(wint_t) override;
 private:
  todo* todo_ref_;

  int selected_item_ = 0;
};
}
}

#endif //NCDO_INCLUDE_UI_EXIT_POPUP_H_
