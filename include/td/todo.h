#ifndef NCDO_TODO_H
#define NCDO_TODO_H

#include <string>
#include <vector>
#include <fstream>
#include <locale>
#include <codecvt>
#include <set>
#include <exception>
#include "config.h"
#include "task.h"

using namespace std;

namespace cdo {
namespace td {
ostream &operator<<(ostream &, const task &);

class todo {
 public:
  todo() = default;
  explicit todo(config &c);
  ~todo();

  void save();

  set<wstring> projects() { return projects_; }
  set<wstring> contexts() { return contexts_; }

  vector<task>::const_iterator begin() { return tasks_.begin(); }
  vector<task>::const_iterator end() { return tasks_.end(); }

  size_t size() { return tasks_.size(); }
  bool isChanged() { return is_changed; }

  void addTask(const wstring &str, int pos = -1);

  void setComplete(const task &, bool);
  void placeBefore(const task &, const task &);
  void placeAfter(const task &, const task &);
  void remove(const task &);

  const task &operator[](size_t i) { return tasks_[i]; }

 private:
  bool done_in_todo = false;
  bool is_changed = false;

  string todo_file;
  string done_file;
  string report_file;

  vector<task> tasks_;
  set<wstring> projects_;
  set<wstring> contexts_;

  void extract_task_fields(const wstring &);
};
}
}

#endif //NCDO_TODO_H
