#pragma once

#include <unordered_set>
#include "../utils/Statements.h"

namespace MochaLang {
namespace Passes {
namespace BasePass {

	class BasePass {
	private:
		std::unordered_set<std::string> knownClasses;

	public:
		BasePass(std::unordered_set<std::string>& knownClasses) : knownClasses(knownClasses) { }

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

		
	};

}
}
}