# Reading the C++ Standard Library Source
**Category:** C++ | **Date:** 2026-10-02

## How to use this guide

**The goal.** Open any function or class in `/usr/include/c++/13/` and fill in
a four-line **reading card** for it, without any outside help:

| Line | Question it answers |
|---|---|
| **EXPECTS** | What must I pass in? Which types are allowed, and what must they be able to do? |
| **RETURNS** | What exactly do I get back? A value or a position? A copy or a reference? Can it dangle? What do I get when there's no answer? |
| **DOES** | What does it compute, step by step, including edge cases (empty input, everything true, everything false)? |
| **COSTS** | How many operations, and does that change with the kind of container I pass? |

**Why library code looks so hard.** It is written to satisfy four goals at the
same time, and each goal adds its own layer of syntax:

1. It must work for *every* type, which brings in **templates**.
2. Your code must not be able to break it by accident, which brings in **ugly
   names and macros**.
3. Wrong usage must fail early with a readable error, which brings in
   **concepts and constraints**.
4. It must be as fast as possible for each kind of input, which brings in
   **many overloads**, each tuned to one case.

Each part below peels off one layer. You learn the idea on a small example
you could have written yourself, and only then see the same idea in real
library code. Nothing is used before the part that explains it.

**Assumed knowledge:** functions, `struct`/`class`, member functions, basic
references (`int&`, `const int&` as function parameters), `auto` for local
variables, and `std::vector`. Everything else is explained here.

**Pace.** Do one or two parts per sitting. Each part ends with a **Check**.
Move on only when you can answer it without scrolling up. The answers are
at the very end of the document. Compile every snippet yourself with
`g++ -std=c++23 file.cpp`; every snippet in this guide was compiled on this
machine (GCC 13.3) before it was written down.

| Part | Layer it removes |
|---|---|
| 0 | Where the code lives |
| 1 | Function templates: what `template<typename T>` means |
| 2 | The noise layer: underscores, macros, attributes |
| 3 | Types inside types: member types, `typename`, `decltype`, type functions |
| 4 | Value categories and forwarding references: what `_Range&&` means |
| 5 | Callables: function objects, lambdas, `std::invoke`, algorithm objects |
| 6 | Iterators and sentinels |
| 7 | Concepts and constraints: how a template says what it EXPECTS |
| 8 | Ranges and views: `range`, `iota_view`, borrowed ranges, `dangling` |
| 9 | **The reading recipe**: the step-by-step method |
| 10 | Worked example (function): `ranges::partition_point` |
| 11 | Worked example (class): `ranges::iota_view` and `views::iota` |
| 12 | Tools for when you're stuck |
| 13 | Practice ladder |

---

## Part 0 — Where the code lives

Your compiler (GCC 13) uses the library called **libstdc++**. Most of it is
templates, and template code has to be visible to the compiler at the point
where you use it, so almost all of the library ships as readable header
files on your disk. Those files are what you're reading.

The compiler searches these folders when you write `#include <...>` (seen
with `g++ -std=c++23 -xc++ -E -v - </dev/null`):

```
/usr/include/c++/13                       <- the standard library itself
/usr/include/x86_64-linux-gnu/c++/13      <- machine-specific config
```

A header like `<algorithm>` is mostly a list of `#include`s. The real code is
in `bits/`:

| You want | File under `/usr/include/c++/13/` |
|---|---|
| `std::ranges::` algorithms (`sort`, `partition_point`, `lower_bound`, …) | `bits/ranges_algo.h`, `bits/ranges_algobase.h` |
| `ranges::distance`, `ranges::advance`, `begin`, range concepts | `bits/ranges_base.h` |
| `std::views::` (`iota`, `filter`, `transform`, …) | `ranges` (a file with no extension) |
| iterator concepts (`forward_iterator`, …) | `bits/iterator_concepts.h` |
| classic `std::` algorithms (`std::sort`, `std::max`, …) | `bits/stl_algo.h`, `bits/stl_algobase.h` |
| containers | `bits/stl_vector.h`, `bits/stl_map.h`, `bits/stl_tree.h`, `bits/stl_list.h`, … |
| `std::identity`, `ranges::less` | `bits/ranges_cmp.h` |

**How to jump there:**

- **In Zed:** put the cursor on the name and press `F12` (go to definition).
  This needs `clangd` (installed at `/usr/bin/clangd`) and this repo's
  `compile_flags.txt` (which sets `-std=c++23`).
- **Fallback, always works:**
  ```bash
  grep -rn "partition_point" /usr/include/c++/13/bits/ /usr/include/c++/13/ranges
  ```
  Many names appear twice: once as `std::partition_point` (classic, in
  `stl_algo.h`) and once as `std::ranges::partition_point` (in
  `ranges_algo.h`). Read the one in the namespace you actually call.

**Check 0.** Find the line where `std::ranges::partition_point` is defined,
and the line where the classic `std::partition_point` is defined. Write down
both `file:line`s.

---

## Part 1 — Function templates

### 1.1 The problem templates solve

```cpp
int    my_max(int a, int b)       { return (a < b) ? b : a; }
double my_max(double a, double b) { return (a < b) ? b : a; }
// ...and again for long long, string, ...
```

The body is identical every time; only the type changes. A **template**
writes the body once, with a placeholder where the type goes:

```cpp
template<typename T>
T my_max(T a, T b) { return (a < b) ? b : a; }
```

### 1.2 Two parameter lists

A template function has **two** parameter lists. Keeping them apart is the
single most important reading skill in this guide:

```
template<typename T>          <- TEMPLATE parameter list: TYPES, decided at compile time
T my_max(T a, T b)            <- FUNCTION parameter list: VALUES, passed at run time
```

- `typename T` declares a **template parameter**: a name that stands for
  some type. `class T` means exactly the same thing; libstdc++ uses both.
- `T a, T b` are ordinary **function parameters** whose type happens to be
  `T`.

In library code, almost all of the **EXPECTS** information lives in the
first list. That's where types are named and where their requirements are
written (Part 7).

### 1.3 Instantiation and deduction

When you call `my_max(3, 5)`, the compiler:

1. **Deduces** the template parameters from the argument types: `3` is
   `int`, so `T = int`.
2. **Instantiates** the template: it stamps out a real function
   `my_max<int>(int a, int b)` and compiles that.

Deduction fails when the arguments disagree:

```cpp
my_max(3, 5.0);   // T = int from the first, T = double from the second
```

```
error: no matching function for call to 'my_max(int, double)'
note:   deduced conflicting types for parameter 'T' ('int' and 'double')
```

You can skip deduction by writing the template argument yourself in angle
brackets: `my_max<double>(3, 5.5)` makes `T = double`, and `3` converts.

### 1.4 Several template parameters, and defaults

```cpp
template<typename T, typename U = int>
void show(T a, U b = {}) { std::cout << a << " " << b << "\n"; }

show(1);        // T = int, U = int (the default), b = 0       -> prints "1 0"
show(1, 2.5);   // T = int, U = double (deduced from 2.5)      -> prints "1 2.5"
```

There are two different kinds of default here. Don't mix them up:

