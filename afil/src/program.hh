#pragma once

#include "complete_statement.hh"
#include "incomplete_statement.hh"
#include "complete_scope.hh"
#include "complete_expression.hh"
#include "function_id.hh"
#include "scope_stack.hh"
#include "syntax_error.hh"
#include "utils/callc.hh"
#include "utils/function_ptr.hh"
#include <optional>
#include <map>

namespace instantiation
{
	struct TemplateCache;
}

namespace complete
{

	struct Type
	{
		struct BuiltIn {};
		struct Pointer
		{
			TypeId value_type;
		};
		struct Array
		{
			TypeId value_type;
			int size;
			FunctionId destructor;
			FunctionId copy_constructor;
			FunctionId move_constructor;
		};
		struct ArrayPointer
		{
			TypeId value_type;
		};
		struct Struct
		{
			int struct_index;
		};

		struct TemplateInstantiation
		{
			StructTemplateId template_id;
			std::vector<TypeId> parameters;
		};

		int size;
		int alignment;
		std::string ABI_name;
		std::variant<BuiltIn, Pointer, Array, ArrayPointer, Struct> extra_data;
		std::optional<TemplateInstantiation> template_instantiation;
	};

	struct Function : Scope
	{
		int parameter_count = 0;     // From the variables, how many are arguments. The rest are locals.
		int parameter_size = 0;	     // Size in bytes needed for parameters.
		TypeId return_type;
		std::vector<Expression> preconditions;
		std::vector<Statement> statements;
		std::string ABI_name;
		bool is_callable_at_compile_time;
		bool is_callable_at_runtime;
	};

	struct ExternFunction
	{
		int parameter_size;
		int parameter_alignment;
		TypeId return_type;
		std::vector<TypeId> parameter_types;
		callc::CFunctionCaller caller;
		void const * function_pointer;
		std::string ABI_name;
	};

	struct IntrinsicFunction
	{
		TypeId return_type;
		std::vector<TypeId> parameter_types;
		std::string_view name;
		bool is_callable_at_runtime;
	};

	struct ResolvedTemplateParameter
	{
		std::string name;
		complete::TypeId type;
	};

	struct FunctionTemplateParameterType
	{
		struct BaseCase
		{
			TypeId type;
		};
		struct TemplateParameter
		{
			int index;
		};
		struct Pointer
		{
			value_ptr<FunctionTemplateParameterType> pointee;
		};
		struct Array
		{
			value_ptr<FunctionTemplateParameterType> value_type;
			int size;
		};
		struct ArrayPointer
		{
			value_ptr<FunctionTemplateParameterType> pointee;
		};
		struct TemplateInstantiation
		{
			StructTemplateId template_id;
			std::vector<FunctionTemplateParameterType> parameters;
		};

		std::variant<BaseCase, TemplateParameter, Pointer, Array, ArrayPointer, TemplateInstantiation> value;
		bool is_mutable : 1;
		bool is_reference : 1;
	};

	struct IntrinsicFunctionTemplate
	{
		function_ptr<auto(span<TypeId const>, Program &) -> Function> instantiation_function;
		std::vector<FunctionTemplateParameterType> parameter_types;
		std::vector<FunctionId> concepts;
		std::string ABI_name;
		int template_parameter_count;
	};

	struct FunctionTemplate
	{
		incomplete::FunctionTemplate incomplete_function;
		std::vector<FunctionId> concepts;
		std::vector<FunctionTemplateParameterType> parameter_types;
		std::vector<ResolvedTemplateParameter> scope_template_parameters;
		instantiation::ScopeStack scope_stack;
		std::string ABI_name;
	};

	struct MemberVariable : Variable
	{
		std::optional<Expression> initializer_expression;
	};
	struct Constructor
	{
		FunctionId function;
		std::string name;
	};
	struct Struct
	{
		std::vector<MemberVariable> member_variables;
		FunctionId destructor = function_id_constants::invalid;
		std::vector<Constructor> constructors;
		FunctionId default_constructor = function_id_constants::invalid;
		FunctionId copy_constructor = function_id_constants::invalid;
		FunctionId move_constructor = function_id_constants::invalid;
		bool has_compiler_generated_constructors;
	};

	struct StructTemplate
	{
		incomplete::StructTemplate incomplete_struct;
		std::vector<FunctionId> concepts;
		std::vector<ResolvedTemplateParameter> scope_template_parameters;
		instantiation::ScopeStack scope_stack;
		std::string ABI_name;
	};

	struct Namespace : Scope
	{
		std::string name;
		std::vector<Namespace> nested_namespaces;
	};

