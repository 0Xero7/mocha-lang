# CMakeList.txt : CMake project for neon-lang, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("mocha-lang")

# Add source to this project's executable.
add_executable (mocha-lang "mocha-lang.cpp" "mocha-lang.h"   "src/Lexer.cpp"  "src/Lexer.h" "src/utils/TokenStream.h" "src/utils/Token.h" "src/Parser.h" "src/utils/Statements.h" "src/Statements.cpp" "src/Parser.cpp" "src/output/StatementTreePrinter.h" "src/utils/Attributes.h"  "src/utils/AttributeUtils.h")
target_compile_features(mocha-lang PRIVATE cxx_std_17)

# TODO: Add tests and install targets if needed.
