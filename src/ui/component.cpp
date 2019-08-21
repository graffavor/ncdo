#include <cassert>
#include "ui/component.h"

namespace cdo {
namespace ui {
component::component(std::wstring name, cdo::term *t) : term_ref_(t) {
  assert(!name.empty());
  name_ = std::move(name);
}

component::component(cdo::ui::component &&other) noexcept {
  term_ref_ = other.term_ref_;
  wnd_ = other.wnd_;
  is_focused_ = other.is_focused_;

  other.term_ref_ = nullptr;
  other.wnd_ = nullptr;
}

component::~component() {
  if (wnd_ != nullptr) terminate();
}

void component::focus() {
  if (!is_focused_) {
    is_focused_ = true;
    draw();
  }
}

void component::blur() {
  if (is_focused_) {
    is_focused_ = false;
    draw();
  }
}

void component::init(int w, int h, int x, int y) {
  assert(wnd_ == nullptr);

  x_ = x;
  y_ = y;
  w_ = w;
  h_ = h;

  wnd_ = newwin(h, w, y, x);
  if (is_scrollable_) scrollok(wnd_, true);
}

void component::resize(int w, int h, int x, int y) {
  assert(wnd_ != nullptr);

  if (y > -1) y_ = y;
  if (x > -1) x_ = x;
  w_ = w;
  h_ = h;

  terminate();
  init(w_, h_, x_, y_);
  draw();
}

void component::terminate() {
  wclear(wnd_);
  wborder(wnd_, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(wnd_);
  delwin(wnd_);
  wnd_ = nullptr;
  is_focused_ = false;
}

keypress_handler::keypress_handler(cdo::term *t) {
  fn_ref_ = t->on_key_pressed += [this](wint_t key) {
    this->on_key_pressed(key);
  };

  detach_ = [this, t]() {
    t->on_key_pressed -= this->fn_ref_;
  };
}

keypress_handler::~keypress_handler() {
  detach_();
}

rawpress_handler::rawpress_handler(cdo::term *t) {
  fn_ref_ = t->on_raw_pressed += [this](wint_t key) {
    this->on_key_pressed(key);
  };

  detach_ = [this, t]() {
    t->on_raw_pressed -= this->fn_ref_;
  };
}

rawpress_handler::~rawpress_handler() {
  detach_();
}
}
}