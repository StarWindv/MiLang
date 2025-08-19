#ifndef PARSER_HPP
    #define PARSER_HPP

    #include "../MiLang.hpp"
    #include "../lexer/Lexer.hpp"
    #include "tokenTools.cpp"

    using namespace std;

    struct UnaryOpNode : ASTNode {
        Token op;
        unique_ptr<ASTNode> expr;

        UnaryOpNode(Token op, unique_ptr<ASTNode> expr)
            : op(op), expr(std::move(expr)) {}

        Value evaluate(Interpreter& interpreter) override {
            Value val = expr->evaluate(interpreter);
            InnerMethod& innermethod = interpreter.getInnerMethod();

            if (op.type == TokenType::NOT) {
                if (holds_alternative<IntType>(val)) {
                    return get<IntType>(val) == 0;
                } else if (holds_alternative<FloatType>(val)) {
                    return get<FloatType>(val) == 0.0;
                } else if (holds_alternative<BoolType>(val)) {
                    return !get<BoolType>(val);
                } else if (holds_alternative<StringType>(val)) {
                    return get<StringType>(val).empty();
                }
                throw runtime_error("Type error: Cannot apply '!' to type " + innermethod.getTypeName(val));
            }
            throw runtime_error("Unknown unary operator");
        }
    };

    class Parser {
    private:
        Lexer& lexer;
        Token currentToken;

        int precedence(TokenType type) {
            switch (type) {
                case TokenType::NOT: return 5;
                case TokenType::POWER: return 4;
                case TokenType::PYPOWER: return 4;
                case TokenType::MULTIPLY:
                case TokenType::DIVIDE: return 3;
                case TokenType::PLUS:
                case TokenType::MINUS: return 2;

                case TokenType::EQ:
                case TokenType::NEQ:
                case TokenType::GT:
                case TokenType::LT:
                case TokenType::GTE:
                case TokenType::LTE:
                case TokenType::NOT_GT:
                case TokenType::NOT_LT: return 1;
                default: return 0;
            }
        }

        bool isRightAssociative(TokenType type) {
            if (type == TokenType::POWER) {return true;}
            if (type == TokenType::PYPOWER) {return true;}
            return false;
        }

        void error(const string& message) {
            throw runtime_error("Parse error (line " + to_string(currentToken.line) + ", current token: " + TokenTypePrint(currentToken.type) + "): " + message);
        }

        void eat(TokenType type) {
            if (currentToken.type == type) {
                currentToken = lexer.getNextToken();
            } else {
                error("Expected token type " + TokenTypePrint(type) +
                     ", got " + TokenTypePrint(currentToken.type));
                cout <<"Expected token type " + TokenTypePrint(type) +
                      ", got " + TokenTypePrint(currentToken.type) << endl;
            }
        }

        unique_ptr<ASTNode> parseFactor() {
            Token token = currentToken;

            switch (token.type) {
                case TokenType::INTEGER: {
                    IntType value = stoi(token.value);
                    eat(TokenType::INTEGER);
                    if (currentToken.type == TokenType::LPAREN) {
                        error("Missing multiplication operator; use " + token.value + " * (...) instead");
                    }
                    return make_unique<NumberNode>(value);
                }

                case TokenType::FLOAT: {
                    FloatType value = stof(token.value);
                    eat(TokenType::FLOAT);
                    if (currentToken.type == TokenType::LPAREN) {
                        error("Missing multiplication operator; use " + token.value + " * (...) instead");
                    }
                    return make_unique<NumberNode>(value);
                }
                case TokenType::STRING: {
                    StringType value = token.value;
                    eat(TokenType::STRING);
                    return make_unique<StringNode>(value);
                }
                case TokenType::BOOLEAN: {
                    BoolType value = (token.value == "true");
                    eat(TokenType::BOOLEAN);
                    return make_unique<BooleanNode>(value);
                }
                case TokenType::NOT: {
                    eat(TokenType::NOT);
                    auto expr = parseFactor();
                    return make_unique<UnaryOpNode>(
                        Token(TokenType::NOT, "!", token.line),
                        std::move(expr)
                    );

                }
                case TokenType::IDENTIFIER: {
                    std::string id = token.value;
                    eat(TokenType::IDENTIFIER);

                    if (currentToken.type == TokenType::LPAREN) {
                        eat(TokenType::LPAREN);
                        vector<unique_ptr<ASTNode>> args;

                        if (currentToken.type != TokenType::RPAREN) {
                            args.push_back(parseExpression());

                            while (currentToken.type == TokenType::COMMA) {
                                eat(TokenType::COMMA);
                                args.push_back(parseExpression());
                            }
                        }

                        eat(TokenType::RPAREN);
                        return make_unique<CallNode>(id, std::move(args));
                    } else {
                        if (currentToken.type == TokenType::LPAREN) {
                            error("Missing multiplication operator; use " + id + " * (...) instead");
                        }
                        return make_unique<VariableNode>(id);
                    }
                }
                case TokenType::LPAREN: {
                    eat(TokenType::LPAREN);
                    auto expr = parseExpression();
                    eat(TokenType::RPAREN);
                    return expr;
                }
                case TokenType::MINUS: {
                    eat(TokenType::MINUS);
                    auto factor = parseFactor();
                    return make_unique<BinOpNode>(
                        make_unique<NumberNode>(static_cast<IntType>(0)),
                        Token(TokenType::MINUS, "-", token.line),
                        std::move(factor)
                    );
                }
                default:
                    error("Invalid token at start of expression");
                    return nullptr;
            }
        }

        unique_ptr<ASTNode> parseExpression() {
            vector<unique_ptr<ASTNode>> output;
            vector<Token> opStack;

            output.push_back(parseFactor());

            while (true) {
                TokenType currentType = currentToken.type;
                if (!(currentType == TokenType::PLUS ||
                      currentType == TokenType::MINUS ||
                      currentType == TokenType::MULTIPLY ||
                      currentType == TokenType::DIVIDE ||
                      currentType == TokenType::POWER ||
                      currentType == TokenType::PYPOWER ||
                      currentType == TokenType::EQ ||
                      currentType == TokenType::NEQ ||
                      currentType == TokenType::GT ||
                      currentType == TokenType::LT ||
                      currentType == TokenType::GTE ||
                      currentType == TokenType::LTE ||
                      currentType == TokenType::NOT_GT ||
                      currentType == TokenType::NOT_LT)) {
                    break;
                }

                Token currentOp = currentToken;

                while (!opStack.empty()) {
                    Token topOp = opStack.back();

                    if (
                        (!isRightAssociative(currentOp.type) &&
                         precedence(currentOp.type) <= precedence(topOp.type)) ||
                        (isRightAssociative(currentOp.type) &&
                         precedence(currentOp.type) < precedence(topOp.type))) {

                        opStack.pop_back();

                        auto right = std::move(output.back());
                        output.pop_back();
                        auto left = std::move(output.back());
                        output.pop_back();

                        output.push_back(make_unique<BinOpNode>(std::move(left), topOp, std::move(right)));
                    } else {
                        break;
                    }
                }

                opStack.push_back(currentOp);
                eat(currentOp.type);
                output.push_back(parseFactor());
            }

            while (!opStack.empty()) {
                Token op = opStack.back();
                opStack.pop_back();

                auto right = std::move(output.back());
                output.pop_back();
                auto left = std::move(output.back());
                output.pop_back();

                output.push_back(make_unique<BinOpNode>(std::move(left), op, std::move(right)));
            }

            if (output.size() != 1) {
                error("Malformed expression");
            }

            return std::move(output[0]);
        }

        unique_ptr<ASTNode> parseAssignment() {
            std::string varName = currentToken.value;
            eat(TokenType::IDENTIFIER);
            eat(TokenType::ASSIGN);
            auto expr = parseExpression();
            return make_unique<AssignNode>(varName, std::move(expr));
        }
        
        unique_ptr<BlockNode> parseBlock() {
            vector<unique_ptr<ASTNode>> statements;

            while (currentToken.type != TokenType::DEDENT &&
                   currentToken.type != TokenType::EOF_TOKEN) {

                statements.push_back(parseStatement());
            }

            if (currentToken.type == TokenType::DEDENT) {
                eat(TokenType::DEDENT);
            }

            return make_unique<BlockNode>(std::move(statements));
        }
        
        unique_ptr<WhileNode> parseWhileStatement() {
            int line = currentToken.line;
            eat(TokenType::WHILE);
            
            unique_ptr<ASTNode> condition = parseExpression();
            
            eat(TokenType::COLON);
            
            
            if (currentToken.type != TokenType::INDENT) {
                error("Expected indentation after 'while' statement");
            }
            eat(TokenType::INDENT);
            
            
            auto body = parseBlock();
            
            return make_unique<WhileNode>(std::move(condition), std::move(body), line);
        }

        unique_ptr<ASTNode> parseExpressionOrAssignment() {
            if (currentToken.type == TokenType::IDENTIFIER) {
                size_t pos = lexer.getPos();
                char currentChar = lexer.getCurrentChar();
                int currentLine = currentToken.line;

                Token nextToken = lexer.getNextToken();
                bool isAssignment = (nextToken.type == TokenType::ASSIGN);

                lexer.setPos(pos);
                lexer.setCurrentChar(currentChar);
                currentToken = Token(TokenType::IDENTIFIER, currentToken.value, currentLine);

                if (isAssignment) {
                    return parseAssignment();
                }
            }
            return parseExpression();
        }

        
        unique_ptr<ForNode> parseForStatement() {
            int line = currentToken.line;
            eat(TokenType::FOR); 

            eat(TokenType::LPAREN); 

            
            unique_ptr<ASTNode> init;
            if (currentToken.type != TokenType::SEMICOLON) {
                init = parseExpressionOrAssignment();
            }
            eat(TokenType::SEMICOLON); 

            
            unique_ptr<ASTNode> condition;
            if (currentToken.type != TokenType::SEMICOLON) {
                condition = parseExpressionOrAssignment();
            } else {
                
                condition = make_unique<BooleanNode>(true);
            }
            eat(TokenType::SEMICOLON); 

            
            unique_ptr<ASTNode> update;
            if (currentToken.type != TokenType::RPAREN) {
                update = parseExpressionOrAssignment();
            }
            eat(TokenType::RPAREN); 

            eat(TokenType::COLON); 

            
            if (currentToken.type != TokenType::INDENT) {
                error("Expected indentation after 'for' statement");
            }
            eat(TokenType::INDENT);

            
            auto body = parseBlock();

            return make_unique<ForNode>(std::move(init), std::move(condition),
                                       std::move(update), std::move(body), line);
        }

        unique_ptr<IfNode> parseIfStatement() {
            vector<IfNode::Branch> branches;

            
            eat(TokenType::IF);
            auto condition = parseExpression();
            eat(TokenType::COLON);

            
            if (currentToken.type != TokenType::INDENT) {
                error("Expected indentation after 'if' statement");
            }
            eat(TokenType::INDENT);

            auto ifBody = parseBlock();
            branches.push_back({std::move(condition), std::move(ifBody)});

            
            while (currentToken.type == TokenType::ELIF) {
                eat(TokenType::ELIF);
                auto elifCondition = parseExpression();
                eat(TokenType::COLON);

                if (currentToken.type != TokenType::INDENT) {
                    error("Expected indentation after 'elif' statement");
                }
                eat(TokenType::INDENT);

                auto elifBody = parseBlock();
                branches.push_back({std::move(elifCondition), std::move(elifBody)});
            }

            
            unique_ptr<BlockNode> elseBlock = nullptr;
            if (currentToken.type == TokenType::ELSE) {
                eat(TokenType::ELSE);
                eat(TokenType::COLON);

                if (currentToken.type != TokenType::INDENT) {
                    error("Expected indentation after 'else' statement");
                }
                eat(TokenType::INDENT);

                elseBlock = parseBlock();
            }

            return make_unique<IfNode>(std::move(branches), std::move(elseBlock));
        }


        unique_ptr<ASTNode> parseStatement() {
            switch (currentToken.type) {
                case TokenType::IDENTIFIER: {
                    std::string varName = currentToken.value;

                    size_t pos = lexer.getPos();
                    char currentChar = lexer.getCurrentChar();
                    int currentLine = currentToken.line;

                    Token nextToken = lexer.getNextToken();
                    bool isAssignment = (nextToken.type == TokenType::ASSIGN);

                    lexer.setPos(pos);
                    lexer.setCurrentChar(currentChar);
                    currentToken = Token(TokenType::IDENTIFIER, varName, currentLine);

                    if (isAssignment) {
                        return parseAssignment();
                    } else {
                        return parseExpression();
                    }
                }
                case TokenType::WHILE: {
                    return parseWhileStatement();
                }
                case TokenType::FOR: {
                    return parseForStatement();
                }
                case TokenType::IF: {
                    return parseIfStatement();
                }

                default:
                    return parseExpression();
            }
        }

    public:
        Parser(Lexer& lexer) : lexer(lexer), currentToken(lexer.getNextToken()) {}

        unique_ptr<BlockNode> parseProgram() {
            vector<unique_ptr<ASTNode>> statements;

            while (currentToken.type != TokenType::EOF_TOKEN) {
                if (currentToken.type == TokenType::INDENT) {
                    error("Unexpected indentation");
                } else if (currentToken.type == TokenType::DEDENT) {
                    error("Unexpected dedentation");
                } else {
                    statements.push_back(parseStatement());
                }
            }

            return make_unique<BlockNode>(std::move(statements));
        }
    };

#endif
