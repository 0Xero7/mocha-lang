#pragma once

#include <iostream>
#include <unordered_map>
#include "../utils/Statements.h"

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

			{ StmtType::IF, "If" },
			{ StmtType::BLOCK, "Block" },

			{ StmtType::VARDECL, "VariableDeclaration" },
			{ StmtType::RETURN, "Return" },
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
		};

		void debug_bin_op(std::string&, Statement*, int);
		void debug_if(std::string&, IfStmt*, int);
		void debug_block(std::string&, BlockStmt*, int);
		void debug_function_call(std::string& indentText, FunctionCall* stmt, int indent);
		void debug_vardecl(std::string&, VarDecl*, int);
		void debug_funcdecl(std::string& indentText, FunctionDecl* stmt, int indent);
		void debug_return(std::string& indentText, ReturnStmt* stmt, int indent);

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
		}

		void debug_funcdecl(std::string& indentText, FunctionDecl* stmt, int indent) {
			std::cout << indentText << "[" << stmtDebugStrings.at(StmtType::FUNCTION_DECL) << "] " 
				<< stmt->getFunctionName() << " :: " << stmt->getReturnType() << std::endl;
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
	}
}