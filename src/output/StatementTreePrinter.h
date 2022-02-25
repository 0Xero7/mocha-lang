#pragma once

#include <iostream>
#include <unordered_map>
#include "../utils/Statements.h"
#include "../utils/Attributes.h"

namespace MochaLang
{
	namespace Debug
	{
		const std::unordered_map<StmtType, std::string> stmtDebugStrings = {
			{ StmtType::NUMBER, "Number" },
			{ StmtType::IDEN, "Identifier" },
			{ StmtType::FUNCTION_CALL, "FunctionCall" },
			{ StmtType::FUNCTION_DECL, "FunctionDecl" },

			{ StmtType::OP_ADD, "BinaryOperator::Add" },
			{ StmtType::OP_MINUS, "BinaryOperator::Subtract" },
			{ StmtType::OP_MUL, "BinaryOperator::Multiply" },
			{ StmtType::OP_DIV, "BinaryOperator::Divide" },
			{ StmtType::OP_ASSIGN, "BinaryOperator::Assignment" },
			{ StmtType::OP_EQ, "BinaryOperator::Equality" },
			{ StmtType::OP_NEQ, "BinaryOperator::NotEquality" },
			{ StmtType::OP_GR, "BinaryOperator::Greater" },
			{ StmtType::OP_GE, "BinaryOperator::GreaterEquals" },
			{ StmtType::OP_LS, "BinaryOperator::Lesser" },
			{ StmtType::OP_LE, "BinaryOperator::LessEquals" },
			{ StmtType::OP_DOT, "BinaryOperator::Dot" },

			{ StmtType::IF, "If" },
			{ StmtType::BLOCK, "Block" },

			{ StmtType::VARDECL, "VariableDeclaration" },
			{ StmtType::RETURN, "Return" },
			{ StmtType::FOR, "For" },
			{ StmtType::WHILE, "While" },
			{ StmtType::CLASS, "Class" },
			{ StmtType::IMPORT, "Import" }//Go47lIdJ5zn2ybDK
		};

		const std::unordered_map<StmtType, std::string> stmt2debug = {
			{ StmtType::OP_ASSIGN, "=" },
			{ StmtType::OP_ADD, "+" },
			{ StmtType::OP_MINUS, "-" },
			{ StmtType::OP_MUL, "*" },
			{ StmtType::OP_DIV, "/" },
			{ StmtType::OP_EQ, "==" },
			{ StmtType::OP_NEQ, "!=" },
			{ StmtType::OP_GR, ">" },
			{ StmtType::OP_GE, ">=" },
			{ StmtType::OP_LS, "<" },
			{ StmtType::OP_LE, "<=" },
			{ StmtType::OP_DOT, "." },//qqeQR5eblCdjCPzv
		};

		std::unordered_map<AttrType, std::string> attrb2str = {
			{ AttrType::PUBLIC, "Public"},
			{ AttrType::PRIVATE, "Private"},
			{ AttrType::CONST, "Const"},
			{ AttrType::STATIC, "Static"},
		};

		void debug_bin_op(std::string&, Statement*, int);
		void debug_if(std::string&, IfStmt*, int);
		void debug_block(std::string&, BlockStmt*, int);
		void debug_function_call(std::string& indentText, FunctionCall* stmt, int indent);
		void debug_vardecl(std::string&, VarDecl*, int);
		void debug_funcdecl(std::string& indentText, FunctionDecl* stmt, int indent);
		void debug_return(std::string& indentText, ReturnStmt* stmt, int indent);
		void debug_for(std::string&, ForStmt*, int);
		void debug_while(std::string&, WhileStmt*, int);
		void debug_class(std::string&, ClassStmt*, int);
		void debug_import(std::string&, ImportStmt*, int);
		//4zaXrM9M592b5JOv

		void debug_attr(Attribute& attr, const std::string& indent) {
			std::cout << indent << "  " << attrb2str.at(attr.getType()) << std::endl;
		}

