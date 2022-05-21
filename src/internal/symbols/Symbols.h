#pragma once

#include "../../utils/Statements.h"
#include <unordered_map>
#include <unordered_set>
#include <string>

namespace MochaLang {
namespace Internal {

	using namespace std;

	const static unordered_map<string, string> operatorToString = {
		{ "+", "__ADD__"},
		{ "-", "__MINUS__"},
		{ "*", "__MULTIPLY__"},
		{ "/", "__DIVIDE__"}
	};

	const static unordered_map<StmtType, string> typeToOperator = {
		{ StmtType::OP_ADD,		"+"},
		{ StmtType::OP_MINUS,	"-"},
		{ StmtType::OP_MUL,		"*"},
		{ StmtType::OP_DIV,		"/"},
	};

	const static unordered_set<StmtType> validOverloadOperators = {
		StmtType::OP_ADD,
		StmtType::OP_MINUS,
		StmtType::OP_MUL,
		StmtType::OP_DIV
	};

}
}