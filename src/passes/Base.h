#pragma once

#include <unordered_set>
#include "../utils/Statements.h"
#include "../utils/context/ContextModel.h"
#include "../utils/DotExprToVecStr.h"
#include "../utils/context/ContextFinder.h"
#include "../utils/TypeHelper.h"

namespace MochaLang {
namespace Passes {
namespace BasePass {

	class BasePass {
	private:
		MochaLang::Symbols::ContextModel* context;

		std::vector<MochaLang::Symbols::ContextModel*> importContexts;

	public:
		BasePass(MochaLang::Symbols::ContextModel* context) : context(context) { }

		Expr* getIndexVariable(Expr* op);
		void getIndexIndices(Expr* op, std::vector<Expr*>& collect);

		void handleProgram(Statement* S, Statement** source);
		void handlePackage(Statement* S, Statement** source);
		void handleExpr(Statement* S, Statement** source);
		void handleFunctionCall(Statement* S, Statement** source);
		void handleFunctionDecl(Statement* S, Statement** source);
		void handleVarDecl(Statement* S, Statement** source);
		void handleClass(Statement* S, Statement** source);
		void handleFor(Statement* S, Statement** source);
		void handleIf(Statement* S, Statement** source);
		void handleReturn(Statement* S, Statement** source);
		void handleWhile(Statement* S, Statement** source);
		void performBasePass(Statement* stmt, Statement** source);
		void handleBlock(Statement* stmt, Statement** source);
		void handleImports(Statement* stmt, Statement** source);
		void handleIndex(Statement* stmt, Statement** source);

		
	};

}
}
}