#pragma once

namespace MochaLang
{

	enum class TokenType
	{
		NEWLINE,

		MULTILINE_COMMENT_START,
		MULTILINE_COMMENT_END,

		VOID,
		INT,
		FLOAT,
		STRING,

		RETURN,

		SEMICOLON,
		COMMA,
		DOT,
		IF,
		ELSE,

		IDEN,
		NUMBER,

		ADD,
		MINUS,
		MUL,
		DIV,

		RAW_STRING,

		ASSIGN,
		EQ,
		NEQ,
		GR,
		GE,
		LS,
		LE, 

		ARROW, // =>

		PRINTLN,

		PAREN_OP,
		PAREN_CL,
		BRACE_OP,
		BRACE_CL,
		BRACKET_OP,
		BRACKET_CL,

		INVALID,
		FOR,
		WHILE,

		PUBLIC,
		PRIVATE,
		CONST,
		STATIC,
		CLASS,
		IMPORT,
		PACKAGE,
		
		OPERATOR//3zvqzoHKduFtbeJy
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
