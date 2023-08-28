#include <coroutine>
#include <iostream>

// example usage of co_yield and co_return


struct CoroObj {
  struct promise_type {
    struct {
      unsigned value{};
    } data;

    CoroObj get_return_object() noexcept {
      return {.h = Handle::from_promise(*this)};
    }

    std::suspend_never initial_suspend() const noexcept { return {}; };
    std::suspend_always final_suspend() const noexcept { return {}; };

    std::suspend_always yield_value(unsigned value) {
      data.value = value;
      return {}; // default construct std::suspend_always
    }

    ~promise_type() { std::cout << "promise type destroyed\n"; }

    // for coro_return
    void return_value(std::string return_string) {
      std::cout << return_string
                << " - will now set value to length of this string\n";
      data.value = return_string.length();
    }
    void unhandled_exception() {}
  };

  using Handle = std::coroutine_handle<promise_type>;

  Handle h;
  operator Handle() { return h; }

  static const decltype(CoroObj::promise_type::data) *
  get_promise_data(Handle h) {
    return &h.promise().data;
  }
};

using Handle = std::coroutine_handle<CoroObj::promise_type>;

CoroObj counter() {
  for (int i = 0; i < 3; i++) {
    co_yield i;
  }

  co_return "Coroutine over";
}

int main() {
  Handle handle = counter();
  auto data = CoroObj::get_promise_data(handle);

  while (!handle.done()) {
    std::cout << "counter value: " << data->value << "\n";
    handle();
  }

  std::cout << data->value << " - the final data\n";

  handle.destroy();
}

/*
# Output of this program:
counter value: 0
counter value: 1
counter value: 2
Coroutine over - will now set value to length of this string
14 - the final data
promise type destroyed
*/
