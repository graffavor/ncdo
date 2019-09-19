#include <cstdarg>
#include "term.h"

namespace cdo {
term::term() {
  setlocale(LC_ALL, "");

  initscr();
  use_default_colors();
  cbreak();
  noecho();
  keypad(stdscr, true);
  start_color();
  curs_set(0);

  // get screen size
  getmaxyx(stdscr, height_, width_);
  // not sure how cross-platform this is...
  set_escdelay(0);
}

term::~term() {
  endwin();
}

void term::update(WINDOW *wnd) {
  wrefresh(wnd);
}

void term::clear(WINDOW *wnd) {
  werase(wnd);
}

void term::setMode(mode m) {
  switch (m) {
    case MODE_EDITOR: {
      curs_set(1);
      break;
    }
    default: {
      curs_set(0);
    }
  }

  mode_ = m;
}

void term::run() {
  update();
  on_init();
  update();

  for (;!end_;) {
    wint_t ch;
    get_wch(&ch);

    on_raw_pressed(ch);

    if (ch == KEY_F(4) || (mode_ == MODE_NORMAL && (ch == 'q' || ch == 'Q'))) {
      end_ = true;
      on_exit();
    } else if (mode_ != MODE_SKIP) {
      on_key_pressed(ch);
    }
  }
}

void term::resize() {
  getmaxyx(stdscr, height_, width_);
  on_resize();
}

_color_definer term::defineColors() {
  return _color_definer(this);
}

short term::getColorId(const std::string& name) {
  try {
    return std::get<0>(colors_.at(name));
  } catch (std::out_of_range&) {
    return -1;
  }
}

_color_definer& _color_definer::operator()(const std::string& name, short fg_color, short bg_color) {
  auto next_pair_id = ++term_ref_->color_pair_counter_;
  init_pair(next_pair_id, fg_color, bg_color);

  term_ref_->colors_[name] = std::make_tuple(next_pair_id, fg_color, bg_color);
  return *this;
}
}