#pragma once

#include <vector>
#include <string>

#include "Token.h"

namespace MochaLang
{
	class TokenStream 
	{
	private:
		int i = 0;
		std::vector<Token> tokens;

	public:

		void push_back(std::string& tokenValue, TokenType tokType, int line, int charAt) {
			tokens.emplace_back(tokenValue, tokType, line, charAt);
		}

		bool eof() { return i >= tokens.size(); }

		bool match(TokenType typ) {
			return (!eof() && tokens[i].tokType == typ);
		}

		void ignore() {
			if (!eof()) ++i;
		}

		bool accept(Token& token) {
			if (eof()) return false;
			token = tokens[i++];
			return true;
		}

		void rewind() {
			i = (i - 1 < 0 ? 0 : i - 1);
		}

		bool peek(Token& token) {
			if (eof()) return false;
			token = tokens[i];
			return true;
		}

		TokenType peekType(int lookForwardBy = 0) {
			if (i + lookForwardBy >= (int)tokens.size()) return TokenType::INVALID;
			return tokens[i + lookForwardBy].tokType;
		}


		std::vector<Token> get_tokens() { return tokens; }

	};
}