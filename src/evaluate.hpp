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
        // 创建循环作用域
        interpreter.pushFrame();
        
        int loopCount = 0;
        while (true) {
            // 检查循环条件
            Value condValue = condition->evaluate(interpreter);
            InnerMethod& inner = interpreter.getInnerMethod();
            
            // 检查条件值是否为真
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
            
            // 执行循环体
            body->evaluate(interpreter);
            
            // 防止无限循环
            if (++loopCount > MAX_DEAD_LOOP) {
                throw runtime_error("Possible infinite loop detected at line " + to_string(line));
            }
        }
        
        // 退出循环作用域
        interpreter.popFrame();
        
        return 0; // 循环不返回值
    }
    
    Value ForNode::evaluate(Interpreter& interpreter) {
        // 创建循环作用域
        interpreter.pushFrame();
        
        // 执行初始化语句（如果有）
        if (init) {
            init->evaluate(interpreter);
        }
        
        int loopCount = 0;
        while (true) {
            // 检查循环条件（如果有）
            if (condition) {
                Value condValue = condition->evaluate(interpreter);
                InnerMethod& inner = interpreter.getInnerMethod();
                
                // 检查条件值是否为真
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
                    throw runtime_error("Type error in for loop condition at line " + to_string(line));
                }
                
                if (!conditionTrue) {
                    break;
                }
            }
            
            // 执行循环体
            body->evaluate(interpreter);
            
            // 执行更新语句（如果有）
            if (update) {
                update->evaluate(interpreter);
            }
            
            // 防止无限循环
            if (++loopCount > MAX_DEAD_LOOP) {
                throw runtime_error("Possible infinite loop detected at line " + to_string(line));
            }
        }
        
        // 退出循环作用域
        interpreter.popFrame();
        
        return 0; // 循环不返回值
    }

#endif
