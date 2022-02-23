#include "Parser.h"

namespace MochaLang
{
	namespace Parser
	{
		// Ignores all whitespace tokens until stream ends of a non-whitespace token is found
		// Returns true if stream ends before any non-whitespace token is found
		bool Parser::ignoreUntilNextStatement(TokenStream& tk) {
			while (!tk.eof() && tk.peekType() == TokenType::NEWLINE) tk.ignore();
			return tk.eof();
		}

		Statement* Parser::parse(TokenStream& tk, bool without_braces = false, bool topLevel = false) {
			Token _token;
			if (!without_braces) tk.accept(_token); // ignore the first { of a block

			BlockStmt* block = new BlockStmt();

			while (!tk.eof() && tk.peekType() != TokenType::BRACE_CL) {
				switch (tk.peekType()) {
				case TokenType::NEWLINE:
					tk.accept(_token);
					break;

				case TokenType::IF:
					block->push_back(parseIf(tk));
					break;

				case TokenType::IDEN:
				case TokenType::NUMBER:
					block->push_back(parseExpr(tk, false));
					break;

				case TokenType::BRACE_OP:
					block->push_back(parse(tk, true));
					break;

				case TokenType::INT:
				case TokenType::FLOAT:
					if (tk.peekType(2) == TokenType::PAREN_OP) {
						block->push_back(parseFunctionDecl(tk));
					}
					else {
						auto [decl, init] = parseVarDecl(tk);
						block->push_back(decl);
						if (init) block->push_back(init);
					}
					break;

				case TokenType::RETURN:
					block->push_back(parseReturn(tk));
					break;
								
				case TokenType::FOR:
					block->push_back(parseFor(tk));
					break;
				//sIVarkdd4EKKkoCR
				}

				if (topLevel) continue;
				if (without_braces) break;
			}

			// ignore the closing }
			if (!without_braces) tk.ignore();

			if (without_braces) return block->get(0);
			return block;

		}


		IfStmt* Parser::parseIf(TokenStream& tk) {

			Expr* conditional = nullptr;
			Statement* trueBlock = nullptr;
			Statement* falseBlock = nullptr;

			// current token is IF
			Token token;
			tk.accept(token);

			if (tk.match(TokenType::PAREN_OP)) {
				// parse the conditional
				conditional = parseExpr(tk, true);
			}

			// TODO: Add error for true block not found
			ignoreUntilNextStatement(tk);

			// True Block
			if (tk.peekType() == TokenType::BRACE_OP) {
				trueBlock = parse(tk, false);
			}
			else {
				trueBlock = parse(tk, true);
			}

			ignoreUntilNextStatement(tk);
			if (tk.peekType() == TokenType::ELSE) {
				tk.ignore();

				// TODO: Add error for false block not found
				ignoreUntilNextStatement(tk);

				if (tk.peekType() == TokenType::BRACE_OP) {
					falseBlock = parse(tk, false);
				}
				else {
					falseBlock = parse(tk, true);
				}
			}

			return new IfStmt(conditional, trueBlock, falseBlock);
		}

		Expr* Parser::parseExpr(TokenStream& tk, bool openClose = false, 
				bool functionParamMode, bool stopAtSemicolon, bool stopWhenParensInvalid) {

			Number ret(0);
			std::vector<Expr*> term;
			std::vector<Expr*> ops;

			auto popBinOp = [&]() {
				BinaryOp* top = static_cast<BinaryOp*>(ops.back());
				ops.pop_back();

				top->setRight(term.back());
				term.pop_back();
				top->setLeft(term.back());
				term.pop_back();

				term.push_back(top);
			};

			Token token;
			int balance = 0;
			bool exitFlag = false;
			while (tk.peek(token) && !exitFlag) {
				if (balance < 0 && stopWhenParensInvalid) break;
				if (tk.peekType() == TokenType::SEMICOLON && stopAtSemicolon) {
					tk.ignore();
					break;
				}

				tk.accept(token);

				switch (token.tokType) {
				case TokenType::COMMA:
					if (balance == 0) {
						exitFlag = true;
					}
					break;

				case TokenType::NUMBER:
					term.push_back(new Number(stoi(token.tokenValue)));
					break;

				case TokenType::PAREN_OP:
					++balance;
					ops.push_back(new BinaryOp(nullptr, StmtType::PAREN_OPEN, nullptr));
					break;

				case TokenType::PAREN_CL:
					insertUntilParenOpen(ops, term);
					--balance;
					if (functionParamMode && balance < 0) {
						tk.rewind();
						exitFlag = true;
					}
					if (openClose && balance == 0) exitFlag = true;
					break;

				case TokenType::ADD:
				case TokenType::MINUS:
				case TokenType::MUL:
				case TokenType::DIV:
				case TokenType::EQ:
				case TokenType::NEQ:
				case TokenType::GR:
				case TokenType::GE:
				case TokenType::LS:
				case TokenType::LE:
				case TokenType::ASSIGN:
					insert_binary_op(token, ops, term);
					break;

				case TokenType::IDEN:
					if (tk.peekType() == TokenType::PAREN_OP) { // function call
						std::string funcName = token.tokenValue;
						auto* fcall = new FunctionCall(funcName);

						auto parameters = parseFunctionCall(tk);
						for (auto param : parameters) {
							fcall->addParameter(param);
						}

						term.push_back(fcall);
					}
					else {
						term.push_back(new Identifier(token.tokenValue));
					}
					break;
				}

			}

			while (!ops.empty()) {
				auto binOp = static_cast<BinaryOp*>(ops.back());
				popBinOp();
			}

			return term.back();
		}

