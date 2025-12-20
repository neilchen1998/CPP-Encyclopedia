# C++ Encyclopedia

## Introduction

This is my journey of learning amazing features from modern C++.
In school we might have learned C++, but missed out the latest and greatest parts of modern C++.
I document what I have learn and the use cases.
I hope this can benefit you.

## Features

* `std::variant` (C_++17)

### `std::variant`

In this section, we are learning about `std::variant`.
In modern C++, `std::variant` is used to replace `union` in C.
We are going to create a vector of `std::variant` and then print them out dependng on the actual type of each element.

Full source code:

```cpp
#include <variant>
#include <vector>

#include <fmt/core.h>

using value_t = std::variant<int, long, double, std::string>;

template<class... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};

auto myVisitor = overload
{
    [](int arg){ fmt::print("new value (int): {}\n", arg); },
    [](long arg){ fmt::print("new value (long): {}\n", arg); },
    [](double arg){ fmt::print("new value (double): {}\n", arg); },
    [](const std::string& arg){ fmt::print("new value (string): \"{}\"\n", arg); },
};

int main()
{
    std::vector<value_t> vec {10, 15l, 1.5, "Neil"};

    for (auto& v : vec)
    {
        std::visit([](auto&& arg)
        {
            fmt::print("old val: {}\t", arg);
        }, v);

        value_t w = std::visit([](auto&& arg) -> value_t { return arg + arg; }, v);

        // Approach (i): use a lambda function which compares the type of arg to figure out what print method to use
        std::visit([](auto&& arg)
        {
            // Retreive the base type of 'arg' since it can be int& for instance
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, int>)
            {
                fmt::print("new value (int): {}\n", arg);
            }
            else if constexpr (std::is_same_v<T, long>)
            {
                fmt::print("new value (long): {}\n", arg);
            }
            else if constexpr (std::is_same_v<T, double>)
            {
                fmt::print("new value (double): {}\n", arg);
            }
            else if constexpr (std::is_same_v<T, std::string>)
            {
                fmt::print("new value (string): \"{}\"\n", arg);
            }
            else
            {
                static_assert(false, "non-exhaustive visitor!\n");
            }
        }, w);

        // Approach (ii): use a lambda that has overloaded operator() methods
        std::visit(myVisitor, w);
    }

    return 0;
}
```

We need to decide the data types ahead so the compiler know all the possible data types in compile time.
In this case, we pick **int**, **long**, **double**, and **std::string** to be the four possible types.
We create a new data type called *value_t* with the aforementioned data types in the template argument list.

The compiler does not know how to handle each data type, therefore we need to provide an individual way for each type.

We use `std::visit` to visit all the possible data type of our `std::variant` element.
```cpp
std::visit([](auto&& arg)
{
    // Retrieve the base type of 'arg' since it can be int& for instance
    using T = std::decay_t<decltype(arg)>;

    if constexpr (std::is_same_v<T, int>)
    {
        fmt::print("new value (int): {}\n", arg);
    }
    else if constexpr (std::is_same_v<T, long>)
    {
        fmt::print("new value (long): {}\n", arg);
    }
    else if constexpr (std::is_same_v<T, double>)
    {
        fmt::print("new value (double): {}\n", arg);
    }
    else if constexpr (std::is_same_v<T, std::string>)
    {
        fmt::print("new value (string): \"{}\"\n", arg);
    }
    else
    {
        static_assert(false, "non-exhaustive visitor!\n");
    }
}, w);
```
`std::visit` takes a lambda function and the element itself.
We use *auto&& arg* since it acts as **forwarding reference**.
If we use **forwarding reference** in the lambda function signature, we can pass by **reference** or pass by **rvalue reference** without the need to write two seperate methods. In this example, *vec* holds the objects.
This lambda function will still work if we pass in *getVariant()*.

Let's take a closer look at the body of the lambda function, the base type of *arg* is retrieved.
We need to use *std::declay_t* to get the base type since *arg* can be *int&* or *int&&* and we do not care if it is of type rvalue or lvalue.

We need to provide all the scenarios so that the compiler will know what to do if the data type is *std::string* for instance.
In C++17, we can use *if constexpr* to ask the compiler to perform conditional branching at compile time.
We can simply compare the type of *arg* that we just retrieved and compare with some basic types using *std::is_same_v* (*std::is_same_v<U, V>* is the same as *std::is_same<U, V>::value*)

However, this approach can be improved in C++20.
In C++ 20, we can use this approach:

```cpp
template<class... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};

auto myVisitor = overload
{
    [](int arg){ fmt::print("new value (int): {}\n", arg); },
    [](long arg){ fmt::print("new value (long): {}\n", arg); },
    [](double arg){ fmt::print("new value (double): {}\n", arg); },
    [](const std::string& arg){ fmt::print("new value (string): \"{}\"\n", arg); },
};
```

The template **overload** is a **variadic template**, shown as *...*. It can accepts any number of types.
We use *: Ts...* to make **overload** inherit every lambda that we pass in.
The compiler will get confused by the different *operator()*'s. Therefore we need to tell the compiler to bring in all the *operator()*'s into a single scope.

We need to provide the actual lambda functions such that the compiler can fill them into *overload* template.
This is thanks to the advent of **Class Template argument Deduction (CTAD)** in modern C++.

# Reference

* [Class Template Argument Deduction (CTAD)](https://en.cppreference.com/w/cpp/language/class_template_argument_deduction.html)
