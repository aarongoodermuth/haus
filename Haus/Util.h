#pragma once

#include <type_traits>

template <typename Enum>
constexpr typename std::underlying_type<Enum>::type EnumCast(Enum enumval) noexcept
{
    return static_cast<typename std::underlying_type<Enum>::type>(enumval);
}