#ifndef EVALUATE_HPP
#define EVALUATE_HPP

    #include "MiLang.hpp"
    #include "interpreter/Interpreter.hpp"

    using namespace std;

    Value VariableNode::evaluate(Interpreter& interpreter) {
        Value value;
        if (interpreter.getVariable(name, value)) {
            return value;
        }
        throw runtime_error("Undefined variable: " + name);
    }

    Value CallNode::evaluate(Interpreter& interpreter) {
        vector<Value> args;
        for (auto& argNode : arguments) {
            args.push_back(argNode->evaluate(interpreter));
        }
        return interpreter.callBuiltin(name, args);
    }

    Value AssignNode::evaluate(Interpreter& interpreter) {
        Value value = expr->evaluate(interpreter);
        interpreter.setVariable(varName, value);
        return value;
    }
    
    Value WhileNode::evaluate(Interpreter& interpreter) {
        interpreter.pushFrame();
        int loopCount = 0;
        while (true) {
            Value condValue = condition->evaluate(interpreter);
            InnerMethod& inner = interpreter.getInnerMethod();
            
            bool conditionTrue = false;
            if (holds_alternative<IntType>(condValue)) {
                conditionTrue = (get<IntType>(condValue) != 0);
            } else if (holds_alternative<FloatType>(condValue)) {
                conditionTrue = (get<FloatType>(condValue) != 0.0);
            } else if (holds_alternative<BoolType>(condValue)) {
                conditionTrue = get<BoolType>(condValue);
            } else if (holds_alternative<StringType>(condValue)) {
                conditionTrue = !get<StringType>(condValue).empty();
            } else {
                throw runtime_error("Type error in while condition at line " + to_string(line));
            }
            
            if (!conditionTrue) {
                break;
            }

            body->evaluate(interpreter);

            if (++loopCount > MAX_DEAD_LOOP) {
                throw runtime_error("Possible infinite loop detected at line " + to_string(line));
            }
        }
        
        
        interpreter.popFrame();
        
        return 0; 
    }

    
    Value ForNode::evaluate(Interpreter& interpreter) {
        
        interpreter.pushFrame();

        try {
            
            if (init) {
                init->evaluate(interpreter);
            }

            int loopCount = 0;
            while (true) {
                
                Value condValue = condition->evaluate(interpreter);
                InnerMethod& inner = interpreter.getInnerMethod();

                
                bool conditionTrue = false;
                if (holds_alternative<IntType>(condValue)) {
                    conditionTrue = (get<IntType>(condValue) != 0);
                } else if (holds_alternative<FloatType>(condValue)) {
                    conditionTrue = (get<FloatType>(condValue) != 0.0);
                } else if (holds_alternative<BoolType>(condValue)) {
                    conditionTrue = get<BoolType>(condValue);
                } else if (holds_alternative<StringType>(condValue)) {
                    conditionTrue = !get<StringType>(condValue).empty();
                } else {
                    throw runtime_error("Type error in for condition at line " + to_string(line));
                }

                if (!conditionTrue) {
                    break;
                }

                
                body->evaluate(interpreter);

                
                if (update) {
                    update->evaluate(interpreter);
                }

                
                if (++loopCount > MAX_DEAD_LOOP) {
                    throw runtime_error("Possible infinite loop detected at line " + to_string(line));
                }
            }
        } catch (...) {
            interpreter.popFrame(); 
            throw;
        }

        
        interpreter.popFrame();

        return 0; 
    }
    Value IfNode::evaluate(Interpreter& interpreter) {
        for (auto& branch : branches) {
            Value conditionValue = branch.condition->evaluate(interpreter);
            InnerMethod& inner = interpreter.getInnerMethod();

            
            bool conditionTrue = false;
            if (holds_alternative<IntType>(conditionValue)) {
                conditionTrue = (get<IntType>(conditionValue) != 0);
            } else if (holds_alternative<FloatType>(conditionValue)) {
                conditionTrue = (get<FloatType>(conditionValue) != 0.0);
            } else if (holds_alternative<BoolType>(conditionValue)) {
                conditionTrue = get<BoolType>(conditionValue);
            } else if (holds_alternative<StringType>(conditionValue)) {
                conditionTrue = !get<StringType>(conditionValue).empty();
            } else {
                throw runtime_error("Type error in if condition");
            }

            if (conditionTrue) {
                return branch.body->evaluate(interpreter);
            }
        }

        if (elseBlock) {
            return elseBlock->evaluate(interpreter);
        }

        return 0; 
    }

#endif
