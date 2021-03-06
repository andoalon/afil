#pragma once

#include "function_id.hh"
#include "program.hh"
#include "template_instantiation.hh"
#include "utils/expected.hh"
#include "utils/overload.hh"
#include "utils/span.hh"
#include "utils/unreachable.hh"
#include "utils/utils.hh"
#include "utils/variant.hh"
#include "utils/warning_macro.hh"
#include <string_view>
#include <variant>
#include <vector>
#include <cassert>

namespace interpreter
{

	struct ProgramStack
	{
		std::vector<char> memory;
		int base_pointer = 0;
		int top_pointer = 0;
	};
	auto read_word(ProgramStack const & stack, int address) noexcept -> int;
	auto write_word(ProgramStack & stack, int address, int value) noexcept -> void;
	template <typename T> auto read(ProgramStack const & stack, int address) noexcept -> T const &;
	template <typename T> auto write(ProgramStack & stack, int address, T const & value) noexcept -> void;
	template <typename T> auto push(ProgramStack & stack, T const & value) -> void;
	auto alloc_stack(ProgramStack & stack, int stack_size_in_bytes) noexcept -> void;
	auto alloc(ProgramStack & stack, int size, int alignment = 4) noexcept -> int;
	auto free_up_to(ProgramStack & stack, int address) noexcept -> void;
	auto pointer_at_address(ProgramStack & stack, int address) noexcept -> char *;

	enum struct ControlFlowType
	{
		Nothing,
		Return,
		Break,
		Continue
	};
	struct ControlFlow
	{
		ControlFlowType type = ControlFlowType::Nothing;
		int destroyed_stack_frame_size = 0;
	};
	constexpr ControlFlow ControlFlow_Nothing = ControlFlow{ControlFlowType::Nothing, 0};

	struct UnmetPrecondition
	{
		FunctionId function;
		int precondition;
	};

	struct RuntimeContext
	{
		complete::Program const & program;
	};

	struct CompileTimeContext
	{
		complete::Program & program;
		std::vector<complete::ResolvedTemplateParameter> & template_parameters;
		instantiation::ScopeStack & scope_stack;
		instantiation::TemplateCache & template_cache;
	};

	template <typename ExecutionContext>
	[[nodiscard]] auto call_function_with_parameters_already_set(FunctionId function_id, ProgramStack & stack, ExecutionContext context, char * return_address) noexcept
		->expected<void, UnmetPrecondition>;

	template <typename ExecutionContext, typename SetParameters>
	[[nodiscard]] auto call_function(FunctionId function_id, ProgramStack & stack, ExecutionContext context, char * return_address, SetParameters set_parameters) noexcept
		-> expected<void, UnmetPrecondition>;

	template <typename ExecutionContext>
	[[nodiscard]] auto call_function(FunctionId function_id, span<complete::Expression const> parameters, ProgramStack & stack, ExecutionContext context, char * return_address) noexcept
		->expected<void, UnmetPrecondition>;

	template <typename ExecutionContext>
	[[nodiscard]] auto eval_expression(complete::Expression const & tree, ProgramStack & stack, ExecutionContext context) noexcept -> expected<int, UnmetPrecondition>;

	template <typename ExecutionContext>
	[[nodiscard]] auto eval_expression(complete::Expression const & expr, ProgramStack & stack, ExecutionContext context, char * return_address) noexcept -> expected<void, UnmetPrecondition>;

	template <typename ExecutionContext>
	[[nodiscard]] auto run_statement(complete::Statement const & tree, ProgramStack & stack, ExecutionContext context, char * return_address) noexcept
		-> expected<ControlFlow, UnmetPrecondition>;

	[[nodiscard]] auto evaluate_constant_expression(
		complete::Expression const & expression, 
		instantiation::SemanticAnalysisArgs args,
		void * outValue
	) noexcept -> expected<void, UnmetPrecondition>;

	template <typename T>
	[[nodiscard]] auto evaluate_constant_expression_as(
		complete::Expression const & expression, 
		instantiation::SemanticAnalysisArgs args
	) noexcept -> expected<T, UnmetPrecondition>;

	// TODO: argc, argv. Decide a good stack size.
	auto run(complete::Program const & program, int stack_size = 2048) noexcept -> expected<int, UnmetPrecondition>;

} // namespace interpreter

#include "interpreter.inl"
