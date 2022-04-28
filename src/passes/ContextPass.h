#pragma once

#include "../utils/Statements.h"
#include "../utils/context/ContextModel.h"
#include "../utils/DotExprToVecStr.h"
#include <string>

namespace MochaLang {
namespace Passes {
namespace ContextPass {

	static void handleClass(Statement* _S, MochaLang::Symbols::ContextModel* context) {
		auto* S = (ClassStmt*)_S;
		context = context->addContext(S->getClassName());

	}

	static void handlePackage(Statement* _S, MochaLang::Symbols::ContextModel* context) {
		auto* S = (PackageStmt*)_S;
		auto pkgName = MochaLang::Utils::flattenDotExpr(S->getPackageName());

		for (std::string& s : pkgName) {
			context = context->addContext(s);
		}

		auto* block = S->packageContents;
		for (int i = 0; i < block->size(); ++i) {
			auto* stmt = block->get(i);
			if (stmt->getType() == StmtType::CLASS) {
				auto* head = context;
				handleClass(stmt, context);
				context = head;
			}
		}
	}

	static MochaLang::Symbols::ContextModel* generateContext(Statement* stmt) {
		MochaLang::Symbols::ContextModel* context = new MochaLang::Symbols::ContextModel("", "", nullptr);
		auto* head = context;

		handlePackage(stmt, context);

		/*auto* program = (BlockStmt*)stmt;
		for (int i = 0; i < program->size(); ++i) {
			context = head;
			handlePackage(program->get(i), context);
		}*/

		return head;
	}

}
}
}