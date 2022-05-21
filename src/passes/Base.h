#pragma once

#include <unordered_set>
#include "../utils/Statements.h"
#include "../utils/context/ContextModel.h"
#include "../utils/context/ContextGroup.h"
#include "../utils/DotExprToVecStr.h"
#include "../utils/context/ContextFinder.h"
#include "../utils/TypeHelper.h"
#include "../utils/TypeResolver.h"
#include "../internal/symbols/Symbols.h"
#include "iostream"

namespace MochaLang {
namespace Passes {
namespace BasePass {

	class BasePass {
	private:
		////// For static context tree
		//MochaLang::Symbols::ContextModel* context;

		////// In a function? Use this
		////MochaLang::Symbols::ContextModel* activeContext;

		//std::vector< Symbols::ContextModel*> contexts;

		//std::vector<MochaLang::Symbols::ContextModel*> importContexts;

		Utils::ContextGroup* contextGroup;

	public:
		BasePass(MochaLang::Symbols::ContextModel* context)
		{
			contextGroup = new Utils::ContextGroup(context);
			/*std::vector<Symbols::ContextModel*> _contexts = { context };
			contexts.push_back(Utils::_tryFindContext({ "mocha", "internal", "int" }, _contexts));*/
		}

		Expr* getIndexVariable(Expr* op);
		void getIndexIndices(Expr* op, std::vector<Expr*>& collect);

		void lowerOperatorCalls(Statement* op, Statement** source);

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