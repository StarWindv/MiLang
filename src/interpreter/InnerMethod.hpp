#ifndef INNER_METHOD_HPP
#define INNER_METHOD_HPP

    #include "../MiLang.hpp"
    #include "../colors.hpp"

    using FuncVector = std::vector<
        std::pair<
            std::string,
            std::function<Value(InnerMethod&, const std::vector<Value>&)>
        >
    >;

    void printVariant(const std::variant<long long int, long double, std::string, bool, FunctionType>& var) {
                std::visit([](const auto& value) {
                    using T = std::decay_t<decltype(value)>;
                    if constexpr (std::is_same_v<T, long long int>) {
                        std::cout << value;
                    } else if constexpr (std::is_same_v<T, long double>) {
                        std::cout << value;
                    } else if constexpr (std::is_same_v<T, std::string>) {
                        std::cout << value;
                    } else if constexpr (std::is_same_v<T, bool>) {
                        std::cout << std::boolalpha << value;
                    } else if constexpr (std::is_same_v<T, FunctionType>) {
                        std::cout << "FunctionType(" << value.name << ")";
                    }
                }, var);
            }

    class InnerMethod {
    private:
        bool canCompareInternal(const Value& a, const Value& b) const {
            if (holds_alternative<BoolType>(a) || holds_alternative<BoolType>(b)) {
                return holds_alternative<BoolType>(a) && holds_alternative<BoolType>(b);
            }

            if (holds_alternative<StringType>(a) || holds_alternative<StringType>(b)) {
                return holds_alternative<StringType>(a) && holds_alternative<StringType>(b);
            }

            return true;
        }

    public:
        std::string getTypeName(const Value& val) {
            if (holds_alternative<IntType>(val)) {
                return "int";
            } else if (holds_alternative<FloatType>(val)) {
                return "float";
            } else if (holds_alternative<StringType>(val)) {
                return "string";
            } else if (holds_alternative<BoolType>(val)) {
                return "bool";
            } else if (holds_alternative<FunctionType>(val)) {
                return "function";
            }
                return "unknown";
        }

        bool canCompare(const Value& a, const Value& b) const {
            return canCompareInternal(a, b);
        }

        pair<FloatType, FloatType> convertToNumbers(const Value& a, const Value& b) {
            FloatType aVal, bVal;

            if (holds_alternative<IntType>(a)) {
                aVal = static_cast<FloatType>(get<IntType>(a));
            } else if (holds_alternative<FloatType>(a)) {
                aVal = get<FloatType>(a);
            } else {
                throw runtime_error("Type error: Cannot perform math operation on string/boolean");
            }

            if (holds_alternative<IntType>(b)) {
                bVal = static_cast<FloatType>(get<IntType>(b));
            } else if (holds_alternative<FloatType>(b)) {
                bVal = get<FloatType>(b);
            } else {
                throw runtime_error("Type error: Cannot perform math operation on string/boolean");
            }

            return {aVal, bVal};
        }

        std::string valueToString(const Value& val) {
            if (holds_alternative<IntType>(val)) {
                return to_string(get<IntType>(val));
            } else if (holds_alternative<FloatType>(val)) {
                float f = get<FloatType>(val);
                if (f == floor(f)) {
                    return to_string(static_cast<int>(f)) + ".0";
                }
                stringstream ss;
                ss << fixed << setprecision(6) << f;
                std::string str = ss.str();
                str.erase(str.find_last_not_of('0') + 1, string::npos);
                if (str.back() == '.') {
                    str += '0';
                }
                return str;
            } else if (holds_alternative<StringType>(val)) {
                return get<StringType>(val);
            } else if (holds_alternative<BoolType>(val)) {
                return get<BoolType>(val) ? "True" : "False";
            } else if (holds_alternative<FunctionType>(val)) {
                return "<Function \"" + get<FunctionType>(val).name + "\">";
            }
            return "Error in \"valueToString\"";
        }

        Value intFunction(const vector<Value>& args) {
            if (args.size() != 1) {
                throw runtime_error("int() requires exactly one argument");
            }

            const Value& arg = args[0];
            if (holds_alternative<IntType>(arg)) {
                return arg;
            } else if (holds_alternative<FloatType>(arg)) {
                return IntType(floor(get<FloatType>(arg)));
            } else if (holds_alternative<StringType>(arg)) {
                std::string s = get<StringType>(arg);
                try {
                    size_t pos;
                    int32_t num = stoi(s, &pos);
                    if (pos == s.size()) {
                        return IntType(num);
                    }
                    float f = stof(s, &pos);
                    if (pos == s.size()) {
                        return IntType(floor(f));
                    }
                } catch (...) {
                    throw runtime_error("Cannot convert to integer: " + s);
                }
                throw runtime_error("Cannot convert to integer: " + s);
            }
            throw runtime_error("Unsupported type for int conversion");
        }

        Value floatFunction(const vector<Value>& args) {
            if (args.size() != 1) {
                throw runtime_error("float() requires exactly one argument");
            }

            const Value& arg = args[0];
            if (holds_alternative<FloatType>(arg)) {
                return arg;
            } else if (holds_alternative<IntType>(arg)) {
                return FloatType(get<IntType>(arg));
            } else if (holds_alternative<StringType>(arg)) {
                std::string s = get<StringType>(arg);
                try {
                    return FloatType(stof(s));
                } catch (...) {
                    throw runtime_error("Cannot convert to float: " + s);
                }
            }
            throw runtime_error("Unsupported type for float conversion");
        }

        Value boolFunction(const vector<Value>& args) {
            if (args.size() != 1) {
                throw runtime_error("bool() requires exactly one argument");
            }

            const Value& arg = args[0];
            if (holds_alternative<BoolType>(arg)) {
                return arg;
            } else if (holds_alternative<IntType>(arg)) {
                return BoolType(get<IntType>(arg) != 0);
            } else if (holds_alternative<FloatType>(arg)) {
                return BoolType(get<FloatType>(arg) != 0.0f);
            } else if (holds_alternative<StringType>(arg)) {
                std::string s = get<StringType>(arg);
                return BoolType(!s.empty() && s != "false" && s != "0");
            }
            return BoolType(false);
        }

        Value stringFunction(const vector<Value>& args) {
            if (args.size() != 1) {
                throw runtime_error("string() requires exactly one argument");
            }

            return StringType(this->valueToString(args[0]));
        }

        Value typeFunction(const vector<Value>& args) {
            if (args.size() != 1) {
                throw runtime_error("type() requires exactly one argument");
            }

            const Value& arg = args[0];
            if (holds_alternative<FunctionType>(arg)) {
                return StringType("<Function \"" + get<FunctionType>(arg).name + "\">");
            }
            return StringType(getTypeName(arg));
        }

        Value receiveFunction(const vector<Value>& args) {
            if (!args.empty() && holds_alternative<StringType>(args[0])) {
                cout << get<StringType>(args[0]);
            }

            std::string input;
            getline(cin, input);
            return StringType(input);
        }

        int countPlaceholders(const string& s) {
            int count = 0;
            size_t pos = 0;

            while (pos < s.size()) {
                size_t start = s.find('{', pos);
                if (start == string::npos) {
                    break;
                }

                if (start + 1 < s.size() && s[start + 1] == '}') {
                    count++;
                    pos = start + 2;
                } else {
                    pos = start + 1;
                }
            }

            return count;
        }

        Value writelnFunction(const vector<Value>& args, bool need_new_line = true) {
            std::string name;
            if (need_new_line) {
                name = "writeln()";
            } else {
                name = "write()";
            }
            if (args.empty()) {
                if (name == "writeln()") {cout << endl;}
                return StringType("");
            }

            if (holds_alternative<StringType>(args[0])) {
                std::string format = get<StringType>(args[0]);
                vector<Value> params(args.begin() + 1, args.end());

                int placeholderCount = countPlaceholders(format);

                if (placeholderCount != 0 && placeholderCount != params.size()) {
                   throw runtime_error(
                        name + " placeholder count mismatch: " +
                        to_string(placeholderCount) + " placeholders, " +
                        to_string(params.size()) + " arguments"
                   );
                }

                if (placeholderCount > 0) {
                    size_t paramIndex = 0;
                    size_t pos = 0;

                    while (pos < format.size() && paramIndex < params.size()) {
                        size_t start = format.find('{', pos);
                        size_t end = format.find('}', start);

                        if (start == string::npos || end == string::npos) {
                            break;
                        }

                        cout << format.substr(pos, start - pos);
                        cout << this->valueToString(params[paramIndex]);

                        paramIndex++;
                        pos = end + 1;
                    }

                    cout << format.substr(pos);
                    if (need_new_line) cout << endl;
                    return StringType("");
                }
            }

            for (const auto& arg : args) {
                cout << this->valueToString(arg);
            }
            if(need_new_line) {
                cout << endl;
            }
            return StringType("");
        }

        Value printlnFunction(const vector<Value>& args, bool need_new_line = true) {
            static const auto unescapeChar = [](char c) -> char {
                switch (c) {
                    case 'n': return '\n';
                    case 't': return '\t';
                    case 'r': return '\r';
                    case 'e': return '\033'; 
                    case 'a': return '\a';
                    case 'b': return '\b';
                    case 'f': return '\f';
                    case 'v': return '\v';
                    case '0': return '\0';
                    default: return c;
                }
            };

            auto outputWithEscape = [](const std::string& s) {
                bool escaping = false;
                for (char c : s) {
                    if (escaping) {
                        cout << unescapeChar(c);
                        escaping = false;
                    } else if (c == '\\') {
                        escaping = true;
                    } else {
                        cout << c;
                    }
                }
                
                if (escaping) {
                    cout << '\\';
                }
            };

            auto countEmptyPlaceholders = [](const std::string& s) -> int {
                int count = 0;
                for (size_t i = 0; i < s.size(); ++i) {
                    if (s[i] == '{' && i + 1 < s.size() && s[i + 1] == '}') {
                        count++;
                        i++; 
                    }
                }
                return count;
            };

            if (args.empty()) {
                if (need_new_line) cout << endl;
                return StringType("");
            }

            
            if (holds_alternative<StringType>(args[0])) {
                const std::string& format = get<StringType>(args[0]);
                const vector<Value> params(args.begin() + 1, args.end());
                const int placeholderCount = countEmptyPlaceholders(format);

                if (placeholderCount != 0 && placeholderCount != static_cast<int>(params.size())) {
                    throw runtime_error(
                        std::string(need_new_line ? "writeln()" : "write()") +
                        " placeholder count mismatch: " +
                        std::to_string(placeholderCount) +
                        " placeholders, " +
                        std::to_string(params.size()) +
                        " arguments"
                    );
                }

                if (placeholderCount > 0) {
                    size_t paramIndex = 0;
                    size_t pos = 0;
                    bool escaping = false;

                    while (pos < format.size()) {
                        if (escaping) {
                            cout << unescapeChar(format[pos]);
                            escaping = false;
                            pos++;
                            continue;
                        }

                        if (format[pos] == '\\') {
                            escaping = true;
                            pos++;
                            continue;
                        }

                        if (format[pos] == '{' && pos + 1 < format.size() && format[pos + 1] == '}') {
                            
                            outputWithEscape(format.substr(pos, pos - pos));

                            
                            if (paramIndex < params.size()) {
                                if (holds_alternative<StringType>(params[paramIndex])) {
                                    outputWithEscape(get<StringType>(params[paramIndex]));
                                } else {
                                    cout << this->valueToString(params[paramIndex]);
                                }
                                paramIndex++;
                            }

                            pos += 2; 
                            continue;
                        }

                        
                        if (format[pos] == '{' || format[pos] == '}') {
                            size_t start = pos;
                            size_t end = format.find_first_of("{}", pos + 1);

                            if (end == string::npos) {
                                
                                outputWithEscape(format.substr(pos));
                                break;
                            }

                            
                            outputWithEscape(format.substr(pos, end - pos + 1));
                            pos = end + 1;
                            continue;
                        }

                        
                        size_t nextSpecial = format.find_first_of("\\{}", pos);
                        if (nextSpecial == string::npos) {
                            
                            outputWithEscape(format.substr(pos));
                            break;
                        }

                        
                        outputWithEscape(format.substr(pos, nextSpecial - pos));
                        pos = nextSpecial;
                    }

                    if (need_new_line) cout << endl;
                    return StringType("");
                }
            }

            
            for (const auto& arg : args) {
                if (holds_alternative<StringType>(arg)) {
                    outputWithEscape(get<StringType>(arg));
                } else {
                    cout << this->valueToString(arg);
                }
            }

            if (need_new_line) cout << endl;
            return StringType("");
        }

        Value cleanScreen(const vector<Value>& args) {
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
            return StringType("");
        }

        Value exitFunction(const vector<Value>& args) {
            cout << RESET << "Exit MiLang REPL" << endl;
            #ifndef __GNUC__
                quick_exit(0);
            #else
                exit(0);
            #endif
            return StringType("");
        }

        Value funcList(const vector<Value>& args, function<const FuncVector&()> getFuncList) {
            auto data = getFuncList();
            int count = 0;
            for (const auto& [name, func] : data) {
                cout << name << "      ";
                count++;
                if (count % 4 == 0) {
                    cout << endl;
                }
            }
            if (count % 4 != 0) {
                cout << endl;
            }
            return StringType("");
        }
    };

#endif
