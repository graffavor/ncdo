#ifndef NCDO_TASK_H
#define NCDO_TASK_H

#include <boost/date_time/gregorian/gregorian.hpp>
#include <string>

using namespace std;
namespace bdt = boost::gregorian;

namespace cdo {
namespace td {
struct task {
  bool done = false;
  char priority = 0;
  bdt::date created;
  bdt::date completed;
  wstring desc;

  // relative position of task inside list. this controls persistent positioning of tasks with no sorting applied
  // and also order in which they will be stored in file
  unsigned int rel_pos = 0;

  task() : created(), completed() {}

  friend bool operator==(const task &a, const task &b) {
    return a.done == b.done && a.priority == b.priority && a.desc == b.desc;
  }
};

bool operator<(const task &, const task &);
}
}

#endif //NCDO_TASK_H
