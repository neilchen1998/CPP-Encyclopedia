#pragma once

#include <fmt/core.h>   // fmt::print

#include <functional>

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

/// @brief A dummy structure
struct Foo
{
    Foo (int n) : num_(n)
    {

    }

    /// @brief Prints the sum of num_ and the given argument
    /// @param i An integer
    void print_add (int i) const
    {
        fmt::println("{}", (i + num_));
    }

    int num_;
};

/// @brief Prints the given integer
auto print_int = [](int v)
{
    fmt::println("{}", v);
};
