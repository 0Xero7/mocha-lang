#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <queue>
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
				std::string indentText;
				PrettyWriter pw;

				std::filesystem::path currentOutputDirectory;

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
					auto* prog = (Program*)program;
					for (auto [pkgName, pkg] : prog->packages) {
						auto pkgNameRaw = ((Identifier*)pkg->getPackageName())->get_raw();

						auto temp = currentOutputDirectory;

						for (auto& pkgPart : pkgNameRaw) {
							currentOutputDirectory = currentOutputDirectory.append(pkgPart);
							std::filesystem::create_directory(currentOutputDirectory);
						}

						//pw = PrettyWriter(indentText);
						writePackage(pkg);

						currentOutputDirectory = temp;


						
						

						/*std::ofstream file;
						file.open(outputPath + "\\compiled.java");
						file << pw.getString();
						file.close();*/
					}
				}

				void flushToFile(std::string filename) {
					auto path = currentOutputDirectory.append(filename);
					std::ofstream file;
					file.open(path);
					file << pw.getString();
					file.close();
					currentOutputDirectory = currentOutputDirectory.parent_path();
				}

			public:
				JavaWriter(std::string indentText) : pw(PrettyWriter(indentText)), indentText(indentText) { }

				void transpileToJava(std::string outputPath, Statement* program) {
					// TODO : create a dynamic class finder with main() function
					auto className = "Entrypoint";

					currentOutputDirectory = std::filesystem::path(outputPath);

					write(program);

					pw = PrettyWriter(indentText);

					std::stringstream ss;
					ss << "package com.company;\n";
					ss << "class Main {\n";
					ss << "  public static void main(String[] args) {\n";
					ss << "    var userClass = new " << className << "();\n";
					ss << "    userClass.main();\n";
					ss << "  }\n";
					ss << "}\n\n";
					pw.rawWrite(ss.str());
					pw.writeNewLine();

					std::ofstream file;
					file.open(outputPath + "\\com\\company\\" + ((Program*)program)->programName + ".java");
					file << pw.getString();
					file.close();
				}
			};
		}
	}
}
