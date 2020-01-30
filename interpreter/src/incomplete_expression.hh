#pragma once

#include "incomplete_scope.hh"
#include "operator.hh"

namespace incomplete
{

	struct Statement;
	struct Expression;

	namespace expression
	{

		template <typename T>
		struct Literal
		{
			T value;
		};

		struct Identifier
		{
			std::string name;
		};

		struct MemberVariable
		{
			std::string name;
			value_ptr<Expression> owner;
		};

		struct Addressof
		{
			value_ptr<Expression> operand;
		};

		struct Dereference
		{
			value_ptr<Expression> operand;
		};

		struct Subscript
		{
			value_ptr<Expression> array;
			value_ptr<Expression> index;
		};

		struct Function
		{
			incomplete::Function function;
		};

		struct FunctionTemplate
		{
			incomplete::FunctionTemplate function_template;
		};

		struct FunctionCall
		{
			std::vector<Expression> parameters;
		};

		struct UnaryOperatorCall
		{
			Operator op;
			value_ptr<Expression> operand;
		};

		struct BinaryOperatorCall
		{
			Operator op;
			value_ptr<Expression> left;
			value_ptr<Expression> right;
		};

		struct If
		{
			value_ptr<Expression> condition;
			value_ptr<Expression> then_case;
			value_ptr<Expression> else_case;
		};

		struct StatementBlock
		{
			std::vector<incomplete::Statement> statements;
		};

		struct Constructor
		{
			TypeId constructed_type;
			std::vector<Expression> parameters;
		};

		namespace detail
		{
			using ExpressionTreeBase = std::variant<
				Literal<int>, Literal<float>, Literal<bool>,
				Dereference, Addressof, Subscript,
				Identifier, MemberVariable,
				Function, FunctionTemplate,	FunctionCall, UnaryOperatorCall, BinaryOperatorCall,
				If, StatementBlock,
				Constructor
			>;
		}

	} // namespace expression

	struct Expression : public expression::detail::ExpressionTreeBase
	{
		using Base = expression::detail::ExpressionTreeBase;
		using Base::Base;
		constexpr auto as_variant() noexcept -> Base & { return *this; }
		constexpr auto as_variant() const noexcept -> Base const & { return *this; }
	};

	struct MemberVariable
	{
		std::string name;
		TypeId type;
		std::optional<Expression> initializer_expression;
	};

	struct Struct
	{
		std::string name;
		std::vector<MemberVariable> member_variables;
	};

	struct StructTemplate : Struct
	{
		std::vector<TemplateParameter> template_parameters;
	};

} // namespace incomplete
