#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"
#ifndef __GNUC__
    #pragma GCC diagnostic ignored "-Winvalid-source-encoding"
#endif

#include "MiLang.hpp"
#include "lexer/Lexer.hpp"
#include "interpreter/InnerMethod.hpp"
#include "binop/BinOp.hpp"
#include "parser/Parser.hpp"
#include "interpreter/Interpreter.hpp"
#include "Title.hpp"
#include "evaluate.hpp"
#include "utils.hpp"
#include "colors.hpp"

using namespace std;

using IntType = intmax_t;
using FloatType = long double;
using StringType = string;
using BoolType = bool;
const string prompt      = ">>> ";
const string wait_prompt = "..> ";


int main(int argc, char* argv[]) {
    bool isREPL;
    std::string source;
    std::string filename = "Default.mi";
    int EXIT_NUM = 0;
    Interpreter interpreter;

    if (argc != 2) {
        isREPL = true;
        title();
        cout << "Type \"inner()\" for built-in function's list " << endl;
    } else {
        isREPL = false;
        filename = argv[1];
        source = readFile(filename);
        if (source == "") {
            return 0;
        }
    }

    while(true) {
        string full_prompt;
        try {
            if (isREPL) {
                full_prompt = CYAN + prompt + RESET;
                cout << full_prompt;
                source = ReplReceive();
                if (source.back() == ':') {
                    while (true) {
                        full_prompt = PURPLE + wait_prompt + RESET;
                        cout << PURPLE << wait_prompt << RESET;
                        string temp = ReplReceive();
                        source += '\n' + temp;
                        if (temp.front()!=' ') {
                            break;
                        }
                    }
                }
            }
            if (source == "") {
                continue;
            }
            sourcePrint(source, filename);

            Lexer lexer(source);

            Parser parser(lexer);
            auto program = parser.parseProgram();

            Value result = interpreter.execute(std::move(program));
            if (isREPL) {
                if (holds_alternative<IntType>(result) ||
                        holds_alternative<FloatType>(result) ||
                        holds_alternative<BoolType>(result) ||
                        !get<StringType>(result).empty()) {
                        std::string returns = interpreter.getInnerMethod().valueToString(result);
                        if (returns != "") {
                            cout << interpreter.getInnerMethod().valueToString(result) << endl;
                        }
                    }
                continue;
            }
            break;
        } catch (const exception& e) {
            cerr << e.what() << endl;
            if (isREPL) {
                continue;
            }
            EXIT_NUM = 1;
            break;
        }
    }

    cout << RESET << endl;
    return EXIT_NUM;
}