		void Parser::insertUntilParenOpen(std::vector<Expr*>& ops, std::vector<Expr*>& term) {
			while (!ops.empty()) {
				auto binOp = static_cast<BinaryOp*>(ops.back());

				if (binOp->getType() == StmtType::PAREN_OPEN) {
					ops.pop_back();
					return;
				}

				BinaryOp* top = static_cast<BinaryOp*>(ops.back());
				ops.pop_back();

				top->setRight(term.back());
				term.pop_back();
				top->setLeft(term.back());
				term.pop_back();

				term.push_back(top);
			}
		}

		void Parser::insert_binary_op(Token& token, std::vector<Expr*>& ops, std::vector<Expr*>& term) {
			auto opType = token2stmt.at(token.tokType);
			int curP = exprPrecedence.at(opType);
			while (!ops.empty()) {
				auto binOp = static_cast<BinaryOp*>(ops.back());
				if (exprPrecedence.at(binOp->getType()) >= curP) {
					BinaryOp* top = static_cast<BinaryOp*>(ops.back());
					ops.pop_back();

					top->setRight(term.back());
					term.pop_back();
					top->setLeft(term.back());
					term.pop_back();

					term.push_back(top);
				}
				else
					break;
			}

			ops.push_back(new BinaryOp(nullptr, opType, nullptr));
		}


		std::vector<Expr*> Parser::parseFunctionCall(TokenStream& tk) {
			// current token is a (
			tk.ignore();
			// parse func()
			if (tk.match(TokenType::PAREN_CL)) return { };

			std::vector<Expr*> params;

			while (!tk.eof()) {
				auto expr = parseExpr(tk, false, true);
				params.push_back(expr);

				if (tk.peekType() == TokenType::PAREN_CL) break;
				if (tk.peekType() == TokenType::COMMA) tk.ignore();
			}

			// consume ending )
			tk.ignore();

			return params;
		}

		std::pair<VarDecl*, Expr*> Parser::parseVarDecl(TokenStream& tk, bool readEnd) {
			Token token;
			tk.accept(token);
			auto varType = token.tokenValue;

			tk.accept(token);
			auto varName = token.tokenValue;

			auto decl = new VarDecl(varName, varType, false);
			Expr* expr = nullptr;

			if (tk.match(TokenType::ASSIGN)) {
				tk.ignore();
				auto rhs = parseExpr(tk, false);
				expr = new BinaryOp(new Identifier(varName), StmtType::OP_ASSIGN, rhs);
			}

			// Expect a ; at the end
			if (readEnd)
				tk.ignore();

			return { decl, expr };
		}

		FunctionDecl* Parser::parseFunctionDecl(TokenStream& tk) {
			Token token;
			tk.accept(token);
			auto returnType = token.tokenValue;

			tk.accept(token);
			auto functionName = token.tokenValue;

			tk.ignore(); // (

			std::vector<VarDecl*> formalParams;
			while (!tk.eof() && tk.peekType() != TokenType::PAREN_CL) {
				auto [decl, init] = parseVarDecl(tk, false);
				formalParams.push_back(decl);

				// TODO: implement default values for params
				if (tk.peekType() == TokenType::COMMA) {
					tk.ignore();
				}
			}

			// ignore closing )
			tk.ignore();

			// now parse the body
			Statement* block = parse(tk, false);

			return new FunctionDecl(returnType, functionName, formalParams, (BlockStmt*)block);
		}

		ReturnStmt* Parser::parseReturn(TokenStream& tk) {
			tk.ignore(); // ignore return keyword

			Expr* expr = parseExpr(tk);
			return new ReturnStmt(expr);
		}
				
		ForStmt* Parser::parseFor(TokenStream& tk) {
			tk.ignore(); // ignore for keywork
			tk.ignore(); // ignore (

			Expr* init = parseExpr(tk);
			Expr* counter = parseExpr(tk);
			Expr* check = parseExpr(tk, false, false, false, true);

			if (!tk.match(TokenType::BRACE_OP)) throw "Invalid syntax for For block!";

			BlockStmt* body = (BlockStmt*)parse(tk);

			return new ForStmt(init, counter, check, body);
		}
		//Afukmr1Whs8jqWQC
	}
}