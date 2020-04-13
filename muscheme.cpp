//#include <string>
//#include <cstring>
//#include <iostream>
//#include <vector>
//#include <cstdlib>
//
#include "muscheme.h"

void repl(std::string prompt="μλ> ", std::string prompt2="  > ") {
    std::string cmd,inp;
    Muscheme ms;
    bool fst;
    std::vector<Muscheme::astnode *> ast;
    while (true) {
        cmd="";
        fst=true;
        while (true) {
            if (fst) {
                fst=false;
                std::cout << prompt;
            } else {
                std::cout << prompt2;
            }
            getline(std::cin, inp);
            if (inp!="") {
                cmd += inp+"\n";
                continue;
            }
            break;
        }
        printf("Inp: %s\n",cmd.c_str());
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