	struct Program
	{
		Program();
		Program(Program const &) = delete;
		Program(Program &&) = default;
		Program & operator = (Program const &) = delete;
		Program & operator = (Program &&) = default;

		std::vector<Type> types;
		std::vector<Struct> structs;
		std::vector<StructTemplate> struct_templates;
		std::vector<OverloadSet> overload_set_types;
		std::vector<Function> functions;
		std::vector<ExternFunction> extern_functions;
		std::vector<FunctionTemplate> function_templates;
		std::vector<Statement> global_initialization_statements;
		Namespace global_scope;
		FunctionId main_function = function_id_constants::invalid;
	};

	auto add_type(Program & program, Type new_type) noexcept -> TypeId;
	auto type_with_id(Program const & program, TypeId id) noexcept -> Type const &;
	auto type_size(Program const & program, TypeId id) noexcept -> int;
	auto type_alignment(Program const & program, TypeId id) noexcept -> int;
	auto is_default_constructible(Struct const & type) noexcept -> bool;
	auto is_default_constructible(TypeId type, Program const & program) noexcept -> bool;
	auto has_compiler_generated_constructors(Struct const & type) noexcept -> bool;
	auto synthesize_default_constructor(TypeId type_id, Struct const & struct_data) noexcept -> Expression;
	auto synthesize_default_constructor(TypeId type_id, Type::Array array_data, Program const & program) noexcept -> expression::Constructor;
	auto synthesize_default_constructor(TypeId type_id, Program const & program) noexcept -> Expression;
	auto ABI_name(Program & program, TypeId id) noexcept -> std::string &;
	auto ABI_name(Program const & program, TypeId id) noexcept -> std::string_view;
	auto is_trivially_destructible(Program const & program, TypeId id) noexcept -> bool;
	auto is_destructible_at_compile_time(Program const & program, TypeId id) noexcept -> bool;
	auto is_destructible_at_runtime(Program const & program, TypeId id) noexcept -> bool;
	auto destructor_for(Program const & program, TypeId id) noexcept -> FunctionId;
	auto is_copy_constructible(Program const & program, TypeId id) noexcept -> bool;
	auto is_trivially_copy_constructible(Program const & program, TypeId id) noexcept -> bool;
	auto is_copy_constructible_at_compile_time(Program const & program, TypeId id) noexcept -> bool;
	auto copy_constructor_for(Program const & program, TypeId id) noexcept->FunctionId;
	auto is_move_constructible(Program const & program, TypeId id) noexcept -> bool;
	auto is_trivially_move_constructible(Program const & program, TypeId id) noexcept -> bool;
	auto is_move_constructible_at_compile_time(Program const & program, TypeId id) noexcept -> bool;
	auto move_constructor_for(Program const & program, TypeId id) noexcept -> FunctionId;
	auto constructor_overload_set(Struct const & struct_data, std::string_view constructor_name) noexcept -> std::vector<FunctionId>;

	auto add_struct_type(Program & program, Type new_type, Struct new_struct) -> std::pair<TypeId, int>;
	auto add_struct_template(Program & program, StructTemplate new_template) noexcept -> StructTemplateId;
	auto is_struct(Type const & type) noexcept -> bool;
	auto struct_for_type(Program const & program, Type const & type) noexcept -> Struct const *;
	auto struct_for_type(Program const & program, TypeId type) noexcept -> Struct const *;
	auto find_member_variable(Struct const & type, std::string_view member_name) noexcept -> int;

	auto is_pointer(Type const & type) noexcept -> bool;
	auto pointer_type_for(TypeId pointee_type, Program & program) noexcept -> TypeId;
	auto pointee_type(Type const & pointer_type) noexcept -> TypeId;
	auto pointee_type(TypeId pointer_type_id, Program const & program) noexcept -> TypeId;

	auto is_array(Type const & type) noexcept -> bool;
	auto array_type_for(TypeId pointee_type, int size, Program & program) noexcept -> TypeId;
	auto array_size(Type const & array_type) noexcept -> int;
	auto array_value_type(Type const & array_type) noexcept -> TypeId;
	auto array_value_type(TypeId array_type_id, Program const & program) noexcept -> TypeId;

	auto is_array_pointer(Type const & type) noexcept -> bool;
	auto is_pointer_or_array_pointer(Type const & type) noexcept -> bool;
	auto array_pointer_type_for(TypeId value_type, Program & program) noexcept -> TypeId;

