#pragma once

#include "types.hpp"

#include <lexertl/memory_file.hpp>
#include <lexertl/state_machine.hpp>
#include <parsertl/rules.hpp>
#include <parsertl/state_machine.hpp>

void process_json(const switches& sw, const lexertl::memory_file& mf,
	const parsertl::state_machine& json_parser,
	const lexertl::state_machine& json_lexer, const json_actions& actions);
void process_json(const switches& sw, const lexertl::memory_file& mf,
	const parsertl::rules& prules, const lexertl::state_machine& sm);