- `typename U = int` is a **default template argument**: a default *type*.
- `U b = {}` is a **default function argument**: a default *value*. `{}`
  means "a default-constructed `U`" (for `int` that's `0`; for a struct with
  no data it's just an empty object).

Both are needed because **a default function argument never takes part in
deduction**. In `show(1)` nothing tells the compiler what `U` is, so `U`
needs its own default type. You'll see exactly this pair in every ranges
algorithm: `typename _Proj = identity` in the template list together with
`_Proj __proj = {}` in the function list.

### 1.5 Overloading

Several functions can share a name as long as their parameter lists differ.
The compiler picks the **best match** for each call. Templates overload the
same way. Library functions often come as a *family* of overloads, for
example one taking an iterator pair and one taking a whole container. When
you read, treat the family as a list of "ways to call this", then work out
which one your call picks. Part 7.6 shows how constraints take part in that
choice.

**Check 1.** Given

```cpp
template<typename T, typename U = long>
U f(T x, U y = {});
```

what are `T` and `U` for `f(2.5)`? For `f(2.5, 'c')`? What does `f(2.5)`
return?

---

## Part 2 — The noise layer

This is the first layer to strip, and the cheapest one. Very little of the
visible clutter changes what a function EXPECTS or RETURNS.

### 2.1 Underscore names

The C++ standard **reserves** every name that starts with an underscore
followed by a capital letter (`_Tp`, `_Iter`), and every name that contains
a double underscore (`__first`, `__len`). Your programs may not use such
names, so the library uses *only* such names.

Why: macros are plain text replacement. If the library named a variable
`len` and your code did `#define len 5` before including `<algorithm>`, the
library code would silently become `5`. Your code can't legally define
`__len`, so the library is safe.

**Reading rule: delete the underscores in your head.**

| You see | Read it as |
|---|---|
| `_Tp`, `_Iter`, `_Range`, `_Pred` | `T`, `Iter`, `Range`, `Pred` (template parameters) |
| `__first`, `__last`, `__len` | `first`, `last`, `len` (parameters or local variables) |
| `_M_value`, `_M_bound` | data **m**ember `value`, `bound` |
| `_S_iter_concept()` | **s**tatic member function `iter_concept()` |
| `std::__invoke`, `std::__detail::...` | internal helpers; `__invoke` is the library's private copy of `std::invoke` (Part 5.5) |

### 2.2 Macros

Names in ALL CAPS that start with `_GLIBCXX` or `__glibcxx` are
configuration macros. Each expands to a keyword or to nothing, depending on
the C++ version and build flags:

| Macro | Expands to (in C++23) | Read it as |
|---|---|---|
| `_GLIBCXX14_CONSTEXPR`, `_GLIBCXX20_CONSTEXPR` | `constexpr` | ignore |
| `_GLIBCXX_NODISCARD` | `[[nodiscard]]` | see 2.3 |
| `_GLIBCXX_NOEXCEPT` | `noexcept` | see 2.3 |
| `__glibcxx_function_requires(...)` | nothing (old-style type checks, off by default) | ignore |
| `__glibcxx_assert(cond)` | a check that only runs in debug builds | "this must be true", a precondition |
| `_GLIBCXX_BEGIN_NAMESPACE_VERSION` | versioning plumbing | ignore |

### 2.3 Keywords and attributes that don't change EXPECTS/RETURNS

| Word | Meaning | Reading consequence |
|---|---|---|
| `constexpr` | may run at compile time | none for normal use |
| `noexcept` / `noexcept(cond)` | promises not to throw (if `cond` is true) | none for CP |
| `[[nodiscard]]` | compiler warns if you ignore the result | the return value *is* the point; can sit in odd places like `operator() [[nodiscard]] (...)` |
| `inline` | the definition may appear in many files (needed for things defined in headers) | none |
| `#if __cplusplus > 202002L` … `#endif` | code that exists only in C++23 and later (C++20 is `202002L`) | your `compile_flags.txt` says `-std=c++23`, so these blocks *are* active for you |

### 2.4 Doc comments

Comments of the form `/** @brief ... @param ... @return ... */` are real
documentation. **Read them first when they exist.** The classic `std::`
algorithms in `stl_algo.h` / `stl_algobase.h` usually have them. The
`ranges::` versions mostly don't, so when a ranges algorithm is unclear, its
classic twin's comment often explains the intent.

### 2.5 First real read: `std::max`

Raw, from `bits/stl_algobase.h:257`:

```cpp
  /**
   *  @brief This does what you think it does.
   *  @param  __a  A thing of arbitrary type.
   *  @param  __b  Another thing of arbitrary type.
   *  @return   The greater of the parameters.
   */
  template<typename _Tp>
    _GLIBCXX14_CONSTEXPR
    inline const _Tp&
    max(const _Tp& __a, const _Tp& __b)
    {
      // concept requirements
      __glibcxx_function_requires(_LessThanComparableConcept<_Tp>)
      //return  __a < __b ? __b : __a;
      if (__a < __b)
	return __b;
      return __a;
    }
```

After stripping the noise:

```cpp
template<typename T>
const T& max(const T& a, const T& b)
{
    if (a < b) return b;
    return a;
}
```

Reading card:

- **EXPECTS:** two arguments of the *same* type `T` (deduction, 1.3), so
  `std::max(2, 3LL)` fails. `T` must support `<`.
- **RETURNS:** `const T&`, a **reference to one of your arguments, not a
  copy**. If you pass temporaries, that reference outlives them:
  `const int& m = std::max(1, 2);` leaves `m` referring to a dead
  temporary. `int m = std::max(1, 2);` is fine because it copies right
  away.
- **DOES:** when the two are equal, `a < b` is false, so it returns `a`
  (the first argument).
- **COSTS:** one `<` comparison.

**Check 2.** Strip this line in your head and say what each piece is:
`_GLIBCXX20_CONSTEXPR inline _Tp* __find_end(_Tp* __first, _Tp* __last)`.

---

## Part 3 — Types inside types

### 3.1 Class templates

A class can be a template too:

```cpp
template<typename T>
struct Box {
    using value_type = T;          // a member type (3.2)
    T inside;
    T get() const { return inside; }
};

Box<int> b{7};                      // you name T explicitly (until CTAD, Part 8.4)
```

`std::vector<T>`, `std::pair<T1, T2>`, and `std::ranges::iota_view<W, B>`
are all class templates.

### 3.2 Member types

`using value_type = T;` puts a **type** inside the class. You reach it with
`::`, like a static member:

```cpp
Box<int>::value_type v = b.get();   // v is an int
```

Library classes publish their important types this way:
`std::vector<int>::value_type` (`int`), `::iterator`, `::size_type`. When
you read a class (Part 9), its member types are a large part of what it
RETURNS to you.

### 3.3 `const` after a member function

`T get() const` means "calling `get` does not modify the object". You'll
see it on almost every library `operator()`. It changes nothing about what
you pass or receive.

### 3.4 The `typename` keyword in front of `X::Y`

```cpp
template<typename C>
typename C::value_type first_of(const C& c) { return c.inside; }
```

Inside a template, `C::value_type` could in principle be a type or a
static variable; the compiler can't know until `C` is known. Writing
`typename` in front promises "this is a type".

**Reading rule:** `typename X::Y` means "the type named `Y` inside `X`".
Library example (`bits/iterator_concepts.h:801`):

```cpp
using projected = typename __detail::__projected<_Iter, _Proj>::__type;
```

reads as "`projected` is the type `__type` that lives inside
`__projected<Iter, Proj>`."

### 3.5 Alias templates: type functions

`using` can take template parameters too:

```cpp
template<typename T>
using Pair = std::pair<T, T>;

Pair<int> p{1, 2};       // same as std::pair<int, int>
```

Think of an alias template as a **type function**: types go in, a type
comes out. The ranges library is full of them, and you'll learn to read
each one as a function call:

| Type function | Input → output |
|---|---|
| `iterator_t<R>` | range type → its iterator type |
| `sentinel_t<R>` | range type → its end-marker type |
| `range_value_t<R>` | range type → element type |
| `iter_value_t<I>` | iterator type → element type |
| `borrowed_iterator_t<R>` | range type → its iterator type, or `ranges::dangling` (Part 8.6) |
| `remove_cvref_t<T>` | strips `const` and `&`: `const vector<int>&` → `vector<int>` |

### 3.6 `decltype` and `declval`

- `decltype(expr)` is **the type of `expr`**. The expression is *not* run;
  the compiler only works out its type.
- `std::declval<T>()` means "pretend I have a value of type `T`". It may
  only appear in places that are never run, such as inside `decltype`.

Together they ask "if I did this, what type would I get?". Library example
(`bits/ranges_base.h:516`):

```cpp
template<range _Range>
  using sentinel_t = decltype(ranges::end(std::declval<_Range&>()));
```

reads as "`sentinel_t<R>` is the type you'd get by calling `ranges::end` on
an `R&`."

### 3.7 How a return type can be spelled

| Spelling | Where to find the real type |
|---|---|
| `int`, `_Iter`, `const _Tp&` | written right there |
| `borrowed_iterator_t<_Range>` | a type function: expand it for *your* argument types |
| `auto` | deduced from the body: find the `return` statements |
| `auto f(...) -> decltype(expr)` | "trailing return type": the type of `expr` |

### 3.8 Compile-time branching

```cpp
template<typename T>
void describe(T x) {
    if constexpr (std::is_integral_v<T>) std::cout << x << " is an integer\n";
    else                                 std::cout << x << " is not an integer\n";
}
```

`if constexpr` picks a branch **at compile time**, and the branch not taken
is thrown away for that `T`. The library uses it to pick the fastest code
path per type (you'll see it in `iota_view::end()` and `ranges::distance`).

The type-level version is `std::conditional_t<cond, A, B>` (libstdc++
spells it `__conditional_t`): the type `A` if `cond` is true, otherwise
`B`.

### 3.9 Type traits: yes/no questions about types

`<type_traits>` has many small questions you can ask about a type:

- names ending in `_v` give a **bool value**: `std::is_integral_v<int>` is
  `true`, `std::is_same_v<A, B>` asks whether A and B are the same type,
  `std::is_lvalue_reference_v<T>` asks whether T is some `X&`.
- names ending in `_t` give a **type**: `std::remove_cvref_t<T>`,
  `std::make_unsigned_t<int>` (= `unsigned int`).

**Check 3.** In words, what is `decltype(std::declval<std::vector<int>&>().begin())`?
And what does `std::conditional_t<sizeof(int) == 4, long, short>` give on
this machine?

---

## Part 4 — Value categories, references, forwarding

Every ranges algorithm takes its range as `_Range&& __r`. This part explains
what that means, and Part 8 needs it to explain `dangling`.

### 4.1 lvalues and rvalues

A practical test:

- An **lvalue** has a name or an address and lives on after the expression:
  `n`, `v`, `v[0]`, `*ptr`.
- An **rvalue** is a temporary that disappears at the end of the full
  expression: `5`, `n + 1`, `std::vector<int>{1, 2}`, the result of a
  function that returns by value, `std::views::iota(0, 10)`.

### 4.2 Three kinds of reference parameter

| Parameter | Binds to | Meaning |
|---|---|---|
| `int&` | lvalues only | "I'll look at (and maybe change) *your* variable" |
| `const int&` | lvalues and rvalues | "I'll only look" |
| `int&&` | rvalues only | "you're giving me a temporary; I may take its insides" |

```cpp
void which(int&)  { std::cout << "lvalue\n"; }
void which(int&&) { std::cout << "rvalue\n"; }

int n = 5;
which(n);   // lvalue
which(5);   // rvalue
```

### 4.3 `std::move`

`std::move(x)` **moves nothing**. It is a cast that turns `x` into an
rvalue, which says "treat this as a temporary; you may steal from it". The
actual stealing happens in whatever function receives it (for example,
`vector`'s move constructor takes over the buffer instead of copying it).
When you see `std::move(__pred)` in the library, read it as "hand `pred`
over without a copy".

### 4.4 Forwarding references: `T&&` where `T` is deduced

Here is the twist: when `T` is a **deduced template parameter**, `T&&` is
*not* an rvalue reference. It is a **forwarding reference**, and it binds to
**anything**. To make that work, the compiler encodes "was it an lvalue?"
into `T` itself:

| You pass | `T` becomes | Parameter type `T&&` becomes |
|---|---|---|
| an lvalue of type `X` (e.g. a named `vector<int> v`) | `X&` | `X& &&` → collapses to `X&` |
| an rvalue of type `X` (e.g. `vector<int>{}`) | `X` | `X&&` |

("Reference collapsing": `X& &&` becomes `X&`. If any `&` is single, the
result is a single `&`.)

```cpp
template<typename T>
void what_is_T(T&&) {
    if constexpr (std::is_lvalue_reference_v<T>) std::cout << "T is an lvalue reference\n";
    else                                         std::cout << "T is a plain type\n";
}

std::vector<int> vec;
what_is_T(vec);                 // T is an lvalue reference   (T = vector<int>&)
what_is_T(std::vector<int>{});  // T is a plain type          (T = vector<int>)
```

### 4.5 `std::forward<T>(x)`

Inside the function the parameter has a name, so it is an lvalue, even if
the caller passed a temporary. `std::forward<T>(x)` restores the caller's
original category, using the information stored in `T`:

```cpp
template<typename T>
void pass_along(T&& x) { which(std::forward<T>(x)); }

pass_along(n);   // lvalue
pass_along(5);   // rvalue
```

### 4.6 Reading rule

`template<typename _Range> ... operator()(_Range&& __r, ...)` means
**"accepts any range, named or temporary, and `_Range` remembers which"**.
You'll see the remembered form in error messages. This is real output from
calling `ranges::sort` on a named `std::list<int> l`:

```
[with _Range = std::__cxx11::list<int>&; _Comp = std::ranges::less; _Proj = std::identity]
```

The `&` at the end of `list<int>&` tells you that you passed an lvalue.

**Check 4.** For `template<class T> void g(T&&);` and `std::vector<int> v;`,
what is `T` in `g(v)`, in `g(std::move(v))`, and in `g(v[0])`?

---

## Part 5 — Callables

Predicates, comparators, projections, and the algorithms themselves are all
**callables**. This part covers every form they take.

### 5.1 Function objects

Any object whose class defines `operator()` can be called like a function:

```cpp
struct IsEven {
    bool operator()(int x) const { return x % 2 == 0; }
};

IsEven e;
e(4);          // true: this calls e.operator()(4)
IsEven{}(3);   // false: build a temporary IsEven, call it
```

Because it's an object, it can carry **state** in its data members.

### 5.2 Lambdas are function objects the compiler writes for you

```cpp
int limit = 10;
auto below = [limit](int x) { return x < limit; };
```

The compiler turns this into roughly:

```cpp
struct __lambda_below {          // a name you never see
    int limit;                   // each captured variable becomes a data member
    bool operator()(int x) const { return x < limit; }   // const by default
};
auto below = __lambda_below{limit};
```

- `[limit]` copies `limit` into the object; `[&]` / `[&limit]` stores a
  reference instead (cheap to copy, sees later changes).
- The lambda's **parameter** type decides what happens on each call. In your
  ABC477-C solution, the lambda took `std::vector<int>` *by value*, so every
  call copied a vector. Write `const std::vector<int>&` to avoid that.

### 5.3 A template `operator()`

```cpp
struct twice_fn {
    template<typename T>
    T operator()(T x) const { return x + x; }
};
```

The template sits **on `operator()`**, not on the struct. One `twice_fn`
object works for `int`, `double`, `std::string`, and so on, with deduction
happening at each call. (A lambda with an `auto` parameter is exactly this.)

### 5.4 Algorithm objects: how every `ranges::` algorithm is written

```cpp
struct twice_fn { /* as above */ };
inline constexpr twice_fn twice{};   // a global constant object named `twice`

twice(21);             // 42: this calls twice.operator()(21)
auto f = twice;        // it's an object, so you can copy it and pass it around
f(std::string("ab"));  // "abab"
```

This is the exact shape of `ranges::partition_point`, `ranges::sort`,
`ranges::distance`, `views::iota`, and the rest:

```cpp
struct __partition_point_fn { /* operator() overloads */ };
inline constexpr __partition_point_fn partition_point{};
```

The ranges designers made them objects so they can be passed around as
values, and so the compiler never quietly picks some unrelated function with
the same name instead.

**Reading consequence:** to read a `ranges::` algorithm, find its `__xxx_fn`
struct and read **its `operator()` overloads**. Those overloads *are* the
function.

### 5.5 Member pointers and `std::invoke`

```cpp
struct Person { std::string name; int age; };
```

`&Person::age` is a **pointer to member**: "the `age` field of some
`Person`, whichever one you give me". It can't be called with `()`.
`std::invoke(f, args...)` (from `<functional>`) calls *anything* callable
with one syntax:

| `f` is | `std::invoke(f, x)` does |
|---|---|
| a function or function object or lambda | `f(x)` |
| a pointer to data member like `&Person::age` | `x.age` |
| a pointer to member function like `&Person::get` | `x.get()` |

```cpp
Person pe{"ana", 21};
std::invoke(IsEven{}, 4);          // true
std::invoke(&Person::age, pe);     // 21
```

The library calls your predicates through `std::__invoke` (its private copy
of `std::invoke`). That's why you're allowed to pass `&Person::age` where a
callable is expected.

### 5.6 Predicates and projections

- A **predicate** is a callable whose result can be used as a `bool`
  ("yes/no about one element").
- A **projection** is a callable applied to each element *before* the
  predicate or comparator sees it. `ranges::partition_point(ps, pred, &Person::age)`
  makes the predicate receive `p.age` instead of `p`.
- The default projection is `std::identity`, which hands the element back
  unchanged.

### 5.7 Second real read: `std::identity`

From `bits/ranges_cmp.h:47`. Every piece of it has now been covered:

```cpp
  struct identity
  {
    template<typename _Tp>
      [[nodiscard]]
      constexpr _Tp&&
      operator()(_Tp&& __t) const noexcept
      { return std::forward<_Tp>(__t); }

    using is_transparent = __is_transparent;
  };
```

Line by line:

- `struct identity` with a template `operator()` is a function object that
  works for every type (5.3).
- `[[nodiscard]] constexpr ... noexcept` is noise (Part 2).
- `_Tp&& __t` is a forwarding reference: it takes anything (4.4).
- It returns `_Tp&&` by doing `std::forward<_Tp>(__t)`, which hands back
  *the same object* in the same category (4.5). No copy is made.
- `using is_transparent = ...;` is a marker type, used by some lookup
  functions in `map`/`set`. You can ignore it here.

Reading card: **EXPECTS** anything. **RETURNS** the very same object, as a
reference. **DOES** nothing. **COSTS** nothing.

**Check 5.** What does
`ranges::partition_point(ps, [](int a) { return a < 25; }, &Person::age)`
call, for each element `p`, in terms of `std::invoke`?

---

## Part 6 — Iterators and sentinels

### 6.1 An iterator is a position

An iterator marks **a position in a sequence**. It's a generalised pointer.
The operations you can do depend on its kind (6.3):

| Expression | Meaning |
|---|---|
| `*it` | the element at this position |
| `++it` | move to the next position |
| `it == it2` | same position? |
| `--it` | move to the previous position |
| `it + n`, `it += n` | jump `n` positions |
| `it2 - it` | how many positions apart |

### 6.2 Half-open ranges `[first, last)`

`first` points at the first element. `last` points **one past** the final
element and must never be dereferenced. A sequence is empty exactly when
`first == last`. When an algorithm "returns `last`", it means "no such
element".

### 6.3 Iterator categories form a ladder

Each rung can do everything the rung below it can, plus more:

| Category | Adds | Typical sources |
|---|---|---|
| input | `*it`, `++it`; can only be walked **once** | reading `cin` with `istream_iterator` |
| forward | you can **copy a position and walk again** (multi-pass) | `forward_list`, `unordered_set` |
| bidirectional | `--it` | `list`, `set`, `map` |
| random access | `it + n`, `it2 - it`, `<` in **O(1)** | `vector`, `deque`, `array`, `views::iota` over integers |
| contiguous | elements sit next to each other in memory | `vector`, `array`, `string` |

### 6.4 Why the category matters: COSTS

`ranges::advance(it, n)` (move `n` steps) and `ranges::distance(first, last)`
(count steps) are **O(1)** on random-access iterators and **O(n)** on
everything else, because a `set` iterator can only move one step at a time.
An algorithm that "does O(log n) comparisons" can still do O(n) iterator
steps. Part 7.6 shows the code that makes this choice, and Part 10 shows
the consequence for `partition_point` on a `set`.

### 6.5 What `*it` hands you

Three type functions describe an iterator `I`:

| Type function | Meaning | `vector<int>` iterator | `iota` iterator over `int` |
|---|---|---|---|
| `iter_reference_t<I>` | the exact type of `*it` | `int&` (refers to the stored element; you can assign through it) | `int` (a **fresh value**; nothing is stored) |
| `iter_value_t<I>` | the bare element type | `int` | `int` |
| `iter_difference_t<I>` | the type of `it2 - it` | `long` (`ptrdiff_t`) | `long` |

Here is the `iota` iterator's `operator*` from `ranges:393`:

```cpp
	constexpr _Winc
	operator*() const noexcept(is_nothrow_copy_constructible_v<_Winc>)
	{ return _M_value; }
```

It returns `_Winc` **by value**: the iterator holds the current number in
its member `_M_value` and gives you a copy. (The `noexcept(...)` reads as
"doesn't throw if copying a `_Winc` doesn't throw", which is noise for
reading.)

### 6.6 Sentinels: an end marker of a different type

Classic C++ required `first` and `last` to have the **same** type. C++20
ranges relaxed this: `last` may be a **sentinel**, any type that can be
compared with `==` against the iterator to answer "am I done?".

- `views::iota(0)` (no end) uses `std::unreachable_sentinel_t`, whose `==`
  is always false, so the sequence never ends.
- `views::iota(0, n)` with `int` and `long long` uses a special
  `_Sentinel` type that stores the `long long` bound, because an `int`
  iterator can't be the end marker of a `long long` bound.
- A **sized sentinel** additionally supports `last - first` in O(1). If it
  doesn't, measuring the length means walking it (6.4).

**Check 6.** (a) Can `ranges::partition_point` give a correct answer on a
range read once from `cin`? Look at which ladder rung the algorithm needs:
it copies a position and walks forward again from it. (b) For `std::set<int> s`,
what is `iter_reference_t` of its iterator, roughly? Can you assign through
it?

---

## Part 7 — Concepts and constraints

This is where library code states what it **EXPECTS**.

### 7.1 The problem

`my_max` from Part 1 accepts *any* `T`. Call it with two
`std::vector<int>`s and it compiles fine at the call site, then fails deep
inside the body, often with a long error. Concepts move the check to the
front door: the template states its requirements up front, and calls that
don't meet them are rejected immediately.

### 7.2 A concept is a named compile-time yes/no question about types

```cpp
template<typename T>
concept Addable = requires(T a, T b) {
    a + b;
};
```

Read it as: "**pretend** `a` and `b` are `T`s. Does `a + b` compile?"
Nothing is run, and no `T` is ever created. `Addable<int>` and
`Addable<std::string>` are `true`; `Addable<std::vector<int>>` is `false`.

### 7.3 The four kinds of line inside `requires { ... }`

```cpp
template<typename T>
concept HasValueType = requires {
    typename T::value_type;              // (2) type requirement
};

template<typename I>
concept Steppable = requires(I i) {
    *i;                                  // (1) simple: this expression must compile
    { ++i } -> std::same_as<I&>;         // (3) compound: must compile AND its type must satisfy same_as<..., I&>
};
```

| Kind | Form | Meaning |
|---|---|---|
| (1) simple | `expr;` | `expr` must compile |
| (2) type | `typename X;` | the type `X` must exist |
| (3) compound | `{ expr } -> Concept<Args>;` | `expr` must compile and `Concept<type-of-expr, Args>` must hold |
| (4) nested | `requires bool-expr;` | this compile-time bool must be `true` |

Concepts combine with `&&`, `||`, and `!`, and can test any compile-time
bool:

```cpp
template<typename T, typename U>
concept SameSizeAs = sizeof(T) == sizeof(U);
```

### 7.4 Three equivalent ways to attach a concept

```cpp
template<Addable T>                   // (a) in place of `typename`
T add1(T a, T b);

template<typename T>
    requires Addable<T>               // (b) a requires-clause
T add2(T a, T b);

auto add3(Addable auto a, Addable auto b);   // (c) on an `auto` parameter
```

All three mean "only if `Addable<T>`". The library uses (a) and (b), often
both at once.

### 7.5 The hidden first argument: the biggest source of confusion

When a concept is used in the template list, **the declared type is slipped
in as the concept's first argument**:

```cpp
template<SameSizeAs<int> T>     // means:  requires SameSizeAs<T, int>
```

The same rule applies in compound requirements: `{ ++i } -> std::same_as<I&>`
means `std::same_as<decltype((++i)), I&>`.

With that rule, the scariest lines in `partition_point` become plain:

| Written | Means |
|---|---|
| `sentinel_for<_Iter> _Sent` | `sentinel_for<_Sent, _Iter>`: "`Sent` is an end marker for `Iter`" |
| `indirect_unary_predicate<projected<_Iter, _Proj>> _Pred` | `indirect_unary_predicate<_Pred, projected<_Iter, _Proj>>`: "`Pred` is a yes/no question about `proj(*it)`" |

### 7.6 Overloads chosen by constraints: constraints document COSTS

Real code, `ranges::distance` (`bits/ranges_base.h:934`), noise stripped:

```cpp
struct distance_fn {
    // (A) the end marker can't be subtracted: count by walking
    template<input_or_output_iterator It, sentinel_for<It> Sent>
        requires (!sized_sentinel_for<Sent, It>)
    iter_difference_t<It> operator()(It first, Sent last) const {
        iter_difference_t<It> n = 0;
        while (first != last) { ++first; ++n; }
        return n;                                      // O(n)
    }

    // (B) the end marker can be subtracted: one subtraction
    template<input_or_output_iterator It, sized_sentinel_for<It> Sent>
    iter_difference_t<It> operator()(const It& first, const Sent& last) const {
        return last - first;                           // O(1)
    }

    // (C) a whole range: uses size() when the range knows it
    template<range R>
    range_difference_t<R> operator()(R&& r) const;
};
```

Overloads (A) and (B) take the same arguments; only their constraints
differ, and the constraints are mutually exclusive, so exactly one fits any
call. **The constraint is the complexity documentation**: once you know
which one your types satisfy, you know the cost.

The same mechanism works on **member functions**. A trailing `requires`
makes a member exist only for some types. For example, inside the `iota`
iterator:

```cpp
	constexpr _Iterator&
	operator--() requires __detail::__decrementable<_Winc>
```

reads as "this iterator has `--` only if the counted type can be
decremented".

When two overloads both fit and one's constraints *include* the other's
plus more, the **more constrained** one wins. Part 11 uses this for
`iota_view::end()`.

### 7.7 Reading a library concept: one level deep

From `bits/iterator_concepts.h:667`:

```cpp
  template<typename _Iter>
    concept forward_iterator = input_iterator<_Iter>
      && derived_from<__detail::__iter_concept<_Iter>, forward_iterator_tag>
      && incrementable<_Iter> && sentinel_for<_Iter, _Iter>;
```

Translate each `&&` part into a plain sentence, and **stop when you reach a
name you already know** (from the glossary in 7.8). Treat concept
definitions as a glossary, not a proof to verify.

| Part | Plain meaning |
|---|---|
| `input_iterator<_Iter>` | you can read with `*` and step with `++` |
| `derived_from<__iter_concept<_Iter>, forward_iterator_tag>` | the iterator *declares* itself forward-or-better (see below) |
| `incrementable<_Iter>` | copies are independent: `it++` returns a copy of the old position, so you can save a position and come back to it (multi-pass) |
| `sentinel_for<_Iter, _Iter>` | two iterators can be compared with `==` |

**Tags.** Each iterator declares its rung with an empty "tag" struct, and
the tags inherit from each other:
`random_access_iterator_tag` → `bidirectional_iterator_tag` →
`forward_iterator_tag` → `input_iterator_tag` (each derives from the next).
`derived_from<X, forward_iterator_tag>` therefore means "X is the forward
tag or a tag above it". You'll see this exact wording in errors (7.9).

### 7.8 Glossary of common library concepts

| Concept | Plain meaning |
|---|---|
| `same_as<A, B>` | A and B are the same type |
| `convertible_to<A, B>` | an A can be converted to a B |
| `integral<T>` | T is a built-in integer type (`int`, `long long`, `char`, `bool`, …) |
| `movable<T>` / `copyable<T>` | T can be moved / copied and assigned |
| `semiregular<T>` | copyable and default-constructible |
| `regular<T>` | semiregular and has `==` |
| `totally_ordered<T>` | `<`, `>`, `<=`, `>=`, `==` all work and agree |
| `invocable<F, Args...>` | `std::invoke(f, args...)` compiles |
| `predicate<F, Args...>` | invocable, and the result is usable as `bool` |
| `weakly_incrementable<I>` | `++i` works (and `i++`) |
| `incrementable<I>` | `++` works and copies are independent (multi-pass) |
| `indirectly_readable<I>` | `*i` works |
| `input_iterator<I>` … `random_access_iterator<I>`, `contiguous_iterator<I>` | the ladder rung from 6.3 |
| `sentinel_for<S, I>` | S is an end marker for I (`s == i` works) |
| `sized_sentinel_for<S, I>` | also `s - i` in O(1) |
| `range<R>` | `ranges::begin(r)` and `ranges::end(r)` work |
| `input_range<R>` … `random_access_range<R>` | a range whose iterator is on that rung |
| `sized_range<R>` | `ranges::size(r)` works |
| `common_range<R>` | begin and end have the **same** type (no separate sentinel) |
| `borrowed_range<R>` | iterators stay valid after `r` itself is gone (Part 8.6) |
| `view<R>` | a range that is cheap to copy (doesn't own a big buffer) |
| `indirect_unary_predicate<F, I>` | `f(*it)` works for I's iterator, gives a bool, and F is copyable |
| `indirect_strict_weak_order<F, I1, I2>` | `f(*a, *b)` behaves like a valid `<` |
| `sortable<I, Comp, Proj>` | the elements at I can be swapped around and compared with `comp` on their projections |

`projected<I, Proj>` is **not** a concept. It is a type function (3.5)
that gives a fake iterator whose `*` yields `proj(*it)`. Its `operator*`
is literally declared with the comment `// not defined`: it exists only so
concepts can ask "what would `proj(*it)` be?". With that,
`indirect_unary_predicate<Pred, projected<I, Proj>>` means
"`pred(proj(*it))` works and gives a bool".

### 7.9 Reading a constraint failure

Calling `add1(a, b)` with two vectors gives (real output):

```
error: no matching function for call to 'add1(std::vector<int>&, std::vector<int>&)'
note: candidate: 'template<class T>  requires  Addable<T> T add1(T, T)'
note: constraints not satisfied
  required for the satisfaction of 'Addable<T>' [with T = std::vector<int, std::allocator<int> >]
note: the required expression '(a + b)' is invalid
```

Read it **bottom-up**. The last `note:` is the root cause ("`a + b` doesn't
compile"). The `required for the satisfaction of` lines above it are the
chain of concepts that led there, and the `[with T = ...]` part tells you
what each template parameter was.

A real library example: `std::ranges::sort(l)` on a `std::list<int>`
(trimmed, real output):

```
error: no match for call to '(const std::ranges::__sort_fn) (std::__cxx11::list<int>&)'
note: candidate: ... requires (random_access_range<_Range>) && (sortable<...>) ... operator()(_Range&&, _Comp, _Proj) const
note: constraints not satisfied
  required for the satisfaction of 'derived_from<...>' [with _Iter = std::_List_iterator<int>]
  required for the satisfaction of 'random_access_iterator<...>' [with _Tp = std::__cxx11::list<int>&]
  required for the satisfaction of 'random_access_range<_Range>' [with _Range = std::__cxx11::list<int>&]
note:   'std::random_access_iterator_tag' is not a base of 'std::bidirectional_iterator_tag'
```

Bottom-up: "the list iterator's tag is bidirectional, not random access"
→ so it isn't a `random_access_iterator` → so `list<int>&` isn't a
`random_access_range` → so `sort` rejects it. (Use `l.sort()` instead.)
Note `_Range = list<int>&`: Part 4.6 in action.

If the error is cut short, add `-fconcepts-diagnostics-depth=2` to the
compile command for more detail.

**Check 7.** (a) Rewrite `template<std::integral T, std::sentinel_for<T*> S>`
using only `typename` and one `requires` clause. (b) In your own words,
what does `indirect_unary_predicate<projected<_Iter, _Proj>> _Pred` demand of
the lambda you pass?

---

## Part 8 — Ranges and views

### 8.1 What a range is

From `bits/ranges_base.h:501`:

```cpp
  template<typename _Tp>
    concept range = requires(_Tp& __t)
      {
	ranges::begin(__t);
	ranges::end(__t);
      };
```

A range is anything you can ask for a beginning and an end: `vector`,
`string`, `set`, a C array, `iota_view`. (`ranges::begin` is itself an
algorithm object from Part 5.4 that calls `.begin()` or a free `begin()`.
You don't need to read it.)

Range categories just reuse the iterator ladder (`bits/ranges_base.h:594`):

```cpp
  template<typename _Tp>
    concept forward_range
      = input_range<_Tp> && forward_iterator<iterator_t<_Tp>>;
```

"A forward range is a range whose iterator type (3.5) is a forward
iterator."

### 8.2 Views

A **view** is a range that is cheap to copy because it doesn't own a big
buffer. It either refers to someone else's elements or **generates** them
on the fly. `iota_view` stores just two numbers, a start and a bound, and
its iterator makes each value as you go. That's why
`views::iota(0, 1'000'000'000)` costs nothing to create.

### 8.3 Variable templates and specialisation

A constant can have template parameters too:

```cpp
template<typename T>
inline constexpr bool enable_borrowed_range = false;          // the default for every T

template<typename W, typename B>
inline constexpr bool enable_borrowed_range<iota_view<W, B>> = true;   // override for iota_view
```

The second line is a **specialisation**: "for any `iota_view<W, B>`, the
value is `true` instead". The library uses this pattern to let a type
switch on a property (an "opt-in trait"). Both lines above are real
(`bits/ranges_base.h:51` and `ranges:671`).

### 8.4 Class template argument deduction (CTAD) and deduction guides

Since C++17 you can often skip a class template's `<...>` and let the
compiler deduce it from the constructor arguments:

```cpp
std::pair p(1, 2.5);       // std::pair<int, double>
std::vector v{1, 2, 3};    // std::vector<int>
```

A **deduction guide** is a rule written next to the class that tells the
compiler how to do this:

```cpp
template<typename A, typename B>
  ClassName(A, B) -> ClassName<A, B>;     // "called with an A and a B? make ClassName<A, B>"
```

It can carry a `requires` clause like any template, and then CTAD **fails**
when the clause is false.

### 8.5 `views::iota` builds an `iota_view` using CTAD

From `ranges:702`:

```cpp
  struct _Iota
  {
    template<__detail::__can_iota_view _Tp>
      constexpr auto
      operator() [[nodiscard]] (_Tp&& __e) const
      { return iota_view(std::forward<_Tp>(__e)); }

    template<typename _Tp, typename _Up>
      requires __detail::__can_iota_view<_Tp, _Up>
      constexpr auto
      operator() [[nodiscard]] (_Tp&& __e, _Up&& __f) const
      { return iota_view(std::forward<_Tp>(__e), std::forward<_Up>(__f)); }
  };

  inline constexpr _Iota iota{};
```

Everything here is now familiar: an algorithm object (5.4) with two
`operator()` overloads, one for one argument (unbounded) and one for two
(bounded). Both take forwarding references (4.4) and return `auto` (3.7),
so you read the body: `iota_view(e, f)` with **no `<...>`**, which is CTAD.
So the deduction guide decides the result type. It's at `ranges:667`:

```cpp
  template<typename _Winc, typename _Bound>
    requires (!__detail::__is_integer_like<_Winc>
	|| !__detail::__is_integer_like<_Bound>
	|| (__detail::__is_signed_integer_like<_Winc>
	    == __detail::__is_signed_integer_like<_Bound>))
    iota_view(_Winc, _Bound) -> iota_view<_Winc, _Bound>;
```

Reading: "given a `Winc` and a `Bound`, make `iota_view<Winc, Bound>`,
**provided that**, if both are integers, they're both signed or both
unsigned". The consequence is a classic error:

```cpp
std::vector<int> v(5);
auto r = std::views::iota(0, v.size());   // int is signed, size_t is unsigned
```

```
error: no match for call to '(const std::ranges::views::_Iota) (int, std::vector<int>::size_type)'
note: constraints not satisfied
  required for the satisfaction of '__can_iota_view<_Tp, _Up>' [with _Tp = int; _Up = long unsigned int]
```

Fix: make the types agree, e.g. `views::iota(0uz, v.size())` or
`views::iota(0, (int)v.size())`.

The constructor that guide pairs with has a detail you'll meet again:

```cpp
      constexpr
      iota_view(type_identity_t<_Winc> __value,
		type_identity_t<_Bound> __bound)
```

`type_identity_t<X>` is just `X`. Wrapping a parameter in it tells the
compiler "**don't** deduce template arguments from this parameter", which
forces CTAD to go through the guide above.

### 8.6 Borrowed ranges and `ranges::dangling`

**The problem.** An algorithm that returns an iterator into a *temporary*
container would return a position inside memory that is freed at the end
of the statement:

```cpp
auto it = std::ranges::partition_point(std::vector<int>{1, 2, 3}, pred);
// the vector is already destroyed here; `it` would point into freed memory
```

**The library's answer** is the return type `borrowed_iterator_t<_Range>`
(`bits/ranges_base.h:1060`):

```cpp
  template<range _Range>
    using borrowed_iterator_t = __conditional_t<borrowed_range<_Range>,
						iterator_t<_Range>,
						dangling>;
```

A type function with a type-level `if` (3.8): "the range's iterator type
if `_Range` is a borrowed range, otherwise `ranges::dangling`". `dangling`
is an empty struct with no `operator*`, so misuse fails at compile time:

```
error: no match for 'operator*' (operand type is 'std::ranges::dangling')
```

**When is a range borrowed?** (`bits/ranges_base.h:509` and `:84`):

```cpp
  template<typename _Tp>
    concept borrowed_range
      = range<_Tp> && __detail::__maybe_borrowed_range<_Tp>;

    template<typename _Tp>
      concept __maybe_borrowed_range
	= is_lvalue_reference_v<_Tp>
	  || enable_borrowed_range<remove_cvref_t<_Tp>>;
```

This is where Part 4 pays off. Remember that `_Range` is deduced from a
forwarding reference:

| You pass | `_Range` | Borrowed? | You get back |
|---|---|---|---|
| a named `vector<int> v` | `vector<int>&` (lvalue ref) | yes, by the first condition | `vector<int>::iterator` |
| `vector<int>{1, 2, 3}` (temporary) | `vector<int>` | no: not an lvalue ref, and vectors don't opt in | `ranges::dangling` |
| `views::iota(0, 10)` (temporary) | `iota_view<int, int>` | yes: `iota_view` opts in via the specialisation in 8.3 | the `iota` iterator |

Why can `iota_view` safely opt in? Its iterator stores the current number
**inside itself** (`_M_value`, 6.5). It never points back into the view,
so nothing can dangle when the view disappears.

**Check 8.** (a) Why does `views::iota(0, n)` compile when `n` is a
`long long`, but not when it's a `size_t`? (b) What is the return type of
`ranges::partition_point(std::string("abc"), pred)`? (c) What is it when
the argument is a named `std::string s`?

---

## Part 9 — The reading recipe

You now have every piece. This is the method; use it every time. Each step
ends with a **done when** condition. Don't move to the next step until it
holds.

### For a function

**Step 1: Locate.** F12 in Zed, or grep (Part 0). For a `ranges::` name,
find its `__name_fn` struct (5.4).
*Done when:* you have the `file:line` of the definition **in the namespace
you actually call** (`std::` vs `std::ranges::`).

**Step 2: Strip.** Copy only the *declarations* (template list + signature,
no bodies) into a scratch file or your head, with clean names and no
macros or attributes (Part 2).
*Done when:* no `_`/`__` prefixes, `_GLIBCXX` macros, or attributes remain.

**Step 3: List the overloads.** One line per overload: just the function
parameters.
*Done when:* you can say which overload **your** call picks and why: first
by number of arguments, then by which constraints your types satisfy
(7.6).

**Step 4: EXPECTS.** For the chosen overload, make one row per template
parameter and one row per function parameter:

- **Template parameter:** its constraint → translate it with the hidden
  first argument (7.5) and the glossary (7.8) → what *your* type is → does
  it satisfy it?
- **Function parameter:** passed by value (copied), by `const&`, or as a
  forwarding `&&`? Does it have a default (1.4)?

*Done when:* every row is filled in, including the defaulted ones.

**Step 5: RETURNS.** Read the return type (3.7). Expand any type function
for *your* argument types (`borrowed_iterator_t`, `iterator_t`, …). If it's
`auto`, find the `return` statements.
*Done when:* you can write down the exact type, and say (a) is it a value,
a reference, or a position (iterator)? (b) can it dangle? (c) what is
returned when there is "no answer" (empty input, nothing matches)?

**Step 6: DOES and COSTS.** Now read the body. Follow the loop, note what
each branch does, and check the edge cases: empty input, all true, all
false. Count every operation that matters (predicate calls, comparisons,
copies), and price each iterator operation (`distance`, `advance`, `+`) by
your iterator's rung (6.4, 7.6). Note any precondition the body
**assumes but never checks**.
*Done when:* you can work a 3–5 element example by hand and predict the
output.

**Step 7: Confirm.** Write a tiny program: `static_assert`s for the types
you predicted, prints for the values, and a call counter for the costs
(Part 12).
*Done when:* the program agrees with every prediction. If it doesn't, go
back to the step where your prediction went wrong. **That mismatch is the
real lesson.**

### For a class

Same idea, different checklist:

1. **Template parameters and their constraints** tell you which types you
   may plug in.
2. **Constructors and deduction guides** (8.4) tell you what you pass when
   creating one, and which type you get from CTAD.
3. **Public member types** (`value_type`, `iterator`, …) tell you what it
   hands back.
4. **Public member functions**: treat each one you'll call as a small
   function and run steps 3–6 on it. A trailing `requires` means the member
   exists only for some types (7.6). When the class has its own iterator,
   read that iterator's `operator*` (6.5).
5. **Trait specialisations** after the class, such as
   `enable_borrowed_range<...> = true` (8.3), switch on properties.

Skip `private:` sections unless a public function's behaviour depends on
them.

### The reading card (copy this)

```
NAME:      std::ranges::xxx            FILE: bits/xxx.h:NNN
OVERLOAD:  (which one my call picks, and why)
EXPECTS:
  template params:  Name — constraint — plain English — my type — ok?
  function params:  name — by value / const& / && — default?
RETURNS:   exact type — value / reference / position — can dangle? — "no answer" case
DOES:      one-paragraph summary + edge cases + unchecked preconditions
COSTS:     predicate/comparison calls; iterator ops priced by my iterator's rung
CONFIRMED: (the test program's output)
```

---

## Part 10 — Worked example: `ranges::partition_point`

**Step 1: Locate.** `bits/ranges_algo.h:2440`, struct `__partition_point_fn`,
object at `:2479`.

**Step 2: Strip.**

```cpp
struct partition_point_fn {
    // overload A: iterator + end marker
    template<forward_iterator Iter, sentinel_for<Iter> Sent,
             typename Proj = identity,
             indirect_unary_predicate<projected<Iter, Proj>> Pred>
    Iter operator()(Iter first, Sent last, Pred pred, Proj proj = {}) const;

    // overload B: a whole range
    template<forward_range Range, typename Proj = identity,
             indirect_unary_predicate<projected<iterator_t<Range>, Proj>> Pred>
    borrowed_iterator_t<Range> operator()(Range&& r, Pred pred, Proj proj = {}) const;
};
inline constexpr partition_point_fn partition_point{};
```

**Step 3: Overloads.**

- A: `(first, last, pred[, proj])`, 3 or 4 arguments.
- B: `(range, pred[, proj])`, 2 or 3 arguments.

`ranges::partition_point(views::iota(0, 10), pred)` has 2 arguments, so
it's **B**. Your `itmo-step2-a.cpp` calls
`partition_point(range.begin(), range.end(), pred)` with 3 arguments, so
**A**. (With 3 arguments, B would need the 3rd to be a projection and the
2nd to be a predicate; an iterator isn't a predicate, so B's constraints
rule it out.)

**Step 4: EXPECTS** (overload A):

| Parameter | Constraint, translated | For `iota(1LL, r+1)` + your lambda |
|---|---|---|
| `Iter` | `forward_iterator`: readable, multi-pass | iota iterator over `long long`: random access ✓ |
| `Sent` | `sentinel_for<Sent, Iter>`: an end marker for `Iter` | same type as `Iter` (both bounds are `long long`) ✓ |
| `Proj` | none; defaults to `identity` | `identity` |
| `Pred` | `pred(proj(*it))` compiles, gives a bool, `Pred` copyable | lambda `(ll) -> bool` ✓ |
| `first`, `last` | by value | iterators are cheap to copy |
| `pred` | by value | a `[&]` lambda holds only references, so it's cheap |
| `proj` | by value, default `{}` | `identity{}` |

Overload B adds: `Range&&` is a forwarding reference (4.4), so any range is
accepted, named or temporary.

**Step 5: RETURNS.**

- A returns `Iter`: a **position**, of the iterator type, even though
  `last` may be a different sentinel type.
- B returns `borrowed_iterator_t<Range>`. For a temporary `iota_view` that
  is its iterator (8.6); for a named vector, the vector's iterator; for a
  temporary vector, `dangling`.
- What it points at is decided by the body (step 6).
- **Turning the position into an answer:** `*it` is the **value** at that
  position. `it - first` is the **offset** from the start. For
  `iota(lo, hi)`, the value is `lo + offset`. They're equal only when
  `lo == 0`. Look at what your `itmo-step2-a.cpp` prints with that in mind.

**Step 6: DOES and COSTS.** The body, stripped:

```cpp
auto len = ranges::distance(first, last);
while (len > 0) {
    auto half = len / 2;
    auto middle = first;               // copies a position and walks from it
    ranges::advance(middle, half);     //   -> this is why it needs forward, not input
    if (invoke(pred, invoke(proj, *middle))) {
        first = middle; ++first;       // pred true at middle: the answer is to its right
        len = len - half - 1;
    } else {
        len = half;                    // pred false at middle: the answer is middle or left of it
    }
}
return first;
```

Worked by hand, on `iota(0, 10)` with `pred = x < 4`:

| `len` | `half` | `middle` | `pred` | then |
|---|---|---|---|---|
| 10 | 5 | 5 | false | `len = 5` |
| 5 | 2 | 2 | true | `first = 3`, `len = 2` |
| 2 | 1 | 4 | false | `len = 1` |
| 1 | 0 | 3 | true | `first = 4`, `len = 0` |

It returns the position of `4`: the **first element where `pred` is
false**.

- **Edge cases:** empty range → returns `first`. `pred` all true → returns
  the end position. `pred` all false → returns `first`.
- **Unchecked precondition:** the range must be *partitioned*, meaning all
  the trues come before all the falses. Nothing in the body checks this. On
  an unpartitioned range you silently get *some* boundary.
- **COSTS:** about `log2(n) + 1` predicate calls, always. The iterator work
  depends on the rung, and on whether the end marker is sized (7.6):

| Input | `distance(first, last)` | `advance` per step | Total iterator work |
|---|---|---|---|
| `vector`, or `iota` with **matching** types (`iota(0LL, n)`) | O(1) | O(1) | **O(log n)** |
| `iota(0, n)` with `n` a `long long` | **O(n)**: the `_Sentinel` isn't a sized sentinel (11.4) | O(1) | **O(n)** |
| `std::set` | O(n) | O(half) | **O(n)** → use `s.lower_bound` instead |

The middle row was measured: `partition_point(views::iota(0, n), ...)` with
`n = 2e9` took **4.8 s at `-O0`** and made only 31 predicate calls. All the
time went into `distance` walking two billion steps. At `-O2`, GCC happened
to optimise that particular loop away (0.002 s), but don't rely on the
optimiser. **Make the two `iota` bounds the same type.**

**Step 7: Confirm.**

```cpp
#include <algorithm>
#include <ranges>
#include <vector>
#include <iostream>
namespace ranges = std::ranges; namespace views = std::views;

int main() {
    auto it = ranges::partition_point(views::iota(0, 10), [](int x) { return x < 4; });
    std::cout << *it << "\n";                                          // 4

    std::vector<int> v{1, 3, 5, 7, 9};
    auto it2 = ranges::partition_point(v, [](int x) { return x < 6; });
    std::cout << *it2 << " at index " << (it2 - v.begin()) << "\n";    // 7 at index 3

    auto d = ranges::partition_point(std::vector<int>{1, 2, 3}, [](int x) { return x < 2; });
    static_assert(std::is_same_v<decltype(d), ranges::dangling>);      // temporary vector
}
```

**Card.**

```
NAME:      std::ranges::partition_point     FILE: bits/ranges_algo.h:2440
OVERLOAD:  A (first, last, pred[, proj]) or B (range, pred[, proj]), by argument count
EXPECTS:   forward (multi-pass) iterator/range; end marker for it;
           pred(proj(*it)) -> bool, true-block then false-block (unchecked)
RETURNS:   position of the first element where pred is false; the end position if none.
           B: dangling for temporary non-borrowed ranges (temporary iota is fine)
DOES:      binary search for the true/false boundary
COSTS:     ~log2(n)+1 pred calls; iterator work O(log n) only if random access
           AND sized end marker -> match iota's types; O(n) on set/list
```

---

## Part 11 — Worked example: `ranges::iota_view` and `views::iota`

### 11.1 Template parameters (`ranges:354`)

```cpp
  template<weakly_incrementable _Winc,
	   semiregular _Bound = unreachable_sentinel_t>
    requires std::__detail::__weakly_eq_cmp_with<_Winc, _Bound>
      && copyable<_Winc>
    class iota_view : public view_interface<iota_view<_Winc, _Bound>>
```

| Parameter | Translated | In practice |
|---|---|---|
| `Winc` | `weakly_incrementable` + `copyable`: something you can `++` and copy | the type of the values: `int`, `long long`, even an iterator |
| `Bound` | `semiregular`, and comparable with `Winc` using `==` | the end value's type; defaults to "no end" |

`: public view_interface<...>` means it inherits helper members such as
`empty()`, `front()`, and `operator[]`. Those are written once in
`view_interface` in terms of `begin()`/`end()`.

### 11.2 Constructors and deduction guide

```cpp
      iota_view() requires default_initializable<_Winc> = default;
      constexpr explicit iota_view(_Winc __value);                       // unbounded: value, value+1, ...
      constexpr iota_view(type_identity_t<_Winc> __value,
                          type_identity_t<_Bound> __bound)               // [value, bound)
      { if constexpr (totally_ordered_with<_Winc, _Bound>)
          __glibcxx_assert( bool(__value <= __bound) ); }              // precondition: value <= bound
```

The deduction guide from 8.5 picks `iota_view<Winc, Bound>` from the two
argument types, and refuses a signed/unsigned mix. Note the
`__glibcxx_assert`: `iota(5, 3)` breaks a precondition that is only checked
in debug builds.

### 11.3 The iterator: what it hands you

```cpp
      struct _Iterator : __detail::__iota_view_iter_cat<_Winc>
      {
	static auto _S_iter_concept()
	{
	  if constexpr (__advanceable<_Winc>)        return random_access_iterator_tag{};
	  else if constexpr (__decrementable<_Winc>) return bidirectional_iterator_tag{};
	  else if constexpr (incrementable<_Winc>)   return forward_iterator_tag{};
	  else                                       return input_iterator_tag{};
	}
      public:
	using iterator_concept = decltype(_S_iter_concept());
	using value_type = _Winc;
	using difference_type = __detail::__iota_diff_t<_Winc>;
	constexpr _Winc operator*() const { return _M_value; }
	...
```

- **Its rung** is chosen by `if constexpr` (3.8) from what `Winc` supports.
  Integers support `+=`, `-`, and `<`, so the rung is **random access**.
  `decltype(_S_iter_concept())` (3.6) turns the returned tag into the
  member type `iterator_concept`.
- `value_type` is `Winc`. `operator*` returns `Winc` **by value** (6.5).
- `difference_type` is a signed type wide enough for `it2 - it`: `long` for
  `int`, and `__int128` for `long long` (checked with `static_assert`).

### 11.4 `begin`, `end`, `size`

```cpp
      constexpr _Iterator begin() const { return _Iterator{_M_value}; }

      constexpr auto end() const
      {
	if constexpr (same_as<_Bound, unreachable_sentinel_t>)
	  return unreachable_sentinel;           // iota(x): never ends
	else
	  return _Sentinel{_M_bound};            // a separate sentinel type
      }

      constexpr _Iterator end() const requires same_as<_Winc, _Bound>
      { return _Iterator{_M_bound}; }          // same types: end is an ordinary iterator

      constexpr auto size() const
      requires (same_as<_Winc, _Bound> && __detail::__advanceable<_Winc>)
      || (integral<_Winc> && integral<_Bound>)
      || sized_sentinel_for<_Bound, _Winc>;
```

There are two `end()` overloads. When `Winc` and `Bound` are the same type,
both fit, and the constrained one wins (7.6). Then `end()` is an iterator
and the view is a `common_range`. When they differ, only the first fits,
and it returns a `_Sentinel`.

The `_Sentinel`'s subtraction operator is constrained:

```cpp
	friend constexpr iter_difference_t<_Winc>
	operator-(const _Sentinel& __x, const _Iterator& __y)
	  requires sized_sentinel_for<_Bound, _Winc>
```

For `Winc = int`, `Bound = long long`, the constraint asks
`sized_sentinel_for<long long, int>`: "is a `long long` a sized end marker
for an `int` **used as an iterator**?" `sentinel_for` (7.8) requires its
second argument to be an iterator, and `*i` doesn't compile for an `int`.
So the answer is no for *every* pair of plain integers, even
`sized_sentinel_for<long long, long long>` (checked with `static_assert`).
This operator is meant for `iota` over actual iterators. For integer
bounds, **it never exists**.

So the two cases split on which `end()` you get:

- **Same types** (`iota(0LL, n)`): `end()` is an `_Iterator`, and two
  random-access `_Iterator`s can be subtracted, so `distance` is O(1).
- **Different types** (`iota(0, n)` with `long long n`): `end()` is a
  `_Sentinel` with no `-`, so `ranges::distance(begin, end)` takes the
  O(n) loop from 7.6. That's the slow row in Part 10.

Notice that `size()` *does* exist for the mixed pair (`integral &&
integral`), but `partition_point`'s overload B passes `begin`/`end` to
overload A, which measures with `distance(first, last)` and never calls
`size()`.

### 11.5 Traits

```cpp
  template<typename _Winc, typename _Bound>
    inline constexpr bool
      enable_borrowed_range<iota_view<_Winc, _Bound>> = true;
```

A temporary `iota_view` is a borrowed range, so algorithms return real
iterators into it (8.6).

### 11.6 Card for `views::iota(a, b)`

```
NAME:      std::views::iota (object of type _Iota)     FILE: ranges:702 (view: ranges:354)
OVERLOAD:  iota(a) -> unbounded; iota(a, b) -> [a, b)
EXPECTS:   a: ++-able and copyable; b: ==-comparable with a;
           if both are integers: same signedness (deduction guide); a <= b (debug-only check)
RETURNS:   iota_view<A, B> by value: stores two numbers. *it yields A by value.
           Random access over integers. Borrowed.
           common_range (end() is an iterator) only if A == B
DOES:      generates a, a+1, ..., b-1 lazily
COSTS:     O(1) to build and per step; distance(begin, end) is O(1) only if A == B
           -> write iota(0LL, n) for long long n, not iota(0, n)
```

---

## Part 12 — Tools for when you're stuck

**Make the compiler print a type.** Declare a template and never define it.
Using it forces an error that spells out the type:

```cpp
template<typename T> struct TD;          // declared, never defined

long long n = 5;
auto r = std::views::iota(0, n);
TD<decltype(r)> show;
// error: aggregate 'TD<std::ranges::iota_view<int, long long int> > show' has incomplete type
```

**Ask yes/no questions with `static_assert`.** Any concept (7.8) or type
trait (3.9) works:

```cpp
static_assert(std::ranges::random_access_range<decltype(r)>);
static_assert(std::sized_sentinel_for<decltype(r.end()), decltype(r.begin())>);   // fails for iota(0, n) with long long n
static_assert(std::is_same_v<decltype(*r.begin()), int>);                           // * gives int by value
```

**Count calls.** Capture a counter by reference in your predicate to see
how often the algorithm calls it:

```cpp
int calls = 0;
auto it = std::ranges::partition_point(r, [&](int x) { ++calls; return x < 3; });
```

**Get longer constraint errors:** compile with `-fconcepts-diagnostics-depth=2`.

**Read the classic twin's doc comment** (2.4) when the `ranges::` version
has none.

---

## Part 13 — Practice ladder

Do these in order. Each one should get a full reading card (Part 9) and a
**Step 7 confirm program**. No answers are given here: your confirm
program is the answer key.

1. **`std::min`** (`bits/stl_algobase.h`, just above `max`). When the two
   arguments are equal, which one is returned? Is that consistent with
   `max`?
2. **`std::ranges::count_if`** (`bits/ranges_algo.h:313`). What is the
   *exact* return type? (It's not `int`.)
3. **`std::ranges::max_element`** (`bits/ranges_algo.h:3104`). What is
   returned for an empty range? When several elements tie for maximum, which
   one do you get?
4. **`std::ranges::lower_bound`** (`bits/ranges_algo.h:2049`). Compare its
   loop with `partition_point`'s. Which predicate passed to
   `partition_point` gives the same result as `lower_bound(r, value)`?
5. **`std::ranges::filter_view`** (`ranges:1510`) as a class. Which rung is
   its iterator on, and why can't it be random access? Is it a borrowed
   range? (Look for an `enable_borrowed_range` specialisation; its absence
   is an answer too.)
6. **`std::vector::push_back`** (`bits/stl_vector.h:1281`). Follow it into
   the function it calls when the vector is full. Find the line that
   decides the new capacity, and connect it to
   [the amortized analysis note](2026-08-04-amortized-analysis.md).

---

## Answers to the checks

**Check 0.** `bits/ranges_algo.h:2440` (`struct __partition_point_fn`;
the object is at `:2479`). Classic: `bits/stl_algo.h:558`.

**Check 1.** `f(2.5)`: `T = double`, `U = long` (default type; `y = 0L`),
returns `long`. `f(2.5, 'c')`: `T = double`, `U = char`.

**Check 2.** "`constexpr` (C++20 noise), inline function `find_end`
templated on `T`, taking two `T*` pointers `first` and `last`, returning a
`T*`."

**Check 3.** The iterator type of `std::vector<int>` (i.e.
`std::vector<int>::iterator`), the type you'd get by calling `.begin()` on
a non-const `vector<int>&`. `long` (since `int` is 4 bytes here).

**Check 4.** `vector<int>&`; `vector<int>`; `int&` (`v[0]` is an lvalue of
type `int`).

**Check 5.** `std::invoke(pred, std::invoke(&Person::age, p))`, which is
`pred(p.age)`.

**Check 6.** (a) No. It copies `first` into `middle` and walks forward from
the copy, then continues from `first` or `middle` again. That needs
multi-pass (forward), and `cin` can only be read once (input). The
`forward_iterator` constraint rejects it at compile time. (b) `const int&`.
Set elements are read-only through iterators, because changing one would
break the tree's ordering. So you can't assign through it.

**Check 7.** (a)

```cpp
template<typename T, typename S>
    requires std::integral<T> && std::sentinel_for<S, T*>
```

(b) Calling it as `pred(proj(*it))` (with the default projection, just
`pred(*it)`) must compile for the range's element, the result must be
usable as a `bool`, and the lambda must be copyable.

**Check 8.** (a) The deduction guide requires matching signedness when
both are integers: `int` and `long long` are both signed, while `int` and
`size_t` are not. (b) `std::ranges::dangling`: a temporary string isn't
borrowed. (c) `std::string::iterator`: `_Range = std::string&`, an lvalue
reference, so it's borrowed.
