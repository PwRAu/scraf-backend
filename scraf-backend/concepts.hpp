#pragma once

#include <string>
#include <concepts>

template<typename type>
concept Text = std::convertible_to<type, std::string>;
