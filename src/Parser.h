#pragma once

#include <unordered_map>

#include "utils/TokenStream.h"
#include "utils/Statements.h"

namespace MochaLang
{
	namespace Parser
	{
		const std::unordered_map<StmtType, int> exprPrecedence = {
			{ StmtType::PAREN_OPEN, -9999 },
			{ StmtType::OP_DOT, 10000 },

			{ StmtType::OP_EQ, -1 },
			{ StmtType::OP_NEQ, -1 },
			{ StmtType::OP_GR, -1 },
			{ StmtType::OP_GE, -1 },
			{ StmtType::OP_LS, -1 },
			{ StmtType::OP_LE, -1 },

			{ StmtType::OP_ASSIGN, -1 },

			{ StmtType::OP_ADD, 0 },
			{ StmtType::OP_MINUS, 0 },

			{ StmtType::OP_MUL, 1 },
			{ StmtType::OP_DIV, 1 },
		};

		const std::unordered_map<TokenType, StmtType> token2stmt = {
			{ TokenType::ADD, StmtType::OP_ADD },
			{ TokenType::MINUS, StmtType::OP_MINUS },
			{ TokenType::MUL, StmtType::OP_MUL },
			{ TokenType::DIV, StmtType::OP_DIV },
			{ TokenType::ASSIGN, StmtType::OP_ASSIGN },
			{ TokenType::EQ, StmtType::OP_EQ },
			{ TokenType::NEQ, StmtType::OP_NEQ },
			{ TokenType::GR, StmtType::OP_GR },
			{ TokenType::GE, StmtType::OP_GE },
			{ TokenType::LS, StmtType::OP_LS },
			{ TokenType::LE, StmtType::OP_LE },
			{ TokenType::DOT, StmtType::OP_DOT },
		};

		class Parser
		{
		public:

			Statement* parse(TokenStream&, bool, bool);

			bool ignoreUntilNextStatement(TokenStream&);

			Expr* parseExpr(TokenStream&, bool, bool = false, bool = true, bool = false);
			void insert_binary_op(Token&, std::vector<Expr*>&, std::vector<Expr*>&);
			void insertUntilParenOpen(std::vector<Expr*>&, std::vector<Expr*>&);

			std::pair<VarDecl*, Expr*> Parser::parseVarDecl(TokenStream&, bool = true, const std::vector<Attribute> & = {});

			std::vector<Expr*> parseFunctionCall(TokenStream&);

			IfStmt* parseIf(TokenStream&);

			FunctionDecl* parseFunctionDecl(TokenStream&, const std::vector<Attribute> & = {});

			ReturnStmt* parseReturn(TokenStream&);
			
			ForStmt* parseFor(TokenStream&);
			
			WhileStmt* parseWhile(TokenStream&);
			
			ClassStmt* parseClass(TokenStream&, std::vector<Attribute>);
			
			ImportStmt* parseImport(TokenStream&);
			//g4HDtPgnTveS2ddw
		};
	}
}