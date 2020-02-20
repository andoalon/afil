#pragma once

#include <optional>
#include <variant>

template <typename T>
struct Error
{
	Error(T const & v) : value(v) {}
	Error(T && v) : value(std::move(v)) {}

	T value;
};

template <typename T, typename ErrorT>
struct expected
{
	constexpr expected(T t) noexcept : value_or_error(std::move(t)) {}
	constexpr expected(Error<ErrorT> err) noexcept : value_or_error(std::move(err)) {}

	template <typename U, typename = std::enable_if_t<std::is_convertible_v<U, T> && !std::is_same_v<U, T>>>
	constexpr expected(U u) noexcept : value_or_error(std::move(u)) {}

	template <typename U, typename = std::enable_if_t<std::is_convertible_v<U, T> && !std::is_same_v<U, T>>>
	constexpr expected(expected<U, ErrorT> other) noexcept
	{
		if (other.has_value())
			value_or_error = std::move(other.value());
		else
			value_or_error = Error(std::move(other.error()));
	}

	constexpr bool has_value() const noexcept { return value_or_error.index() == 0; }
	constexpr operator bool() const noexcept { return has_value(); }

	constexpr T & operator * () & noexcept { return value(); }
	constexpr T && operator * () && noexcept { return std::move(value()); }
	constexpr T const & operator * () const & noexcept { return value(); }
	constexpr T const && operator * () const && noexcept { return std::move(value()); }

	constexpr T & value () & noexcept { return std::get<0>(value_or_error); }
	constexpr T && value() && noexcept { return std::move(std::get<0>(value_or_error)); }
	constexpr T const & value() const & noexcept { return std::get<0>(value_or_error); }
	constexpr T const && value() const && noexcept { return std::move(std::get<0>(value_or_error)); }

	constexpr T * operator -> () & noexcept { return &value(); }
	constexpr T const * operator -> () && noexcept { return &value(); }

	constexpr ErrorT & error() & noexcept { return std::get<1>(value_or_error).value; }
	constexpr ErrorT && error() && noexcept { return std::move(std::get<1>(value_or_error).value); }
	constexpr ErrorT const & error() const & noexcept { return std::get<1>(value_or_error).value; }
	constexpr ErrorT const && error() const && noexcept { return std::move(std::get<1>(value_or_error).value); }

private:
	std::variant<T, Error<ErrorT>> value_or_error;
};

struct success_t {};
constexpr success_t success;

template <typename ErrorT>
struct expected<void, ErrorT>
{
	constexpr expected() noexcept = default;
	constexpr expected(success_t) noexcept {};
	constexpr expected(Error<ErrorT> err) noexcept : maybe_error(std::move(err.value)) {}

	constexpr bool has_value() const noexcept { return !maybe_error.has_value(); }
	constexpr operator bool() const noexcept { return has_value(); }

	constexpr ErrorT & error() & noexcept { return *maybe_error; }
	constexpr ErrorT && error() && noexcept { return std::move(*maybe_error); }
	constexpr ErrorT const & error() const & noexcept { return *maybe_error; }
	constexpr ErrorT const && error() const && noexcept { return std::move(*maybe_error); }

private:
	std::optional<ErrorT> maybe_error;
};

template <typename T>
struct assign_to
{
	explicit assign_to(T & var) noexcept : variable_to_assign(std::addressof(var)) {}

	auto operator() (T const & t) const noexcept -> void { *variable_to_assign = t; }
	auto operator() (T && t) const noexcept -> void { *variable_to_assign = std::move(t); }

private:
	T * variable_to_assign;
};

#define try_call(result_function, expression)											\
	{																					\
		if (auto zzzResult = expression; zzzResult.has_value())							\
			static_cast<void>(result_function(std::move(zzzResult.value())));			\
		else																			\
			return Error(std::move(zzzResult.error()));									\
	}																					\


#define CONCAT2(x, y) x ## y
#define CONCAT(x, y) CONCAT2(x, y)

#define try_call_decl(result_declaration, expression)									\
	auto CONCAT(zzzResult, __LINE__) = expression;										\
	if (!CONCAT(zzzResult, __LINE__).has_value())										\
		return Error(std::move(CONCAT(zzzResult, __LINE__).error()));					\
	result_declaration = std::move(CONCAT(zzzResult, __LINE__).value());				\


#define try_call_void(expression)														\
	{																					\
		if (auto zzzResult = expression; !zzzResult.has_value())						\
			return Error(std::move(zzzResult.error()));									\
	}																					\
