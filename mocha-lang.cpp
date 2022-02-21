// neon-lang.cpp : Defines the entry point for the application.
//

#include <string>
#include <fstream>
#include <sstream>

#include "mocha-lang.h"
#include "src/Lexer.h"
#include "src/Parser.h"
#include "src/output/StatementTreePrinter.h"

using namespace std;

int main()
{// if (x * (abc + def)) 

	std::ifstream in("C:/Users/smpsm/source/repos/neon-lang/test/if_test.neo", std::ios::in);
	string fileContents((std::istreambuf_iterator<char>(in)),
		std::istreambuf_iterator<char>());
	//std::stringstream buffer;
	//buffer << in.rdbuf();

	std::string s = fileContents; // buffer.str(); // "if (x * (abc + def)) { \n3 + 4\n abc * def }";
	cout << s << endl << endl;
	auto lexer = MochaLang::Lexer::Tokenizer();
	auto tokens = lexer.tokenize(s);

	for (auto& v : tokens.get_tokens()) cout << "[" << v.tokenValue << "]" << endl;

	auto parser = MochaLang::Parser::Parser();
	auto tree = parser.parse(tokens, false, true);

	cout << endl << endl;
	MochaLang::Debug::debug(tree, 0);
	//cout << "ok";
}
