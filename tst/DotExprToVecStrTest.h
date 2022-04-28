#pragma once
#include <assert.h>
#include "../src/utils/DotExprToVecStr.h"
#include "../src/utils/Statements.h"

using namespace MochaLang;

void testFlattenDotExpr1() {
	std::string COM = "com";
	auto expr = new Identifier(COM);

	auto result = MochaLang::Utils::flattenDotExpr(expr);
	std::vector<std::string> expected = { "com" };

	assert(result == expected);
}

void testFlattenDotExpr2() {
	std::string COM = "com";
	std::string COMPANY = "company";
	auto expr = new BinaryOp(new Identifier(COM), StmtType::OP_DOT, new Identifier(COMPANY));

	auto result = MochaLang::Utils::flattenDotExpr(expr);
	std::vector<std::string> expected = { "com", "company" };

	assert(result == expected);
}

void testFlattenDotExpr3() {
	std::string COM = "com";
	std::string COMPANY = "company";
	std::string EXAMPLE = "example";
	auto expr = new BinaryOp(new BinaryOp(new Identifier(COM), StmtType::OP_DOT, new Identifier(COMPANY)), StmtType::OP_DOT,
		new Identifier(EXAMPLE));

	auto result = MochaLang::Utils::flattenDotExpr(expr);
	std::vector<std::string> expected = { "com", "company", "example" };

	assert(result == expected);
}

int main() {
	testFlattenDotExpr1();
}