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

		Statement* context;

		ContextModel* parent;
		unordered_map<string, ContextModel*> childContexts;

		ContextModel(string shortName, string longName, ContextModel* parent,
			ContextModelType contextType, bool genericType = false, 
			MochaLang::Type* variableType = nullptr, Statement* context = nullptr) :
				shortName(shortName), longName(longName), parent(parent), genericType(genericType),
				contextType(contextType), variableType(variableType), context(context) {}

		ContextModel* addContext(string shortName, ContextModelType contextType, bool genericType = false, 
			MochaLang::Type* variableType = nullptr, Statement* context = nullptr) {
			if (childContexts.count(shortName)) return childContexts[shortName];

			auto newLongName = (longName == "" ? "" : (longName + ".")) + shortName;
			return childContexts[shortName] = new ContextModel(shortName, newLongName, this, contextType, genericType, variableType, context);
		}

		ContextModel* addLocalContext()
		{
			return childContexts["$$LOCAL$$"] = new ContextModel("", "", this, ContextModelType::LOCAL);
		}

		void popContext(string shortName) {
			ContextModel** ref = &childContexts[shortName];
			childContexts.erase(childContexts.find(shortName));
			delete* ref;
		}

		ContextModel* tryFind(const vector<string>& path, const vector<ContextModel*>& additionalContexts = {})
		{
			if (path[0] == "other")
				int k = 0;

			int n = path.size();
			int i = 0;
			auto* ptr = this;
			bool found = false;

			if (this->shortName == path[0])
			{
				found = true;
				++i;
			}

			while (i < n && !found)
			{
				if (!ptr) break;

				if (ptr->childContexts.count(path[i]))
				{
					found = true;
					break;
				}

				ptr = ptr->parent;
			}

			if (!found)
				return nullptr;

			while (i < n)
			{
				if (ptr->contextType == ContextModelType::VARIABLE)
				{
					auto searchString = ptr->variableType->type;
					ContextModel* v = tryFind(searchString, additionalContexts);

					for (auto* i : additionalContexts)
					{
						if (v) break;

						if (i == this) continue;

						if (v = i->tryFind(searchString, additionalContexts)) break;
					}

					if (!v)
						return nullptr;

					ptr = v;
				}

				if (ptr->childContexts.count(path[i]))
				{
					ptr = ptr->childContexts.at(path[i++]);
				}
				else
					return nullptr;
			}

			return ptr;
		}

	};
}
}