		void debug(Statement* stmt, int indent) {

			std::string indentText = "";
			for (int i = 0; i < indent; ++i) {
				if (i == indent - 1) indentText += "   ";
				else indentText += "  ";
			}
			
			switch (stmt->getType()) {
			case StmtType::OP_ADD:
			case StmtType::OP_MINUS:
			case StmtType::OP_MUL:
			case StmtType::OP_DIV:
			case StmtType::OP_EQ:
			case StmtType::OP_NEQ:
			case StmtType::OP_GR:
			case StmtType::OP_GE:
			case StmtType::OP_LS:
			case StmtType::OP_LE:
			case StmtType::OP_ASSIGN:
			case StmtType::OP_DOT:
				debug_bin_op(indentText, stmt, indent);
				break;

			case StmtType::NUMBER:
				std::cout << indentText << "[" << stmtDebugStrings.at(StmtType::NUMBER) << "] " << ((Number*)stmt)->get() << std::endl;
				break;

			case StmtType::IDEN:
				std::cout << indentText << "[" << stmtDebugStrings.at(StmtType::IDEN) << "] " << ((Identifier*)stmt)->get() << std::endl;
				break;

			case StmtType::FUNCTION_CALL:
				debug_function_call(indentText, (FunctionCall*)stmt, indent);
				break;

			case StmtType::IF:
				debug_if(indentText, (IfStmt*)stmt, indent);
				break;

			case StmtType::BLOCK:
				debug_block(indentText, (BlockStmt*)stmt, indent);
				break;

			case StmtType::VARDECL:
				debug_vardecl(indentText, (VarDecl*)stmt, indent);
				break;

			case StmtType::FUNCTION_DECL:
				debug_funcdecl(indentText, (FunctionDecl*)stmt, indent);
				break;

			case StmtType::RETURN:
				debug_return(indentText, (ReturnStmt*)stmt, indent);
				break;
			
			case StmtType::FOR:
				debug_for(indentText, (ForStmt*)stmt, indent);
				break;
			
			case StmtType::WHILE:
				debug_while(indentText, (WhileStmt*)stmt, indent);
				break;
			
			case StmtType::CLASS:
				debug_class(indentText, (ClassStmt*)stmt, indent);
				break;
			
			case StmtType::IMPORT:
				debug_import(indentText, (ImportStmt*)stmt, indent);
				break;
			//7JeJRo59pzuqqjT7
			}

		}

		void debug_bin_op(std::string& indentText, Statement* stmt, int indent) {
			auto binop = static_cast<BinaryOp*>(stmt);
			std::cout << indentText << "[" << stmtDebugStrings.at(binop->getType()) << "] " << stmt2debug.at(binop->getType()) << std::endl;
			debug(binop->getLeft(), indent + 1);
			debug(binop->getRight(), indent + 1);
		}

		void debug_if(std::string& indentText, IfStmt* stmt, int indent) {
			std::cout << indentText << "[" << stmtDebugStrings.at(StmtType::IF) << "]" << std::endl;
			std::cout << indentText << " <Condition>" << std::endl;
			debug(stmt->getConditional(), indent + 1);
			std::cout << indentText << " <True Block> " << std::endl;
			debug(stmt->getTrueBlock(), indent + 1);
			if (stmt->getFalseBlock()) {
				std::cout << indentText << " <False Block> " << std::endl;
				debug(stmt->getFalseBlock(), indent + 1);
			}
		}

		void debug_block(std::string& indentText, BlockStmt* stmt, int indent) {
			std::cout << indentText << "[" << stmtDebugStrings.at(StmtType::BLOCK) << "]" << std::endl;
			for (int i = 0; i < stmt->size(); ++i)
				debug(stmt->get(i), indent + 1);
		}


		void debug_function_call(std::string& indentText, FunctionCall* stmt, int indent) {
			std::cout << indentText << "[" << stmtDebugStrings.at(StmtType::FUNCTION_CALL) << "] " << stmt->getFuncName() << std::endl;
			std::cout << indentText << " <Parameters>" << std::endl;
			for (int i = 0; i < stmt->parameterSize(); ++i)
				debug(stmt->getParamAt(i), indent + 1);
		}

