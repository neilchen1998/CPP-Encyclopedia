# C++ Encyclopedia

## Introduction

This is my journey of learning amazing features from modern C++.
In school we might have learned C++, but missed out some of the latest and greatest parts of modern C++.
I document what I have learned on my journey.
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

### `std::invoke`

`std::invoke` is a method to invoke callable objects in modern C++.
There are several types of callable ojbect, including: a function, a lambda, a member pointer.
Prior to C++17, one needs to write several different implementations in order to accomodate all different types of callable objects.

In this chapter, we are going to call different types of callables using *std::invoke*.

Full source code:

```cpp
#include <fmt/core.h>   // fmt::print

#include <functional>   // std::invoke

/// @brief Logs and invokes a callable
/// @tparam Callable The type of the callable (function, lambda, member pointer)
/// @tparam ...Args The type of the arguments
/// @param func The callable instance (a function or a lambda)
/// @param ...args The arguments to be forwarded to the given callable
template <typename Callable, typename... Args>
void LogAndCall(Callable&& func, Args&&... args)
{
    fmt::println("Executing call...");

    std::invoke(std::forward<Callable>(func), std::forward<Args>(args)...);
}

struct Foo
{
    Foo (int n) : num_(n) {}

    /// @brief Prints the sum of num_ and the given argument
    /// @param i An integer
    void print_add (int i) const
    {
        fmt::println("{}", (i + num_));
    }

    int num_;
};

auto print_int = [](int v)
{
    fmt::println("{}", v);
};

int main()
{
    Foo e1 {111}, e2 {222};

    // the old way
    // (e2.*&Foo::print_add)(2);

    std::invoke(&Foo::print_add, e1, 1);
    std::invoke(&Foo::print_add, e2, 2);

    std::invoke(print_int, 12);

    LogAndCall(&Foo::print_add, e2, 44);

    return 0;
}
```

We create a class that has a member function *print_add*.
In the old days, we need to first get the address of the member function by &Foo::print_add*,
then we need to dereference it by adding *\** in front to be *\*&Foo:print_add*, then finally adding *.* in front of the function pointer to call it, ending up with *(e2.\*&Foo::print_add)(2)*.

The old approach is very hairy, therefore we are using *std::invoke* instead.
We can just pass the memory address of **Foo::print_add** as the first argument, the instance of the class as the second argument, and an integer as the third argument to *std::invoke*.

The place where *std::invoke* truely shines is when we create a template that takes all different type of callable objects.
Look at the example where **LogAndCall** takes a callable and some number of arguments.
We use *std::invoke* under the hood by forwarding the callable object to it and we do not need to worry about the type of the callable, be it lambda, or function pointer.

### `std::optional`

`std::optional` was introduced in C++17. It offers a method to hold a value that may or may not be present.
An instance of **optional** either contains a value or does not.

In C++, RVO (return value optimization) offers a method to avoid unnecessary construction and destruction when it comes to returning a value in functions.
However, in order for this to work, the type of the return needs to match the return type of the function exactly.

Here is a function **Entity** and we print out the function calls explictly so that we can understand what happens under the hood.

```cpp
#include <optional>
#include <fmt/core.h>   // fmt::print

struct Entity
{
    Entity(int a) noexcept
    {
        fmt::println("Constructor w/ argument");
    }

    Entity() noexcept
    {
        fmt::println("Constructor w/o argument");
    }

    Entity(Entity&&) noexcept
    {
        fmt::println("Move constructor");
    }

    Entity(const Entity&) noexcept
    {
        fmt::println("Copy constructor");
    }

    ~Entity() noexcept
    {
        fmt::println("Destructor");
    }

    Entity& operator=(const Entity&) noexcept
    {
        fmt::println("Copy assignment");

        return *this;
    }

    Entity& operator=(Entity&&) noexcept
    {
        fmt::println("Move assignment");

        return *this;
    }
};

std::optional<Entity> getEntity()
{
    return Entity{42};
}

std::optional<Entity> getEntityRVO()
{
    return std::optional<Entity>{42};
}

std::optional<Entity> getEntityInPlace()
{
    return std::optional<Entity>{std::in_place, 42};
}

std::optional<Entity> getEntityMakeOptional()
{
    return std::make_optional<Entity>(42);
}

int main()
{
    {
        fmt::println("getEntity:");
        getEntity();
        fmt::println("");
    }

    {
        fmt::println("getEntityRVO:");
        getEntityRVO();
        fmt::println("");
    }

    {
        fmt::println("getEntityInPlace:");
        getEntityInPlace();
        fmt::println("");
    }

    {
        fmt::println("getEntityMakeOptional:");
        getEntityMakeOptional();
        fmt::println("");
    }

    return 0;
}
```

One may write a function like *getEntity* and think that RVO is acheived.
However, after running it, you can find the following functions are called:

```bash
getEntity:
Constructor w/ argument
Move constructor
Destructor
Destructor
```

Ignore the last **Destructor** since that when the instance runs out of scope.
The constructor was called, followed by a move operation and a destructor.
This is due to the compiler performs implicit conversion to convert **Entity** to **std::optional<Entity>**.
As we mentioned earlier, RVO can only be achieved if the type of the return value matches exactly the function signature.

If we run the other functions, like *getEntityRVO* or *getEntityInPlace*.
We can see that only the constructor is called:

```bash
getEntityInPlace:
Constructor w/ argument
Destructor
```

Since we explictly construct an **std::optional<Entity>**, the compiler performs RVO.
*std::make_optional* and *std::in_place* can also be used to acheive the same result.

# Reference

* [Class Template Argument Deduction (CTAD)](https://en.cppreference.com/w/cpp/language/class_template_argument_deduction.html)
