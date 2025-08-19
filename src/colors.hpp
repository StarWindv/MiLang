#ifndef COLOR_HPP
    #define COLOR_HPP

    constexpr const char* RED    = "\x1b[31m";
    constexpr const char* GREEN  = "\x1b[32m";
    constexpr const char* GOLD   = "\x1b[33m";
    constexpr const char* BLUE   = "\x1b[34m";
    constexpr const char* PURPLE = "\x1b[35m";
    constexpr const char* CYAN   = "\x1b[36m";
    constexpr const char* BLACK  = "\x1b[30m";
    constexpr const char* RESET  = "\x1b[39m";


    class Color {
        public:
            string RED    = RED;
            string GREEN  = GREEN;
            string GOLD   = GOLD;
            string BLUE   = BLUE;
            string PURPLE = PURPLE;
            string CYAN   = CYAN;
            string BLACK  = BLACK;
            string RESET  = RESET;
    };

#endif