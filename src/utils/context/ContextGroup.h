#pragma once

#include "ContextModel.h"
#include "ContextFinder.h"

namespace MochaLang {
namespace Utils {

	class ContextGroup
	{
	public:
		Symbols::ContextModel* currentContext;
		std::vector<Symbols::ContextModel*> import_contexts;

		ContextGroup(Symbols::ContextModel* currentContext) : currentContext(currentContext) { }

		void clear_imports() { import_contexts.clear(); }

		void addAndMoveToSubContext() { currentContext = currentContext->addLocalContext(); };
		void popSubContextAndMoveToParent() {
			auto* ptr = currentContext->parent;
			//delete currentContext;

			currentContext = ptr;
			ptr->childContexts.erase(ptr->childContexts.find("$$LOCAL$$"));
		};

		/*void addToCurrentSubContext(Symbols::ContextModel* model)
		{
			sub_contexts.back().push_back(model);
		}*/

		Symbols::ContextModel* tryFindContext(const std::vector<std::string>& path)
		{
			Symbols::ContextModel* fnd = nullptr;

			//// start searching from the latest sub context
			//for (auto it = sub_contexts.rbegin(); it != sub_contexts.rend(); ++it)
			//{
			//	for (auto it2 = it->rbegin(); it2 != it->rend(); ++it2) {
			//		/*if ((fnd = findContext(path, *it2)))
			//			return fnd;*/
			//		if ((fnd = (*it2)->tryFind(path)))
			//			return fnd;
			//	}
			//}
			
			if (fnd = currentContext->tryFind(path, import_contexts)) return fnd;
			
			for (auto* context : import_contexts)
			{
				if ((fnd = context->tryFind(path, import_contexts)))
					return fnd;
			}

			return nullptr;
		}
	};
		
}
}