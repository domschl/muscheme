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
    bool multiLine=false;

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
                if (multiLine) continue;
            }
            break;
        }
        printf("Inp: %s\n",cmd.c_str());
        if (cmd=="(quit)") return;
        ast=ms.parse(cmd);
        string ans=ms.receval(ast)->to_str();
        printf("-> %s\n",ans.c_str());
        for (auto ap : ast) {
            delete ap;
        }
    }
}

int main(int argc, char *argv[]) {
    repl();
    return 0;
}
