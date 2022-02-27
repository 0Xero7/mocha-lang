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
			"void",
			"int",
			"float",
			"string",
			"for",
			"while",
			
			"pub",
			"private",
			"const",
			"static",

			"class",
			"import",
			"package"//oszYLldDhg48OSUd
		};


		std::unordered_set<std::string> operators = {
			",",
			".",

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
			{ ".", TokenType::DOT },

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
			{ "void", TokenType::VOID },
			{ "int", TokenType::INT },
			{ "float", TokenType::FLOAT },
			{ "return", TokenType::RETURN },
			{ "for", TokenType::FOR },
			{ "while", TokenType::WHILE },
			{ "private", TokenType::PRIVATE },
			{ "pub", TokenType::PUBLIC },
			{ "const", TokenType::CONST },
			{ "static", TokenType::STATIC },
			{ "class", TokenType::CLASS },
			{ "import", TokenType::IMPORT },
			{ "package", TokenType::PACKAGE }//4tk6pV7DLNOFEDil
		};



		TokenStream Tokenizer::tokenize(bool parsingInterpolatedString) {

			TokenStream tokens;

			int len = data.size();

#define CURR data[i]
#define PREV data[i - 1]

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

				// Raw String Literals
				if (CURR == '"') {
					if (parsingInterpolatedString) throw "Raw Strings not allowed inside interpolated string experessions.";

					std::string str;
					++i;
					bool exprAdded = false;
					std::string __ADD = "+";
					std::string __PAREN_OPEN = "(";
					std::string __PAREN_CLOSE = ")";
					while (!(CURR == '"' && PREV != '\\')) {
						if (CURR == '$') {
							i += 2; // TODO: check if next character is {

							tokens.push_back(str, TokenType::RAW_STRING, 0, 0);
							tokens.push_back(__ADD, TokenType::ADD, 0, 0);

							auto expr = tokenize(true);

							if (expr.get_tokens().size() > 1) tokens.push_back(__PAREN_OPEN, TokenType::PAREN_OP, 0, 0);
							for (auto token : expr.get_tokens()) {
								tokens.push_back(token.tokenValue, token.tokType, token.linenumber, token.charnumber);
							}
							if (expr.get_tokens().size() > 1) tokens.push_back(__PAREN_CLOSE, TokenType::PAREN_CL, 0, 0);
							exprAdded = true;
							str = "";
							continue;
						}

						if (exprAdded) {
							exprAdded = false;
							tokens.push_back(__ADD, TokenType::ADD, 0, 0);
						}
						str.push_back(CURR);
						++i;
					}
					if (!str.empty())
						tokens.push_back(str, TokenType::RAW_STRING, 0, 0);
					++i;
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
					if (CURR == '}' && parsingInterpolatedString) { ++i; break; }

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