#include "utils/Statements.h"

namespace MochaLang
{
	Statement::Statement(StmtType type) : type(type) { }
	StmtType Statement::getType() { return type; }

	BlockStmt::BlockStmt() : Statement(StmtType::BLOCK) { }
	void BlockStmt::push_back(Statement* stmt) {
		statements.push_back(stmt);
	}
	int BlockStmt::size() { return statements.size(); }
	Statement* BlockStmt::get(int index) { return statements[index]; }
	
	void MochaLang::BlockStmt::replace(int index, Statement* replaceWith) { statements[index] = replaceWith; }

	Expr::Expr(StmtType type) : Statement(type) { }

	BinaryOp::BinaryOp(Expr* left, StmtType type, Expr* right) :
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

	RawString::RawString(std::string string) :
		Expr(StmtType::RAW_STRING), string(string) { }
	std::string RawString::get() { return string; }

	InlineArrayInit::InlineArrayInit(std::vector<Expr*>& values) 
		: Expr(StmtType::INLINE_ARRAY_INIT), values(values) { }

	ExplicitArrayInit::ExplicitArrayInit(std::string arrayType, std::vector<Expr*>& values)
		: Expr(StmtType::EXPLICIT_ARRAY_INIT), arrayType(arrayType), values(values) { }

	FunctionCall::FunctionCall(std::string& functionName) :
		Expr(StmtType::FUNCTION_CALL), functionName(functionName) { }
	std::string FunctionCall::getFuncName() { return functionName; }
	int FunctionCall::parameterSize() { return parameters.size(); }
	void FunctionCall::addParameter(Expr* expr) { parameters.push_back(expr); }
	Expr* FunctionCall::getParamAt(int index) { return parameters[index]; }
	void FunctionCall::replaceParamAt(int index, Statement* stmt) { parameters[index] = (Expr*)stmt; }

	ConstructorCall::ConstructorCall(FunctionCall* cnstrCall)
			: Expr(StmtType::CONSTRUCTOR_CALL), cnstrCall(cnstrCall) { }
	FunctionCall* MochaLang::ConstructorCall::getFunctionCall() { return cnstrCall; }
	

	IfStmt::IfStmt() : Statement(StmtType::IF) { }
	IfStmt::IfStmt(Expr* conditional, Statement* trueBlock, Statement* falseBlock) :
		Statement(StmtType::IF), conditional(conditional), trueBlock(trueBlock), falseBlock(falseBlock) { }
	Expr* IfStmt::getConditional() { return conditional; }
	Statement* IfStmt::getTrueBlock() { return trueBlock; }
	Statement* IfStmt::getFalseBlock() { return falseBlock; }

	VarDecl::VarDecl(const std::string& varName, const std::string& varType, std::vector<Attribute> attrbs, Expr* init) :
		Statement(StmtType::VARDECL), varName(varName), varType(varType), attrbs(attrbs), init(init) { }
	std::string VarDecl::get() { return varName; }
	std::string VarDecl::getVarType() { return varType; }
	std::vector<Attribute> VarDecl::getAttrbs() { return attrbs; }
	Expr* VarDecl::getInit() { return init; }

	FunctionDecl::FunctionDecl(std::vector<Attribute> attrbs, const std::string& returnType, const std::string& functionName,
		std::vector<VarDecl*> formalParams, BlockStmt* body) :
		Statement(StmtType::FUNCTION_DECL), attrbs(attrbs), returnType(returnType),
		formalParams(formalParams), functionName(functionName), body(body) { }
	std::string FunctionDecl::getFunctionName() { return functionName; }
	std::string FunctionDecl::getReturnType() { return returnType; }
	std::vector<VarDecl*> FunctionDecl::getFormalParams() { return formalParams; }
	BlockStmt* FunctionDecl::getBody() { return body; }
	std::vector<Attribute> FunctionDecl::getAttrbs() { return attrbs; }

	ReturnStmt::ReturnStmt(Expr* value) : Statement(StmtType::RETURN), value(value) { }
	Expr* ReturnStmt::getValue() { return value; }

	ForStmt::ForStmt(VarDecl* init, Expr* check, Expr* counter, BlockStmt* body)
		: Statement(StmtType::FOR), init(init), check(check), counter(counter), body(body) { }
	VarDecl* ForStmt::getInit() { return init; }
	Expr* ForStmt::getCounter() { return counter; }
	Expr* ForStmt::getCheck() { return check; }
	BlockStmt* ForStmt::getBody() { return body; }

	WhileStmt::WhileStmt(Expr* check, BlockStmt* body)
		: Statement(StmtType::WHILE), check(check), body(body) { }
	Expr* WhileStmt::getCheck() { return check; }
	BlockStmt* WhileStmt::getBody() { return body; }

	ClassStmt::ClassStmt(std::vector<FunctionDecl*> memberFunctions, std::vector<VarDecl*> memberVariables, std::vector<Attribute> attrbs, std::string className)
		: Statement(StmtType::CLASS), memberFunctions(memberFunctions), memberVariables(memberVariables), attrbs(attrbs), className(className) { }
	std::vector<FunctionDecl*> ClassStmt::getMemberFunctions() { return memberFunctions; }
	std::vector<VarDecl*> ClassStmt::getMemberVariables() { return memberVariables; }
	std::vector<Attribute> ClassStmt::getAttrbs() { return attrbs; }
	std::string ClassStmt::getClassName() { return className; }
		
	ImportStmt::ImportStmt(std::vector<Expr*> imports)
	: Statement(StmtType::IMPORT), imports(imports){ }
	std::vector<Expr*> ImportStmt::getImports() { return imports; }
		
	PackageStmt::PackageStmt(Expr* packageName)
	: Statement(StmtType::PACKAGE), packageName(packageName){ }
	Expr* PackageStmt::getPackageName() { return packageName; }
	//ypFDtJzEFW08p3xH
}