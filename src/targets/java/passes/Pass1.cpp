#include "Pass1.h"

void MochaLang::Targets::Java::Passes::Pass1::handleBlock(Statement* S, Statement** source)
{
	auto block = (BlockStmt*)S;

	for (int i = 0; i < block->size(); ++i) {
		Statement* V = block->get(i);
		runPass(V, (Statement**)(&V));
		block->replace(i, V);
	}
}

void MochaLang::Targets::Java::Passes::Pass1::handleClass(Statement* S, Statement** source)
{
	auto cls = (ClassStmt*)S;

	currentClass = cls->className;

	// Replace int with $int class
	if (currentClass == "int" && currentPackage == "mocha.internal")
	{
		cls->className = "$int";
	}

	for (auto*& vdcl : cls->memberVariables)
	{
		handleVarDecl(vdcl, (Statement**)(&vdcl));
	}

	for (auto*& fdcl : cls->memberFunctions)
	{
		handleFunctionDecl(fdcl, (Statement**)(&fdcl));
	}
}

void MochaLang::Targets::Java::Passes::Pass1::handleVarDecl(Statement* S, Statement** source)
{
	auto vdcl = (VarDecl*)S;

	auto type = vdcl->getVarType();

	auto init = vdcl->getInit();
	if (init)
		handleExpr(init, (Statement**)&init);

	VarDecl* temp = new VarDecl(vdcl->get(), handleType(type), vdcl->getAttrbs(), init);
	*source = temp;
}

void MochaLang::Targets::Java::Passes::Pass1::handleFunctionDecl(Statement* S, Statement** source)
{
	auto fdcl = (FunctionDecl*)S;

	auto fixType = handleType(fdcl->returnType);

	// Constructor
	if (fixType == nullptr)
	{
		if (currentClass == "int" && currentPackage == "mocha.internal")
		{
			fdcl->functionName = "$int";
		}
	} else
	{
		fdcl->returnType = fixType;
	}

	for (auto*& v : fdcl->formalParams)
	{
		handleVarDecl(v, (Statement**)&v);
	}

	handleBlock(fdcl->body, (Statement**)&(fdcl->body));
}

void MochaLang::Targets::Java::Passes::Pass1::handleImport(Statement* S, Statement** source)
{
	auto imp = (ImportStmt*)S;

	for (Expr*& expr : imp->imports)
	{
		auto* x = (Identifier*)expr;
		if (x->get() == "mocha.internal.int")
		{
			std::vector<std::string> temp = { "mocha", "internal", "$int" };
			Identifier* id = new Identifier(temp);

			expr = id;
		}
	}
}

void MochaLang::Targets::Java::Passes::Pass1::handlePackage(Statement* S, Statement** source)
{
	auto pkg = (PackageStmt*)(S);

	auto pkgName = ((Identifier*)(pkg->getPackageName()))->get();
	currentPackage = pkgName;

	handleBlock(pkg->packageContents, (Statement**)(pkg->packageContents));

	currentPackage = "";
}

void MochaLang::Targets::Java::Passes::Pass1::handleProgram(Statement* S, Statement** source)
{
	for (auto [pkgName, pkg] : ((Program*)S)->packages) {
		handlePackage(pkg, (Statement**)&pkg);
	}
}

void MochaLang::Targets::Java::Passes::Pass1::handleFunctionCall(Statement* S, Statement** source)
{
	auto fcall = (FunctionCall*)S;

}

void MochaLang::Targets::Java::Passes::Pass1::handleConstructorCall(Statement* S, Statement** source)
{
	auto ctr = (ConstructorCall*)S;

	if (ctr->cnstrCall->getFuncName()->get() == "int"
		|| ctr->cnstrCall->getFuncName()->get() == "mocha.internal.int") // TODO - maybe ignore user classes called int [design pending]
	{
		std::vector<std::string> temp = { "$int" };
		ctr->cnstrCall->functionName = new Identifier(temp);
	}

	auto f = ctr->cnstrCall;
	handleFunctionCall(f, (Statement**)&f);
}

void MochaLang::Targets::Java::Passes::Pass1::handleExpr(Statement* S, Statement** source)
{
	const std::vector<StmtType> binaryOperators = { StmtType::OP_ADD, StmtType::OP_ASSIGN };

	if (S->getType() == StmtType::CONSTRUCTOR_CALL)
	{
		handleConstructorCall(S, source);
		return;
	}

	if (std::find(binaryOperators.begin(), binaryOperators.end(), S->getType()) != binaryOperators.end()) {
		auto binop = (BinaryOp*)S;

		handleExpr(binop->left, (Statement**)&(binop->left));
		handleExpr(binop->right, (Statement**)&(binop->right));
	}
}

void MochaLang::Targets::Java::Passes::Pass1::handleReturn(Statement* S, Statement** source)
{
	auto ret = (ReturnStmt*)S;

	handleExpr(ret->value, (Statement**)&(ret->value));
}

void MochaLang::Targets::Java::Passes::Pass1::runPass(Statement* S, Statement** source)
{
	switch (S->getType()) {

	case StmtType::PROGRAM:
		handleProgram(S, source);
		break;

	case StmtType::PACKAGE:
		handlePackage(S, source);
		break;

	case StmtType::BLOCK:
		handleBlock(S, source);
		break;

	case StmtType::IMPORT:
		handleImport(S, source);
		break;

	case StmtType::CLASS:
		handleClass(S, source);
		break;

	case StmtType::VARDECL:
		handleVarDecl(S, source);
		break;

	case StmtType::FUNCTION_DECL:
		handleFunctionDecl(S, source);
		break;

	case StmtType::CONSTRUCTOR_CALL:
	{
		auto fcall = ((ConstructorCall*)S)->cnstrCall;
		handleConstructorCall(fcall, (Statement**)&fcall);
		break;
	}

	case StmtType::FUNCTION_CALL:
		handleFunctionCall(S, source);
		break;

	case StmtType::OP_EQ:
	case StmtType::OP_ADD:
	case StmtType::OP_MINUS:
	case StmtType::OP_MUL:
	case StmtType::OP_DIV:
		handleExpr(S, source);
		break;

	case StmtType::RETURN:
		handleReturn(S, source);
		break;


		/*case StmtType::VARDECL:
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
*/
	}
}