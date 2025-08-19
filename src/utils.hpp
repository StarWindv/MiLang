#ifndef UTILS_HPP
    #define UTILS_HPP

        #include <fstream>
        #include <string>
        #include <iostream>
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



#endif
