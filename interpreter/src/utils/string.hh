#pragma once

#include <string>
#include <array>
#include <cassert>
#include <filesystem>

template <typename T>
auto to_string(T const & t) noexcept -> decltype(std::to_string(t))
{
	return std::to_string(t);
}

constexpr auto to_string(std::string const & s) noexcept -> std::string const &
{
	return s;
}

inline auto to_string(char const * s) noexcept -> std::string
{
	return s;
}

inline auto to_string(std::string_view s) noexcept -> std::string
{
	return std::string(s);
}

inline auto to_string(bool b) noexcept -> std::string
{
	return b ? "true" : "false";
}

inline auto to_string(char c) noexcept -> std::string
{
	return std::string(1, c);
}

template <typename ... Ts>
auto join(Ts const & ... ts) noexcept -> std::string
{
	return (to_string(ts) + ...);
}

template <size_t N = 256>
auto make_null_terminated(std::string_view str) noexcept -> std::array<char, N>
{
	assert(str.size() < N);
	std::array<char, N> buffer;
	memcpy(buffer.data(), str.data(), str.size());
	buffer[str.size()] = '\0';
	return buffer;
}

auto is_null_terminated(std::string_view str) noexcept -> bool;
auto load_whole_file(std::string_view path) noexcept -> std::string;
auto load_whole_file(std::filesystem::path path) noexcept -> std::string;
auto replace(std::string_view string, std::string_view old_substr, std::string_view new_substr) noexcept -> std::string;
