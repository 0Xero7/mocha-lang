// neon-lang.cpp : Defines the entry point for the application.
//

#include <string>
#include <fstream>
#include <sstream>

#include "mocha-lang.h"
#include "src/Lexer.h"
#include "src/Parser.h"
#include "src/passes/Base.h"
#include "src/output/StatementTreePrinter.h"
#include "src/targets/java/JavaWriter.h"
#include "src/DependencyResolver.h"
#include "src/passes/ContextPass.h"

//#include "tst/DotExprToVecStrTest.h"

using namespace std;

int main()
{
	std::vector<std::string> filesToParse = {
		"C:\\Projects\\mocha-lang\\test\\java_transpile_test.mocha",
	};

	std::string mainFile = "C:\\Projects\\mocha-lang\\test\\java_transpile_test.mocha";

	std::unordered_map<std::string, MochaLang::Statement*> parseTrees;
	std::unordered_set<std::string> classes;
	classes.insert("int");

	for (std::string& path : filesToParse) {
		std::ifstream in(path, std::ios::in);
		string fileContents((std::istreambuf_iterator<char>(in)),
			std::istreambuf_iterator<char>());
		//std::stringstream buffer;
		//buffer << in.rdbuf();

		std::string s = fileContents;
		cout << s << endl << endl;
		auto lexer = MochaLang::Lexer::Tokenizer(s);
		auto tokens = lexer.tokenize();

		for (auto& v : tokens.get_tokens()) cout << "[" << v.tokenValue << "]" << endl;

		auto parser = MochaLang::Parser::Parser();
		auto tree = parser.parse(tokens, false, true);

		auto temp = (MochaLang::BlockStmt*)(((MochaLang::PackageStmt*)tree)->packageContents);
		for (int i = 0; i < temp->size(); ++i) {
			if (temp->get(i)->getType() == MochaLang::StmtType::CLASS)
				classes.insert(((MochaLang::ClassStmt*)(temp->get(i)))->getClassName());
		}
		
		parseTrees[path] = tree;

		std::cout << "DEBUG MODE\n\nPrinting AST for " << path << "\n\n";
		MochaLang::Debug::debug(tree, 0);
		std::cout << "####################################################################\n\n";
	}

	auto dependencyResolver = MochaLang::Passes::DependencyResolver(parseTrees);
	dependencyResolver.resolveDependecies(parseTrees.at(mainFile));
	auto tree = parseTrees.at(mainFile);

	auto context = MochaLang::Passes::ContextPass::generateContext(tree);

	auto basePass = MochaLang::Passes::BasePass::BasePass(classes, context);
	basePass.performBasePass(tree, &tree);

	cout << endl << endl;
	MochaLang::Debug::debug(tree, 0);
	//cout << "ok";

	auto jw = MochaLang::Targets::Java::JavaWriter("  ");
	jw.transpileToJava("C:/Projects/mocha-lang/dev/build/Main.java", tree);
}
