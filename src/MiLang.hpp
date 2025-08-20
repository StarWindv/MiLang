#ifndef MILANG_HPP
    #define MILANG_HPP

    #include <iostream>
    #include <cstdlib>
    #include <fstream>
    #include <string>
    #include <vector>
    #include <unordered_map>
    #include <variant>
    #include <sstream>
    #include <regex>
    #include <stdexcept>
    #include <cmath>
    #include <functional>
    #include <algorithm>
    #include <iomanip>
    #include <memory>
    #include <stack>
    #include <utility>
    #include <cstdint>

    using namespace std;

    using IntType = intmax_t;
    using FloatType = long double;
    using StringType = string;
    using BoolType = bool;
    using NullType = std::monostate;

    extern bool DEBUG; // Under Development
    const int MAX_DEAD_LOOP = 200000;

    struct FunctionType {
        std::string name;
    };

    using Value = variant<IntType, FloatType, StringType, BoolType, FunctionType, NullType>;

    enum class TokenType {
        INTEGER,      // 整数
        FLOAT,        // 浮点数
        STRING,       // 字符串
        BOOLEAN,      // 布尔值
        NULL_TYPE,         // 空值

        IDENTIFIER,   // 标识符(变量名或函数名)
        ASSIGN,       // =
        LPAREN,       // (
        RPAREN,       // )
        COMMA,        // ,
        LBRACE,       // {
        RBRACE,       // }
        PLUS,         // +
        MINUS,        // -
        MULTIPLY,     // *
        DIVIDE,       // /
        POWER,        // ^ (乘方)
        PYPOWER,      // ** (Pythonic Power)
        /*
        :: 比较运算符
        */
        EQ,           // ==
        NEQ,          // !=
        GT,           // >
        LT,           // <
        GTE,          // >=
        LTE,          // <=
        NOT_GT,       // !> 等价于 <=
        NOT_LT,       // !< 等价于 >=
        NOT,          // !  非运算符

        SEMICOLON,    // ; 分号
        COLON,        // :

        WHILE,
        FOR,

        IF,
        ELIF,
        ELSE,

        BREAK,
        CONTINUE,

        INDENT,       // 缩进
        DEDENT,       // 解除缩进
        EOF_TOKEN,    // 文件结束
        COUNT,        // C++ 自动计数器
    };


    class Lexer;
    class Parser;
    class InnerMethod;
    class Interpreter;

    struct Token {
        TokenType type;
        std::string value;
        int line;
        Token(TokenType type, const string& value, int line)
            : type(type), value(value), line(line) {}
    };


    struct ASTNode {
        virtual ~ASTNode() = default;
        virtual Value evaluate(Interpreter& interpreter) = 0;
    };


    struct NumberNode : ASTNode {
        Value value;

        NumberNode(IntType val) : value(val) {}
        NumberNode(FloatType val) : value(val) {}

        Value evaluate(Interpreter& interpreter) override {
            return value;
        }
    };


    struct StringNode : ASTNode {
        StringType value;

        StringNode(const StringType& val) : value(val) {}

        Value evaluate(Interpreter& interpreter) override {
            return value;
        }
    };

    struct BooleanNode : ASTNode {
        BoolType value;

        BooleanNode(BoolType val) : value(val) {}

        Value evaluate(Interpreter& interpreter) override {
            return value;
        }
    };


    struct VariableNode : ASTNode {
        std::string name;

        VariableNode(const string& name) : name(name) {}

        Value evaluate(Interpreter& interpreter) override;
    };


    struct CallNode : ASTNode {
        std::string name;
        vector<unique_ptr<ASTNode>> arguments;

        CallNode(const string& name, vector<unique_ptr<ASTNode>> args)
            : name(name), arguments(std::move(args)) {}

        Value evaluate(Interpreter& interpreter) override;
    };


    struct AssignNode : ASTNode {
        std::string varName;
        unique_ptr<ASTNode> expr;

        AssignNode(const string& name, unique_ptr<ASTNode> expr)
            : varName(name), expr(std::move(expr)) {}

        Value evaluate(Interpreter& interpreter) override;
    };


    struct BinOpNode : ASTNode {
        unique_ptr<ASTNode> left;
        Token op;
        unique_ptr<ASTNode> right;

        BinOpNode(unique_ptr<ASTNode> left, Token op, unique_ptr<ASTNode> right)
            : left(std::move(left)), op(op), right(std::move(right)) {}

        Value evaluate(Interpreter& interpreter) override;
    };


    struct BlockNode : ASTNode {
        vector<unique_ptr<ASTNode>> statements;

        BlockNode(vector<unique_ptr<ASTNode>> stmts)
            : statements(std::move(stmts)) {}

        Value evaluate(Interpreter& interpreter) override {
            Value lastResult;
            for (auto& stmt : statements) {
                lastResult = stmt->evaluate(interpreter);
            }
            return lastResult;
        }
    };

    struct NullNode : ASTNode {
        Value evaluate(Interpreter& interpreter) override {
            return NullType();
        }
    };

    struct WhileNode : ASTNode {
        unique_ptr<ASTNode> condition;
        unique_ptr<BlockNode> body;
        int line;

        WhileNode(unique_ptr<ASTNode> condition, unique_ptr<BlockNode> body, int line)
            : condition(std::move(condition)), body(std::move(body)), line(line) {}

        Value evaluate(Interpreter& interpreter) override;
    };

    struct ForNode : ASTNode {
        unique_ptr<ASTNode> init;
        unique_ptr<ASTNode> condition;
        unique_ptr<ASTNode> update;
        unique_ptr<BlockNode> body;
        int line;

        ForNode(unique_ptr<ASTNode> init, unique_ptr<ASTNode> condition,
                unique_ptr<ASTNode> update, unique_ptr<BlockNode> body, int line)
            : init(std::move(init)), condition(std::move(condition)),
              update(std::move(update)), body(std::move(body)), line(line) {}

        Value evaluate(Interpreter& interpreter) override;
    };

    struct IfNode : ASTNode {
        struct Branch {
            unique_ptr<ASTNode> condition;
            unique_ptr<BlockNode> body;
        };
        vector<Branch> branches;
        unique_ptr<BlockNode> elseBlock;

        IfNode(vector<Branch> branches, unique_ptr<BlockNode> elseBlock)
            : branches(std::move(branches)), elseBlock(std::move(elseBlock)) {}

        Value evaluate(Interpreter& interpreter) override;
    };

    struct BreakNode : ASTNode {
        int line;

        BreakNode(int line) : line(line) {}

        Value evaluate(Interpreter& interpreter) override;
    };

    struct ContinueNode : ASTNode {
        int line;

        ContinueNode(int line) : line(line) {}

        Value evaluate(Interpreter& interpreter) override;
    };

    struct Frame {
        unordered_map<string, Value> variables;
        Frame* parent;

        Frame(Frame* parent = nullptr) : parent(parent) {}

        bool find(const string& name, Value& outValue) const {
            auto it = variables.find(name);
            if (it != variables.end()) {
                outValue = it->second;
                return true;
            }

            if (parent) {
                return parent->find(name, outValue);
            }
            return false;
        }

        bool existsInCurrentScope(const string& name) const {
            return variables.find(name) != variables.end();
        }

        void set(const string& name, const Value& value) {

            Value tmp;
            if (find(name, tmp)) {
                Frame* target = findFrameContaining(name);
                if (target) {
                    target->variables[name] = value;
                    return;
                }
            }

            variables[name] = value;
        }

    private:
        Frame* findFrameContaining(const string& name) const {
            if (variables.find(name) != variables.end()) {
                return const_cast<Frame*>(this);
            }

            if (parent) {
                return parent->findFrameContaining(name);
            }

            return nullptr;
        }
    };


    template<typename F>
    auto wrapIMFunc(F&& func) {
        /*
        #  包装、注册内置函数
        */
        return [func = std::forward<F>(func)](InnerMethod& i, const std::vector<Value>& args) {
            return (i.*func)(args);
        };
    }

    template<typename F, typename Arg1>
    auto wrapIMFuncWithArg(F&& func, Arg1&& arg1) {
        /*
        #  同上, 但是可带参数
        */
        return [func = std::forward<F>(func), arg1 = std::forward<Arg1>(arg1)](
            InnerMethod& i, const std::vector<Value>& args
        ) {
            return (i.*func)(args, arg1);
        };
    }


#endif