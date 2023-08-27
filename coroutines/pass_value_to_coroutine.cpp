#include <coroutine>
#include <iostream>

// example to show communciation between the coroutine and main

struct CoroReturnObj {
  struct promise_type {
    // used to communicate between the coroutine and main
    int add_right_val{};

    CoroReturnObj get_return_object() {
      return {.handle = Handle::from_promise(*this)};
    }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void unhandled_exception(){};
  };

  using Handle = std::coroutine_handle<promise_type>;

  // store the handle, and have the correct user defined
  // conversion operators for it
  Handle handle;
  operator std::coroutine_handle<>() const { return handle; }
  operator Handle() const { return handle; }
};
using Handle = std::coroutine_handle<CoroReturnObj::promise_type>;

struct CoAwaiter {
  int *promise_obj_right_val{};
  int add_left_val{};

  constexpr bool await_ready() const noexcept { return false; }
  void await_suspend(Handle handle) noexcept {
    // saves pointer to value in the promise_type
    promise_obj_right_val = &handle.promise().add_right_val;
  }
  constexpr auto await_resume() const noexcept {
    // uses value passed from main to compute the return
    return *promise_obj_right_val + add_left_val;
  }
};

auto value_generating_task(int add_val_left) -> CoAwaiter {
  return CoAwaiter{.add_left_val = add_val_left};
}

auto coroutine_func() -> CoroReturnObj {
  int add_val_left = 5;
  auto ret_val = co_await value_generating_task(add_val_left);

  std::cout << add_val_left << " + a value from main is: " << ret_val << "\n";
}

auto main() -> int {
  std::coroutine_handle<CoroReturnObj::promise_type> handler = coroutine_func();

  handler.promise().add_right_val = 12;
  handler();

  handler.destroy(); // need to cleanup
}

/*
# Output of this program:
5 + a value from main is: 17
*/