#ifndef UTILS_HPP
    #define UTILS_HPP

        #include <fstream>
        #include <string>
        #include <iostream>
        #include "cctype"
        #include "colors.hpp"

        using namespace std;

        string readFile(const std::string& filename) {
            std::ifstream file(filename, std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file: " + filename);
            }
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            return content;
        }


        string ReplReceive() {
            string line;
            getline(cin, line);
            return line;
        }

        void sourcePrint(string source, string filename = "Default.mi") {
            cout << PURPLE << "=============== | " << filename << " SOURCE | ===============" << endl;
            cout << CYAN << source << endl;
            cout << GOLD   << "=============== |     END SOURCE    | ===============" << RESET << endl;
        }

        string processLine(const std::string& source) {
            std::string result="";
            if (source.empty()) {
                return result;
            }

            // 预留足够空间，避免频繁扩容
            result.reserve(source.size());

            const char* start = source.data();
            const char* end = source.data() + source.size();
            const char* current = start;

            while (current < end) {
                // 找到当前行的起始位置（跳过前导空白）
                const char* lineStart = current;

                // 找到当前行的结束位置（换行符或字符串末尾）
                while (current < end && *current != '\n' && *current != '\r') {
                    ++current;
                }
                const char* lineEnd = current;

                // 检查当前行是否只包含空白字符
                bool isEmptyLine = true;
                const char* check = lineStart;
                while (check < lineEnd) {
                    if (!std::isspace(static_cast<unsigned char>(*check))) {
                        isEmptyLine = false;
                        break;
                    }
                    ++check;
                }

                // 如果不是空行，则将其添加到结果中
                if (!isEmptyLine) {
                    result.append(lineStart, lineEnd - lineStart);
                }

                // 跳过换行符（处理CRLF和LF两种情况）
                if (current < end && (*current == '\r' || *current == '\n')) {
                    // 记录当前换行符，用于后续判断
                    char lineBreak = *current;
                    ++current;

                    // 如果是CRLF组合，跳过第二个字符
                    if (current < end && *current == '\n' && lineBreak == '\r') {
                        ++current;
                    }

                    // 非空行需要保留一个换行符
                    if (!isEmptyLine) {
                        result += '\n';
                    }
                }
            }

            // 移除最后可能多余的换行符
            if (!result.empty() && result.back() == '\n') {
                result.pop_back();
            }

            return result;
        }


#endif
