//#include <string>
//#include <cstring>
//#include <iostream>
//#include <vector>
//#include <cstdlib>
//
#include "muscheme.h"

using msc::munum;
using msc::Muscheme;
using std::string;

void repl(std::string prompt = "μλ> ", std::string prompt2 = "  > ") {
    std::string cmd, inp;
    Muscheme ms;
    bool fst;
    std::vector<Muscheme::astnode *> ast;
    string ans;
    Muscheme::parse_state state = Muscheme::parse_state::failure;

    while (true) {
        cmd = "";
        fst = true;
        while (true) {
            if (fst) {
                fst = false;
                std::cout << prompt;
            } else {
                std::cout << prompt2;
            }
            getline(std::cin, inp);
            if (inp == "") {
                cmd = "";
                fst = true;
                continue;
            } else {
                cmd += inp + "\n";
            }
            printf("Inp: %s", cmd.c_str());
            if (cmd == "(quit)\n") {
                ms.freesyms();
                std::cout << "done freesyms" << std::endl;
                return;
            }
            ast = ms.parse(cmd, &state);
            if (state == Muscheme::parse_state::incomplete)
                continue;
            break;
        }
        if (state == Muscheme::parse_state::failure) {
            printf("Err\n");
            continue;
        }
        Muscheme::astnode *past = ms.receval(ast);
        if (past != nullptr) {
            ans = past->to_str();
            if (past->val != nullptr) {
                free(past->val);
                past->val = nullptr;
            }
            delete past;
        } else
            ans = "ok.";
        printf("-> %s\n", ans.c_str());
        for (auto ap : ast) {
            if (past != ap) {
                if (ap->val != nullptr)
                    free(ap->val);
                ap->val = nullptr;
                delete ap;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    repl();
    std::cout << "end-repl" << std::endl;
    return 0;
}
