#include <coroutine>
#include <iostream>
#include <utility>

// this program shows how to properly communicate between the coroutine
// and main, similar to pass_value_to_coroutine, but cleaner

struct CoroObj {
  struct promise_type {
    struct {
      int value{};
    } data;

    CoroObj get_return_object() { return {.h = Handle::from_promise(*this)}; }

    std::suspend_never initial_suspend() { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void unhandled_exception() {}
  };

  using Handle = std::coroutine_handle<CoroObj::promise_type>;

  // don't want to erase the promise type this time, since want to communicate
  // data properly
  Handle h;
  operator Handle() const { return h; }
};

using CoroPromise = CoroObj::promise_type;
using Handle = std::coroutine_handle<CoroPromise>;

template <typename Promise> struct GetPromise {
  Promise *p;

  // means we then call await_suspend
  bool await_ready() const noexcept { return false; }
  bool await_suspend(std::coroutine_handle<Promise> h) noexcept {
    p = &h.promise();
    return false;
  }
  Promise *await_resume() { return p; }
};

template <typename CoroObjT>
auto get_promise(std::coroutine_handle<CoroObjT> h) -> decltype(&h.promise()) {
  return &h.promise();
}

CoroObj counter() {
  // won't suspend here - is used purely to get the promise_type pointer
  auto promise_pointer = co_await GetPromise<CoroPromise>();

  while (true) {
    promise_pointer->data.value++;
    std::cout << promise_pointer->data.value << "\n";
    co_await std::suspend_always{};
  }
}

int main() {
  Handle h = counter();

  auto promise = get_promise(h);

  for (int i = 0; i < 5; i++) {
    promise->data.value *= 2;
    h();
  }
  
  h.destroy();
}

/*
# Output of this program:
1
3
7
15
31
63
*/
