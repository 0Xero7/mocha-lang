#include "JavaWriter.h"

std::unordered_map<MochaLang::AttrType, std::string> attr2java{
				{ MochaLang::AttrType::PUBLIC, "public" },
				{ MochaLang::AttrType::PRIVATE, "private" },
				{ MochaLang::AttrType::CONST, "const" },
				{ MochaLang::AttrType::STATIC, "static" },

				{ MochaLang::AttrType::__MOCHA__CLASS_CONTRUCTOR, "" },
};

const std::unordered_map<MochaLang::StmtType, std::string> stmt2debug = {
			{ MochaLang::StmtType::OP_ASSIGN, "=" },
			{ MochaLang::StmtType::OP_ADD, "+" },
			{ MochaLang::StmtType::OP_MINUS, "-" },
			{ MochaLang::StmtType::OP_MUL, "*" },
			{ MochaLang::StmtType::OP_DIV, "/" },
			{ MochaLang::StmtType::OP_EQ, "==" },
			{ MochaLang::StmtType::OP_NEQ, "!=" },
			{ MochaLang::StmtType::OP_GR, ">" },
			{ MochaLang::StmtType::OP_GE, ">=" },
			{ MochaLang::StmtType::OP_LS, "<" },
			{ MochaLang::StmtType::OP_LE, "<=" },
			{ MochaLang::StmtType::OP_DOT, "." },//qqeQR5eblCdjCPzv
};

void MochaLang::Targets::Java::JavaWriter::writeStatement(Statement* S) {
	switch (S->getType()) {
	case StmtType::NUMBER:
	case StmtType::IDEN:
	case StmtType::OP_ASSIGN:
	case StmtType::OP_ADD:
	case StmtType::OP_MINUS:
	case StmtType::OP_MUL:
	case StmtType::OP_DIV:
	case StmtType::OP_EQ:
	case StmtType::OP_NEQ:
	case StmtType::OP_GR:
	case StmtType::OP_GE:
	case StmtType::OP_LS:
	case StmtType::OP_LE:
	case StmtType::OP_DOT:
	case StmtType::INDEX:
	case StmtType::INLINE_ARRAY_INIT:
	case StmtType::EXPLICIT_ARRAY_INIT:
		writeExpr((Expr*)S, true);
		break;

	case StmtType::BLOCK:
		writeBlock((BlockStmt*)S, false);
		break;

	case StmtType::RETURN:
		writeReturn((ReturnStmt*)S);
		break;

	case StmtType::FUNCTION_DECL:
		writeFuncDecl((FunctionDecl*)S);
		break;

	case StmtType::FUNCTION_CALL:
		writeExpr((FunctionCall*)S);
		break;

	case StmtType::CONSTRUCTOR_CALL:
		writeExpr((ConstructorCall*)S);
		break;

	case StmtType::VARDECL:
		writeVarDecl((VarDecl*)S);
		break;

	case StmtType::IMPORT:
		writeImports((ImportStmt*)S);
		break;

	case StmtType::CLASS:
		writeClass((ClassStmt*)S);
		break;

	case StmtType::FOR:
		writeFor((ForStmt*)S);
		break;

	case StmtType::IF:
		writeIf((IfStmt*)S);
		break;
		
	case StmtType::PACKAGE:
		writePackage((PackageStmt*)S);
		break;
	}
}

