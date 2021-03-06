#pragma once

#include "program.hh"
#include "syntax_error.hh"
#include "utils/out.hh"
#include "utils/expected.hh"
#include <string_view>

namespace complete { struct Program; }

namespace afil
{

	[[nodiscard]] auto parse_module(std::string_view module_name) noexcept->expected<complete::Program, SyntaxError>;
	[[nodiscard]] auto parse_module(std::string_view module_name, std::string_view source) noexcept->expected<complete::Program, SyntaxError>;

} // namespace afil
