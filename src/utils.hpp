#ifndef UTILS_HPP
    #define UTILS_HPP

        #include <fstream>
        #include <string>
        #include <iostream>

        using namespace std;

        string readFile(const string& filename) {
            ifstream file(filename);
            if (!file.is_open()) {
                throw runtime_error("Cannot open file: " + filename);
            }

            std::string content((istreambuf_iterator<char>(file)),
                                istreambuf_iterator<char>());
            return content;
        }


        string ReplReceive() {
            string line;
            getline(cin, line);
            return line;
        }



#endif
