#pragma once
#include <unordered_map>
#include <string>
#include "ContextModelType.h"
#include "../Statements.h"

namespace MochaLang {
namespace Symbols {

	using namespace std;

	class ContextModel {
	public:
		string shortName;
		string longName;
		bool genericType;

		ContextModelType contextType;
		MochaLang::Type* variableType;

		ContextModel* parent;
		unordered_map<string, ContextModel*> childContexts;

		ContextModel(string shortName, string longName, ContextModel* parent,
			ContextModelType contextType, bool genericType = false, MochaLang::Type* variableType = nullptr) :
				shortName(shortName), longName(longName), parent(parent), genericType(genericType),
				contextType(contextType), variableType(variableType) {}

		ContextModel* addContext(string shortName, ContextModelType contextType, bool genericType = false, MochaLang::Type* variableType = nullptr) {
			if (childContexts.count(shortName)) return childContexts[shortName];

			auto newLongName = (longName == "" ? "" : (longName + ".")) + shortName;
			return childContexts[shortName] = new ContextModel(shortName, newLongName, this, contextType, genericType, variableType);
		}

		void popContext(string shortName) {
			ContextModel** ref = &childContexts[shortName];
			childContexts.erase(childContexts.find(shortName));
			delete* ref;
		}

	};
}
}