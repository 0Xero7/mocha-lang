#pragma once

#include "Lexer.h"
#include <ctype.h>

namespace MochaLang
{
	namespace Lexer
	{

		std::vector<std::string> keywords = {
			"if",
			"else",
			"elif",
			"return",

			"var",
			"int",
			"float",
			"string"//oszYLldDhg48OSUd
		};


		std::unordered_set<std::string> operators = {
			",",

			"(",
			")",
			"{",
			"}",

			"+",
			"-",
			"*",
			"/",
			"=",
			"++",
			"--",

			"!",
			"==",
			"!=",
			">",
			">=",
			"<",
			"<=",

			";"//Cpkd10dEaLjNR3z5
		};


		const std::unordered_map<std::string, TokenType> op2tokType = {
			{ ",", TokenType::COMMA },

			{ "+", TokenType::ADD },
			{ "-", TokenType::MINUS },
			{ "*", TokenType::MUL },
			{ "/", TokenType::DIV },
			{ "(", TokenType::PAREN_OP },
			{ ")", TokenType::PAREN_CL },
			{ "{", TokenType::BRACE_OP },
			{ "}", TokenType::BRACE_CL },

			{ "=", TokenType::ASSIGN },
			{ "==", TokenType::EQ },
			{ "!=", TokenType::NEQ },
			{ ">", TokenType::GR },
			{ ">=", TokenType::GE },
			{ "<", TokenType::LS },
			{ "<=", TokenType::LE },
			{ ";", TokenType::SEMICOLON }//a92D5oRrnMRVR1d0
		};

		const std::unordered_map<std::string, TokenType> tok2KeyWType = {
			{ "if", TokenType::IF },
			{ "else", TokenType::ELSE },
			{ "int", TokenType::INT },
			{ "float", TokenType::FLOAT },
			{ "return", TokenType::RETURN }//4tk6pV7DLNOFEDil
		};



		TokenStream Tokenizer::tokenize(std::string& data) {

			TokenStream tokens;

			int len = data.size();
			int i = 0;

#define CURR data[i]

			while (i < len) {
				if (CURR == '\n') {
					std::string str = "";
					tokens.push_back(str, TokenType::NEWLINE, 0, 0);
					++i;
					continue;
				}

				if (isspace(CURR)) {
					++i;
					continue;
				}


				// number
				if (isdigit(CURR)) {

					bool done = false;
					std::string number;

					while (i < len && !done) {

						switch (CURR) {
						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						case '9':
							number.push_back(CURR);
							break;

						default:	// fail
							done = true;
							break;
						}


						if (done) {
							break;
						}
						++i;
					}

					tokens.push_back(number, TokenType::NUMBER, 0, 0);
					continue;
				}


				// Identifiers and Keywords
				if (isalpha(CURR) || CURR == '_') {

					std::string str;
					while (i < len && (isalnum(CURR) || CURR == '_')) {
						str.push_back(CURR);
						++i;
					}

					if (tok2KeyWType.count(str)) {
						tokens.push_back(str, tok2KeyWType.at(str), 0, 0);
					}
					else {
						tokens.push_back(str, TokenType::IDEN, 0, 0);
					}
					continue;
				}

				// Operators
				{
					std::string op;
					while (op.empty() || (i < len && operators.count(op))) {
						op.push_back(CURR);
						if (!operators.count(op)) {
							op.pop_back();
							break;
						}
						++i;
					}

					tokens.push_back(op, op2tokType.at(op), 0, 0);
					continue;
				}

			}

			return tokens;
		}

	}
}