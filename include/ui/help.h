#ifndef CURSEDO_INCLUDE_UI_HELP_H_
#define CURSEDO_INCLUDE_UI_HELP_H_

#include "includes.h"
#include "component.h"
#include "hook.h"

namespace cdo {
namespace ui {
class help_window : public component, public rawpress_handler {
 public:
  explicit help_window(term* t) : component(L"help", t), rawpress_handler(t) {};

  void draw() override;
  void on_key_pressed(wint_t) override;

  hook<void()> on_close;
};
}
}

#endif //CURSEDO_INCLUDE_UI_HELP_H_
