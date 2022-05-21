#pragma once

#include "../../../utils/Statements.h"
#include <string>


/*  Replaces identifiers with names that would conflict with built-in keywords in Java
 *	Eg - int -> $int
 */

namespace MochaLang {
namespace Targets {
namespace Java {
namespace Passes {

	using namespace MochaLang;

	// Replace internal class with Java compatible class name
	// For eg -
	//		Replace int with $int, as int is a inbuilt type in Java
	class Pass1
	{
	private:
		std::string currentPackage, currentClass;

		void handleBlock(Statement* S, Statement** source);

		void handleProgram(Statement* S, Statement** source);

		void handlePackage(Statement* S, Statement** source);

		void handleClass(Statement* S, Statement** source);

		void handleVarDecl(Statement* S, Statement** source);

		void handleImport(Statement* S, Statement** source);

		void handleFunctionDecl(Statement* S, Statement** source);

		void handleFunctionCall(Statement* S, Statement** source);

		void handleConstructorCall(Statement* S, Statement** source);

		void handleExpr(Statement* S, Statement** source);

		void handleReturn(Statement* S, Statement** source);

		Type* handleType(Statement* S)
		{
			if (S == nullptr) return nullptr;

			Type* type = (Type*)S;

			if (type->type[0] == "int") // TODO - make sure it also detects mocha.internal.int
			{
				std::vector<std::string> temp = { "$int" };
				type = new Type(temp, {}, 0);
			}

			return type;
		}

	public:
		void runPass(Statement* S, Statement** source);
	};
				
}
}
}
}
