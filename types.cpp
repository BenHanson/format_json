#include "types.hpp"

#include <cstddef>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

void json_state::indent(const std::size_t indent_val)
{
	_indent += indent_val;
}

void json_state::dedent(const std::size_t indent_val)
{
	_indent -= indent_val;
}

void json_state::output_indent(const char c) const
{
	std::cout << std::format("{}{}", std::string(_indent, ' '), c);
}

void json_state::output_indent(const std::string_view& vw) const
{
	std::cout << std::format("{}{}", std::string(_indent, ' '), vw);
}
