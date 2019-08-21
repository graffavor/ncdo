#ifndef CURSEDO_INCLUDE_UI_COMPONENT_H_
#define CURSEDO_INCLUDE_UI_COMPONENT_H_

#include "includes.h"
#include <list>
#include <string>
#include "util.h"
#include "term.h"

namespace cdo {
namespace ui {
class component : public util::non_copyable {
 public:
  component() = delete;
  component(std::wstring, term *);
  ~component();

  component(component &&) noexcept;

  void init(int w, int h, int x = 0, int y = 0);

  // Controls component focus
  void focus();
  void blur();

  // Redraw component
  virtual void draw() = 0;

  // Property getters
  const std::wstring &name() { return name_; }
  bool is_focused() { return is_focused_; }

  // Triggered when terminal size changes
  // x and y = -1 means component position is not changed
  void resize(int w, int h, int x = -1, int y = -1);
 protected:
  WINDOW *wnd_ = nullptr;
  term *term_ref_ = nullptr;
  bool is_focused_ = false;
  std::wstring name_;

  // current window position
  int x_ = 0, y_ = 0, w_ = 0, h_ = 0;

  // ncurses window properties
  bool is_scrollable_ = true;

  void terminate();
};

class keypress_handler : public util::non_copyable {
 public:
  keypress_handler() = delete;
  explicit keypress_handler(term *);

  ~keypress_handler();

  virtual void on_key_pressed(wint_t) = 0;
 private:
  std::function<void()> detach_;
  decltype(term::on_key_pressed)::fun_t fn_ref_;
};

class rawpress_handler : public util::non_copyable {
 public:
  rawpress_handler() = delete;
  explicit rawpress_handler(term*);

  ~rawpress_handler();

  virtual void on_key_pressed(wint_t) = 0;
 private:
  std::function<void()> detach_;
  decltype(term::on_key_pressed)::fun_t fn_ref_;
};
}
}

#endif //CURSEDO_INCLUDE_UI_COMPONENT_H_
