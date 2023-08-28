#include <concepts>
#include <iostream>
#include <type_traits>
#include <utility>

template<typename T>
T identity(T&& val) { // identity function
  return std::forward<T>(val);
}

template<typename G, typename F>
auto compose(G g, F f) {
  return [g, f](auto &&input) {
    return g(f(std::forward<decltype(input)&&>(input)));
  };
}

int main() {
  auto add1 = [](int x) {
    return x + 1;
  };

  auto add2 = [](int y){
    return y + 2;
  };


  auto add3 = identity(compose(add1, add2));

  std::cout << "32 + 3 = " << add3(32) << "\n";
}