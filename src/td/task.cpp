#include "td/task.h"

namespace cdo {
namespace td {
bool operator<(const task &a, const task &b) {
  if (a.done && !b.done) {
    return false;
  } else if (!a.done && b.done) {
    return true;
  } else {
    if (a.priority != 0 && b.priority == 0) {
      return true;
    }

    if (a.priority != 0 && a.priority < b.priority) {
      return true;
    }
  }

  return false;
}
}
}