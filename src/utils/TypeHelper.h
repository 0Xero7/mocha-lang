#pragma once

#include <string>
#include "Statements.h"

namespace MochaLang {
namespace Utils {
namespace TypeHelper {

	// Returns the actual type without template arguments or array notation
	static std::string getConcreteType(Type* type) {
		if (type == nullptr) return ""; // Constructor

		std::string typeStr = "";
		for (auto& s : type->type)
			typeStr += s + ".";
		typeStr.pop_back();

		return typeStr;
	}

	static std::string getArrayNotation(Type* type) {
		if (type == nullptr) return ""; // Constructor

		std::string typeStr = "";

		for (int i = 0; i < type->arrayDims; ++i)
			typeStr += "[]";

		return typeStr;
	}

	// Gets a Type* and returns how it would be written in the language
	static std::string getTypeString(Type* type) {
		if (type == nullptr) return ""; // Constructor

		std::string typeStr = getConcreteType(type);

		if (!type->genericArgs.empty()) {
			typeStr.push_back('<');

			for (Type* tempArg : type->genericArgs)
				typeStr += getTypeString(tempArg) + ", ";

			typeStr = typeStr.substr(0, (int)typeStr.size() - 2);
			typeStr.push_back('>');
		}

		typeStr += getArrayNotation(type);

		return typeStr;
	}

}
}
}
