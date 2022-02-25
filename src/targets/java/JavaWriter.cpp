#include "JavaWriter.h"

std::unordered_map<MochaLang::AttrType, std::string> attr2java{
				{ MochaLang::AttrType::PUBLIC, "public" },
				{ MochaLang::AttrType::PRIVATE, "private" },
				{ MochaLang::AttrType::CONST, "const" },
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

void MochaLang::Targets::Java::JavaWriter::writeExpr(Expr* expr, bool endWithSemiColon) {
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
		pw.write({ "(" });
		writeExpr(((BinaryOp*)expr)->getLeft(), false);
		pw.write({ " ", stmt2debug.at(expr->getType()), " " });
		writeExpr(((BinaryOp*)expr)->getRight(), false);
		pw.write({ ")" });
		break;
	case StmtType::OP_DOT:
		writeExpr(((BinaryOp*)expr)->getLeft(), false);
		pw.write({ stmt2debug.at(expr->getType()) });
		writeExpr(((BinaryOp*)expr)->getRight(), false);
		break;
	}

	if (endWithSemiColon) pw.write({ ";", "\n" });
}

void MochaLang::Targets::Java::JavaWriter::writeFuncDecl(FunctionDecl* decl) {
	writeAttributes(decl->getAttrbs());
	pw.write({ decl->getReturnType(), " ", decl->getFunctionName(), "(" });
	writeStmtCollection(decl->getFormalParams());
	pw.write({ ") " });
	writeBlock(decl->getBody());
}

void MochaLang::Targets::Java::JavaWriter::writeVarDecl(VarDecl* decl, bool endWithSemiColon) {
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
	pw.write({fcall->getFuncName(), "(" });
	for (int i = 0; i < fcall->parameterSize(); ++i) {
		writeExpr(fcall->getParamAt(i), false);
		if (i < fcall->parameterSize() - 1) pw.write({ ", " });
	}
	pw.write({ ")" });
}

void MochaLang::Targets::Java::JavaWriter::writeBlock(BlockStmt* stmt, bool writeBraces) {
	if (writeBraces) {
		pw.write({ "{", "\n" });
	}
	pw.increaseIndent();

	for (int i = 0; i < stmt->size(); ++i) {
		Statement* S = stmt->get(i);
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
			writeExpr((Expr*)S, true);
			break;

		case StmtType::RETURN:
			writeReturn((ReturnStmt*)S);
			break;

		case StmtType::FUNCTION_DECL:
			writeFuncDecl((FunctionDecl*)S);
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
		}
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
	pw.increaseIndent();

	for (VarDecl* decl : cls->getMemberVariables()) {
		writeVarDecl(decl, true);
	}

	pw.writeNewLine();

	for (FunctionDecl* decl : cls->getMemberFunctions()) {
		writeFuncDecl(decl);
	}

	pw.decreaseIndent();
	pw.write({ "}" });
	pw.writeNewLine();
}

void MochaLang::Targets::Java::JavaWriter::writeAttributes(std::vector<MochaLang::Attribute>& attrbs) {
	int n = attrbs.size();
	for (int i = 0; i < n; ++i) {
		pw.write({ attr2java.at(attrbs[i].getType()) });
		if (i < n - 1) pw.write({ ", " });
	}
	if (n > 0) pw.write({ " " });
}