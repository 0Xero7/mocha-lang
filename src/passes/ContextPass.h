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
		context = context->addContext(S->getClassName(), MochaLang::Symbols::ContextModelType::CLASS);

		auto* head = context;

		for (auto* vdcl : S->getMemberVariables()) {
			context->addContext(vdcl->get()->get(), MochaLang::Symbols::ContextModelType::VARIABLE, false, vdcl->getVarType());
		}

		for (auto* fdcl : S->getMemberFunctions()) {
			context->addContext(fdcl->getFunctionName(), MochaLang::Symbols::ContextModelType::FUNCTION, false, fdcl->getReturnType());
		}

		for (auto* ovr : S->opOverloads) {
			context->addContext(ovr->operatorStr, MochaLang::Symbols::ContextModelType::OPERATOR_OVERLOAD, false, ovr->returnType);
		}

		for (auto* cls : S->nestedClasses) {
			handleClass(cls, context);
			context = head;
		}
	}

	static void handlePackage(Statement* _S, MochaLang::Symbols::ContextModel* context) {
		auto* S = (PackageStmt*)_S;
		auto pkgName = ((Identifier*)S->getPackageName())->get_raw();

		for (std::string& s : pkgName) {
			context = context->addContext(s, MochaLang::Symbols::ContextModelType::PACKAGE);
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
		MochaLang::Symbols::ContextModel* context = new MochaLang::Symbols::ContextModel("", "", nullptr, MochaLang::Symbols::ContextModelType::CLASS);
		auto* head = context;

		//handlePackage(stmt, context);

		auto* program = (Program*)stmt;
		for (auto [pkgName, pkg] : program->packages) {
			handlePackage(pkg, context);
		}

		return head;
	}

}
}
}