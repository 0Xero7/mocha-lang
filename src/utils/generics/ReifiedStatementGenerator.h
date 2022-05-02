#pragma once

#include "../Statements.h"

namespace MochaLang {
namespace Utils {
namespace Generics {

	static VarDecl* reifyGenericVarDecl(VarDecl* varDecl, Identifier* genericTypeName, Identifier* specializedTypeName) {
		auto* reifiedVarDecl = new VarDecl(varDecl->get(), )
	}

}
};
};