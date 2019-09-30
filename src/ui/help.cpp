#include "ui/help.h"

const wchar_t help_text[] =
    L"\n" \
    L" CDO - Todo management tool\n" \
    L"\n" \
    L" Basic controls: \n" \
    L"   LEFT/RIGHT ARROWS           - Navigate between windows\n" \
    L"   TAB                         - Focus on next window\n" \
    L"   UP/DOWN ARROWS              - Select task/project/context in list\n" \
    L"   SPACE                       - In task list: toggle task completion; \n" \
    L"                                 In project/context lists: toggle filtering\n" \
    L"   ENTER                       - Add new task after selected (at the end of list if no task selected)\n" \
    L"   ESC                         - Cancel editing or close context menu (including this help)\n" \
    L"   F                           - Filter tasks by custom string\n" \
    L"   S                           - Sort tasks. All completed tasks moved to the end of the list and \n" \
    L"                                 other sorted by their priorities\n" \
    L"   W                           - Save changes\n" \
    L"   X                           - Remove selected task\n" \
    L"   A(1),B(2),C(3)              - Set task priority. A - highest, C - lowest\n" \
    L"   <,>                         - Scroll through task priorities\n" \
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
  if (wnd_ == nullptr)
    return;

  if (key == 27 || key == 'q' || key == 'Q') {
    terminate();
    on_close();
  }
}
}
}