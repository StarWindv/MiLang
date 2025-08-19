#ifndef TOKEN_TOOLS_CPP
#define TOKEN_TOOLS_CPP

    #include "../MiLang.hpp"
    #include <array>

    using namespace std;

    const array<std::string, static_cast<size_t>(TokenType::COUNT)> tokenTypeNames = {
        "INTEGER",      // 整数
        "FLOAT",        // 浮点数
        "STRING",       // 字符串
        "BOOLEAN",      // 布尔值
        "IDENTIFIER",   // 标识符(变量名或函数名)
        "ASSIGN",       // =
        "LPAREN",       // (
        "RPAREN",       // )
        "COMMA",        // ,
        "LBRACE",       // {
        "RBRACE",       // }
        "PLUS",         // +
        "MINUS",        // -
        "MULTIPLY",     // *
        "DIVIDE",       // /
        "POWER",        // ^ (乘方)
        "PYPOWER",      // ** (Pythonic Power)
        /*
        :: 比较运算符
        */
        "EQ",           // ==
        "NEQ",          // !=
        "GT",           // >
        "LT",           // <
        "GTE",          // >=
        "LTE",          // <=
        "NOT_GT",       // !> 等价于 <=
        "NOT_LT",       // !< 等价于 >=
        "NOT",          // !  非运算符

        "SEMICOLON",    // ; 分号
        "COLON",        // :

        "WHILE",

        "INDENT",       // 缩进
        "DEDENT",       // 解除缩进

        "EOF_TOKEN"
    };

    std::string TokenTypePrint(TokenType type) {
        size_t index = static_cast<size_t>(type);
        if (index < tokenTypeNames.size()) {
            return tokenTypeNames[index];
        }
        return "UNKNOWN";
    }

#endif