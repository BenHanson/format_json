#include "parser.hpp"
#include "process.hpp"
#include "types.hpp"

#include <lexertl/iterator.hpp>
#include <lexertl/match_results.hpp>
#include <lexertl/memory_file.hpp>
#include <lexertl/state_machine.hpp>
#include <parsertl/match_results.hpp>
#include <parsertl/parse.hpp>
#include <parsertl/rules.hpp>
#include <parsertl/state_machine.hpp>

#include <cstdlib>
#include <cstring>
#include <exception>
#include <format>
#include <iostream>
#include <span>
#include <stdexcept>

const char g_help[] = "format_json <pathname> [--help] [--indent <val>] [--leading-ws]";

static void read_switches(const std::span<const char*>& args, switches& sw)
{
	auto iter = args.begin();
	auto end = args.end();

	++iter;

	for (; iter != end; ++iter)
	{
		const char* arg = *iter;

		if (strcmp(arg, "--help") == 0)
		{
			std::cout << g_help << '\n';
			exit(0);
		}
		if (strcmp(arg, "--indent") == 0)
		{
			++iter;

			if (iter == end)
				throw std::runtime_error("Missing value following --indent");

			sw._indent_val = atoi(*iter);
		}
		else if (strcmp(arg, "--leading-ws") == 0)
			sw._leading_ws = true;
		else
			sw._pathname = arg;
	}

	if (!sw._pathname)
		throw std::runtime_error(std::format("No pathname specified\n{}", g_help));
}

int main(int argc, const char* argv[])
{

	try
	{
		switches sw;

		read_switches(std::span(argv, argc), sw);

		const lexertl::memory_file mf(sw._pathname);

		if (sw._leading_ws)
		{
			parsertl::rules prules = create_tokens(false);
			lexertl::state_machine lsm = build_json_lexer(prules, false);
			json_actions actions;
			const parsertl::state_machine gsm = build_json_parser(prules, actions);
			lexertl::citerator iter(mf.data(), mf.data() + mf.size(), lsm);

			if (parsertl::match_results results(iter->id, gsm);
				!parsertl::parse(iter, gsm, results))
			{
				throw std::runtime_error(std::format("Failed to parse {}\n",
					sw._pathname));
			}

			const parsertl::rules prules2 = create_tokens(true);

			lsm = build_json_lexer(prules2, true);
			process_json(sw, mf, prules2, lsm);
		}
		else
		{
			json_actions json_map;
			const auto [json_lexer, json_parser] = build_json_parser(json_map);

			process_json(sw, mf, json_parser, json_lexer, json_map);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}

	return 0;
}
