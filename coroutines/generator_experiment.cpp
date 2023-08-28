#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>

template <typename T> struct CoroGenerator {
  struct promise_type;
  using Handle = std::coroutine_handle<promise_type>;

  struct promise_type {
    struct {
      std::exception_ptr exception_ptr{};
      T value{};
    } data;

    CoroGenerator get_return_object() {
      return CoroGenerator{Handle::from_promise(*this)};
    };

    // want to start it when we want and use
    // .done() afterwards, so always suspend
    std::suspend_always initial_suspend() { return {}; };
    std::suspend_always final_suspend() noexcept { return {}; };

    // anything which can be converted to T is stored
    template <std::convertible_to<T> From>
    std::suspend_always yield_value(From &&value) {
      // perfect forwarding here ensures the exact value category is preserved
      data.value = std::forward<From>(value);
      return {};
    }

    void return_void() {}

    void unhandled_exception() {
      data.exception_ptr = std::current_exception();
    }
  };

  Handle handle;

  CoroGenerator(Handle h) : handle(h) {}
  CoroGenerator(const CoroGenerator &) = delete; // delete copy constructor
  ~CoroGenerator() { handle.destroy(); } // clean up the handle automatically

  // the explicit conversion means, though it can be used in if/while, it won't
  // be treated as an actual bool in places like this true | CoroGenerator<T>,
  // it'd need to be cast like this: true | static_cast<bool>(CoroGenerator<T>)
  explicit operator bool() {
    generate_data();
    return !handle.done();
  }

  T operator()() {
    generate_data();
    // allowed to generate more - we are passing it forward now
    at_capacity = false;
    return std::move(handle.promise().data.value);
  }

private:
  bool at_capacity = false;

  void generate_data() {
    if (at_capacity)
      return;
    handle(); // generator more data

    // would rethrow any exception raised in the coroutine, outside of it
    const decltype(promise_type::data) *data = &handle.promise().data;
    if (data->exception_ptr) {
      std::rethrow_exception(data->exception_ptr);
    }

    at_capacity = true; // generated data
  }
};

CoroGenerator<int> counter() {
  for (int i = 0; i < 5; i++) {
    co_yield i * 2;
  }
}

int main() {
  auto counter_gen = counter();

  while (counter_gen) {
    std::cout << "counter: " << counter_gen() << "\n";
  }
}

/*
# Output of this program:
counter: 0
counter: 2
counter: 4
counter: 6
counter: 8
*/