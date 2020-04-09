//#include <string>
//#include <cstring>
//#include <iostream>
//#include <vector>
//#include <cstdlib>
//
#include "muscheme.h"

void repl(std::string prompt="μλ> ") {
    std::string cmd;
    Muscheme ms;
    std::vector<Muscheme::astnode *> ast;
    while (true) {
        std::cout << prompt;
        getline(std::cin, cmd);
        printf("%s\n",cmd.c_str());
        if (cmd=="(quit)") return;
        ast=ms.parse(cmd);
        for (auto ap : ast) {
            delete ap;
        }
    }
}

int main(int argc, char *argv[]) {
    repl();
    return 0;
}
