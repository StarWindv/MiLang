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
        if (interpreter.isBuiltinFunction(name)) {
            vector<Value> args;
            for (auto& argNode : positionalArguments) {
                args.push_back(argNode->evaluate(interpreter));
            }
            return interpreter.callBuiltin(name, args);
        }
        
        Value funcValue;
        if (!interpreter.getVariable(name, funcValue)) {
            throw runtime_error("Unknown function: " + name);
        }
        if (holds_alternative<FunctionTypePtr>(funcValue)) {
            auto func = get<FunctionTypePtr>(funcValue);
            
            size_t minArgs = 0;
            size_t maxArgs = func->parameters.size();
            
            for (const auto& param : func->parameters) {
                if (!param.hasDefault) {
                    minArgs++;
                }
            }
            
            if (positionalArguments.size() > maxArgs) {
                throw runtime_error("Too many positional arguments for function " + name +
                                   ": expected at most " + std::to_string(maxArgs) +
                                   ", got " + std::to_string(positionalArguments.size()));
            }
            if (positionalArguments.size() < minArgs) {
                
                size_t providedRequired = positionalArguments.size();
                for (const auto& param : func->parameters) {
                    if (!param.hasDefault && namedArguments.find(param.name) != namedArguments.end()) {
                        providedRequired++;
                    }
                }
                if (providedRequired < minArgs) {
                    throw runtime_error("Not enough arguments for function " + name +
                                       ": expected at least " + std::to_string(minArgs) +
                                       ", got " + std::to_string(providedRequired));
                }
            }
            
            interpreter.pushFrame();
            
            for (size_t i = 0; i < positionalArguments.size(); i++) {
                Value argValue = positionalArguments[i]->evaluate(interpreter);
                interpreter.setVariable(func->parameters[i].name, argValue);
            }
            
            for (const auto& namedArg : namedArguments) {
                const std::string& paramName = namedArg.first;
                bool found = false;
                
                for (const auto& param : func->parameters) {
                    if (param.name == paramName) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    throw runtime_error("Unknown parameter '" + paramName + "' for function " + name);
                }
                
                size_t paramIndex = 0;
                for (const auto& param : func->parameters) {
                    if (param.name == paramName) {
                        if (paramIndex < positionalArguments.size()) {
                            throw runtime_error("Parameter '" + paramName +
                                               "' already set by positional argument");
                        }
                        break;
                    }
                    paramIndex++;
                }
                
                Value argValue = namedArg.second->evaluate(interpreter);
                interpreter.setVariable(paramName, argValue);
            }
            
            for (size_t i = positionalArguments.size(); i < func->parameters.size(); i++) {
                const auto& param = func->parameters[i];
                
                if (namedArguments.find(param.name) != namedArguments.end()) {
                    continue;
                }
                if (param.hasDefault) {
                    Value defaultValue = param.defaultValue->evaluate(interpreter);
                    interpreter.setVariable(param.name, defaultValue);
                } else {
                    
                    if (namedArguments.find(param.name) == namedArguments.end()) {
                        throw runtime_error("Missing argument for parameter: " + param.name);
                    }
                }
            }
            
            Value result;
            try {
                result = func->body->evaluate(interpreter);
            } catch (const ReturnException& e) {
                result = e.value; 
            }
            
            interpreter.popFrame();
            return result;
        } else {
            throw runtime_error(name + " is not a function");
        }
    }


    Value ReturnNode::evaluate(Interpreter& interpreter) {
        Value value = expr->evaluate(interpreter);
        throw ReturnException(value, line); 
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
            try{
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
            } catch (const runtime_error& e) {
                string errMsg = e.what();
                if (errMsg.find("Break outside of loop") != string::npos) {
                    break;
                } else if (errMsg.find("Continue outside of loop") != string::npos) {
                    continue;
                } else {
                    throw;
                }
            }
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
                try {
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
                } catch (const runtime_error& e) {
                    string errMsg = e.what();
                    if (errMsg.find("Break outside of loop") != string::npos) {
                        break;
                    } else if (errMsg.find("Continue outside of loop") != string::npos) {
                        if (update) {
                            update->evaluate(interpreter);
                        }
                        continue;
                    } else {
                        throw;
                    }
                }
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

    Value BreakNode::evaluate(Interpreter& interpreter) {
        throw runtime_error("Break outside of loop at line " + to_string(line));
    }

    Value ContinueNode::evaluate(Interpreter& interpreter) {
        throw runtime_error("Continue outside of loop at line " + to_string(line));
    }

    Value FunctionDefinitionNode::evaluate(Interpreter& interpreter) {
        
        auto func = std::make_shared<FunctionType>(name, parameters, std::move(body));
        interpreter.setVariable(name, func);
        
        return StringType("");
    }
#endif
