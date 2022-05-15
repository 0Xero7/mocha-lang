#pragma once

#include <vector>
#include <string>
#include "Attributes.h"

namespace MochaLang
{

	enum class StmtType
	{
		NOTHING,

		BLOCK,

		CONSTRUCTOR_CALL,
		FUNCTION_CALL,
		FUNCTION_DECL,
		VARDECL,

		OPERATOR_OVERLOAD,

		INDEX,

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
		RAW_STRING,
		NUMBER,

		INLINE_ARRAY_INIT,
		EXPLICIT_ARRAY_INIT,

		FOR,
		WHILE,
		CLASS,
		IMPORT,
		PACKAGE,
		PROGRAM,
		
		TYPE//UI5xb4SbtGlYc2W8
	};



	class Statement
	{
	protected:
		StmtType type;

	public:
		Statement(StmtType);

		StmtType getType();
	};

	class Type : public Statement
	{
	public:
		int arrayDims;
		std::vector<std::string> type;
		std::vector<Type*> genericArgs;
		Type(std::vector<std::string> type, std::vector<Type*> genericArgs, int arrayDims);
	};

	class BlockStmt : public Statement {
	private:
		std::vector<Statement*> statements;

	public:
		BlockStmt();

		void push_back(Statement*);
		int size();
		Statement* get(int);
		void replace(int, Statement*);
	};


	class Expr : public Statement {
	public:
		Expr(StmtType);
	};

	class BinaryOp : public Expr {
	private:
	public:
		Expr* left, * right;
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

	class RawString : public Expr {
	private:
		std::string string;
	public:
		RawString(std::string);

		std::string get();
	};

	class Identifier : public Expr {
	private:
		//std::string identifier;
		std::vector<std::string> identifier;
	public:
		//Identifier(std::string&);
		Identifier(std::vector<std::string>&);

		std::string get();
		std::vector<std::string> get_raw();
	};

	class InlineArrayInit : public Expr {
	public:
		std::vector<Expr*> values;
		InlineArrayInit(std::vector<Expr*>&);
	};

	class ExplicitArrayInit : public Expr {
	public:
		std::string arrayType;
		std::vector<Expr*> values;
		ExplicitArrayInit(std::string arrayType, std::vector<Expr*>&);
	};

	class FunctionCall : public Expr {
	private:
	public:
		Identifier* functionName;
		std::vector<Expr*> parameters;

		FunctionCall(Identifier* functionName);

		std::vector<Type*> specializedTypes;
		std::string getFuncNameStr();
		Identifier* getFuncName();
		int parameterSize();
		Expr* getParamAt(int);

		void replaceParamAt(int index, Statement* stmt);
		void addParameter(Expr*);
	};

	class ConstructorCall : public Expr {
	private:

	public:
		FunctionCall* cnstrCall;

		ConstructorCall(FunctionCall*);
		FunctionCall* getFunctionCall();
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
		Identifier* varName;
		Type* varType;

	public:
		Expr* init;
		VarDecl(Identifier*, Type*, std::vector<Attribute> = {}, Expr* = nullptr);

		Identifier* get();
		Type* getVarType();
		std::vector<Attribute> getAttrbs();
		Expr* getInit();
	};

	class FunctionDecl : public Statement
	{
	private:
		std::vector<Attribute> attrbs;
		
	public:
		Type* returnType;
		std::string functionName;
		std::vector<VarDecl*> formalParams;
		BlockStmt *body;

		FunctionDecl(std::vector<Attribute>, Type*, const std::string&, std::vector<VarDecl*>, BlockStmt*);
		
		std::string getFunctionName();
		Type* getReturnType();
		std::vector<VarDecl*> getFormalParams();
		BlockStmt* getBody();
		std::vector<Attribute> getAttrbs();
	};

	class ReturnStmt : public Statement 
	{
	private:
	public:
		Expr* value;
		ReturnStmt(Expr*);

		Expr* getValue();
	};
		
	class ForStmt : public Statement
	{
	private:
			VarDecl* init;
			Expr* check;
			Expr* counter;
			BlockStmt* body;
	public:
			ForStmt(VarDecl*,Expr*,Expr*,BlockStmt*);
			VarDecl* getInit();
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

	class OperatorOverload : public Statement
	{
	public:
		std::vector<Attribute> attrbs;
		std::string operatorStr;
		std::vector<VarDecl*> parameters;
		Type* returnType;
		BlockStmt* block;

		OperatorOverload(std::vector<Attribute> attrbs, std::string& operatorStr, Type* returnType,
			std::vector<VarDecl*>& parameters, BlockStmt* block);
	};
		
	class ClassStmt : public Statement
	{
	private:
			std::vector<Attribute> attrbs;
	public:
			std::vector<FunctionDecl*> memberFunctions;
			std::vector<VarDecl*> memberVariables;
			std::string className;
			std::vector<Type*> genericTemplates;
			std::vector<ClassStmt*> nestedClasses;
			std::vector<OperatorOverload*> opOverloads;
			ClassStmt::ClassStmt(std::vector<FunctionDecl*> memberFunctions, std::vector<VarDecl*> memberVariables,
				std::vector<ClassStmt*> nestedClasses, std::vector<OperatorOverload*> opOverloads, std::vector<Attribute> attrbs, std::string className,
				std::vector<Type*> genericTemplates);
			std::vector<FunctionDecl*> getMemberFunctions();
			std::vector<VarDecl*> getMemberVariables();
			std::vector<Attribute> getAttrbs();
			std::string getClassName();
	};
		
	class ImportStmt : public Statement
	{
	private:
	public:
			std::vector<Expr*> imports;
			ImportStmt(std::vector<Expr*>);
			std::vector<Expr*> getImports();
	};

	class PackageStmt : public Statement
	{
	private:
		Expr* packageName;
	public:
		BlockStmt* packageContents;
		PackageStmt(Expr*);
		Expr* getPackageName();
	};

	class Program : public Statement
	{
	public:
		std::string programName;
		std::unordered_map<std::string, PackageStmt*> packages;
		Program(std::string programName);
		void addPackage(PackageStmt* pkg);
	};
	//I4d7ECHJcvXW1jAh
}