#ifndef BINOP_HPP
#define BINOP_HPP

#include "../MiLang.hpp"
#include "../interpreter/InnerMethod.hpp"
#include "../interpreter/Interpreter.hpp"

using namespace std;


Value BinOpNode::evaluate(Interpreter& interpreter) {
    Value leftVal = left->evaluate(interpreter);
    Value rightVal = right->evaluate(interpreter);
    InnerMethod& innermethod = interpreter.getInnerMethod();

    switch (op.type) {
        case TokenType::NOT: {
            if (holds_alternative<IntType>(rightVal)) {
                return get<IntType>(rightVal) == 0;
            } else if (holds_alternative<FloatType>(rightVal)) {
                return get<FloatType>(rightVal) == 0.0;
            } else if (holds_alternative<BoolType>(rightVal)) {
                return !get<BoolType>(rightVal);
            } else if (holds_alternative<StringType>(rightVal)) {
                return get<StringType>(rightVal).empty();
            }
            throw runtime_error("Type error: Cannot apply '!' to this type");
        }
        case TokenType::PLUS: {
            auto [a, b] = innermethod.convertToNumbers(leftVal, rightVal);
            if (holds_alternative<IntType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<IntType>(leftVal) + get<IntType>(rightVal);
            }
            return a + b;
        }
        case TokenType::MINUS: {
            auto [a, b] = innermethod.convertToNumbers(leftVal, rightVal);
            if (holds_alternative<IntType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<IntType>(leftVal) - get<IntType>(rightVal);
            }
            return (FloatType)(a - b);
        }
        case TokenType::MULTIPLY: {
            auto [a, b] = innermethod.convertToNumbers(leftVal, rightVal);
            if (holds_alternative<IntType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<IntType>(leftVal) * get<IntType>(rightVal);
            }
            return (FloatType)a * (FloatType)b;
        }
        case TokenType::DIVIDE: {
            auto [a, b] = innermethod.convertToNumbers(leftVal, rightVal);
            if (b == 0) {
                throw runtime_error("Division by zero (line " + to_string(op.line) + ")");
            }
            return (FloatType)((FloatType)a / (FloatType)b);
        }
        case TokenType::PYPOWER:
        case TokenType::POWER: {
            auto [a, b] = innermethod.convertToNumbers(leftVal, rightVal);
            return (FloatType)pow((FloatType)a, (FloatType)b);
        }
        default:
            break;
    }

    switch (op.type) {
        case TokenType::EQ: { // ==
            if (!innermethod.canCompare(leftVal, rightVal)) {
                throw runtime_error("Type error: Cannot compare different types");
            }

            if (holds_alternative<IntType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<IntType>(leftVal) == get<IntType>(rightVal);
            }
            if (holds_alternative<FloatType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                return get<FloatType>(leftVal) == get<FloatType>(rightVal);
            }
            if (holds_alternative<IntType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                return static_cast<float>(get<IntType>(leftVal)) == get<FloatType>(rightVal);
            }
            if (holds_alternative<FloatType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<FloatType>(leftVal) == static_cast<float>(get<IntType>(rightVal));
            }
            if (holds_alternative<StringType>(leftVal) && holds_alternative<StringType>(rightVal)) {
                return get<StringType>(leftVal) == get<StringType>(rightVal);
            }
            if (holds_alternative<BoolType>(leftVal) && holds_alternative<BoolType>(rightVal)) {
                return get<BoolType>(leftVal) == get<BoolType>(rightVal);
            }
            return false;
        }

        case TokenType::NEQ: {
                if (holds_alternative<IntType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                    return get<IntType>(leftVal) != get<IntType>(rightVal);
                } else if (holds_alternative<FloatType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                    return get<FloatType>(leftVal) != get<FloatType>(rightVal);
                } else if (holds_alternative<IntType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                    return static_cast<float>(get<IntType>(leftVal)) != get<FloatType>(rightVal);
                } else if (holds_alternative<FloatType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                    return get<FloatType>(leftVal) != static_cast<float>(get<IntType>(rightVal));
                } else if (holds_alternative<StringType>(leftVal) && holds_alternative<StringType>(rightVal)) {
                    return get<StringType>(leftVal) != get<StringType>(rightVal);
                } else if (holds_alternative<BoolType>(leftVal) && holds_alternative<BoolType>(rightVal)) {
                    return get<BoolType>(leftVal) != get<BoolType>(rightVal);
                }
            throw runtime_error("Unsupported types for inequality comparison (line " + to_string(op.line) + ")");
        }

        case TokenType::GT: { // >
            if (!innermethod.canCompare(leftVal, rightVal)) {
                throw runtime_error("Type error: Cannot compare different types");
            }

            if (holds_alternative<IntType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<IntType>(leftVal) > get<IntType>(rightVal);
            }
            if (holds_alternative<FloatType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                return get<FloatType>(leftVal) > get<FloatType>(rightVal);
            }
            if (holds_alternative<IntType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                return static_cast<float>(get<IntType>(leftVal)) > get<FloatType>(rightVal);
            }
            if (holds_alternative<FloatType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<FloatType>(leftVal) > static_cast<float>(get<IntType>(rightVal));
            }
            if (holds_alternative<StringType>(leftVal) && holds_alternative<StringType>(rightVal)) {
                return get<StringType>(leftVal) > get<StringType>(rightVal);
            }
            throw runtime_error("Type error: Strings do not support > operator");
        }

        case TokenType::LT: { // <
            if (!innermethod.canCompare(leftVal, rightVal)) {
                throw runtime_error("Type error: Cannot compare different types");
            }

            if (holds_alternative<IntType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<IntType>(leftVal) < get<IntType>(rightVal);
            }
            if (holds_alternative<FloatType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                return get<FloatType>(leftVal) < get<FloatType>(rightVal);
            }
            if (holds_alternative<IntType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                return static_cast<float>(get<IntType>(leftVal)) < get<FloatType>(rightVal);
            }
            if (holds_alternative<FloatType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<FloatType>(leftVal) < static_cast<float>(get<IntType>(rightVal));
            }
            if (holds_alternative<StringType>(leftVal) && holds_alternative<StringType>(rightVal)) {
                return get<StringType>(leftVal) < get<StringType>(rightVal);
            }
            throw runtime_error("Type error: Strings do not support < operator");
        }

        case TokenType::GTE: {
            if (holds_alternative<IntType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<IntType>(leftVal) >= get<IntType>(rightVal);
            } else if (holds_alternative<FloatType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                return get<FloatType>(leftVal) >= get<FloatType>(rightVal);
            } else if (holds_alternative<IntType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                return static_cast<float>(get<IntType>(leftVal)) >= get<FloatType>(rightVal);
            } else if (holds_alternative<FloatType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<FloatType>(leftVal) >= static_cast<float>(get<IntType>(rightVal));
            } else if (holds_alternative<StringType>(leftVal) && holds_alternative<StringType>(rightVal)) {
                return get<StringType>(leftVal) >= get<StringType>(rightVal);
            }
            throw runtime_error("Unsupported types for greater-than-or-equal comparison (line " + to_string(op.line) + ")");
        }

        case TokenType::LTE: {
            if (holds_alternative<IntType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<IntType>(leftVal) <= get<IntType>(rightVal);
            } else if (holds_alternative<FloatType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                return get<FloatType>(leftVal) <= get<FloatType>(rightVal);
            } else if (holds_alternative<IntType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                return static_cast<float>(get<IntType>(leftVal)) <= get<FloatType>(rightVal);
            } else if (holds_alternative<FloatType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<FloatType>(leftVal) <= static_cast<float>(get<IntType>(rightVal));
            } else if (holds_alternative<StringType>(leftVal) && holds_alternative<StringType>(rightVal)) {
                return get<StringType>(leftVal) <= get<StringType>(rightVal);
            }
            throw runtime_error("Unsupported types for less-than-or-equal comparison (line " + to_string(op.line) + ")");
        }

        case TokenType::NOT_GT: {
            if (holds_alternative<IntType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<IntType>(leftVal) <= get<IntType>(rightVal);
            } else if (holds_alternative<FloatType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                return get<FloatType>(leftVal) <= get<FloatType>(rightVal);
            } else if (holds_alternative<IntType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                return static_cast<float>(get<IntType>(leftVal)) <= get<FloatType>(rightVal);
            } else if (holds_alternative<FloatType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<FloatType>(leftVal) <= static_cast<float>(get<IntType>(rightVal));
            } else if (holds_alternative<StringType>(leftVal) && holds_alternative<StringType>(rightVal)) {
                return get<StringType>(leftVal) <= get<StringType>(rightVal);
            }
            throw runtime_error("Unsupported types for not-greater-than comparison (line " + to_string(op.line) + ")");
        }

        case TokenType::NOT_LT: {
            if (holds_alternative<IntType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<IntType>(leftVal) >= get<IntType>(rightVal);
            } else if (holds_alternative<FloatType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                return get<FloatType>(leftVal) >= get<FloatType>(rightVal);
            } else if (holds_alternative<IntType>(leftVal) && holds_alternative<FloatType>(rightVal)) {
                return static_cast<float>(get<IntType>(leftVal)) >= get<FloatType>(rightVal);
            } else if (holds_alternative<FloatType>(leftVal) && holds_alternative<IntType>(rightVal)) {
                return get<FloatType>(leftVal) >= static_cast<float>(get<IntType>(rightVal));
            } else if (holds_alternative<StringType>(leftVal) && holds_alternative<StringType>(rightVal)) {
                return get<StringType>(leftVal) >= get<StringType>(rightVal);
            }
            throw runtime_error("Unsupported types for not-less-than comparison (line " + to_string(op.line) + ")");
        }

        default:
            throw runtime_error("Unsupported operator (line " + to_string(op.line) + ")");
    }
}

#endif