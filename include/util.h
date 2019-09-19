#ifndef NCDO_INCLUDE_UTIL_H_
#define NCDO_INCLUDE_UTIL_H_

namespace cdo {
namespace util {
class non_copyable {
 public:
  non_copyable() = default;
  non_copyable(const non_copyable &) = delete;
  non_copyable &operator=(non_copyable &) = delete;
};

class non_movable {
 public:
  non_movable() = default;
  non_movable(non_movable&&) = delete;
};
}
}

#endif //NCDO_INCLUDE_UTIL_H_
