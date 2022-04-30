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

		auto* head = context;

		for (auto* cls : S->nestedClasses) {
			context = head;
			handleClass(cls, context);
		}
	}

	static void handlePackage(Statement* _S, MochaLang::Symbols::ContextModel* context) {
		auto* S = (PackageStmt*)_S;
		auto pkgName = ((Identifier*)S->getPackageName())->get_raw();

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