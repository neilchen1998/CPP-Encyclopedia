#pragma once

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
