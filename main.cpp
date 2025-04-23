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

// void funcTest1(int) { println("int"); }
// void funcTest1(float) { println("float"); }
// void funcTest1(bool) { println("bool"); }

// float barbar(int a) { return a + 0.5f; }
//  using barbar_t = void (*)(int);
//
//  barbar_t barA() { return &barbar; }
//  barbar_t barB() {
//    return [](int) { return; };
//  }
// template <float (*F)(int)> float barC(int a) { return F(a); }
// template <float (*F)(int)> float barD(int a) {
//   const auto f = []<auto F = F>(int a) { return F(a); };
//   return f(a);
// }

template <typename T> struct DispatchHelper;
template <typename... Ts> struct DispatchHelper<TypeRegistrar<Ts...>> {
  // static void foo() {
  //   std::string out = "";
  //   ((out += typeid(Ts).name()), ...);
  //   println(out);
  // }
  template <typename T, typename F, typename... D>
  static void registerRow(Dispatcher<D...> &dispatcher, F func) {
    //(dispatcher.registerFunction(func.template operator()<T, Ts>()), ...);
    (dispatcher
         .template reg<const T, const Ts, func.template operator()<T, Ts>()>(),
     ...);
  }

  template <typename F, typename... D>
  static void registerWith(Dispatcher<D...> &dispatcher, F func) {
    (registerRow<Ts>(dispatcher, func), ...);
  }
};

int main() {

  // DispatchHelper<TypeRegistrar<int, float, std::string>>::each(
  //     []<typename T>() { println(typeid(T).name()); });

  Dispatcher<ShapeTypes, int, const Shape &, std::tuple<float, bool>,
             const Shape &, std::tuple<float, bool>>
      dispatcherFoo;

  DispatchHelper<ShapeTypes>::registerWith(
      dispatcherFoo, []<typename A, typename B>() { return &shapeFoo<A, B>; });

  // dispatcherFoo.registerFunction(&shapeFoo<Circle, Circle>);
  // dispatcherFoo.registerFunction(&shapeFoo<Wall, Circle>);
  // dispatcherFoo.registerFunction(&shapeFoo<Polygon, Polygon>);

  // const auto lalalala = []<typename A, typename B>() { return shapeFoo<A, B>;
  // }; dispatcherFoo.reg<const Circle, const Circle,
  //                   lalalala.template operator()<Circle, Circle>()>();

  // DispatchHelper<ShapeTypes>::foo();

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

  // dispatcherFoo(a, 1.4f, false, b, 39.001f, false);
  // dispatcherFoo(x, -914.49f, true, b, 4.4424245f, false);
  // dispatcherFoo(b, 123, true, x, 57, false);
  // dispatcherFoo(i, 9, true, j, 0, true);
  // shapeFoo(i, 9, true, j, 0, true);

  // const auto l = []<typename A, typename B>() { return &shapeFoo<A, B>; };
  // const auto lala = l.template operator()<Circle, Circle>();
  //  l.template operator()<int, int>();

  // iterTuple<ShapeTypes::TypeTuple>();

  // float barA = barC<barbar>(1);
  // float barB = barD<barbar>(2);

  //  Foo<void, int, std::tuple<float, float>, bool, std::tuple<bool>> bar;
  //  bar.func = &foo;
}
