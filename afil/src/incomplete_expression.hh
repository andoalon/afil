#pragma once

#include "built_in_structures.hh"
#include "incomplete_scope.hh"
#include "operator.hh"

namespace incomplete
{

	struct Statement;
	struct Expression;
	struct DesignatedInitializer;
	struct CompilesFakeVariable;
	struct ExpressionToTest;

	namespace expression
	{

		template <typename T>
		struct Literal
		{
			Literal() noexcept = default;
			Literal(T val) noexcept : value(std::move(val)) {}

			T value;
		};

		struct Identifier
		{
			Identifier() noexcept = default;
			Identifier(std::string_view name_) noexcept : name(name_) {}

			std::vector<std::string_view> namespaces;
			std::string_view name;
		};

		struct IdentifierInsideStruct
		{
			TypeId type;
			std::string_view name;
		};

		struct MemberVariable
		{
			std::string_view name;
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

		struct ExternFunction
		{
			incomplete::ExternFunction function;
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

		struct DesignatedInitializerConstructor
		{
			value_ptr<Expression> constructed_type;
			std::vector<DesignatedInitializer> parameters;
		};

		struct Compiles
		{
			std::vector<CompilesFakeVariable> variables;
			std::vector<ExpressionToTest> body;
		};

        struct TypeOf
        {
            value_ptr<Expression> parameter;
        };

		using Variant = std::variant<
			Literal<int>, Literal<float>, Literal<bool>, Literal<std::string>, Literal<char_t>, Literal<null_t>, Literal<TypeId>,
			Dereference, Addressof, Subscript,
			Identifier, MemberVariable, IdentifierInsideStruct,
			Function, FunctionTemplate,	ExternFunction,
			FunctionCall, UnaryOperatorCall, BinaryOperatorCall,
			If, StatementBlock,
			DesignatedInitializerConstructor,
			Compiles, TypeOf
		>;

	} // namespace expression

	struct Expression
	{
		Expression() noexcept = default;
		Expression(expression::Variant var, std::string_view src) noexcept
			: variant(std::move(var))
			, source(src)
		{}

		expression::Variant variant;
		std::string_view source;
	};

	struct MemberVariable
	{
		std::string_view name;
		TypeId type;
		std::optional<Expression> initializer_expression;
	};

	struct Struct
	{
		std::string_view name;
		std::vector<MemberVariable> member_variables;
		std::vector<Constructor> constructors;
		std::variant<nothing_t, Function, defaulted_t> destructor;
		std::variant<nothing_t, Function, defaulted_t> default_constructor;
		std::variant<nothing_t, Function, defaulted_t> copy_constructor;
		std::variant<nothing_t, Function, defaulted_t> move_constructor;
	};

	struct StructTemplate : Struct
	{
		std::vector<TemplateParameter> template_parameters;
	};

	struct DesignatedInitializer
	{
		std::string_view member_name;
		Expression assigned_expression;
	};

	struct CompilesFakeVariable
	{
		Expression type;
		std::string_view name;
	};

	struct ExpressionToTest
	{
		Expression expression;
		std::optional<TypeId> expected_type;
	};

} // namespace incomplete

