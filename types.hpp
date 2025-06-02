#pragma once

#include <parsertl/iterator.hpp>

#include <cstdint>
#include <map>
#include <string_view>

struct switches
{
	std::size_t _indent_val = 2;
	bool _leading_ws = false;
	const char* _pathname = nullptr;
};

struct json_state
{
	parsertl::citerator _iter;
	std::size_t _indent = 0;
	switches _sw;

	void indent(const std::size_t indent_val);
	void dedent(const std::size_t indent_val);

	void output_indent(const char c) const;
	void output_indent(const std::string_view& vw) const;
};

using json_actions = std::map<std::uint16_t, void(*)(json_state&)>;
