#pragma once

#include "../../utils/Statements.h"
#include <unordered_map>
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

}
}