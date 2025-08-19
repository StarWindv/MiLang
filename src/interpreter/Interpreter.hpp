#ifndef INTERPRETER_HPP
    #define INTERPRETER_HPP

    #include "InnerMethod.hpp"
    #include "../colors.hpp"

    using FuncVector = std::vector<
        std::pair<
            std::string,
            std::function<Value(InnerMethod&, const std::vector<Value>&)>
        >
    >;

    class Interpreter {
    private:
        stack<unique_ptr<Frame>> frames;
        using BuiltinFunction = function<Value(InnerMethod&, const vector<Value>&)>;
        unordered_map<string, BuiltinFunction> builtinFunctions;
        InnerMethod innermethod;
        FuncVector funcList;

    public:
        Frame* getCurrentFrame() {
            if (frames.empty()) {
                throw runtime_error("No active frame");
            }
            return frames.top().get();
        }

    public:
        Interpreter() {
            frames.push(make_unique<Frame>());
            const FuncVector funcs = {
                {"int",     wrapIMFunc(&InnerMethod::intFunction)},
                {"float",   wrapIMFunc(&InnerMethod::floatFunction)},
                {"bool",    wrapIMFunc(&InnerMethod::boolFunction)},
                {"string",  wrapIMFunc(&InnerMethod::stringFunction)},
                {"type",    wrapIMFunc(&InnerMethod::typeFunction)},
                {"receive", wrapIMFunc(&InnerMethod::receiveFunction)},
                {"clear", wrapIMFunc(&InnerMethod::cleanScreen)},
                {"exit", wrapIMFunc(&InnerMethod::exitFunction)},
                {"writeln", wrapIMFuncWithArg(&InnerMethod::writelnFunction, true)},
                {"write",   wrapIMFuncWithArg(&InnerMethod::writelnFunction, false)},
                {"println", wrapIMFuncWithArg(&InnerMethod::printlnFunction, true)},
                {"print",   wrapIMFuncWithArg(&InnerMethod::printlnFunction, false)},
            };
            this->funcList = funcs;
            auto getFuncList = [this]() -> const FuncVector& {
                return this->funcList;
            }; 
            auto innerFunc = wrapIMFuncWithArg(&InnerMethod::funcList, getFuncList);
            funcList.push_back({"inner", innerFunc});
            for (const auto& [name, func] : funcList) {
                builtinFunctions[name] = func;
                frames.top()->set(
                    name, FunctionType{name}
                );
            }

        }

        InnerMethod& getInnerMethod() { return innermethod; }

        bool getVariable(const string& name, Value& outValue) const {
            if (frames.empty()) {
                return false;
            }
            return frames.top()->find(name, outValue);
        }

        void setVariable(const string& name, const Value& value) {
            if (frames.empty()) {
                throw runtime_error("No Active Stack Frames.");
            }
            frames.top()->set(name, value);
        }

        void pushFrame(Frame* parent = nullptr) {
            if (parent == nullptr && !frames.empty()) {
                parent = frames.top().get();
            }
            auto newFrame = make_unique<Frame>(parent);

            
            if (parent) {
                for (const auto& [varName, value] : parent->variables) {
                    newFrame->set(varName, value);
                }
            }

            frames.push(std::move(newFrame));
        }

        void popFrame() {
            if (frames.size() > 1) { 
                frames.pop();
            }
        }

        Value execute(unique_ptr<ASTNode> node) {
            return node->evaluate(*this);
        }

        Value callBuiltin(const string& name, const vector<Value>& args) {
            auto it = builtinFunctions.find(name);
            if (it == builtinFunctions.end()) {
                throw runtime_error("Unknown function: " + name);
            }
            return it->second(innermethod, args);
        }

        
        Frame* getParentFrame() const {
            if (frames.size() < 2) return nullptr;
            return frames.top()->parent;
        }
    };

#endif