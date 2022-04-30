#pragma once

#include "ContextModel.h"

namespace MochaLang {
namespace Utils {

	static MochaLang::Symbols::ContextModel* findContext(std::vector<string> path, MochaLang::Symbols::ContextModel* ctx) {
		MochaLang::Symbols::ContextModel* ptr = ctx;

		while (ptr && !ptr->childContexts.count(path.front())) {
			ptr = ptr->parent;
		}

		// Symbol not found
		if (!ptr) return nullptr;

		ptr = ptr->childContexts[path.front()];

		for (int i = 1; i < path.size(); ++i) {
			if (!ptr->childContexts.count(path[i])) return nullptr;
			ptr = ptr->childContexts[path[i]];
		}

		return ptr;
	}

}
}
