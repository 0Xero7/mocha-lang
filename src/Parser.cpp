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

		int Parser::tryParseAndSkipSpecializedArguments(TokenStream& tk) {
			int i = tk.get_current_position();
			int skipBy = 0;

			if (!tk.match(TokenType::LS))
				return 0;

			try {
				parseSpecializedTypes(tk);
				skipBy = tk.get_current_position() - i;
			}
			catch (std::string error) {
				skipBy = 0;
			}
			tk.rewindTo(i);

			return skipBy;
		}

		std::vector<Type*> Parser::parseSpecializedTypes(TokenStream& tk) {
			// Skip <
			tk.ignore();

			std::vector<Type*> specTypes;
			bool errored = false;
			
			while (tk.match(TokenType::IDEN)) {
				try {
					specTypes.push_back(parseType(tk));
				}
				catch (std::string e) {
					throw e;
				}

				if (tk.match(TokenType::COMMA)) { tk.ignore(); continue; }
				if (tk.match(TokenType::GR)) { tk.ignore(); break; }

				throw std::string("Invalid symbol while parsing generic types");
			}
			
			return specTypes;
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

		std::vector<VarDecl*> Parser::parseFormalParameters(TokenStream& tk) {
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

			return formalParams;
		}

		Statement* Parser::parse(TokenStream& tk, bool without_braces = false, bool topLevel = false) {
			Token _token;
			if (!without_braces) tk.accept(_token); // ignore the first { of a block

			std::vector<Attribute> attributeAccumulator;
			BlockStmt* block = new BlockStmt();

			PackageStmt* curPackage;

			while (!tk.eof() && tk.peekType() != TokenType::BRACE_CL) {
				switch (tk.peekType()) {
				case TokenType::MULTILINE_COMMENT_START:
					while (!tk.match(TokenType::MULTILINE_COMMENT_END)) tk.ignore();
					tk.ignore();
					break;

				case TokenType::NEWLINE:
					tk.accept(_token);
					break;

				case TokenType::IF:
					block->push_back(parseIf(tk));
					break;

				case TokenType::NUMBER:
					block->push_back(parseExpr(tk, { TokenType::SEMICOLON }));
					tk.ignore();
					break;

				case TokenType::IDEN:
				{
					if (tk.peekValue() == "BREAKPOINT") {
						int x = 0;
						tk.ignore();
						break;
					}

					// Since identifier can be like X.Y.Z we need to calculate how much to look ahead by
					int lookAheadOffset = 0;
					while (true) {
						if (tk.peekType(lookAheadOffset) != TokenType::IDEN)
							throw "Expected identifier, found " + tk.peekValue(lookAheadOffset);

						++lookAheadOffset;
						if (tk.peekType(lookAheadOffset) == TokenType::DOT) ++lookAheadOffset;
						else break;
					}

					if (tk.peekType(lookAheadOffset) == TokenType::LS) {
						++lookAheadOffset;
						int hack = 1;

						while (hack > 0) {
							/*if (tk.peekType(lookAheadOffset) != TokenType::IDEN)
								throw "Invalid generic template";

							++lookAheadOffset;

							if (tk.peekType(lookAheadOffset) == TokenType::COMMA) { ++lookAheadOffset; continue; }
							if (tk.peekType(lookAheadOffset) == TokenType::GR) { ++lookAheadOffset; break; }*/
							if (tk.peekType(lookAheadOffset) == TokenType::LS) { ++hack; }
							else if (tk.peekType(lookAheadOffset) == TokenType::GR) { --hack; }
							++lookAheadOffset;
						}
					}

					while (tk.peekType(lookAheadOffset) == TokenType::BRACKET_OP) {
						++lookAheadOffset;
						if (!(tk.peekType(lookAheadOffset) == TokenType::BRACKET_CL || tk.peekType(lookAheadOffset + 1) == TokenType::BRACKET_CL))
							throw "Expected ], found " + tk.peekValue(lookAheadOffset);

						if (tk.peekType(lookAheadOffset) == TokenType::BRACKET_CL)
							++lookAheadOffset;
						else
							lookAheadOffset += 2;
					}

					// Handle operator overloads
					if (tk.peekType(lookAheadOffset) == TokenType::OPERATOR) {
						auto overload = parseOperatorOverload(tk, attributeAccumulator);
						block->push_back(overload);

						auto opName = MochaLang::Internal::operatorToString.at(overload->operatorStr);

						auto overloadFn = new FunctionDecl(attributeAccumulator, overload->returnType, opName, overload->parameters, overload->block);
						block->push_back(overloadFn);

						attributeAccumulator.clear();
						break;
					}

					// Handle the default constructor declaration
					if (tk.peekValue() == currentClassName && tk.peekType(lookAheadOffset) == TokenType::PAREN_OP) {
						std::vector<Attribute> cnstrAttrbs = { MochaLangClassConstructorAttr() };
						cnstrAttrbs.insert(cnstrAttrbs.end(), attributeAccumulator.begin(), attributeAccumulator.end());
						block->push_back(parseFunctionDecl(tk, cnstrAttrbs, true));
						attributeAccumulator.clear();
						break;
					}
					// Handle function decls with custom typenames
					if (tk.peekType(lookAheadOffset) == TokenType::IDEN
						&& (tk.peekType(lookAheadOffset + 1) == TokenType::PAREN_OP || tk.peekType(lookAheadOffset + 1) == TokenType::ARROW)) {
						block->push_back(parseFunctionDecl(tk, attributeAccumulator, false));
						attributeAccumulator.clear();
						break;
					}

					// Var Decl
					if (tk.peekType(lookAheadOffset) == TokenType::IDEN) {
						block->push_back(parseVarDecl(tk, true, attributeAccumulator, { TokenType::SEMICOLON }));
						tk.ignore();
						attributeAccumulator.clear();
						break;
					}

					// Otherwise, its just an expr
					block->push_back(parseExpr(tk, { TokenType::SEMICOLON }));

					// Since Exprs dont read the ending token, ignore it
					tk.ignore();
					break;
				}

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
					if (!topLevel)
						throw "Package is only allowed at top level";
					curPackage = parsePackage(tk);

					//block->push_back(parsePackage(tk));
					break;
				//sIVarkdd4EKKkoCR
				}

				if (topLevel) continue;
				if (without_braces) break;
			}

			// ignore the closing }
			if (!without_braces) tk.ignore();

			if (topLevel && curPackage) {
				curPackage->packageContents = block;
				return curPackage;
			}

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

		Identifier* Parser::parseIdentifier(TokenStream& tk)
		{
			std::vector<std::string> iden;
			while (true) {
				if (tk.peekType() != TokenType::IDEN)
					throw "Expected identifier, found " + tk.peekValue();

				if (tk.peekValue().find_first_of('#') != std::string::npos)
				{
					if (currentPackageName != "mocha.internal")
						throw "Invalid symbol '#'";
				}

				iden.push_back(tk.peekValue());
				tk.ignore();

				if (tk.peekType() == TokenType::DOT) tk.ignore();
				else break;
			}

			return new Identifier(iden);
		}

		Expr* Parser::parseExpr(TokenStream& tk, 
			const std::unordered_set<TokenType>& terminatingCharacters = { },
				bool functionParamMode) {

			Number ret(0);
			std::vector<Expr*> term;
			std::vector<Expr*> ops;

			bool indexable = false;

			auto popBinOp = [&]() {
				BinaryOp* top = static_cast<BinaryOp*>(ops.back());
				ops.pop_back();

				top->setRight(term.back());
				term.pop_back();
				top->setLeft(term.back());
				term.pop_back();

				term.push_back(top);
				indexable = false;
			};

			auto pushIndex = [&](Expr* index) {
				auto indexOf = term.back();
				term.pop_back();

				term.push_back(new BinaryOp(indexOf, StmtType::INDEX, index));
				indexable = true;
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
				{
					if (currentPackageName == "mocha.internal" && currentClassName == "int")
					{
						term.push_back(new Number(stoi(token.tokenValue)));
					}
					else {
						std::vector<std::string> id = { "mocha", "internal", "int" };
						auto* fcall = new FunctionCall(new Identifier(id));
						fcall->addParameter(new Number(stoi(token.tokenValue)));
						term.push_back(fcall);
					}
					indexable = true;
					break;
				}

				case TokenType::RAW_STRING:
					term.push_back(new RawString(token.tokenValue));
					indexable = true;
					break;

				case TokenType::PAREN_OP:
					++balance;
					ops.push_back(new BinaryOp(nullptr, StmtType::PAREN_OPEN, nullptr));
					indexable = !true;
					break;

				case TokenType::PAREN_CL:
					insertUntilParenOpen(ops, term);

					--balance;
					if (functionParamMode && balance < 0) {
						tk.rewind();
						exitFlag = true;
					}
					indexable = true;
					break;

				case TokenType::BRACKET_OP:
					if (indexable) {
						pushIndex(parseExpr(tk, { TokenType::BRACKET_CL }));
					}
					else {
						term.push_back(parseInlineArrayInit(tk));
					}
					
					indexable = true;
					tk.ignore();
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
					indexable = !true;
					break;

				//case TokenType::INT:
				//case TokenType::FLOAT:
				//case TokenType::STRING:
				case TokenType::IDEN:
					tk.rewind();

					/*int lookFwd = 0;
					while (tk.peekType(lookFwd) == TokenType::IDEN) {
						++lookFwd;
						if (tk.match(TokenType::DOT)) { lookFwd++; continue; }
					}

					int genericBalance = (tk.match(TokenType::LS));
					while (genericBalance > 0)
					{
						if ()
					}*/

					// Parses A.B.C but not generic args
					auto identifier = parseIdentifier(tk);

					int genArgsSkip = tryParseAndSkipSpecializedArguments(tk);
					bool hasSpecializedArgs = (genArgsSkip > 0);

					if (tk.peekType(genArgsSkip) == TokenType::PAREN_OP) { // function call
						std::vector<Type*> specializedTypes;
						if (hasSpecializedArgs) {
							specializedTypes = parseSpecializedTypes(tk);
						}

						auto funcName = identifier;
						auto* fcall = new FunctionCall(funcName);
						fcall->specializedTypes = specializedTypes;

						auto parameters = parseFunctionCall(tk);
						tk.ignore(); // Ignore )
						for (auto param : parameters) {
							fcall->addParameter(param);
						}

						term.push_back(fcall);
					}
					else {
						term.push_back(identifier);
					}
					indexable = true;
					break;
				}

			}

			while (!ops.empty()) {
				auto binOp = static_cast<BinaryOp*>(ops.back());
				popBinOp();
			}

			if (term.empty()) return nullptr;
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

		Type* Parser::parseType(TokenStream& tk) {
			std::vector<std::string> types;
			while (true) {
				if (tk.peekType() != TokenType::IDEN) {
					throw "Expected identifier, found " + tk.peekValue() + ".";
				}

				Token token;
				tk.accept(token);

				if (token.tokenValue.find_first_of('#') != std::string::npos)
				{
					if (currentPackageName != "mocha.internal") {
						std::cerr << "Invalid symbol '#'!";
						throw "Invalid symbol '#'";
					}
				}

				types.push_back(token.tokenValue);
				if (tk.peekType() == TokenType::DOT)  { tk.ignore(); continue; }
				break;
			}

			std::vector<Type*> genericArgs;
			// Parse generic arguments
			if (tk.peekType() == TokenType::LS) {
				tk.ignore();
				while (true) {
					if (tk.peekType() != TokenType::IDEN)
						throw "Invalid generic identifier!";

					/*genericArgs.push_back(tk.peekValue());
					tk.ignore();*/
					genericArgs.push_back(parseType(tk));

					if (tk.match(TokenType::COMMA)) { tk.ignore(); continue; }
					if (tk.match(TokenType::GR)) { tk.ignore(); break; }
				}
			}

			int arrayDims = 0;
			if (tk.match(TokenType::BRACKET_OP)) {
				while (true) {
					++arrayDims;
					tk.ignore();

					if (!tk.match(TokenType::BRACKET_CL))
						throw std::string("Invalid generic identifier!");
					else
						tk.ignore();

					if (!tk.match(TokenType::BRACKET_OP)) break;
				}
			}

			return new Type(types, genericArgs, arrayDims);
		}


		std::vector<Expr*> Parser::parseFunctionCall(TokenStream& tk) {
			// current token is a (
			tk.ignore();
			// parse func()
			if (tk.match(TokenType::PAREN_CL)) return { };

			std::vector<Expr*> params;

			while (!tk.eof()) {
				auto expr = parseExpr(tk, { TokenType::COMMA, TokenType::PAREN_CL });
				params.push_back(expr);

				if (tk.peekType() == TokenType::PAREN_CL) break;
				if (tk.peekType() == TokenType::COMMA) tk.ignore();
			}

			// consume ending )
			//tk.ignore();

			return params;
		}

		VarDecl* Parser::parseVarDecl(TokenStream& tk, bool readEnd, 
				const std::vector<Attribute>& attrbs,
				const std::unordered_set<TokenType>& terminatingTokens) {
			/*Token token;
			tk.accept(token);*/
			std::vector<std::string> x = { "a" };
			//auto* varType = new Identifier(x); // parseIdentifier(tk);


			auto varType = parseType(tk);
			//auto type = parseType(tk);

			//// Is it an array?
			//while (tk.match(TokenType::BRACKET_OP)) {
			//	tk.ignore();
			//	if (!tk.match(TokenType::BRACKET_CL))
			//		throw "Invalid syntax for array!";
			//	tk.ignore();

			//	//varType += "[]";
			//}

			//tk.accept(token);
			auto varName = parseIdentifier(tk);

			Expr* expr = nullptr;

			if (tk.match(TokenType::ASSIGN)) {
				tk.ignore();
				// TODO : COMMA isn't yet implemented
				auto rhs = parseExpr(tk, terminatingTokens);
				expr = new BinaryOp(varName, StmtType::OP_ASSIGN, rhs);
			}

			auto decl = new VarDecl(varName, varType, attrbs, expr);
			
			// Expect a ; at the end
			if (readEnd)
				tk.ignore();

			return decl;
		}

		FunctionDecl* Parser::parseFunctionDecl(TokenStream& tk, const std::vector<Attribute>& attrbs, bool isConstructor) {
			Type* returnType = nullptr;
			if (!isConstructor)
				returnType = parseType(tk);

			Token token;
			tk.accept(token);
			auto functionName = token.tokenValue;

			auto formalParams = parseFormalParameters(tk);
			BlockStmt* block = parseFunctionBlockOrArrow(tk);

			return new FunctionDecl(attrbs, returnType, functionName, formalParams, block);
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
			//tk.accept(token);

			auto classType = parseType(tk);
			auto className = MochaLang::Utils::TypeHelper::getConcreteType(classType); // token.tokenValue;

			std::string oldClassName = currentClassName;
			currentClassName = MochaLang::Utils::TypeHelper::getConcreteType(classType);

			/*std::vector<Identifier*> genericTemplates;
			if (tk.peekType() == TokenType::LS) {
				tk.ignore();
				if (tk.peekType() == TokenType::GR)
					throw "Generic template cannot be empty.";

				while (true) {
					auto* argument = parseIdentifier(tk);
					genericTemplates.push_back(argument);
					if (tk.peekType() == TokenType::COMMA) {
						tk.ignore();
						continue;
					}
					if (tk.peekType() == TokenType::GR) {
						tk.ignore();
						break;
					}

					throw "Syntax error parsing generic template arguments";
				}
			}*/

			auto block = (BlockStmt*)parse(tk);

			currentClassName = oldClassName;

			std::vector<FunctionDecl*> fdecl;
			std::vector<VarDecl*> vdecl;
			std::vector<ClassStmt*> nestedClasses;
			std::vector<OperatorOverload*> opOverloads;

			for (int i = 0; i < block->size(); ++i) {
				Statement* stmt = block->get(i);
				switch (stmt->getType()) {
				case StmtType::FUNCTION_DECL:
					fdecl.push_back((FunctionDecl*)stmt);
					break;
				case StmtType::VARDECL:
					vdecl.push_back((VarDecl*)stmt);
					break;
				case StmtType::CLASS:
					nestedClasses.push_back((ClassStmt*)stmt);
					break;
				case StmtType::OPERATOR_OVERLOAD:
					opOverloads.push_back((OperatorOverload*)stmt);
					break;
				}
			}

			return new ClassStmt(fdecl, vdecl, nestedClasses, opOverloads, attrbs, className, classType->genericArgs);
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
			auto expr = parseIdentifier(tk);
			currentPackageName = expr->get();

			if (currentPackageName == "mocha.internal")
			{
				if (!parsingInternals)
					throw "Cannot use package mocha.internal.";
			}

			tk.ignore(); // Ignore ;
			return new PackageStmt(expr);
		}

		InlineArrayInit* Parser::parseInlineArrayInit(TokenStream& tk)
		{
			//tk.ignore(); // Ignore [
			std::vector<Expr*> values;
			while (!tk.match(TokenType::BRACKET_CL)) {
				values.push_back(parseExpr(tk, { TokenType::COMMA, TokenType::BRACKET_CL }));
				if (tk.match(TokenType::COMMA)) tk.ignore();
			}
			//tk.ignore(); // Ignore ]

			return new InlineArrayInit(values);
		}

		BlockStmt* Parser::parseFunctionBlockOrArrow(TokenStream& tk) {
			BlockStmt* block;

			// now a function body, which can be in two formats
			// { ... statements ... }
			// or
			// => statement;
			if (tk.match(TokenType::BRACE_OP))
				block = (BlockStmt*)parse(tk, false);
			else if (tk.match(TokenType::ARROW)) {
				auto expr = parseExpr(tk, { TokenType::SEMICOLON });
				tk.ignore(); // Ignore ending ;
				block = new BlockStmt();
				block->push_back(new ReturnStmt(expr));
			}
			else
				throw "Expected '{' or '=>', found " + tk.peekValue();

			return block;
		}

		OperatorOverload* Parser::parseOperatorOverload(TokenStream& tk, std::vector<Attribute>& attributeAccumulator) {
			Type* returnType = parseType(tk);

			tk.ignore(); // Ignore operator keyword

			auto overloadedOperator = tk.peekValue();
			if (tk.match(TokenType::BRACKET_OP)) {
				if (tk.peekType(1) != TokenType::BRACKET_CL)
					throw "Invalid operator being overloaded. Expected [], found [" + tk.peekValue();
				else overloadedOperator = "[]";
				tk.ignore(); // required so that both [ and ] get passed
			}
			tk.ignore();

			// TODO: add further logic here to limit operator overloads
			auto operatorParams = parseFormalParameters(tk);
			auto operatorBody = parseFunctionBlockOrArrow(tk);

			return new OperatorOverload(attributeAccumulator,overloadedOperator, returnType, operatorParams, operatorBody);
		}
		//Afukmr1Whs8jqWQC
	}
}