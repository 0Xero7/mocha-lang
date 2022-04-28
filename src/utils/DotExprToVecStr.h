#include "Statements.h"
#include <vector>
#include <string>

namespace MochaLang {
namespace Utils {
	
	using namespace std;

	static void __flatten__(Expr* expr, vector<string>& collect) {
		if (expr->getType() == StmtType::IDEN) {
			collect.push_back(((Identifier*)expr)->get());
			return;
		}

		if (expr->getType() != StmtType::OP_DOT) throw "Ayo something is very wrong";

		auto dot = (BinaryOp*)expr;

		__flatten__(dot->getLeft(), collect);
		__flatten__(dot->getRight(), collect);
	}

	static vector<string> flattenDotExpr(Expr* expr) {
		vector<string> collect;
		__flatten__(expr, collect);
		return collect;
	}


}
}