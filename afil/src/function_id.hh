#pragma once


struct FunctionId
{
	enum struct Type : unsigned { program, imported, intrinsic };

	FunctionId() noexcept = default;
	constexpr FunctionId(Type type_, unsigned index_) noexcept : type(type_), index(index_) {}

	Type type : 2;
	unsigned index : 30;

};
constexpr FunctionId invalid_function_id = {FunctionId::Type::intrinsic, (1u << 30u) - 1u};

constexpr auto operator == (FunctionId a, FunctionId b) noexcept -> bool { return a.type == b.type && a.index == b.index; }
constexpr auto operator != (FunctionId a, FunctionId b) noexcept -> bool { return !(a == b); }

struct FunctionTemplateId { unsigned index; };
