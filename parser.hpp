#pragma once

#include "types.hpp"

#include <lexertl/state_machine.hpp>
#include <parsertl/rules.hpp>
#include <parsertl/state_machine.hpp>

#include <utility>

lexertl::state_machine build_json_lexer(const parsertl::rules& prules,
	const bool lexer_only);
parsertl::state_machine build_json_parser(parsertl::rules& prules,
	json_actions& actions);

parsertl::rules create_tokens(const bool lexer_only);

std::pair<lexertl::state_machine, parsertl::state_machine>
build_json_parser(json_actions& actions);
