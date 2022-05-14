# CMakeList.txt : CMake project for neon-lang, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("mocha-lang")

# Add source to this project's executable.
add_executable (
	mocha-lang 
	"mocha-lang.cpp" "mocha-lang.h"   "src/Lexer.cpp"  "src/Lexer.h" "src/utils/TokenStream.h" "src/utils/Token.h" "src/Parser.h" "src/utils/Statements.h" "src/Statements.cpp" "src/Parser.cpp" "src/output/StatementTreePrinter.h" "src/utils/Attributes.h"  "src/utils/AttributeUtils.h" "src/targets/java/JavaWriter.h" "src/targets/java/JavaWriterUtils.h" "src/targets/java/JavaWriter.cpp" "src/utils/PrettyWriter.h" "src/common/common.h" "src/DependencyResolver.h" "src/passes/Base.h" "src/passes/Base.cpp" "src/utils/TypeResolver.h" "src/utils/context/ContextModel.h" "src/utils/DotExprToVecStr.h" "src/passes/ContextPass.h" "src/utils/context/ContextFinder.h" "src/passes/GenericLowering.h" "src/utils/generics/ReifiedStatementGenerator.h" "src/utils/TypeHelper.h" "src/utils/context/ContextModelType.h")
target_compile_features(mocha-lang PRIVATE cxx_std_17)

# TODO: Add tests and install targets if needed.
