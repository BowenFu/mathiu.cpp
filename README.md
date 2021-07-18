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

## Features

### Differentiation

```C++
auto const x = "x"_s;
auto const e = x ^ (2_i / 3_i);
auto const d = diff(e, x);
// prints (* 2/3 (^ x -1/3))
std::cout << toString(d) << std::endl;
```

### Equation Solving

```C++
auto const x = "x"_s;
auto const eq = 2_i * x * x - 3_i * x + 1_i == 0_i;
auto const solutions = solve(eq, x);
// prints {1/2 1}
std::cout << toString(solutions) << std::endl;
```

### Inequation Solving

```C++
auto const e1 = ("x"_s ^ 2_i) - 1_i;
auto const e2 = (("x"_s - 3_i) ^ 2_i) - 3_i;;
auto const ineq = e1 > e2;
auto const suolutions = solveInequation(ineq, "x"_s);
// prints (OOInterval 7/6 inf)
std::cout << toString(solutions) << std::endl;
```

### Get Function Range

```C++
auto e = min(max(-"x"_s, "x"_s), 2_i);
auto const domain = interval(-3_i, true, 3_i, true);
auto const range = functionRange(e, "x"_s, domain);
// prints (CCInterval 0 2)
std::cout << toString(range) << std::endl;
```
