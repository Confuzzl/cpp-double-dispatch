export module dispatcher;

import <tuple>;
import <functional>;

export template <typename... Ts> struct TypeRegistrar {
private:
  // https://stackoverflow.com/a/78608011/28796641
  template <typename T> static constexpr std::size_t getIndexImpl() {
    std::size_t i = 0;
    bool found = ((++i && std::is_same_v<T, Ts>) || ...);
    return i - found;
  }

public:
  template <typename T> static constexpr std::size_t getIndex() {
    constexpr std::size_t index = getIndexImpl<T>();
    static_assert(index < sizeof...(Ts), "type not found in registrar");
    return index;
  }
};

export template <typename Types, typename Return, typename A, typename AfterA,
                 typename B, typename AfterB>
struct Dispatcher;
export template <typename... Types, typename Return, typename A,
                 typename... AfterA, typename B, typename... AfterB>
struct Dispatcher<TypeRegistrar<Types...>, Return, A, std::tuple<AfterA...>, B,
                  std::tuple<AfterB...>> {

  using FuncType = Return (*)(A, AfterA..., B, AfterB...);

  static constexpr auto NUM_TYPES = sizeof...(Types);
  FuncType table[NUM_TYPES][NUM_TYPES]{};

  template <typename X, typename Y, Return (*F)(X &, AfterA..., Y &, AfterB...)>
  void reg() {
    static_assert(std::convertible_to<X &, A &>,
                  "type X not convertible to type A");
    static_assert(std::convertible_to<Y &, B &>,
                  "type Y not convertible to type B");

    using Registrar = TypeRegistrar<Types...>;

    static constexpr auto x =
        Registrar::template getIndex<std::remove_cv_t<X>>();
    static constexpr auto y =
        Registrar::template getIndex<std::remove_cv_t<Y>>();

    table[x][y] = []<auto func = F>(A &a, AfterA... as, B &b, AfterB... bs) {
      return func(static_cast<X &>(a), std::forward<AfterA>(as)...,
                  static_cast<Y &>(b), std::forward<AfterB>(bs)...);
    };
  }

  template <typename T, typename F> void registerRow(F func) {
    (reg<const T, const Types, func.template operator()<T, Types>()>(), ...);
  }
  template <typename F> void registerWith(F func) {
    (registerRow<Types>(func), ...);
  }

  Return operator()(A &a, AfterA... as, B &b, AfterB... bs) {
    const auto &func = table[a.type][b.type];
    if (!func)
      throw std::exception{"undefined"};
    return func(a, as..., b, bs...);
  }
};