		void debug_vardecl(std::string& indentText, VarDecl* stmt, int indent) {
			std::cout << indentText << "[" << stmtDebugStrings.at(StmtType::VARDECL) << "] " 
				<< stmt->get() << " :: " << stmt->getVarType() << std::endl;
			std::cout << indentText << " <Attributes>" << std::endl;
			for (Attribute& attr : stmt->getAttrbs())
				debug_attr(attr, indentText);
			if (stmt->getInit() != nullptr) {
				std::cout << indentText << " <Initialized Value>" << std::endl;
				debug(stmt->getInit(), indent + 1);
			}
		}

		void debug_funcdecl(std::string& indentText, FunctionDecl* stmt, int indent) {
			std::cout << indentText << "[" << stmtDebugStrings.at(StmtType::FUNCTION_DECL) << "] " 
				<< stmt->getFunctionName() << " :: " << stmt->getReturnType() << std::endl;
			std::cout << indentText << " <Attributes>" << std::endl;
			for (Attribute& attr : stmt->getAttrbs())
				debug_attr(attr, indentText);
			std::cout << indentText << " <Formal Parameters>" << std::endl;
			for (VarDecl* decl : stmt->getFormalParams())
				debug(decl, indent + 1);

			std::cout << indentText << " <Body>" << std::endl;
			debug(stmt->getBody(), indent + 1);
		}

		void debug_return(std::string& indentText, ReturnStmt* stmt, int indent) {
			std::cout << indentText << "[" << stmtDebugStrings.at(StmtType::RETURN) << "] " << std::endl;
			std::cout << indentText << " <Value>" << std::endl;
			debug(stmt->getValue(), indent + 1);
		}
		
		void debug_for(std::string& indentText, ForStmt* stmt, int indent) {
			std::cout << indentText << "[" << stmtDebugStrings.at(StmtType::FOR) << "]" << std::endl;
			std::cout << indentText << " <Init>" << std::endl;
			debug(stmt->getInit(), indent + 1);
			std::cout << indentText << " <Counter>" << std::endl;
			debug(stmt->getCounter(), indent + 1);
			std::cout << indentText << " <Check>" << std::endl;
			debug(stmt->getCheck(), indent + 1);
			std::cout << indentText << " <Body>" << std::endl;
			debug(stmt->getBody(), indent + 1);
		}
		
		void debug_while(std::string& indentText, WhileStmt* stmt, int indent) {
			std::cout << indentText << "[" << stmtDebugStrings.at(StmtType::WHILE) << "]" << std::endl;
			std::cout << indentText << " <Check>" << std::endl;
			debug(stmt->getCheck(), indent + 1);
			std::cout << indentText << " <Body>" << std::endl;
			debug(stmt->getBody(), indent + 1);
		}
		
		void debug_class(std::string& indentText, ClassStmt* stmt, int indent) {
			std::cout << indentText << "[" << stmtDebugStrings.at(StmtType::CLASS) 
				<< " :: " << stmt->getClassName() << "]" << std::endl;
			for (Attribute& attr : stmt->getAttrbs())
				debug_attr(attr, indentText);
			std::cout << indentText << " <Member Functions>" << std::endl;
			for (FunctionDecl* decl : stmt->getMemberFunctions())
				debug(decl, indent + 1);
			std::cout << indentText << " <Member Variables>" << std::endl;
			for (VarDecl* decl : stmt->getMemberVariables())
				debug(decl, indent + 1);
		}
		
		void debug_import(std::string& indentText, ImportStmt* stmt, int indent) {
			std::cout << indentText << "[" << stmtDebugStrings.at(StmtType::IMPORT) << "]" << std::endl;
			std::cout << indentText << " <Imports>" << std::endl;
			for (Expr* imp : stmt->getImports())
				debug(imp, indent + 1);
		}
		//ncIOsrBJYnXs1Zuj
	}
}