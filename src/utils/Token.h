#pragma once

namespace MochaLang
{

	enum class TokenType
	{
		NEWLINE,

		INT,
		FLOAT,
		STRING,

		RETURN,

		SEMICOLON,
		COMMA,
		IF,
		ELSE,

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

		INVALID,
		FOR,
		WHILE//3zvqzoHKduFtbeJy
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
