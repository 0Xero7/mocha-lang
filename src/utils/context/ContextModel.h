#pragma once
#include <unordered_map>
#include <string>

namespace MochaLang {
namespace Symbols {

	using namespace std;

	class ContextModel {
	public:
		string shortName;
		string longName;

		// string type;

		ContextModel* parent;
		unordered_map<string, ContextModel*> childContexts;

		ContextModel(string shortName, string longName, ContextModel* parent) :
			shortName(shortName), longName(longName), parent(parent) { }

		ContextModel* addContext(string shortName) {
			if (childContexts.count(shortName)) return childContexts[shortName];

			auto newLongName = (longName == "" ? "" : (longName + ".")) + shortName;
			return childContexts[shortName] = new ContextModel(shortName, newLongName, this);
		}

		void popContext(string shortName) {
			ContextModel** ref = &childContexts[shortName];
			childContexts.erase(childContexts.find(shortName));
			delete* ref;
		}

	};
}
}