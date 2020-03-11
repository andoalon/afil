#pragma once

#include "incomplete_statement.hh"
#include "syntax_error.hh"
#include "utils/expected.hh"
#include "utils/span.hh"
#include <vector>
#include <string>

/*******************************************************************************************************
Program
	Es un programa ejecutable
	run: Program -> int // argc, argv en un futuro
	Necesita:
		funciones, functiones externas
		tipos, estructuras,
		statements globales,
		tama�o del scope global,
		funci�n main

Module
	Es un archivo parseado
	link: Module a, Module b... -> Program
	Necesita:
		funciones, functiones externas
		tipos, estructuras,
		statements globales,
		scope global,
		funci�n main
		estructuras y funciones template
		El c�digo del archivo (Las templates tienen estructuras incompletas que dependen del c�digo)
		Identificadores de los m�dulos en los que depende este m�dulo

Problema:
	Todos los ids son �ndices, porque hasta ahora hab�a un solo array.
	A partir de ahora va a haber N arrays, uno por m�dulo.
Posible soluci�n:
	Nuevos tipos de id que tienen un identificador de m�dulo adem�s de �ndice en el array.
	Posibilidad: �ndice del m�dulo en la lista de m�dulos de los que depende este + �ndice en el array.
	link hace la traducci�n de estos ids intermedios a los tipos de id que estamos usando ahora.
********************************************************************************************************/
#if 1
struct ModuleId
{
	// ????
};

namespace incomplete
{
	struct Module
	{
		std::vector<incomplete::Statement> statements;
		std::string source;
		std::vector<ModuleId> dependencies;
	};
}

namespace complete
{
	struct Module
	{
		std::vector<Type> types;
		std::vector<Struct> structs;
		std::vector<StructTemplate> struct_templates;
		std::vector<OverloadSet> overload_set_types;
		std::vector<Function> functions;
		std::vector<ExternFunction> extern_functions;
		std::vector<FunctionTemplate> function_templates;
		std::vector<Statement> global_initialization_statements;
		Scope global_scope;
		FunctionId main_function = invalid_function_id;
		std::string source;
		std::vector<ModuleId> dependencies;
	};
}

auto semantic_analysis(incomplete::Module incomplete_module, span<complete::Module const> dependencies) -> expected<complete::Module, SyntaxError>;

struct Program;
auto link(span<complete::Module const> modules) -> Program;
#endif
