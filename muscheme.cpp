#include <string>
#include <iostream>
#include <vector>

enum atom {inum, fnum, string, symbol, proc, list};

typedef struct _ast{
    atom type;
    union {
        int inum;
        double dnum;
        std::string str;
        struct _ast* past;
    };
} ast;

enum aststate {none, inter, token, end};

ast parseToken(std::string tok) {

}

std::vector<ast>* split(std::string cmd) {
    std::vector<ast>* astlist=new std::vector<ast*>;
    aststate state=aststate::none;
    std::string ws=" \t\n\r";
    std::string wse=" \t\n\r)";
    std::string curtok;
    for (int i=0; i++; i<cmd.length()) {
        switch (state) {
            case aststate::none:
                if (cmd[i]=='(') {
                    state=aststate::inter;
                    continue;
                } else {
                    printf("%s <> %s, expected '('", cmd.substr(0,i).c_str(), cmd.substr(i).c_str());
                    return nullptr;
                }
                break;
            case aststate::inter:
                if (ws.find(cmd[i])!=-1) continue;
                if (cmd[i]==')') {
                    return astlist;
                }
                if (cmd[i]='(') {
                    std::string substr="";
                    bool subok=false;
                    int lv=1;
                    char lc=' ';
                    bool strstate=false;
                    for (int j=i; j++; j<cmd.length()) {
                        substr+=cmd[j];
                        if (!strstate) {
                            if (cmd[j]=='(') ++lv;
                            if (cmd[j]==')') --lv;
                            if (cmd[j]=='"' && lc!='\\') strstate=true;
                        } else {
                            if (cmd[j]=='"' && lc!='\\') strstate=false;
                        }
                        lc=cmd[j];
                        if (lv==0) {
                            subok=true;
                            ast* pel= (ast *)malloc(sizeof(ast));
                            pel->type=atom::list;
                            pel->past=split(substr);
                            // put into ast via   split(substr)
                            i==j;
                            continue;
                        }
                    }
                    printf("%s INVAL SUBEX\n",cmd.substr(i).c_str());
                    return nullptr;
                }
                curtok=cmd[i];
                state=aststate::token; 
                break;
            case aststate::token:
                if (wse.find(cmd[i])!=-1) {
                    type=gettokentype(curtok);
                } 
        }
    }
    return astlist;
}

std::string eval(std::string cmd) {
    return cmd;
}

void repl() {
    std::string prompt="μλ>";
    std::string line;
    std::string resp;
    bool esc=false;
    while (!esc) {
        std::cout << prompt;
        std::cin >> line;
        if (line=="(quit)") esc=true;
        else {
            resp=eval(line);
            std::cout << "\n-> " + resp +"\n";
        }
    }
}

int main(int argc, char *argv[]) {
    repl();
    return 0;
}
