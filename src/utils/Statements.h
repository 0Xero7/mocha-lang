#pragma once

#include <vector>
#include <string>
#include "Attributes.h"

namespace MochaLang
{

	enum class StmtType
	{
		BLOCK,

		FUNCTION_CALL,
		FUNCTION_DECL,
		VARDECL,

		IF,
		RETURN,

		PAREN_OPEN,

		OP_ASSIGN,
		OP_EQ,
		OP_NEQ,
		OP_GR,
		OP_GE,
		OP_LS,
		OP_LE,
		OP_DOT,

		OP_ADD,
		OP_MINUS,
		OP_MUL,
		OP_DIV,

		IDEN,

		NUMBER,
		FOR,
		WHILE,
		CLASS,
		IMPORT//UI5xb4SbtGlYc2W8
	};



	class Statement 
	{
	protected:
		StmtType type; 

	public:
		Statement(StmtType);

		StmtType getType();
		//virtual void debug();
	};


	class BlockStmt : public Statement {
	private:
		std::vector<Statement*> statements;

	public:
		BlockStmt();

		void push_back(Statement*);
		int size();
		Statement* get(int);
	};


	class Expr : public Statement {
	public:
		Expr(StmtType);
	};

	class BinaryOp : public Expr {
	private:
		Expr* left, * right;

	public:
		BinaryOp(Expr*, StmtType, Expr*);

		Expr* getLeft();
		void setLeft(Expr*);
		Expr* getRight();
		void setRight(Expr*);
	};

	class Number : public Expr {
	private:
		int number;
	public:
		Number(int);

		int get();
	};

	class Identifier : public Expr {
	private:
		std::string identifier;
	public:
		Identifier(std::string&);

		std::string get();
	};

	class FunctionCall : public Expr {
	private:
		std::string functionName;
		std::vector<Expr*> parameters;
	public:
		FunctionCall(std::string& functionName);

		std::string getFuncName();
		int parameterSize();
		Expr* getParamAt(int);

		void addParameter(Expr*);
	};

	class IfStmt : public Statement 
	{
	private:
		Expr* conditional		= nullptr;
		Statement* trueBlock	= nullptr;
		Statement* falseBlock	= nullptr;

	public:
		IfStmt();
		IfStmt(Expr* conditional, Statement* trueBlock, Statement* falseBlock);

		Expr* getConditional();
		Statement* getTrueBlock();
		Statement* getFalseBlock();
	};


	class VarDecl : public Statement
	{
	private:
		std::vector<Attribute> attrbs;
		std::string varName;
		std::string varType;
		Expr* init;

	public:
		VarDecl(const std::string&, const std::string&, std::vector<Attribute> = {}, Expr* = nullptr);

		std::string get();
		std::string getVarType();
		std::vector<Attribute> getAttrbs();
		Expr* getInit();
	};

	class FunctionDecl : public Statement
	{
	private:
		std::vector<Attribute> attrbs;
		std::string returnType;
		std::string functionName;
		std::vector<VarDecl*> formalParams;
		BlockStmt *body;
		
	public:
		FunctionDecl(std::vector<Attribute>, const std::string&, const std::string&, std::vector<VarDecl*>, BlockStmt*);
		
		std::string getFunctionName();
		std::string getReturnType();
		std::vector<VarDecl*> getFormalParams();
		BlockStmt* getBody();
		std::vector<Attribute> getAttrbs();
	};

	class ReturnStmt : public Statement 
	{
	private:
		Expr* value;
	public:
		ReturnStmt(Expr*);

		Expr* getValue();
	};
		
	class ForStmt : public Statement
	{
	private:
			Expr* init;
			Expr* check;
			Expr* counter;
			BlockStmt* body;
	public:
			ForStmt(Expr*,Expr*,Expr*,BlockStmt*);
			Expr* getInit();
			Expr* getCheck();
			Expr* getCounter();
			BlockStmt* getBody();
	};
		
	class WhileStmt : public Statement
	{
	private:
			Expr* check;
			BlockStmt* body;
	public:
			WhileStmt(Expr*,BlockStmt*);
			Expr* getCheck();
			BlockStmt* getBody();
	};
		
	class ClassStmt : public Statement
	{
	private:
			std::vector<FunctionDecl*> memberFunctions;
			std::vector<VarDecl*> memberVariables;
			std::vector<Attribute> attrbs;
			std::string className;
	public:
			ClassStmt(std::vector<FunctionDecl*>,std::vector<VarDecl*>,std::vector<Attribute>,std::string);
			std::vector<FunctionDecl*> getMemberFunctions();
			std::vector<VarDecl*> getMemberVariables();
			std::vector<Attribute> getAttrbs();
			std::string getClassName();
	};
		
	class ImportStmt : public Statement
	{
	private:
			std::vector<Expr*> imports;
	public:
			ImportStmt(std::vector<Expr*>);
			std::vector<Expr*> getImports();
	};
	//I4d7ECHJcvXW1jAh
}