void MochaLang::Targets::Java::JavaWriter::writeExpr(Expr* expr, bool endWithSemiColon, bool firstLayer) {

	auto handleIndex = [&]() {
		auto typeResolver = MochaLang::Utils::TypeResolver();
		auto type = typeResolver.resolve(((InlineArrayInit*)(expr))->values[0]);
		pw.write({ "new ", type , "{ " });
		auto values = ((InlineArrayInit*)expr)->values;
		int n = values.size();
		for (int i = 0; i < n; ++i) {
			writeExpr(values[i], false);
			if (i < n - 1)
				pw.write({ ", " });
		}
		pw.write({ "}" });
	};

	auto handleExplicitArrayInit = [&]() {
		auto temp = (ExplicitArrayInit*)expr;
		pw.write({ "new ", temp->arrayType });
		for (Expr* expr : temp->values) {
			pw.write({ "[" });
			if (expr != nullptr) writeExpr(expr, false);
			pw.write({ "]" });
		}
	};

	auto handleInlineArrayInit = [&]() {
		auto typeResolver = MochaLang::Utils::TypeResolver();
		auto type = typeResolver.resolve(expr);
		pw.write({ "new ", type , " { " });
		auto values = ((InlineArrayInit*)expr)->values;
		int n = values.size();
		for (int i = 0; i < n; ++i) {
			writeExpr(values[i], false);
			if (i < n - 1)
				pw.write({ ", " });
		}
		if (n > 0) pw.write({ " " });
		pw.write({ "}" });
	};

	switch (expr->getType()) {
	case StmtType::NUMBER:
		pw.write({ std::to_string(((Number*)expr)->get()) });
		break;
	case StmtType::IDEN:
		pw.write({((Identifier*)expr)->get() });
		break;
	case StmtType::FUNCTION_CALL:
		writeFunctionCall((FunctionCall*)expr);
		break;
	case StmtType::CONSTRUCTOR_CALL:
		writeConstructorCall((ConstructorCall*)expr);
		break;
	case StmtType::RAW_STRING:
		pw.write({ "\"", ((RawString*)expr)->get(), "\""});
		break;
	case StmtType::INDEX:
		writeExpr(((BinaryOp*)expr)->getLeft(), false);
		pw.write({ "[" });
		writeExpr(((BinaryOp*)expr)->getRight(), false);
		pw.write({ "]" });
		break;
	case StmtType::INLINE_ARRAY_INIT:
		handleInlineArrayInit();
		break;
	case StmtType::EXPLICIT_ARRAY_INIT:
		handleExplicitArrayInit();
		break;

	case StmtType::OP_ASSIGN:
	case StmtType::OP_ADD:
	case StmtType::OP_MINUS:
	case StmtType::OP_MUL:
	case StmtType::OP_DIV:
	case StmtType::OP_EQ:
	case StmtType::OP_NEQ:
	case StmtType::OP_GR:
	case StmtType::OP_GE:
	case StmtType::OP_LS:
	case StmtType::OP_LE:
		if (!firstLayer) pw.write({ "(" });
		writeExpr(((BinaryOp*)expr)->getLeft(), false, false);
		pw.write({ " ", stmt2debug.at(expr->getType()), " " });
		writeExpr(((BinaryOp*)expr)->getRight(), false, false);
		if (!firstLayer) pw.write({ ")" });
		break;
	case StmtType::OP_DOT:
		writeExpr(((BinaryOp*)expr)->getLeft(), false, false);
		pw.write({ stmt2debug.at(expr->getType()) });
		writeExpr(((BinaryOp*)expr)->getRight(), false, false);
		break;
	}

	if (endWithSemiColon) pw.write({ ";", "\n" });
}

void MochaLang::Targets::Java::JavaWriter::writeFuncDecl(FunctionDecl* decl) {
	if (!decl->getAttrbs().empty() && decl->getAttrbs()[0].getType() == AttrType::__MOCHA__CLASS_CONTRUCTOR) {
		writeAttributes(decl->getAttrbs());
		pw.write({ decl->getFunctionName(), "(" });
	}
	else {
		writeAttributes(decl->getAttrbs());
		pw.write({ decl->getReturnType(), " ", decl->getFunctionName(), "(" });
	}
	writeStmtCollection(decl->getFormalParams());
	pw.write({ ") " });
	writeBlock(decl->getBody());
}

void MochaLang::Targets::Java::JavaWriter::writeVarDecl(VarDecl* decl, bool endWithSemiColon) {
	writeAttributes(decl->getAttrbs());

	pw.write({decl->getVarType(), " ", decl->get() });
	if (decl->getInit() != nullptr) {
		pw.write({" = " });
		writeExpr(((BinaryOp*)decl->getInit())->getRight(), false);
	}
	if (endWithSemiColon) pw.write({";", "\n" });
}

void MochaLang::Targets::Java::JavaWriter::writeStmtCollection(std::vector<VarDecl*> attrbs) {
	int n = attrbs.size();
	for (int i = 0; i < n; ++i) {
		writeVarDecl((VarDecl*)attrbs[i], false);
		if (i < n - 1) pw.write({", " });
	}
}

void MochaLang::Targets::Java::JavaWriter::writeReturn(ReturnStmt* ret) {
	pw.write({"return " });
	writeExpr(ret->getValue(), false);
	pw.write({";" });
	pw.writeNewLine();
}