	auto add_function(Program & program, Function new_function) noexcept -> FunctionId;
	auto add_function_template(Program & program, FunctionTemplate new_function_template) noexcept -> FunctionTemplateId;
	auto parameter_types_of(Program const & program, FunctionId id) noexcept -> std::vector<TypeId>;
	auto parameter_size(Program const & program, FunctionId id) noexcept -> int;
	auto parameter_alignment(Program const & program, FunctionId id) noexcept -> int;
	auto stack_frame_size(Program const & program, FunctionId id) noexcept -> int;
	auto return_type(Program const & program, FunctionId id) noexcept -> TypeId;
	auto is_callable_at_compile_time(Program const & program, FunctionId id) noexcept -> bool;
	auto is_callable_at_runtime(Program const & program, FunctionId id) noexcept -> bool;

	auto find_namespace(Namespace & current_namespace, std::string_view name) noexcept -> Namespace *;
	auto find_namespace(Namespace & current_namespace, span<std::string_view const> names) noexcept -> Namespace *;
	auto add_namespace(Namespace & current_namespace, std::string_view name) noexcept -> Namespace &;

	auto ABI_name(Program & program, FunctionId id) noexcept -> std::string &;
	auto ABI_name(Program const & program, FunctionId id) noexcept -> std::string_view;
	auto ABI_name(Program & program, FunctionTemplateId id) noexcept -> std::string &;
	auto ABI_name(Program const & program, FunctionTemplateId id) noexcept -> std::string_view;
	auto ABI_name(Program & program, StructTemplateId id) noexcept -> std::string &;
	auto ABI_name(Program const & program, StructTemplateId id) noexcept -> std::string_view;

	auto instantiate_function_template(Program & program, FunctionTemplateId template_id, span<TypeId const> parameters, instantiation::TemplateCache & template_cache) noexcept -> expected<FunctionId, PartialSyntaxError>;
	auto instantiate_struct_template(Program & program, StructTemplateId template_id, span<TypeId const> parameters, instantiation::TemplateCache & template_cache, std::string_view instantiation_in_source) noexcept
		-> expected<TypeId, PartialSyntaxError>;

	namespace template_intrinsics
	{
		auto instantiate_destroy_function_template(span<TypeId const> parameters, Program & program) noexcept -> Function;
		auto instantiate_data_function_template(span<TypeId const> parameters, Program & program) noexcept -> Function;
		auto instantiate_data_function_template_mutable(span<TypeId const> parameters, Program & program) noexcept -> Function;
		auto instantiate_size_function_template(span<TypeId const> parameters, Program & program) noexcept -> Function;
	}

	struct ConversionNotFound
	{
		constexpr ConversionNotFound(TypeId from_, TypeId to_, std::string_view why_) noexcept : from(from_), to(to_), why(why_) {}

		TypeId from;
		TypeId to;
		std::string_view why;
	};
	auto is_mutability_conversion_legal(bool from_is_mutable, bool to_is_mutable) noexcept -> bool;
	auto insert_mutref_conversion_node(Expression && expr, TypeId from, TypeId to, Program const & program) noexcept -> expected<Expression, ConversionNotFound>;
	auto insert_mutref_conversion_node(Expression && expr, TypeId to, Program const & program) noexcept -> expected<Expression, ConversionNotFound>;
	auto insert_mutref_conversion_node(Expression && expr, TypeId from, TypeId to, Program const & program, std::string_view source) noexcept -> expected<Expression, PartialSyntaxError>;
	auto insert_mutref_conversion_node(Expression && expr, TypeId to, Program const & program, std::string_view source) noexcept -> expected<Expression, PartialSyntaxError>;

	struct OverloadSetView
	{
		constexpr OverloadSetView() noexcept = default;
		OverloadSetView(OverloadSet const & lookup_overload_set) noexcept
			: function_ids(lookup_overload_set.function_ids)
			, function_template_ids(lookup_overload_set.function_template_ids)
		{}

		span<FunctionId const> function_ids;
		span<FunctionTemplateId const> function_template_ids;
	};
	auto resolve_function_overloading(
		OverloadSetView overload_set, 
		span<TypeId const> parameters, 
		Program & program, 
		instantiation::TemplateCache & template_cache
	) noexcept -> FunctionId;

	auto resolve_function_overloading_for_conversions(
		OverloadSetView overload_set, 
		TypeId from, TypeId to, 
		Program & program, 
		instantiation::TemplateCache & template_cache
	) noexcept -> FunctionId;

	auto resolve_function_overloading_and_insert_conversions(
		OverloadSetView overload_set, 
		span<Expression> parameters, 
		span<TypeId const> parameter_types, 
		Program & program, 
		instantiation::TemplateCache & template_cache
	) noexcept -> FunctionId;

	auto type_for_overload_set(Program & program, OverloadSet overload_set) noexcept -> TypeId;
	auto overload_set_for_type(Program const & program, TypeId overload_set_type) noexcept -> OverloadSetView;

} // namespace complete
