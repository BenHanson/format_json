#include "types.hpp"

#include <lexertl/iterator.hpp>
#include <lexertl/match_results.hpp>
#include <lexertl/memory_file.hpp>
#include <lexertl/state_machine.hpp>
#include <parsertl/enums.hpp>
#include <parsertl/iterator.hpp>
#include <parsertl/match_results.hpp>
#include <parsertl/rules.hpp>
#include <parsertl/state_machine.hpp>

#include <cstdint>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

void process_json(const switches& sw,
	const lexertl::memory_file& mf,
	const parsertl::state_machine& json_parser,
	const lexertl::state_machine& json_lexer,
	const json_actions& actions)
{
	if (!mf.data())
		throw std::runtime_error(std::format("Failed to open {}", sw._pathname));

	json_state state;
	lexertl::citerator liter(mf.data(), mf.data() + mf.size(), json_lexer);

	state._sw = sw;
	state._iter = parsertl::citerator(liter, json_parser);

	for (; state._iter->entry.action != parsertl::action::accept &&
		state._iter->entry.action != parsertl::action::error; ++state._iter)
	{
		auto iter = actions.find(state._iter->entry.param);

		if (iter != actions.end())
			iter->second(state);
	}

	if (state._iter->entry.action != parsertl::action::accept)
		throw std::runtime_error(std::format("Failed to parse {}",
			sw._pathname));
}

static void print(const bool leading, const int indent, const std::string_view& vw)
{
	if (leading)
		std::cout << std::string(indent, ' ');

	std::cout << vw;
}

void process_json(const switches& sw, const lexertl::memory_file& mf,
	const parsertl::rules& prules,
	const lexertl::state_machine& sm)
{
	lexertl::citerator iter(mf.data(), mf.data() + mf.size(), sm);
	const uint16_t leading_id = prules.token_id("LEADING_WS");
	const uint16_t colon_id = prules.token_id("':'");
	const uint16_t open_curly_id = prules.token_id("'{'");
	const uint16_t open_sq_id = prules.token_id("'['");
	const uint16_t close_curly_id = prules.token_id("'}'");
	const uint16_t close_sq_id = prules.token_id("']'");
	bool leading = false;
	int indent = 0;

	for (; iter->id; ++iter)
	{
		if (iter->id == leading_id)
			leading = true;
		else if (iter->id == colon_id)
			std::cout << ": ";
		else
		{
			if (iter->id == open_curly_id ||
				iter->id == open_sq_id)
			{
				print(leading, indent, iter->view());
				indent += sw._indent_val;
			}
			else if (iter->id == close_curly_id || iter->id == close_sq_id)
			{
				indent -= sw._indent_val;
				print(leading, indent, iter->view());
			}
			else
			{
				print(leading, indent, iter->view());
			}

			leading = false;
		}
	}
}