void MochaLang::Targets::Java::JavaWriter::writeFunctionCall(FunctionCall* fcall) {
	pw.write({fcall->getFuncNameStr(), "(" });
	for (int i = 0; i < fcall->parameterSize(); ++i) {
		writeExpr(fcall->getParamAt(i), false);
		if (i < fcall->parameterSize() - 1) pw.write({ ", " });
	}
	pw.write({ ")" });
}

void MochaLang::Targets::Java::JavaWriter::writeConstructorCall(ConstructorCall* ccall) {
	auto fcall = ccall->getFunctionCall();
	pw.write({"(new ",fcall->getFuncNameStr(), "("});
	for (int i = 0; i < fcall->parameterSize(); ++i) {
		writeExpr(fcall->getParamAt(i), false);
		if (i < fcall->parameterSize() - 1) pw.write({ ", " });
	}
	pw.write({ "))" });
}

void MochaLang::Targets::Java::JavaWriter::writeBlock(BlockStmt* stmt, bool writeBraces) {
	if (writeBraces) {
		pw.write({ "{", "\n" });
	}
	pw.increaseIndent();

	for (int i = 0; i < stmt->size(); ++i) {
		writeStatement(stmt->get(i));
	}

	pw.decreaseIndent();
	if (writeBraces) pw.write({ "}", "\n" });
}

void MochaLang::Targets::Java::JavaWriter::writeImports(ImportStmt* ret) {
	for (Expr* expr : ret->getImports()) {
		pw.write({ "import " });
		writeExpr(expr, true);
	}
}

void MochaLang::Targets::Java::JavaWriter::writeClass(ClassStmt* cls) {
	writeAttributes(cls->getAttrbs());

	pw.write({ "class ", cls->getClassName(), " {" });
	pw.writeNewLine();
	pw.increaseIndent();

	for (VarDecl* decl : cls->getMemberVariables()) {
		writeVarDecl(decl, true);
	}

	pw.writeNewLine();

	for (ClassStmt* stmt : cls->nestedClasses) {
		writeClass(stmt);
	}
	pw.writeNewLine();

	for (FunctionDecl* decl : cls->getMemberFunctions()) {
		writeFuncDecl(decl);
	}

	pw.decreaseIndent();
	pw.write({ "}" });
	pw.writeNewLine();
}

void MochaLang::Targets::Java::JavaWriter::writeFor(ForStmt* cls) {
	pw.write({ "for (" });
	writeVarDecl(cls->getInit(), false);
	pw.write({ "; " });
	writeExpr(cls->getCheck(), false);
	pw.write({ "; " });
	writeExpr(cls->getCounter(), false);
	pw.write({ ") " });
	writeBlock(cls->getBody());
}

void MochaLang::Targets::Java::JavaWriter::writeIf(IfStmt* ifs) {
	pw.write({ "if (" });
	writeExpr(ifs->getConditional(), false);
	pw.write({ ") " });
	writeStatement(ifs->getTrueBlock());

	if (ifs->getFalseBlock() != nullptr) {
		pw.write({ "else " });
		writeStatement(ifs->getFalseBlock());
	}
}

void MochaLang::Targets::Java::JavaWriter::writeAttributes(std::vector<MochaLang::Attribute>& attrbs) {
	int n = attrbs.size();
	for (int i = 0; i < n; ++i) {
		if (attrbs[i].getType() == AttrType::__MOCHA__CLASS_CONTRUCTOR) continue;

		pw.write({ attr2java.at(attrbs[i].getType()) });
		if (i < n - 1) pw.write({ " " });
	}
	if (n > 0) pw.write({ " " });
}


void MochaLang::Targets::Java::JavaWriter::writePackage(PackageStmt* S) {
	/*pw.write({ "package " });
	writeExpr(S->getPackageName());*/

	auto block = (BlockStmt*)S->packageContents;
	std::queue<Statement*> imports;

	for (int i = 0; i < block->size(); ++i) {
		if (block->get(i)->getType() == StmtType::CLASS) {
			pw = PrettyWriter(indentText);
			pw.write({ "package " });
			writeExpr(S->getPackageName());

			while (!imports.empty()) {
				writeImports((ImportStmt*)imports.front());
				imports.pop();
			}

			writeStatement(block->get(i));

			flushToFile(((ClassStmt*)block->get(i))->getClassName() + ".java");
		}
		else if (block->get(i)->getType() == StmtType::IMPORT) {
			imports.push(block->get(i));
		}
		else {
			throw "Java output generator not yet implemented.";
		}
	}
}