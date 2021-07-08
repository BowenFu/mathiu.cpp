# mathiu.cpp

mathiu : a simple computer algebra system in C++

![mathiu](./mathiu.svg)

![Standard](https://img.shields.io/badge/c%2B%2B-17/20-blue.svg)
![Type](https://img.shields.io/badge/type-macro--free-blue)

![Platform](https://img.shields.io/badge/platform-linux-blue)
![Platform](https://img.shields.io/badge/platform-osx-blue)
![Platform](https://img.shields.io/badge/platform-win-blue)

[![CMake](https://github.com/BowenFu/mathiu.cpp/actions/workflows/cmake.yml/badge.svg)](https://github.com/BowenFu/mathiu.cpp/actions/workflows/cmake.yml)
[![CMake](https://github.com/BowenFu/mathiu.cpp/actions/workflows/sanitizers.yml/badge.svg)](https://github.com/BowenFu/mathiu.cpp/actions/workflows/sanitizers.yml)
![GitHub license](https://img.shields.io/github/license/BowenFu/mathiu.cpp.svg)
[![codecov](https://codecov.io/gh/BowenFu/mathiu.cpp/branch/main/graph/badge.svg?token=BNWHFLNNSD)](https://codecov.io/gh/BowenFu/mathiu.cpp)

The library originated as a demo library showing the usage of the C++ pattern matching library [`match(it)`](https://github.com/BowenFu/matchit.cpp).

But now it is developed independently.

Sample 1 for derivative.

```C++
auto const x = symbol("x");
auto const e = x ^ fraction(2, 3);
auto const d = diff(e, x);
// prints (* 2/3 (^ x -1/3))
std::cout << toString(d) << std::endl;
```

Sample 2 for equation solving.

```C++
auto const x = "x"_s;
auto const e = 2_i * x * x - 3_i * x + 1_i;
// prints {1/2 1}
std::cout << toString(solve(e, x)) << std::endl;
```
