#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include "../../utils/Statements.h"
#include "../../utils/Attributes.h"
#include "../../utils/PrettyWriter.h"
#include <type_traits>
//#include "JavaWriterUtils.h"

namespace MochaLang {
	namespace Targets {
		namespace Java {
			class JavaWriter {
			private:
				PrettyWriter pw;

				void writeStatement(Statement*);

				void writeExpr(Expr*, bool endWithSemiColon = true, bool = true);

				void writeBlock(BlockStmt*, bool = true);

				void writeVarDecl(VarDecl*, bool = true);

				void writeStmtCollection(std::vector<VarDecl*>);

				void writeFuncDecl(FunctionDecl*);

				void writeReturn(ReturnStmt*);

				void writeFunctionCall(FunctionCall*);

				void writeImports(ImportStmt*);

				void writeClass(ClassStmt*);

				void writeAttributes(std::vector<Attribute>&);

				void writeFor(ForStmt*);

				void writeIf(IfStmt*);

				void write(Statement* program) {
					writeBlock((BlockStmt*)program, false);
				}

			public:
				JavaWriter(std::string indentText) : pw(PrettyWriter(indentText)) { }

				void transpileToJava(std::string outputPath, Statement* program) {
					auto className = ((ClassStmt*)(((BlockStmt*)program)->get(0)))->getClassName();

					std::stringstream ss;
					ss << "package com.company;\n";
					ss << "\n";
					ss << "class Main {\n";
					ss << "  public static void main(String[] args) {\n";
					ss << "    var userClass = new " << className << "();\n";
					ss << "    userClass.main();\n";
					ss << "  }\n";
					ss << "}\n\n";

					pw.rawWrite(ss.str());

					write(program);
					std::ofstream file;
					file.open(outputPath);
					file << pw.getString();
					file.close();

					std::cout << pw.getString() << std::endl;
				}
			};
		}
	}
}
