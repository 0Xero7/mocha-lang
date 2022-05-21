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

	auto contextSearch = contextGroup->tryFindContext(fname->get_raw());// MochaLang::Utils::_tryFindContext(fname->get_raw(), contexts);
	/*if (!contextSearch) {
		for (auto model : importContexts) {
			contextSearch = MochaLang::Utils::findContext(fname->get_raw(), model);
			if (contextSearch)
				break;
		}
	}*/

	if (contextSearch != nullptr && contextSearch->contextType == Symbols::ContextModelType::CLASS) {
		/*auto fcall = new FunctionCall(fname);

		for (int i = 0; i < stmt->parameterSize(); ++i) {
			Statement* temp = stmt->getParamAt(i);
			performBasePass(temp, &temp);
			fcall->addParameter((Expr*)temp);
		}*/

		*source = (new ConstructorCall(stmt));
	}
}

void MochaLang::Passes::BasePass::BasePass::lowerOperatorCalls(Statement* op, Statement** source)
{
	// TODO - Make sure op is a overloadable operator

	BinaryOp* bop = (BinaryOp*)op;

	if (!Internal::validOverloadOperators.count(bop->getType())) 
		return;

	//std::vector<Symbols::ContextModel*> contexts = { context };
	auto left = MochaLang::Utils::resolveType(bop->left, contextGroup);
	if (left->contextType == Symbols::ContextModelType::FUNCTION)
		left = contextGroup->tryFindContext(left->variableType->type);
	auto right = MochaLang::Utils::resolveType(bop->right, contextGroup);
	if (right->contextType == Symbols::ContextModelType::FUNCTION)
		right = contextGroup->tryFindContext(right->variableType->type);

	std::string op_str = MochaLang::Internal::typeToOperator.at(bop->getType());

	if (left->variableType && left->variableType->type[0][0] == '#') return;

	if (left->contextType != Symbols::ContextModelType::CLASS)
		throw "Invalid lhs for operator '" + op_str + "'";

	ClassStmt* left_class = (ClassStmt*)left->context;
	std::string right_type = right->longName;

	for (OperatorOverload* oo : left_class->opOverloads)
	{
		if (oo->operatorStr != op_str) continue;

		//auto ctx = MochaLang::Utils::_tryFindContext(oo->parameters[0]->getVarType()->type, contexts);
		auto ctx = contextGroup->tryFindContext(oo->parameters[0]->getVarType()->type);

		if (ctx->longName == right_type)
		{
			auto prefix = MochaLang::Utils::TypeHelper::getTypeString(left->variableType);
			auto suffix = MochaLang::Internal::operatorToString.at(oo->operatorStr);
			auto fcall_name = prefix + "." + suffix;

			auto rightF = new FunctionCall(new Identifier({ suffix }));
			rightF->addParameter(bop->right);


			auto converted = new BinaryOp(bop->left, StmtType::OP_DOT, rightF);

			*source = converted;
			break;

			//std::cout << fcall_name << "\n";
		}
	}
}

void MochaLang::Passes::BasePass::BasePass::handleFunctionDecl(Statement* _S, Statement** source) {
	auto S = (FunctionDecl*)_S;

	contextGroup->addAndMoveToSubContext();

	for (VarDecl* v : S->getFormalParams()) handleVarDecl(v, (Statement**)&S);
	performBasePass(S->getBody(), (Statement**)&S);

	contextGroup->popSubContextAndMoveToParent();
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
		lowerOperatorCalls(S, source);
		break;

	case StmtType::FUNCTION_CALL:
		performBasePass(S, (Statement**)&S);
		break;

	case StmtType::INDEX:
		auto left = ((BinaryOp*)S)->left;
		auto indexedExprName = getIndexVariable(left);
		//if (indexedExprName->getType() == StmtType::IDEN && knownClasses.count(((Identifier*)indexedExprName)->get())) {
		//if (indexedExprName->getType() == StmtType::IDEN && MochaLang::Utils::_tryFindContext(((Identifier*)indexedExprName)->get_raw(), contexts)) {
		//	// It is an explicit array init
		//	std::vector<Expr*> collect;
		//	getIndexIndices(S, collect);

		//	*source = new ExplicitArrayInit(((Identifier*)indexedExprName)->get(), collect);
		//	break;
		//}

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

	contextGroup->currentContext->addContext(S->get()->get(), Symbols::ContextModelType::VARIABLE, false, S->getVarType(), S);

	if (S->getInit() != nullptr) {
		handleExpr(S->init, (Statement**)&S->init);
	}
}

