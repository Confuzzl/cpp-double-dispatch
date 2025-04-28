#include <format>
#include <iostream>
#include <typeinfo>

import dispatcher;
import shapes;

void println(const std::string &str = "") { std::cout << str << "\n"; }
template <typename... Args>
void println(std::format_string<Args...> str, Args &&...args) {
  println(std::format(str, std::forward<Args>(args)...));
}

template <typename R, typename A, typename B, typename C, typename D>
struct Foo;
template <typename R, typename A, typename... B, typename C, typename... D>
struct Foo<R, A, std::tuple<B...>, C, std::tuple<D...>> {
  using Func = R (*)(A, B..., C, D...);
  Func func;
};

template <typename A, typename B>
int shapeFoo(const A &a, float x, bool y, const B &b, float z, bool w) {
  println("reversed");
  return shapeFoo(b, z, w, a, x, y);
}
template <>
int shapeFoo<Wall, Wall>(const Wall &a, float x, bool y, const Wall &b, float z,
                         bool w) {
  println("wall wall");
  return 58185;
}
template <>
int shapeFoo<Wall, Circle>(const Wall &a, float x, bool y, const Circle &b,
                           float z, bool w) {
  println("wall circle");
  return 58185;
}
template <>
int shapeFoo<Wall, Polygon>(const Wall &a, float x, bool y, const Polygon &b,
                            float z, bool w) {
  println("wall polygon");
  return 58185;
}
template <>
int shapeFoo<Circle, Circle>(const Circle &a, float x, bool y, const Circle &b,
                             float z, bool w) {
  println("circle circle");
  return 123;
}
template <>
int shapeFoo<Circle, Polygon>(const Circle &a, float x, bool y,
                              const Polygon &b, float z, bool w) {
  println("circle polygon");
  return 123;
}
template <>
int shapeFoo<Polygon, Polygon>(const Polygon &a, float x, bool y,
                               const Polygon &b, float z, bool w) {
  println("polygon polygon");
  return -1;
}

int main() {
  // static constexpr auto dispatcherFoo =
  //     Dispatcher<ShapeTypes, int, const Shape &, std::tuple<float, bool>,
  //                const Shape &,
  //                std::tuple<float, bool>>::create([]<typename A, typename
  //                B>() {
  //       return &shapeFoo<A, B>;
  //     });
  static constexpr Dispatcher<ShapeTypes, int, const Shape &,
                              std::tuple<float, bool>, const Shape &,
                              std::tuple<float, bool>>
      dispatcherFoo{[]<typename A, typename B>() { return &shapeFoo<A, B>; }};

  // dispatcherFoo.registerWith(
  //     []<typename A, typename B>() { return &shapeFoo<A, B>; });

  Wall x;
  Circle a;
  Polygon i;

  dispatcherFoo(x, 0, 0, x, 0, 0);
  dispatcherFoo(x, 0, 0, a, 0, 0);
  dispatcherFoo(x, 0, 0, i, 0, 0);

  dispatcherFoo(a, 0, 0, x, 0, 0);
  dispatcherFoo(a, 0, 0, a, 0, 0);
  dispatcherFoo(a, 0, 0, i, 0, 0);

  dispatcherFoo(i, 0, 0, x, 0, 0);
  dispatcherFoo(i, 0, 0, a, 0, 0);
  dispatcherFoo(i, 0, 0, i, 0, 0);
}
