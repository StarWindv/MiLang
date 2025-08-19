#ifndef LEXER_HPP
    #define LEXER_HPP

    #include "../MiLang.hpp"


    class Lexer {
    private:
        std::string source;
        size_t pos;
        char currentChar;
        int line;
        size_t indentLevel;
        std::stack<size_t> indentStack;
        bool atStartOfLine;
        Token currentToken;
        bool afterNewline;

    private:
        void advance() {
            pos++;
            if (pos < source.size()) {
                currentChar = source[pos];
            } else {
                currentChar = '\0';
            }
        }

        void skipWhitespace() {
            while (currentChar != '\0' && isspace(currentChar) && currentChar != '\n') {
                advance();
            }
        }

        void skipLineComment() {
            while (currentChar != '\0' && currentChar != '\n') {
                advance();
            }
        }

        void skipBlockComment() {
            int count = 1;
            advance();

            while (currentChar != '\0' && count > 0) {
                if (currentChar == '`') {
                    advance();
                    if (currentChar == '`') {
                        advance();
                        if (currentChar == '`') {
                            count--;
                            advance();
                            if (count == 0) break;
                        }
                    }
                } else if (currentChar == '\n') {
                    line++;
                    advance();
                } else {
                    advance();
                }
            }

            if (count > 0) {
                throw runtime_error("Unclosed block comment (line " + to_string(line) + ")");
            }
        }

        Token parseNumber() {
            std::string numberStr;
            bool hasDot = false;

            while (currentChar != '\0' && (isdigit(currentChar) || currentChar == '.')) {
                if (currentChar == '.') {
                    if (hasDot) {
                        throw runtime_error("Syntax error (line " + to_string(line) + "): invalid number format");
                    }
                    hasDot = true;
                }
                numberStr += currentChar;
                advance();
            }

            if (hasDot) {
                return Token(TokenType::FLOAT, numberStr, line);
            } else {
                return Token(TokenType::INTEGER, numberStr, line);
            }
        }

        Token parseString() {
            char quote = currentChar;
            advance();

            std::string str;
            while (currentChar != '\0' && currentChar != quote) {
                str += currentChar;
                advance();
            }

            if (currentChar != quote) {
                throw runtime_error("Syntax error (line " + to_string(line) + "): unclosed string");
            }
            advance();

            return Token(TokenType::STRING, str, line);
        }

        Token parseIdentifierOrBoolean() {
            std::string id;
            while (currentChar != '\0' && (isalnum(currentChar) || currentChar == '_')) {
                id += currentChar;
                advance();
            }
            if (id == "True" || id == "true") {
                return Token(TokenType::BOOLEAN, "true", line);
            }
            if (id == "False" || id == "false") {
                return Token(TokenType::BOOLEAN, "false", line);
            }
            if (id == "while") {
                return Token(TokenType::WHILE, id, line);
            }

            return Token(TokenType::IDENTIFIER, id, line);
        }

    public:
        Lexer(const string& source) : source(source), pos(0),
                                      line(1), indentLevel(0),
                                      atStartOfLine(true), afterNewline(false),
                                      currentToken(TokenType::EOF_TOKEN, "", 1) {
            if (source.empty()) {
                currentChar = '\0';
            } else {
                currentChar = source[0];
            }
            indentStack.push(0);
        }

        Token getNextToken() {
            while (currentChar != '\0') {
                if (afterNewline) {
                    afterNewline = false;
                    size_t spaces = 0;
                    while (currentChar == ' ' || currentChar == '\t') {
                        if (currentChar == ' ') {
                            spaces++;
                        } else if (currentChar == '\t') {
                            spaces += 4;
                        }
                        advance();
                    }
                    
                    if (currentChar == '\n') {
                        line++;
                        advance();
                        continue;
                    }

                    size_t currentIndent = spaces / 4;

                    if (currentIndent > indentStack.top()) {
                        indentStack.push(currentIndent);
                        return Token(TokenType::INDENT, "", line);
                    } else if (currentIndent < indentStack.top()) {
                        indentStack.pop();
                        return Token(TokenType::DEDENT, "", line);
                    }
                }
                if(currentChar == '`') {
                    char temp1 = currentChar;
                    advance();
                    char temp2 = currentChar;
                    advance();
                    char temp3 = currentChar;
                    if (
                        (temp1 == '`' && temp2 == '`' && temp3 == '`')
                    ) {
                        skipBlockComment();
                        continue;
                    }
                    pos -= 2;
                    currentChar = source[pos];
                    skipLineComment();
                    continue;
                }
                if (isspace(currentChar)) {
                    if (currentChar == '\n') {
                        line++;
                        advance();
                        afterNewline = true;
                        continue;
                    }
                    skipWhitespace();
                    continue;
                }

                if (currentChar == ';') {
                    advance();
                    return Token(TokenType::SEMICOLON, ";", line);
                }

                if (currentChar == ':') {
                    advance();
                    return Token(TokenType::COLON, ":", line);
                }

                if (isdigit(currentChar)) {
                    return parseNumber();
                }

                if (currentChar == '"' || currentChar == '\'') {
                    return parseString();
                }

                if (isalpha(currentChar) || currentChar == '_') {
                    return parseIdentifierOrBoolean();
                }

                if (currentChar == '=') {
                    advance();
                    if (currentChar == '=') {
                        advance();
                        return Token(TokenType::EQ, "==", line);
                    } else {
                        return Token(TokenType::ASSIGN, "=", line);
                    }
                }

                if (currentChar == '!') {
                    advance();
                    if (currentChar == '=') {
                        advance();
                        return Token(TokenType::NEQ, "!=", line);
                    } else if (currentChar == '>') {
                        advance();
                        return Token(TokenType::NOT_GT, "!>", line);
                    } else if (currentChar == '<') {
                        advance();
                        return Token(TokenType::NOT_LT, "!<", line);
                    } else {
                        return Token(TokenType::NOT, "!", line);
                    }
                }

                if (currentChar == '>') {
                    advance();
                    if (currentChar == '=') {
                        advance();
                        return Token(TokenType::GTE, ">=", line);
                    } else {
                        return Token(TokenType::GT, ">", line);
                    }
                }

                if (currentChar == '<') {
                    advance();
                    if (currentChar == '=') {
                        advance();
                        return Token(TokenType::LTE, "<=", line);
                    } else {
                        return Token(TokenType::LT, "<", line);
                    }
                }

                if (currentChar == '(') {
                    advance();
                    return Token(TokenType::LPAREN, "(", line);
                }

                if (currentChar == ')') {
                    advance();
                    return Token(TokenType::RPAREN, ")", line);
                }

                if (currentChar == ',') {
                    advance();
                    return Token(TokenType::COMMA, ",", line);
                }

                if (currentChar == '{') {
                    advance();
                    return Token(TokenType::LBRACE, "{", line);
                }

                if (currentChar == '}') {
                    advance();
                    return Token(TokenType::RBRACE, "}", line);
                }

                if (currentChar == '+') {
                    advance();
                    return Token(TokenType::PLUS, "+", line);
                }

                if (currentChar == '-') {
                    advance();
                    return Token(TokenType::MINUS, "-", line);
                }

                if (currentChar == '*') {
                    advance();
                    if (currentChar == '*') {
                        advance();
                        return Token(TokenType::PYPOWER, "**", line);
                    }
                    return Token(TokenType::MULTIPLY, "*", line);
                }

                if (currentChar == '/') {
                    advance();
                    return Token(TokenType::DIVIDE, "/", line);
                }

                if (currentChar == '^') {
                    advance();
                    return Token(TokenType::POWER, "^", line);
                }

                throw runtime_error("Syntax error (line " + to_string(line) + "): unknown character '" + string(1, currentChar) + "'");
            }

            
            if (!indentStack.empty() && indentStack.top() > 0) {
                indentStack.pop();
                return Token(TokenType::DEDENT, "", line);
            }

            return Token(TokenType::EOF_TOKEN, "", line);
        }

        size_t getPos() const { return pos; }
        char getCurrentChar() const { return currentChar; }
        void setPos(size_t p) { pos = p; }
        void setCurrentChar(char c) { currentChar = c; }
    };

#endif