void MochaLang::Passes::BasePass::BasePass::handleClass(Statement* _S, Statement** source) {
	auto S = (ClassStmt*)_S;

	//int beforeQty = contexts.size();
	contextGroup->currentContext = contextGroup->currentContext->addContext(S->getClassName(), MochaLang::Symbols::ContextModelType::CLASS);
	contextGroup->addAndMoveToSubContext();

	//contexts.push_back()

	for (auto* tmp : S->genericTemplates) {
		//context->addContext(MochaLang::Utils::TypeHelper::getTypeString(tmp), MochaLang::Symbols::ContextModelType::CLASS, true);
		contextGroup->currentContext->addContext(
			MochaLang::Utils::TypeHelper::getTypeString(tmp),
			MochaLang::Symbols::ContextModelType::CLASS,
			true
		);

		/*contextGroup->addToCurrentSubContext(
			new Symbols::ContextModel(
				MochaLang::Utils::TypeHelper::getTypeString(tmp), 
				MochaLang::Utils::TypeHelper::getTypeString(tmp),
				nullptr,
				MochaLang::Symbols::ContextModelType::CLASS,
				true
			)
		);*/
	}

	for (auto v : S->getMemberVariables()) handleVarDecl(v, (Statement**)&S);

	for (auto v : S->getMemberFunctions()) handleFunctionDecl(v, (Statement**)&S);
	for (auto v : S->opOverloads)
	{
		handleBlock(v->block, (Statement**)(&v->block));
	}

	//context = context->parent;
	contextGroup->popSubContextAndMoveToParent();
	contextGroup->currentContext = contextGroup->currentContext->parent;
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

	contextGroup->clear_imports();

	auto* orig = contextGroup->currentContext;
	auto packageName = ((Identifier*)S->getPackageName())->get_raw();

	for (auto& part : packageName) {
		contextGroup->currentContext = contextGroup->currentContext->addContext(part, MochaLang::Symbols::ContextModelType::PACKAGE);
	}

	handleBlock(S->packageContents, (Statement**)&S);

	contextGroup->currentContext = orig;
}

void MochaLang::Passes::BasePass::BasePass::handleProgram(Statement* _S, Statement** source) {
	auto S = (Program*)_S;

	auto* orig = contextGroup->currentContext;
	for (auto [pkgName, pkg] : S->packages) {
		handlePackage(pkg, (Statement**)&pkg);
		contextGroup->currentContext = orig;
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

		auto derivedContext = contextGroup->tryFindContext(id->get_raw());
		if (!derivedContext)
			throw "Package " + id->get() + " not found!";

		contextGroup->import_contexts.push_back(derivedContext);

		//importContexts.push_back(MochaLang::Utils::findContext(id->get_raw(), context));
	}
}

void MochaLang::Passes::BasePass::BasePass::handleBlock(Statement* _S, Statement** source) {
	auto S = (BlockStmt*)_S;

	//contextGroup->addAndMoveToSubContext();

	for (int i = 0; i < S->size(); ++i) {
		Statement* V = S->get(i);
		performBasePass(V, &V);
		S->replace(i, V);
		//performBasePass(S->get(i), (S->&get(i)));
	}

	//contextGroup->popSubContextAndMoveToParent();
}