export module dispatcher;

import <tuple>;
import <functional>;

export using TypeSizeType = unsigned int;

export template <typename T> TypeSizeType getType(const T &t);

export template <typename... Ts> struct TypeRegistrar {
private:
  // https://stackoverflow.com/a/78608011/28796641
  template <typename T> static constexpr TypeSizeType getIndexImpl() {
    TypeSizeType i = 0;
    bool found = ((++i && std::is_same_v<T, Ts>) || ...);
    return i - found;
  }

public:
  template <typename T> static constexpr TypeSizeType getIndex() {
    constexpr TypeSizeType index = getIndexImpl<T>();
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

  constexpr Dispatcher() = default;
  template <typename F> constexpr Dispatcher(F func) { registerWith(func); }

  template <typename X, typename Y, Return (*F)(X &, AfterA..., Y &, AfterB...)>
  constexpr void registerFunction() {
    static_assert(std::convertible_to<X &, A &>,
                  "type X not convertible to type A");
    static_assert(std::convertible_to<Y &, B &>,
                  "type Y not convertible to type B");

    using Registrar = TypeRegistrar<Types...>;

    constexpr auto x = Registrar::template getIndex<std::remove_cv_t<X>>();
    constexpr auto y = Registrar::template getIndex<std::remove_cv_t<Y>>();

    table[x][y] = []<auto func = F>(A &a, AfterA... as, B &b, AfterB... bs) {
      return func(static_cast<X &>(a), std::forward<AfterA>(as)...,
                  static_cast<Y &>(b), std::forward<AfterB>(bs)...);
    };
  }

private:
  template <typename T, typename F> constexpr void registerRow(F func) {
    (registerFunction<const T, const Types,
                      func.template operator()<T, Types>()>(),
     ...);
  }

public:
  template <typename F> constexpr void registerWith(F func) {
    (registerRow<Types>(func), ...);
  }

  Return operator()(A &a, AfterA... as, B &b, AfterB... bs) const {
    const FuncType &func = table[getType(a)][getType(b)];
    if (!func)
      throw std::exception{"undefined"};
    return func(a, as..., b, bs...);
  }
};

export template <typename Types, typename Return, typename T, typename AfterT>
using SimpleDispatcher = Dispatcher<Types, Return, T, AfterT, T, AfterT>;