// neon-lang.cpp : Defines the entry point for the application.
//

#include <string>
#include <fstream>
#include <sstream>

#include "mocha-lang.h"
#include "src/Lexer.h"
#include "src/Parser.h"
#include "src/output/StatementTreePrinter.h"
#include "src/targets/java/JavaWriter.h"

using namespace std;

int main()
{// if (x * (abc + def)) 

	std::ifstream in("C:/Users/smpsm/source/repos/mocha-lang/test/java_transpile_test.mocha", std::ios::in);
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

	cout << endl << endl;
	MochaLang::Debug::debug(tree, 0);
	//cout << "ok";

	auto jw = MochaLang::Targets::Java::JavaWriter("  ");
	jw.transpileToJava("C:/Users/smpsm/source/repos/mocha-lang/dev/build/Main.java", tree);
}
