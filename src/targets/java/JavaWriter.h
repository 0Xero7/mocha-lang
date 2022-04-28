#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include "../../utils/Statements.h"
#include "../../utils/Attributes.h"
#include "../../utils/PrettyWriter.h"
#include "../../utils/TypeResolver.h"
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
				
				void writeConstructorCall(ConstructorCall*);

				void writeImports(ImportStmt*);

				void writeClass(ClassStmt*);

				void writeAttributes(std::vector<Attribute>&);

				void writeFor(ForStmt*);

				void writeIf(IfStmt*);

				void writePackage(PackageStmt*);

				void write(Statement* program) {
					writePackage((PackageStmt*)program);
				}

			public:
				JavaWriter(std::string indentText) : pw(PrettyWriter(indentText)) { }

				void transpileToJava(std::string outputPath, Statement* program) {
					// TODO : create a dynamic class finder with main() function
					auto className = "Entrypoint";

					write(program);

					std::cout << pw.getString() << std::endl;

					std::stringstream ss;
					ss << "class Main {\n";
					ss << "  public static void main(String[] args) {\n";
					ss << "    var userClass = new " << className << "();\n";
					ss << "    userClass.main();\n";
					ss << "  }\n";
					ss << "}\n\n";
					pw.rawWrite(ss.str());
					pw.writeNewLine();

					std::ofstream file;
					file.open(outputPath);
					file << pw.getString();
					file.close();
				}
			};
		}
	}
}
