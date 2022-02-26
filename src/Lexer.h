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
		private:
			std::string data;
			int i;

		public:
			Tokenizer(std::string& data) : data(data), i(0) { }
			TokenStream tokenize(bool = false);
		};

	}
};
