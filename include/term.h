#ifndef NCDO_TERM_H
#define NCDO_TERM_H

#include "includes.h"
#include <locale>
#include <string>
#include <map>
#include <tuple>
#include "hook.h"

namespace cdo {
class term;

enum _term_mode {
  MODE_NORMAL = 0,
  MODE_EDITOR,
  MODE_SKIP
};

class _color_definer {
 public:
  _color_definer() = delete;
  explicit _color_definer(term* t) : term_ref_(t) {}

  _color_definer& operator()(const std::string& name, short fg_color = -1, short bg_color = -1);
 private:
  term* term_ref_;
};

class term {
 public:
  typedef _term_mode mode;

  term();
  ~term();

  void update(WINDOW *wnd = stdscr);
  void clear(WINDOW *wnd = stdscr);

  void setMode(mode m);
  mode getMode() { return mode_; }

  int getWidth() { return width_; }
  int getHeight() { return height_; }

  _color_definer defineColors();
  short getColorId(const std::string& name);

  void run();
  void resize();
  void stop() { end_ = true; };

  // prevent loop termination if it was requested by other component or via key press
  void preventExit() { end_ = false; };

  hook<void()> on_resize;
  hook<void()> on_init;
  hook<void()> on_exit;
  // keypress handlers in normal mode
  hook<void(wint_t)> on_key_pressed;
  // keypress handlers in skip mode (all key presses will be send to this hooks)
  // NOTE! same key will be sent for raw_pressed and for key_pressed if component subscribed for both events
  hook<void(wint_t)> on_raw_pressed;
 private:
  int width_;
  int height_;

  bool end_ = false;

  std::map<std::string, std::tuple<short, short, short>> colors_;
  short color_pair_counter_ = 0;

  mode mode_ = mode::MODE_NORMAL;

  friend class _color_definer;
};
}

#endif //NCDO_TERM_H
