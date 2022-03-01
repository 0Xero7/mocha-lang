#pragma once

#include "Statements.h"

namespace MochaLang {
	namespace Utils {

		using String = std::string;

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
					return resolve(((IndexExpr*)expr)->index);

				case StmtType::INLINE_ARRAY_INIT:
					return resolve(((InlineArrayInit*)expr)->values[0]) + "[]";
				}
			}
		};

	}
}
