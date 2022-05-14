// neon-lang.cpp : Defines the entry point for the application.
//

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>

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

void recursively_parse(std::string& path, MochaLang::Program* program) {
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (entry.is_directory()) {
			recursively_parse(entry.path().generic_u8string(), program);
			continue;
		}

		auto path = entry.path();

		std::ifstream in(path, std::ios::in);
		string fileContents((std::istreambuf_iterator<char>(in)),
			std::istreambuf_iterator<char>());

		std::string s = fileContents;
		//cout << s << endl << endl;
		auto lexer = MochaLang::Lexer::Tokenizer(s);
		auto tokens = lexer.tokenize();

		for (auto& v : tokens.get_tokens()) cout << "[" << v.tokenValue << "]" << endl;

		auto parser = MochaLang::Parser::Parser();
		auto tree = parser.parse(tokens, true, true);

		auto pkg = (MochaLang::PackageStmt*)tree;
		program->addPackage(pkg);
	}
}

int main()
{
	std::string sourceDir = "C:\\Projects\\mocha-lang\\test\\list";

	auto* program = new MochaLang::Program("ListTest");

	auto start = std::chrono::high_resolution_clock::now();

	recursively_parse(sourceDir, program);

	//MochaLang::Debug::debug(program, 0);

	auto context = MochaLang::Passes::ContextPass::generateContext(program);

	// Inject primitives
	context->addContext("int", MochaLang::Symbols::ContextModelType::CLASS);
	context->addContext("String", MochaLang::Symbols::ContextModelType::CLASS);

	auto basePass = MochaLang::Passes::BasePass::BasePass(context);
	basePass.performBasePass(program, (MochaLang::Statement**)&program);

	//cout << endl << endl;
	MochaLang::Debug::debug(program, 0);
	//cout << "ok";

	auto jw = MochaLang::Targets::Java::JavaWriter("  ", context);
	auto outputPath = R"(C:\Users\Soumya Pattanayak\IdeaProjects\MochaLangRuns\src)";
	std::filesystem::remove_all(outputPath);
	std::filesystem::create_directory(outputPath);
	jw.transpileToJava(outputPath, program);

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	std::cout << "Compiled in " << duration << "ms\n";
}
