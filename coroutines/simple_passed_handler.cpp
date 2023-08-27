#include <coroutine>
#include <iostream>

// In this example the coroutine handle is passed to the coroutine,
// and that handler is used to resume the coroutine

// empty return object
struct CoroReturnObj {
  // promise type
  struct promise_type {
    CoroReturnObj get_return_object() { return {}; }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void unhandled_exception() {}
  };
};

struct CoroAwaiter {
  std::coroutine_handle<> *handle;
  constexpr bool await_ready() const noexcept {
    return false;
  } // don't skip suspending
  void await_suspend(std::coroutine_handle<> h) {
    *handle = h;
  } // pass the coroutine handle generated from co_await to the original handle
  constexpr int await_resume() const noexcept { return 3; } // test return value
};

CoroReturnObj counter(std::coroutine_handle<> *continuation_out) {
  CoroAwaiter a{continuation_out};

  std::suspend_never b{};
  std::suspend_always c{};

  for (unsigned i = 0;; ++i) {
    // will suspend here and contain the return value of await_resume
    auto x = co_await a;
    std::cout << " >> passed the suspend co_await with a return value\n";
    co_await b; // will not suspend here
    std::cout << " >> passed the no suspend co_await\n";
    co_await c; // will suspend here
    std::cout << " >> passed the suspend co_await\n";
    std::cout << " >> counter: " << (i += x) << std::endl;
  }
}

int main() {
  std::coroutine_handle<> h;
  counter(&h);

  for (int i = 0; i < 3; ++i) {
    std::cout << "In main function\n";
    h();
  }

  h.destroy();
}

/*
# Output of this program:
In main function
 >> passed the suspend co_await with a return value
 >> passed the no suspend co_await
In main function
 >> passed the suspend co_await
 >> counter: 3
In main function
 >> passed the suspend co_await with a return value
 >> passed the no suspend co_await
*/
