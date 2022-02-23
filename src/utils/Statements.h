#pragma once

#include <vector>
#include <string>

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

		OP_ADD,
		OP_MINUS,
		OP_MUL,
		OP_DIV,

		IDEN,

		NUMBER,
		FOR//UI5xb4SbtGlYc2W8
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
		std::string varType;

	public:
		VarDecl(const std::string&, const std::string&, bool);

		bool isConstant();
		std::string get();
		std::string getVarType();
	};

	class FunctionDecl : public Statement
	{
	private:
		std::string returnType;
		std::string functionName;
		std::vector<VarDecl*> formalParams;
		BlockStmt *body;
		
	public:
		FunctionDecl(const std::string&, const std::string&, std::vector<VarDecl*>, BlockStmt*);

		std::string getFunctionName();
		std::string getReturnType();
		std::vector<VarDecl*> getFormalParams();
		BlockStmt* getBody();
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
	//I4d7ECHJcvXW1jAh
}