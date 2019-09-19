#include <vector>
#include "ui/textutil.h"

namespace cdo {
namespace ui {
std::vector<std::wstring> chunk_string(const std::wstring& str, int max_len) {
  std::vector<std::wstring> res;
  std::wstring tmp(str);

  while((int)tmp.size() > max_len) {
    auto pos = tmp.find_last_of(' ', max_len);

    if (pos != std::wstring::npos) {
      res.emplace_back(std::wstring(tmp.begin(), tmp.begin() + pos));
      std::wstring tmp1 = std::wstring(tmp.begin() + pos, tmp.end());
      tmp = std::wstring(tmp1.begin() + tmp1.find_first_not_of(' '), tmp1.end());
    } else break;
  }

  if (tmp.length()) {
    res.emplace_back(tmp);
  }

  return res;
}

int wwrite_wrap(WINDOW* w, const std::wstring& text, int pad_x, int pad_y) {
  auto maxw = getmaxx(w) - pad_x * 2;
  auto cury = pad_y;
  auto chunks = chunk_string(text, maxw);

  for (auto &line : chunks) {
    wmove(w, cury++, pad_x);
    waddwstr(w, line.c_str());
  }

  return chunks.size();
}
}
}