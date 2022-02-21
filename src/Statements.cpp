#include "utils/Statements.h"

namespace MochaLang
{

	BlockStmt::BlockStmt() : Statement(StmtType::BLOCK) { }
	void BlockStmt::push_back(Statement* stmt) { 
		statements.push_back(stmt); 
	}
	int BlockStmt::size() { return statements.size(); }
	Statement* BlockStmt::get(int index) { return statements[index]; }

	Statement::Statement(StmtType type) : type(type) { }
	StmtType Statement::getType() { return type; }

	Expr::Expr(StmtType type) : Statement(type) { }

	BinaryOp::BinaryOp(Expr *left, StmtType type, Expr *right) :
		Expr(type), left(left), right(right) { }
	Expr* BinaryOp::getLeft() { return left; }
	void BinaryOp::setLeft(Expr* left) { this->left = left; }
	Expr* BinaryOp::getRight() { return right; }
	void BinaryOp::setRight(Expr* right) { this->right = right; }

	Number::Number(int number) :
		Expr(StmtType::NUMBER), number(number) { }
	int Number::get() { return number; }

	Identifier::Identifier(std::string& identifier) :
		Expr(StmtType::IDEN), identifier(identifier) { }
	std::string Identifier::get() { return identifier; }

	FunctionCall::FunctionCall(std::string& functionName) :
		Expr(StmtType::FUNCTION_CALL), functionName(functionName) { }
	std::string FunctionCall::getFuncName() { return functionName; }
	int FunctionCall::parameterSize() { return parameters.size(); }
	void FunctionCall::addParameter(Expr* expr) { parameters.push_back(expr); }
	Expr* FunctionCall::getParamAt(int index) { return parameters[index]; }

	IfStmt::IfStmt() : Statement(StmtType::IF) { }
	IfStmt::IfStmt(Expr* conditional, Statement* trueBlock, Statement* falseBlock) :
		Statement(StmtType::IF), conditional(conditional), trueBlock(trueBlock), falseBlock(falseBlock) { }
	Expr* IfStmt::getConditional() { return conditional; }
	Statement* IfStmt::getTrueBlock() { return trueBlock; }
	Statement* IfStmt::getFalseBlock() { return falseBlock; }

	VarDecl::VarDecl(std::string& varName, bool constant) :
		Statement(StmtType::VARDECL), varName(varName), constantFlag(constant) { }
	bool VarDecl::isConstant() { return constantFlag; }
	std::string VarDecl::get() { return varName; }


}