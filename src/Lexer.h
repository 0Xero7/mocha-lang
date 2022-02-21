#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

#include "utils/TokenStream.h"

namespace MochaLang
{
	namespace Lexer 
	{
		class Tokenizer {
		public:
			TokenStream tokenize(std::string&);
		};

	}
};
