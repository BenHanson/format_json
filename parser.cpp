#include "parser.hpp"
#include "types.hpp"

#include <lexertl/generator.hpp>
#include <parsertl/generator.hpp>
#include <lexertl/rules.hpp>
#include <parsertl/rules.hpp>
#include <lexertl/state_machine.hpp>
#include <parsertl/state_machine.hpp>

#include <format>
#include <iostream>
#include <utility>

lexertl::state_machine build_json_lexer(const parsertl::rules& prules,
	const bool lexer_only)
{
	lexertl::rules lrules;
	lexertl::state_machine json_lexer;

	lrules.insert_macro("AllValid", "[\\x1-\\xff]");
	lrules.insert_macro("Unescaped", R"({AllValid}{-}[\x1-\x19]{-}["\\])");
	lrules.insert_macro("Digit", "[0-9]");
	lrules.insert_macro("Hex", "{Digit}{+}[A-Fa-f]");
	lrules.insert_macro("Digit9", "{Digit}{-}0");

	lrules.push("[{]", prules.token_id("'{'"));
	lrules.push("[}]", prules.token_id("'}'"));
	lrules.push(",", prules.token_id("','"));
	lrules.push(":", prules.token_id("':'"));
	lrules.push("\\[", prules.token_id("'['"));
	lrules.push("\\]", prules.token_id("']'"));
	lrules.push("-?(0|{Digit9}{Digit}*)([.]{Digit}+)?([Ee][-+]?{Digit}+)?",
		prules.token_id("Number"));
	lrules.push(R"(\"({Unescaped}|\\(["\\/bfnrt]|)"
		R"(u{Hex}{Hex}{Hex}{Hex}))*\")",
		prules.token_id("String"));
	lrules.push("true", prules.token_id("true"));
	lrules.push("false", prules.token_id("false"));
	lrules.push("null", prules.token_id("null"));

	if (lexer_only)
	{
		lrules.push("^[ \\t]+", prules.token_id("LEADING_WS"));
		lrules.push("[ \\t]+", lexertl::rules::skip());
		lrules.push("\r?\n", prules.token_id("'\\n'"));
	}
	else
		lrules.push("\\s+", lexertl::rules::skip());

	lexertl::generator::build(lrules, json_lexer);
	return json_lexer;
}

parsertl::state_machine build_json_parser(parsertl::rules& prules,
	json_actions& actions)
{
	parsertl::state_machine json_parser;

	prules.push("Start", "Json");
	prules.push("Json", "Object | Array");
	prules.push("Object", "open_curly close_curly"
		"| open_curly Members close_curly");
	actions[prules.push("open_curly", "'{'")] = [](json_state& state)
		{
			state.output_indent("{\n");
			state.indent(state._sw._indent_val);
		};
	prules.push("Members", "Pair "
		"| Members comma Pair");
	prules.push("Pair", "PairName ':' Value");
	actions[prules.push("PairName", "String")] = [](json_state& state)
		{
			state.output_indent(std::format("{}: ",
				state._iter.dollar(0).view()));
		};
	prules.push("Array", "open_bracket close_bracket "
		"| open_bracket Elements close_bracket");
	actions[prules.push("open_bracket", "'['")] = [](json_state& state)
		{
			state.output_indent("[\n");
			state.indent(state._sw._indent_val);
		};
	prules.push("Elements", "ValueList");
	prules.push("ValueList", "ListValue "
		"| ValueList comma ListValue");
	actions[prules.push("ListValue", "Scalar")] = [](json_state& state)
		{
			state.output_indent(state._iter.dollar(0).view());
		};
	prules.push("ListValue", "Array");
	prules.push("ListValue", "Object");
	actions[prules.push("comma", "','")] = [](json_state& state)
		{
			std::cout << ",\n";
		};
	actions[prules.push("Value", "Scalar")] = [](json_state& state)
		{
			std::cout << state._iter.dollar(0).view();
		};
	prules.push("Value", "ValObject");
	prules.push("ValObject", "val_open_curly close_curly"
		"| val_open_curly Members close_curly");
	actions[prules.push("val_open_curly", "'{'")] = [](json_state& state)
		{
			std::cout << "{\n";
			state.indent(state._sw._indent_val);
		};
	actions[prules.push("close_curly", "'}'")] = [](json_state& state)
		{
			std::cout << '\n';
			state.dedent(state._sw._indent_val);
			state.output_indent('}');
		};
	prules.push("Value", "ValArray");
	prules.push("ValArray", "val_open_bracket close_bracket "
		"| val_open_bracket Elements close_bracket");
	actions[prules.push("val_open_bracket", "'['")] = [](json_state& state)
		{
			std::cout << "[\n";
			state.indent(state._sw._indent_val);
		};
	actions[prules.push("close_bracket", "']'")] = [](json_state& state)
		{
			std::cout << '\n';
			state.dedent(state._sw._indent_val);
			state.output_indent(']');
		};
	prules.push("Scalar", "String");
	prules.push("Scalar", "Number");
	prules.push("Scalar", "true");
	prules.push("Scalar", "false");
	prules.push("Scalar", "null");
	parsertl::generator::build(prules, json_parser);
	return json_parser;
}

parsertl::rules create_tokens(const bool lexer_only)
{
	parsertl::rules rules;

	rules.token("Number String true false null '{' '}' ',' ':' '[' ']'");

	if (lexer_only)
		rules.token("LEADING_WS WS '\\n'");

	return rules;
}

std::pair<lexertl::state_machine, parsertl::state_machine>
build_json_parser(json_actions& actions)
{
	// http://code.google.com/p/bsn-goldparser/wiki/JsonGrammar
	parsertl::rules prules = create_tokens(false);
	lexertl::state_machine json_lexer = build_json_lexer(prules, false);
	parsertl::state_machine json_parser = build_json_parser(prules, actions);

	return std::make_pair(json_lexer, json_parser);
}
