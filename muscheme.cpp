//#include <string>
//#include <cstring>
//#include <iostream>
//#include <vector>
//#include <cstdlib>
//
#include "muscheme.h"

#include <iostream>     // cout, cin, streambuf, hex, endl, sgetc, sbumpc
#include <iomanip>      // setw, setfill
#include <fstream>      // fstream

// These inclusions required to set terminal mode.
#include <termios.h>    // struct termios, tcgetattr(), tcsetattr()
#include <stdio.h>      // perror(), stderr, stdin, fileno()


using msc::munum;
using msc::Muscheme;
using std::string;


bool initCharReader(struct termios *pTermSaved) {
    struct termios t;
    // Set terminal to single character mode.
    tcgetattr(fileno(stdin), pTermSaved);

    t.c_lflag &= (~ICANON & ~ECHO);
    t.c_cc[VTIME] = 0;
    t.c_cc[VMIN] = 1;
    if (tcsetattr(fileno(stdin), TCSANOW, &t) < 0) {
        std::cout << "Unable to set terminal to single character mode" << std::endl;
        return false;
    }
    return true;
}

bool quitCharReader(struct termios *pTermSaved) {

    // Restore terminal mode.
    if (tcsetattr(fileno(stdin), TCSANOW, pTermSaved) < 0) {
        std::cout << "Unable to restore terminal mode" << std::endl;
        return false;
    }
    return true;
}

string charReader(string prompt, bool *pQuit)
{
    *pQuit=false;
    struct termios termSaved;
    initCharReader(&termSaved);
    // Read single characters from cin.
    std::streambuf *pbuf = std::cin.rdbuf();
    bool done = false;
    string inp = "";
    std::cout << prompt;
    
    while (!done) {
        char c;
        if (pbuf->sgetc() == EOF) done = true;
        c = pbuf->sbumpc();
        switch (c) {
            case 0x0a:
            case 0x0d:
                done=true;
                break;
            case 0x03:
            case 0x04:
                done=true;
                *pQuit=true;
                break;
            case 0x7f:
            case 0x08:
                if (inp.length()>0) {
                    inp=inp.substr(0,inp.length()-1);
                    std::cout << "\r"+prompt+inp+" " << std::flush;
                    std::cout << "\r"+prompt+inp << std::flush;
                }
                break;
            default:
                inp+=c;
                if (c<32) {
                    std::cout << "[0x" << std::setw(2) << std::setfill('0') << std::hex << int(c) << "]" << std::flush;
                } else {
                //  std::cout << "[0x" << std::setw(2) << std::setfill('0') << std::hex << int(c) << "]" << std::flush;
                std::cout << c << std::flush;
                }
                break;
        }
    }
    quitCharReader(&termSaved);
    return inp;
}

void quitInterpreter(Muscheme &ms) {
    printf("Quitting...\n");
    ms.freesyms();
    std::cout << "done freesyms" << std::endl;
}

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
            /*
            if (fst) {
                fst = false;
                std::cout << prompt;
            } else {
                std::cout << prompt2;
            }
            
            if (!getline(std::cin, inp)) {
                quitInterpreter(ms);
                return;
            };
            */
            bool bq=false;
            inp=charReader(prompt, &bq);
            /*
            bool il=true;
            inp="";
            char c;
            while (il) {
                std::cin >> c;
                printf("[%d]",(int)c);
                if (c!=13) {
                    inp+=c;
                } else {
                    il=false;
                }
            }
            if (inp == "" && !bq) {
                cmd = "";
                fst = true;
                continue;
            } else {
            */
            cmd += inp + "\n";
            printf("Inp: %s", cmd.c_str());
            if (bq || cmd == "(quit)\n") {
                quitInterpreter(ms);
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
