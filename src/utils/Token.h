#pragma once

namespace MochaLang
{

	enum class TokenType
	{
		NEWLINE,

		INT,
		FLOAT,
		STRING,

		SEMICOLON,
		COMMA,
		IF,
		ELSE,
		DEF,

		IDEN,
		NUMBER,

		ADD,
		MINUS,
		MUL,
		DIV,

		ASSIGN,
		EQ,
		NEQ,
		GR,
		GE,
		LS,
		LE, 

		PRINTLN,

		PAREN_OP,
		PAREN_CL,
		BRACE_OP,
		BRACE_CL,

		INVALID
	};


	class Token
	{
	public:
		std::string tokenValue;
		TokenType tokType;

		int linenumber;
		int charnumber;

		Token() {}

		Token(std::string& tokenValue, TokenType tokType, int lineNumber, int charNumber) :
			tokenValue(tokenValue), tokType(tokType), linenumber(lineNumber), charnumber(charNumber) {}
	};
}
