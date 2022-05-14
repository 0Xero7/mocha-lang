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

	auto contextSearch = MochaLang::Utils::findContext(fname->get_raw(), context);
	if (!contextSearch) {
		for (auto model : importContexts) {
			contextSearch = MochaLang::Utils::findContext(fname->get_raw(), model);
			if (contextSearch)
				break;
		}
	}

	if (contextSearch != nullptr) {
		/*auto fcall = new FunctionCall(fname);

		for (int i = 0; i < stmt->parameterSize(); ++i) {
			Statement* temp = stmt->getParamAt(i);
			performBasePass(temp, &temp);
			fcall->addParameter((Expr*)temp);
		}*/

		*source = (new ConstructorCall(stmt));
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

		Statement* left = temp->left;
		performBasePass(temp->left, (Statement**)&temp->left);

		Statement* right = temp->right;
		performBasePass(temp->right, (Statement**)&temp->right);

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

	case StmtType::INDEX:
		auto left = ((BinaryOp*)S)->left;
		auto indexedExprName = getIndexVariable(left);
		//if (indexedExprName->getType() == StmtType::IDEN && knownClasses.count(((Identifier*)indexedExprName)->get())) {
		if (indexedExprName->getType() == StmtType::IDEN && MochaLang::Utils::findContext(((Identifier*)indexedExprName)->get_raw(), context)) {
			// It is an explicit array init
			std::vector<Expr*> collect;
			getIndexIndices(S, collect);

			*source = new ExplicitArrayInit(((Identifier*)indexedExprName)->get(), collect);
			break;
		}

		performBasePass(left, (Statement**)&left);

		auto right = ((BinaryOp*)S)->right;
		performBasePass(right, (Statement**)&right);
		break;
	}
}

void MochaLang::Passes::BasePass::BasePass::getIndexIndices(Expr* op, std::vector<Expr*>& collect)
{
	if (op->getType() != StmtType::INDEX) return;

	auto index = (BinaryOp*)op;
	getIndexIndices(index->left, collect);
	collect.push_back(index->right);
}

MochaLang::Expr* MochaLang::Passes::BasePass::BasePass::getIndexVariable(Expr* op) {
	// TODO : add other statements which can be lhs of index
	switch (op->getType()) {
	case StmtType::IDEN:
		return ((Identifier*)op);

	case StmtType::INDEX:
		return getIndexVariable(((BinaryOp*)op)->getLeft());
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

	context = context->addContext(S->getClassName(), MochaLang::Symbols::ContextModelType::CLASS);

	for (auto* tmp : S->genericTemplates) {
		context->addContext(MochaLang::Utils::TypeHelper::getTypeString(tmp), MochaLang::Symbols::ContextModelType::CLASS, true);
	}

	for (auto v : S->getMemberFunctions()) handleFunctionDecl(v, (Statement**)&S);
	for (auto v : S->getMemberVariables()) handleVarDecl(v, (Statement**)&S);

	context = context->parent;
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

void MochaLang::Passes::BasePass::BasePass::handlePackage(Statement* _S, Statement** source) {
	auto S = (PackageStmt*)_S;
	importContexts.clear();

	auto* orig = context;
	auto packageName = ((Identifier*)S->getPackageName())->get_raw();// MochaLang::Utils::flattenDotExpr(S->getPackageName());
	for (auto& part : packageName) {
		context = context->addContext(part, MochaLang::Symbols::ContextModelType::PACKAGE);
	}

	handleBlock(S->packageContents, (Statement**)&S);
	context = orig;
	//context->addContext(S->getPackageName(), S->get);
}

void MochaLang::Passes::BasePass::BasePass::handleProgram(Statement* _S, Statement** source) {
	auto S = (Program*)_S;

	auto* orig = context;
	for (auto [pkgName, pkg] : S->packages) {
		context = orig;
		handlePackage(pkg, (Statement**)&pkg);
	}
	//context->addContext(S->getPackageName(), S->get);
}

//
//void MochaLang::Passes::BasePass::BasePass::handleIndex(Statement* stmt, Statement** source) {
//	auto S = (IndexExpr*)stmt;
//	auto left = S->get();
//	performBasePass(S->index, (Statement**)&S->index);
//
//	if ()
//}

void MochaLang::Passes::BasePass::BasePass::performBasePass(Statement* stmt, Statement** source) {
	switch (stmt->getType()) {
	case StmtType::PACKAGE:
		handlePackage(stmt, source);
		break;

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
	case StmtType::INDEX:
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

	case StmtType::PROGRAM:
		handleProgram(stmt, source);
		break;

	case StmtType::IMPORT:
		handleImports(stmt, source);
		break;
	}
}

void MochaLang::Passes::BasePass::BasePass::handleImports(Statement* _S, Statement** source) {
	auto S = (ImportStmt*)_S;

	for (Expr* _id : S->getImports()) {
		Identifier* id = (Identifier*)_id;
		importContexts.push_back(MochaLang::Utils::findContext(id->get_raw(), context));
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