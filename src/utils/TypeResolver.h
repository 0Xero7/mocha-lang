#pragma once

#include "context/ContextModel.h"
#include "context/ContextFinder.h"
#include "TypeHelper.h"
#include "Statements.h"
#include "context/ContextGroup.h"

namespace MochaLang {
	namespace Utils {

		using String = std::string;

		static Symbols::ContextModel* _tryFindContext(std::vector<String> path, 
			std::vector<Symbols::ContextModel*>& contexts)
		{
			Symbols::ContextModel* fnd = nullptr;

			for (auto* model : contexts)
			{
				fnd = findContext(path, model);
				if (fnd) return fnd;
			}

			return fnd;
		}

		static Symbols::ContextModel* resolveType(Expr* expr, ContextGroup* contextGroup)
		{
			
			switch (expr->getType()) {
			case StmtType::NUMBER:
				return contextGroup->tryFindContext({ "mocha", "internal", "int" });

			case StmtType::FUNCTION_CALL:
				return contextGroup->tryFindContext(((FunctionCall*)expr)->getFuncName()->get_raw());

			case StmtType::CONSTRUCTOR_CALL:
				return contextGroup->tryFindContext(((ConstructorCall*)expr)->cnstrCall->functionName->get_raw());

			case StmtType::IDEN:
				return contextGroup->tryFindContext(((Identifier*)expr)->get_raw());

			case StmtType::OP_ADD:
			{
				std::string op_str = "+";
				auto leftType = resolveType(((BinaryOp*)expr)->left, contextGroup);
				auto rightType = resolveType(((BinaryOp*)expr)->right, contextGroup);

				if (leftType->contextType != Symbols::ContextModelType::CLASS)
					throw "Binary operator with non-class type on the left";

				auto left = (ClassStmt*)leftType->context;
				for (auto op : left->opOverloads)
				{
					if (op->operatorStr != op_str) continue;

					auto ctx = contextGroup->tryFindContext(op->parameters[0]->getVarType()->type);

					if (ctx->longName == rightType->longName)
					{
						return contextGroup->tryFindContext(op->returnType->type);
					}
				}


				throw "Cannot apply operator!";
				break;
			}

			case StmtType::OP_DOT:
				auto leftType = resolveType(((BinaryOp*)expr)->left, contextGroup);
				auto newCtxGroup = new ContextGroup(leftType);
				return resolveType(((BinaryOp*)expr)->right, newCtxGroup);
				break;
			}


		}

		class TypeResolver {
		public:
			String resolve(Expr* expr) {
				switch (expr->getType()) {
				case StmtType::NUMBER:
					return "int";
				case StmtType::RAW_STRING: // MAY NEED MODIFICATION LATER
					return "String";
				
				case StmtType::IDEN:
					return ((Identifier*)expr)->get();

				case StmtType::INDEX:
					return resolve(((BinaryOp*)expr)->getLeft());

				case StmtType::INLINE_ARRAY_INIT:
					return resolve(((InlineArrayInit*)expr)->values[0]) + "[]";
				}
			}
		};

	}
}
