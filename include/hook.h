#ifndef CURSEDO_HOOK_H
#define CURSEDO_HOOK_H

#include <list>
#include <functional>
#include <memory>

namespace cdo {
template<typename _Signature>
class hook;

template<class R, class ...Args>
class hook<R(Args...)> {
 public:
  typedef std::function<R(Args...)> fun_t;
  typedef std::shared_ptr<fun_t> fun_ptr_t;

  hook() = default;
  hook(const hook<R(Args...)> &) = delete;
  hook(hook<R(Args...)> &&rhs) noexcept : callbacks(std::move(rhs.callbacks)) {};

  void invoke(Args... args) {
    for (auto &i : callbacks) {
      (*i)(args...);
    }
  }

  friend const fun_t &operator+=(hook<R(Args...)> &i, const fun_t &cb) {
    i.callbacks.push_back(fun_ptr_t(new fun_t(cb)));
    return cb;
  }

  friend const fun_t &operator-=(hook<R(Args...)> &i, fun_t &cb) {
    typedef R(fnType)(Args...);

    i.callbacks.remove_if([&cb](fun_ptr_t v) {
      // pretty dirty comparison but it will work
      return v->target_type() == cb.target_type() &&
          (v->template target<fnType *>()) == (cb.template target<fnType *>());
    });

    return cb;
  }

  void operator()(Args... args) {
    invoke(args...);
  }

  hook<R(Args...)> &operator=(const hook<R(Args...)> &) = delete;

 private:
  std::list<fun_ptr_t> callbacks;
};
}

#endif //CURSEDO_HOOK_H
