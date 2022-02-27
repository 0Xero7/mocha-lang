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

		Attribute generateAttrbForTokenType(TokenType type) {
			switch (type) {
			case TokenType::PRIVATE:
				return PrivateAttr();
			case TokenType::PUBLIC:
				return PublicAttr();
			case TokenType::CONST:
				return ConstAttr();
			case TokenType::STATIC:
				return StaticAttr();
			}
		};

		Statement* Parser::parse(TokenStream& tk, bool without_braces = false, bool topLevel = false) {
			Token _token;
			if (!without_braces) tk.accept(_token); // ignore the first { of a block

			std::vector<Attribute> attributeAccumulator;
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
					// Handle the default constructor declaration
					if (tk.peekValue() == currentClassName) {
						block->push_back(parseFunctionDecl(tk, { MochaLangClassConstructorAttr() }, true));
						break;
					}

					// Otherwise, its just an expr
					block->push_back(parseExpr(tk, { TokenType::SEMICOLON }));

					// Since Exprs dont read the ending token, ignore it
					tk.ignore();
					break;

				case TokenType::BRACE_OP:
					block->push_back(parse(tk, true));
					break;

				case TokenType::VOID:
				case TokenType::INT:
				case TokenType::FLOAT:
					if (tk.peekType(2) == TokenType::PAREN_OP) {
						block->push_back(parseFunctionDecl(tk, attributeAccumulator));
					}
					else {
						block->push_back(parseVarDecl(tk, true, attributeAccumulator, { TokenType::SEMICOLON }));
					}

					attributeAccumulator.clear();
					break;

				case TokenType::RETURN:
					block->push_back(parseReturn(tk));
					break;
								
				case TokenType::FOR:
					block->push_back(parseFor(tk));
					break;
								
				case TokenType::WHILE:
					block->push_back(parseWhile(tk));
					break;

				case TokenType::PUBLIC:
				case TokenType::PRIVATE:
				case TokenType::CONST:
				case TokenType::STATIC:
					attributeAccumulator.push_back(generateAttrbForTokenType(tk.peekType()));
					tk.ignore();
					break;
								
				case TokenType::CLASS:
					block->push_back(parseClass(tk, attributeAccumulator));
					attributeAccumulator.clear();
					break;
								
				case TokenType::IMPORT:
					block->push_back(parseImport(tk));
					break;
								
				case TokenType::PACKAGE:
					block->push_back(parsePackage(tk));
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
				tk.ignore();
				// parse the conditional
				conditional = parseExpr(tk, { TokenType::PAREN_CL });
				tk.ignore(); // Ignore closing )
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

		Expr* Parser::parseExpr(TokenStream& tk, 
			const std::unordered_set<TokenType>& terminatingCharacters = { },
				bool functionParamMode) {

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
				if (balance == 0 && terminatingCharacters.count(tk.peekType())) {
					//tk.ignore();
					break;
				}
				/*if (balance < 0 && stopWhenParensInvalid) break;
				if (tk.peekType() == TokenType::SEMICOLON && stopAtSemicolon) {
					tk.ignore();
					break;
				}*/

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

				case TokenType::RAW_STRING:
					term.push_back(new RawString(token.tokenValue));
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
				case TokenType::DOT:
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
				auto expr = parseExpr(tk, { TokenType::COMMA, TokenType::PAREN_CL }, true);
				params.push_back(expr);

				if (tk.peekType() == TokenType::PAREN_CL) break;
				if (tk.peekType() == TokenType::COMMA) tk.ignore();
			}

			// consume ending )
			tk.ignore();

			return params;
		}

		VarDecl* Parser::parseVarDecl(TokenStream& tk, bool readEnd, 
				const std::vector<Attribute>& attrbs,
				const std::unordered_set<TokenType>& terminatingTokens) {
			Token token;
			tk.accept(token);
			auto varType = token.tokenValue;

			tk.accept(token);
			auto varName = token.tokenValue;

			Expr* expr = nullptr;

			if (tk.match(TokenType::ASSIGN)) {
				tk.ignore();
				// TODO : COMMA isn't yet implemented
				auto rhs = parseExpr(tk, terminatingTokens);
				expr = new BinaryOp(new Identifier(varName), StmtType::OP_ASSIGN, rhs);
			}

			auto decl = new VarDecl(varName, varType, attrbs, expr);
			
			// Expect a ; at the end
			if (readEnd)
				tk.ignore();

			return decl;
		}

		FunctionDecl* Parser::parseFunctionDecl(TokenStream& tk, const std::vector<Attribute>& attrbs, bool isConstructor) {
			Token token;

			if (!isConstructor)
				tk.accept(token);
			auto returnType = (isConstructor ? "" : token.tokenValue);

			tk.accept(token);
			auto functionName = token.tokenValue;

			tk.ignore(); // (

			std::vector<VarDecl*> formalParams;
			while (!tk.eof() && tk.peekType() != TokenType::PAREN_CL) {
				auto decl = parseVarDecl(tk, false, {}, { TokenType::COMMA, TokenType::PAREN_CL });
				formalParams.push_back(decl);

				if (tk.peekType() == TokenType::COMMA) {
					tk.ignore();
				}
			}

			// ignore closing )
			tk.ignore();

			// now parse the body
			Statement* block = parse(tk, false);

			return new FunctionDecl(attrbs, returnType, functionName, formalParams, (BlockStmt*)block);
		}

		ReturnStmt* Parser::parseReturn(TokenStream& tk) {
			tk.ignore(); // ignore return keyword

			Expr* expr = parseExpr(tk, { TokenType::SEMICOLON });
			tk.ignore(); // Ignore the ending ;
			return new ReturnStmt(expr);
		}
				
		ForStmt* Parser::parseFor(TokenStream& tk) {
			tk.ignore(); // ignore for keyword
			tk.ignore(); // ignore (

			VarDecl* init = parseVarDecl(tk, false, {}, { TokenType::SEMICOLON });
			tk.ignore();
			Expr* counter = parseExpr(tk, { TokenType::SEMICOLON });
			tk.ignore();
			Expr* check = parseExpr(tk, { TokenType::PAREN_CL });
			tk.ignore();

			if (!tk.match(TokenType::BRACE_OP)) throw "Invalid syntax for For block!";

			BlockStmt* body = (BlockStmt*)parse(tk);

			return new ForStmt(init, counter, check, body);
		}
				
		WhileStmt* Parser::parseWhile(TokenStream& tk) {
			tk.ignore(); // ignore while keyword
			tk.ignore(); // ignore (

			Expr* check = parseExpr(tk, { TokenType::PAREN_CL });
			tk.ignore(); // ignore )

			if (!tk.match(TokenType::BRACE_OP)) throw "Invalid syntax for While block!";

			BlockStmt* body = (BlockStmt*)parse(tk);

			return new WhileStmt(check, body);
		}
				
		ClassStmt* Parser::parseClass(TokenStream& tk, std::vector<Attribute> attrbs) {
			tk.ignore(); // ignore class keyword
			Token token;
			tk.accept(token);

			auto className = token.tokenValue;

			std::string oldClassName = currentClassName;
			currentClassName = className;

			auto block = (BlockStmt*)parse(tk);

			currentClassName = oldClassName;

			std::vector<FunctionDecl*> fdecl;
			std::vector<VarDecl*> vdecl;

			for (int i = 0; i < block->size(); ++i) {
				Statement* stmt = block->get(i);
				switch (stmt->getType()) {
				case StmtType::FUNCTION_DECL:
					fdecl.push_back((FunctionDecl*)stmt);
					break;
				case StmtType::VARDECL:
					vdecl.push_back((VarDecl*)stmt);
					break;
				}
			}

			return new ClassStmt(fdecl, vdecl, attrbs, className);
		}
				
		ImportStmt* Parser::parseImport(TokenStream& tk) {
			tk.ignore(); // ignore import keyboard

			if (tk.match(TokenType::BRACE_OP)) {
				tk.ignore();

				auto impBlock = (BlockStmt*)parse(tk);
				std::vector<Expr*> imports;
				for (int i = 0; i < impBlock->size(); ++i) {
					// TODO : check if expr is valid classpath
					imports.push_back((Expr*)impBlock->get(i));
				}

				return new ImportStmt(imports);
			}
			else {
				auto imp = parseExpr(tk, { TokenType::SEMICOLON });
				tk.ignore(); // ignore ;
				return new ImportStmt({ imp });
			}
		}
				
		PackageStmt* Parser::parsePackage(TokenStream& tk) {
			tk.ignore(); // Ignore package keyword
			auto expr = parseExpr(tk, { TokenType::SEMICOLON });
			tk.ignore(); // Ignore ;
			return new PackageStmt(expr);
		}
		//Afukmr1Whs8jqWQC
	}
}