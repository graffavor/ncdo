#include <cwctype>
#include <cassert>
#include "ui/editor.h"

namespace cdo {
namespace ui {
void editor::handleInput(wint_t key) {
  if (wnd_ == nullptr)
    return;

  switch (key) {
    case KEY_UP:
    case KEY_DOWN: {
      break;
    }
    case KEY_LEFT: {
      if (cursor_position > 0)
        cursor_position--;
      else if (frame_left > 0) {
        frame_left--;

        if (frame_right - frame_left > wnd_width) {
          frame_right--;
        }
      }
      break;
    }
    case KEY_RIGHT: {
      if (cursor_position < wnd_width)
        cursor_position++;
      else if (frame_right < (int) buffer.length()) {
        frame_right++;

        if (frame_right - frame_left > wnd_width) {
          frame_left++;
        }
      }
      break;
    }
    case 127:
    case KEY_BACKSPACE: {
      remove_before();
      break;
    }
    case KEY_DC: {
      remove_after();
      break;
    }
    case KEY_HOME: {
      curs_beg();
      break;
    }
    case KEY_END: {
      curs_eol();
      break;
    }
    case 27: {    // escape or ALT (which we currently ignore)
      buffer.clear();
      done(buffer);
      return;
    }
    case KEY_ENTER:
    case '\n': {
      done(buffer);
      return;
    }
    default: {
      if (iswprint(key)) {
        insert((wchar_t) key);
      }
    }
  }

  draw();
}

void editor::draw() {
  auto xoff = pre.length();
  // move to where we started
  wmove(wnd_, 0, 0);
  wclrtoeol(wnd_);
  waddwstr(wnd_, (pre + buffer.substr(frame_left, wnd_width)).c_str());
  // move to actual cursor position
  wmove(wnd_, 0, (int) xoff + cursor_position);
  wrefresh(wnd_);
}

void editor::start(wstring prefix, int at_y, int at_x, int width) {
  assert(wnd_ == nullptr);

  init(width, 1, at_x, at_y);;
  cursor_position = buffer.size();
  pre = move(prefix);
  wnd_width = frame_right = width - (int) pre.length();

  term_ref_->setMode(term::mode::MODE_EDITOR);

  draw();
}

void editor::reset() {
  assert(wnd_ != nullptr);
  terminate();
  buffer.clear();
  term_ref_->setMode(term::mode::MODE_NORMAL);
}

wstring editor::value() {
  return buffer;
}

void editor::insert(wchar_t c) {
  if (cursor_position == wnd_width) {
    buffer += c;
  } else {
    buffer.insert(buffer.begin() + cursor_position, c);
  }
  if (cursor_position < wnd_width)
    cursor_position++;
  else {
    frame_right++;

    if (frame_right - frame_left > wnd_width) {
      frame_left++;
    }
  }
}

void editor::remove_before() {
  if (cursor_position > 0 || frame_left > 0) {
    buffer.erase(buffer.begin() + (cursor_position + frame_left - 1));
    if (cursor_position > 0)
      cursor_position--;
    else {
      frame_left--;

      if (frame_right - frame_left > wnd_width) {
        frame_right--;
      }
    }
  }
}

void editor::remove_after() {
  if (cursor_position == wnd_width && frame_right == (int) buffer.length()) {
    return;
  }

  if (cursor_position + frame_left < (int) buffer.length()) {
    buffer.erase(buffer.begin() + (cursor_position + frame_left));

    if (cursor_position == wnd_width && frame_right > wnd_width) {
      frame_right--;

      if (frame_right - frame_left < wnd_width) {
        frame_left--;
      }
    }
  }
}

void editor::curs_beg() {
  cursor_position = 0;
  frame_left = 0;
  frame_right = wnd_width;
}

void editor::curs_eol() {
  cursor_position = (int) buffer.length() > wnd_width ? wnd_width : (int) buffer.length();
  frame_right = (int) buffer.length() < wnd_width ? wnd_width : (int) buffer.length();
  frame_left = frame_right - wnd_width;

  if (frame_left < 0) frame_left = 0;
}
}
}