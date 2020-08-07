#include "afil.hh"
#include "interpreter.hh"
#include "utils/compatibility.hh"
#include <iostream>

auto main(int argc, char const * const argv[]) -> int
{
	(void)(argc);
	(void)(argv);
	
	auto program = afil::parse_module("main");
	if (program.has_value())
	{
		auto result = interpreter::run(*program);
		if (result.has_value())
		{
			return *result;
		}
		else
		{
			std::cout << "Unmet precondition: " << reinterpret_cast<int &>(result.error().function) << ", " << result.error().precondition << '\n';
			return -1;
		}
	}
	else
	{
		std::cout << program.error() << '\n';
		system_pause();
		return -1;
	}
}
