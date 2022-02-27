#pragma once

#include <unordered_map>
#include "utils/Statements.h"

namespace MochaLang {
	namespace Passes {
		
		class DependencyResolver {
		private:
			std::unordered_map<std::string, Statement*> parsedFiles;

		public:
			DependencyResolver(std::unordered_map<std::string, Statement*>& parsedFiles)
				: parsedFiles(parsedFiles) { }

			void resolveDependecies(Statement* stmt) {
				if (stmt->getType() != StmtType::BLOCK) return;
				
				BlockStmt* block = (BlockStmt*)stmt;
				int n = block->size();
				for (int i = 0; i < n; ++i) {
					Statement* S = block->get(i);
					if (S->getType() == StmtType::IMPORT) {
						ImportStmt* imp = (ImportStmt*)S;
						for (Expr* expr : imp->getImports()) {
							if (expr->getType() != StmtType::RAW_STRING) 
								throw "Unsupported format for imports!";

							auto importName = ((RawString*)expr)->get();
							if (!parsedFiles.count(importName))
								throw "File " + importName + " not found!";

							resolveDependecies(parsedFiles.at(importName));
							block->replace(i, parsedFiles.at(importName));
						}
					}
				}
			}
		};

	}
}