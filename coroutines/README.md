# What is this?
Experiments I'm trying while following along with https://www.scs.stanford.edu/~dm/blog/c++-coroutines.html#fnref2.

Below are some notes I made along the way, as well as anything else I found interesting along the way.

# Notes
- `co_await` makes a `coroutine_handle` and calls `await_suspend` with it
- `await_ready` is an optimisation - if it returns true then `co_await` doesn't suspend the function, this is more efficient than resuming in `await_suspend` since that would mean bundling stuff onto the heap then calling resume so less efficient
- Return type of a co routine must have `promise_type` as a nested type (though can be overriden by specialising `std::coroutine_traits`)
- Result of `get_return_object()` is the return value of the coroutine - this value is often referred to as a future, not the same as `std::future`

A `std::coroutine_handle` by default takes a `promise_type` (as a template parameter) of `void`. When it isn't `void`, there is a static method that can be invoked to convert from a `promise_type` to a `std::coroutine_handler<the promise_type>` (`std::coroutine_handle<promise_type>::from_promise(promise object)`).
A `std::coroutine_handle` with parameter `void` can be used to start/stop a coroutine just fine, but you can switch back and forth between a `promise_type` and the `std::coroutine_handle<promise_type>` if you use the specialised version.

Important - the `std::coroutine_handler` necessarily references some sort of coroutine state. This state has an instance of `promise_type` at a known offset (which is erased when the parameter is `void` so cannot be accessed). Then you can use the `from_promise` method above to get the coroutine handle, and so in `get_return_object` you can return a coroutine handle.

A useful thing to note - you can have type casts as an operator (user defined conversion operators):
```cpp
struct thingy {
  operator std::string() const { return "thingy"; }
  operator int() const { return 10; }
};

int main() {
  std::string thingyString = thingy(); // will be 
  int thingyNum = thingy(); // will be 10
  auto thingyObj = thingy(); // will be of type thingy
}
```
So using the above, you can - via just assigning the return of the coroutine to a `std::coroutine_handle<promise_type>` - or even easier to use `std::coroutine_handle<>` by simply having those types as operators.

So if you set the return type of a coroutine to this:
```cpp
struct ReturnObject2 {
  struct promise_type {
    ReturnObject2 get_return_object() {
      return {
        // Uses C++20 designated initializer syntax
        .h_ = std::coroutine_handle<promise_type>::from_promise(*this)
      };
    }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void unhandled_exception() {}
  };

  std::coroutine_handle<promise_type> h_;
  operator std::coroutine_handle<promise_type>() const { return h_; }
  // A coroutine_handle<promise_type> converts to coroutine_handle<>
  operator std::coroutine_handle<>() const { return h_; }
};
```
You'd be able to do this `std::coroutine_handler<> handle = coroutine_func();` and use that handle to resume the coroutine.

You can implicitly convert from `std::coroutine_handle<T>` to `std::coroutine_handle<void>` and still use it to resume the coroutine, which is why the above works.
This is in the coroutine header file:
```cpp
// this is a method of std::coroutine_handle <_Promise> not
// std::coroutine_handle<void>
// [coroutine.handle.conv], conversion
constexpr operator coroutine_handle<>() const noexcept {
  return coroutine_handle<>::from_address(address());
}

// and from_address is defined for std::coroutine_handle<> as such
constexpr static coroutine_handle from_address(void *__a) noexcept {
  // "coroutine_handle" is equivalent to "coroutine_handle<>" since C++17
  // in this context - it's called class template argument deduction
  coroutine_handle __self;
  __self._M_fr_ptr = __a;
  return __self;
}
// this presumably basically copies over the 
```

To actually suspend with `co_await`, `await_ready` must return false and `await_suspend` must return true IF it returns `bool` at all (instead of `void`).

