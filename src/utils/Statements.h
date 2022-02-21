#pragma once

#include <vector>
#include <string>

namespace MochaLang
{

	enum class StmtType 
	{
		BLOCK,

		FUNCTION_CALL,
		VARDECL,

		IF,

		PAREN_OPEN,

		OP_ASSIGN,
		OP_EQ,
		OP_NEQ,
		OP_GR,
		OP_GE,
		OP_LS,
		OP_LE,

		OP_ADD,
		OP_MINUS,
		OP_MUL,
		OP_DIV,

		IDEN,

		NUMBER
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
		bool constantFlag		= false;
		std::string varName;

	public:
		VarDecl(std::string&, bool);

		bool isConstant();
		std::string get();
	};

}