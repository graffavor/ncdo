#ifndef CURSEDO_INCLUDE_UI_MENU_H_
#define CURSEDO_INCLUDE_UI_MENU_H_

#include "includes.h"
#include <string>
#include <vector>
#include "component.h"

namespace cdo {
namespace ui {
typedef std::vector<std::wstring> menu_state;

class menu : public component {
 public:
  menu() = delete;
  explicit menu(term* t, menu_state p);

  void draw() override;
  void push_state(menu_state s);
  void pop_state();

 private:
  std::vector<menu_state> states_;
  menu_state persistent_;
  int pa_length_ = 0;
};
}
}

#endif //CURSEDO_INCLUDE_UI_MENU_H_
