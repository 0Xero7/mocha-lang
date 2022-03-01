#include "Base.h"

void MochaLang::Passes::BasePass::BasePass::handleFunctionCall(Statement* _S, Statement** source) {
	auto stmt = (FunctionCall*)_S;

	auto fname = stmt->getFuncName();

	int qw = 4;

	for (int i = 0; i < stmt->parameterSize(); ++i) {
		Statement* temp = stmt->getParamAt(i);
		performBasePass(temp, &temp);
		stmt->replaceParamAt(i, temp);
	}


	if (knownClasses.count(fname)) {
		auto fcall = new FunctionCall(fname);

		for (int i = 0; i < stmt->parameterSize(); ++i) {
			Statement* temp = stmt->getParamAt(i);
			performBasePass(temp, &temp);
			fcall->addParameter((Expr*)temp);
		}

		*source = (new ConstructorCall(fcall));
	}
}

void MochaLang::Passes::BasePass::BasePass::handleFunctionDecl(Statement* _S, Statement** source) {
	auto S = (FunctionDecl*)_S;
	for (VarDecl* v : S->getFormalParams()) handleVarDecl(v, (Statement**)&S);
	performBasePass(S->getBody(), (Statement**)&S);
}

void MochaLang::Passes::BasePass::BasePass::handleExpr(Statement* _S, Statement** source) {
	auto S = (Expr*)_S;

	auto binaryOps = [&](Expr* SS) {
		auto temp = (BinaryOp*)SS;

		Statement* left = temp->getLeft();
		performBasePass(left, &left);

		Statement* right = temp->getRight();
		performBasePass(right, &right);

		if (left->getType() == StmtType::CONSTRUCTOR_CALL || right->getType() == StmtType::CONSTRUCTOR_CALL)
			*source = new BinaryOp((Expr*)left, SS->getType(), (Expr*)right);
	};

	switch (S->getType()) {
	case StmtType::OP_ADD:
	case StmtType::OP_MINUS:
	case StmtType::OP_MUL:
	case StmtType::OP_DIV:
	case StmtType::OP_DOT:
	case StmtType::OP_ASSIGN:
		binaryOps(S);
		break;

	case StmtType::FUNCTION_CALL:
		performBasePass(S, (Statement**)&S);
		break;
	}
}

void MochaLang::Passes::BasePass::BasePass::handleVarDecl(Statement* _S, Statement** source) {
	auto S = (VarDecl*)_S;

	if (S->getInit() != nullptr) {
		handleExpr(S->init, (Statement**)&S->init);
	}
}

void MochaLang::Passes::BasePass::BasePass::handleClass(Statement* _S, Statement** source) {
	auto S = (ClassStmt*)_S;

	for (auto v : S->getMemberFunctions()) handleFunctionDecl(v, (Statement**)&S);
	for (auto v : S->getMemberVariables()) handleVarDecl(v, (Statement**)&S);
}

void MochaLang::Passes::BasePass::BasePass::handleFor(Statement* _S, Statement** source) {
	auto S = (ForStmt*)_S;

	performBasePass(S->getInit(), (Statement**)&S);
	performBasePass(S->getCheck(), (Statement**)&S);
	performBasePass(S->getCounter(), (Statement**)&S);

	performBasePass(S->getBody(), (Statement**)&S);
}

void MochaLang::Passes::BasePass::BasePass::handleIf(Statement* _S, Statement** source) {
	auto S = (IfStmt*)_S;

	performBasePass(S->getConditional(), (Statement**)&S);
	performBasePass(S->getTrueBlock(), (Statement**)&S);
	if (S->getFalseBlock() != nullptr)
		performBasePass(S->getConditional(), (Statement**)&S);
}

void MochaLang::Passes::BasePass::BasePass::handleReturn(Statement* _S, Statement** source) {
	auto S = (ReturnStmt*)_S;
	performBasePass(S->value, (Statement**)&S->value);
}

void MochaLang::Passes::BasePass::BasePass::handleWhile(Statement* _S, Statement** source) {
	auto S = (WhileStmt*)_S;

	performBasePass(S->getCheck(), (Statement**)&S);
	performBasePass(S->getBody(), (Statement**)&S);
}

void MochaLang::Passes::BasePass::BasePass::performBasePass(Statement* stmt, Statement** source) {
	switch (stmt->getType()) {
	case StmtType::BLOCK:
		handleBlock(stmt, source);
		break;

	case StmtType::FUNCTION_DECL:
		handleFunctionDecl(stmt, source);
		break;

	case StmtType::VARDECL:
		handleVarDecl(stmt, source);
		break;

	case StmtType::FUNCTION_CALL:
		handleFunctionCall(stmt, source);
		break;

	case StmtType::CLASS:
		handleClass(stmt, source);
		break;

	case StmtType::IF:
		handleIf(stmt, source);
		break;

	case StmtType::OP_ADD:
	case StmtType::OP_MINUS:
	case StmtType::OP_MUL:
	case StmtType::OP_DIV:
	case StmtType::OP_DOT:
	case StmtType::OP_ASSIGN:
		handleExpr(stmt, source);
		break;

	case StmtType::FOR:
		handleFor(stmt, source);
		break;

	case StmtType::WHILE:
		handleWhile(stmt, source);
		break;

	case StmtType::RETURN:
		handleReturn(stmt, source);
		break;
	}
}

void MochaLang::Passes::BasePass::BasePass::handleBlock(Statement* _S, Statement** source) {
	auto S = (BlockStmt*)_S;

	for (int i = 0; i < S->size(); ++i) {
		Statement* V = S->get(i);
		performBasePass(V, &V);
		S->replace(i, V);
		//performBasePass(S->get(i), (S->&get(i)));
	}
}