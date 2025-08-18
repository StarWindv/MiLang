#ifndef COLOR_HPP
    #define COLOR_HPP

    constexpr string RED    = "\x1b[31m";
    constexpr string GREEN  = "\x1b[32m";
    constexpr string GOLD   = "\x1b[33m";
    constexpr string BLUE   = "\x1b[34m";
    constexpr string PURPLE = "\x1b[35m";
    constexpr string CYAN   = "\x1b[36m";
    constexpr string BLACK  = "\x1b[30m";
    constexpr string RESET  = "\x1b[39m";

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