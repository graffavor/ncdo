#include "ui/help.h"

const wchar_t *help_text =
    L"\n" \
    L" CDO - Todo management tool\n" \
    L"\n" \
    L" Basic controls: \n" \
    L"   LEFT/RIGHT ARROWS           - Navigate between windows\n" \
    L"   TAB                         - Focus on next window\n" \
    L"   UP/DOWN ARROWS              - Select task/project/context in list\n" \
    L"   SPACE                       - In task list: toggle task completion; in project/context lists: toggle filtering\n" \
    L"   ENTER                       - Add new task after selected (at the end of list if no task selected)\n" \
    L"   ESC                         - Cancel editing or close context menu (including this help)\n" \
    L""
    L"\n\n" \
    L" Editor controls: \n" \
    L"   LEFT/RIGHT ARROWS           - Navigate inside text\n" \
    L"   HOME                        - Move cursor to the beginning of string\n" \
    L"   END                         - Move cursor to the end of string\n" \
    L"   BACKSPACE                   - Delete previous character\n" \
    L"   DELETE                      - Delete next character\n" \
    L"   ENTER                       - Submit changes / create new task\n" \
    L"   ESC                         - Discard changes and cancel editing\n";

namespace cdo {
namespace ui {
void help_window::draw() {
  term_ref_->clear(wnd_);
  mvwaddwstr(wnd_, 0, 0, help_text);
  term_ref_->update(wnd_);
}

void help_window::on_key_pressed(wint_t key) {
  if (key == 27) {
    terminate();
    on_close();
  }
}
}
}