#include <string>
#include <iostream>
#include <vector>

enum atom {nul, inum, fnum, string, symbol, proc, list};
enum astval {valid, invalid, empty};

struct ast;
typedef std::vector<ast *> astlist;

struct ast {
    atom type;
    astval state;
    union {
        int inum;
        double fnum;
        std::string str;
        astlist vlist;
    };
    ast() {
        type=atom::nul;
        state=astval::empty;
    }
    ~ast() {

    }
};

enum aststate {start, inter, token, end};

ast* parseToken(std::string tok) {
    ast *past=new ast();
    std::string iss="-0123456789";
    std::string isn="0123456789";
    std::string fss="-.0123456789";
    if (tok.find("\"")!=std::string::npos) { // check string
        if (tok[0]!='"' || tok[tok.length()-1]!='"') {
            printf("Invalid string: %s",tok.c_str());
            free(past);
            return nullptr;
        } 
        // more esc checks
        past->type=atom::string;
        past->state=astval::valid;
        past->str=std::string(tok.substr(1,tok.length()-2));
        return past;
    } else if (fss.find(tok[0]) == std::string::npos) { // check symbol
        past->type=atom::symbol;
        past->state=astval::valid;  // XXX checks
        past->str=std::string(tok);
        return past;
    } else if (tok.find('.') != std::string::npos) { // check float
        past->type=atom::fnum;
        past->state=astval::valid;  // XXX checks
        past->fnum=atof(tok.c_str());
        return past;
    } else { // check int
        past->type=atom::inum;
        past->state=astval::valid;  // XXX checks
        past->inum=atoi(tok.c_str());
        return past;
    }
}

astlist split(std::string cmd) {
    astlist vlist;
    astlist empty;
    aststate state=aststate::start;
    std::string ws=" \t\n\r";
    std::string wse=" \t\n\r)(";
    std::string curtok;
    for (int i=0; i<cmd.length(); i++) {
        printf("%c",cmd[i]);
        switch (state) {
            case aststate::token:
                printf("<t>");
                if (wse.find(cmd[i])!=std::string::npos) {
                    ast *pel=parseToken(curtok);
                    if (pel && pel->state==astval::valid) {
                        vlist.push_back(pel);
                    }
                    printf("->i>");
                    state=aststate::inter; 
                }
                break;
            case aststate::inter:
                printf("<i>");
                if (ws.find(cmd[i])!=std::string::npos) break;
                if (cmd[i]==')') {
                    return vlist;
                }
                if (cmd[i]=='(') {
                    std::string substr="";
                    bool subok=false;
                    int lv=0;
                    char lc=' ';
                    bool strstate=false;
                    for (int j=i; j<cmd.length(); j++) {
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
                            ast *pel=new ast();
                            pel->type=atom::list;
                            pel->vlist=split(substr);
                            if ((pel->vlist).size()>0) {
                                pel->state=astval::valid;
                                vlist.push_back(pel);
                            } else {
                                printf("Parsing failed with sub-expression: %s", substr.c_str());
                                free(pel);
                                return empty;
                            }
                            i==j;
                            printf("->i>");
                            state=aststate::inter; 
                            break;
                        }
                    }
                    printf("%s INVAL SUBEX\n",cmd.substr(i).c_str());
                    return empty;
                }
                curtok=cmd[i];
                printf("->t>");
                state=aststate::token; 
                break;
            case aststate::start:
                printf("<s>");
                if (cmd[i]=='(') {
                    printf("->i>");
                    state=aststate::inter;
                } else {
                    printf("%s <> %s, expected '('", cmd.substr(0,i).c_str(), cmd.substr(i).c_str());
                    return empty;
                }
                break;
        }
    }
    return vlist;
}

void printAstlist(astlist al) {
    for (int i=0; i<al.size(); i++ ) {
        if (al[i]->state==astval::invalid) {
            printf("INV ");
        } else if (al[i]->state==astval::empty) {
            printf("EMPTY ");
        } else {
            switch (al[i]->type) {
                case atom::fnum:
                    printf("fnum %f ",al[i]->fnum);
                    break;
                case atom::inum:
                    printf("inum %d ",al[i]->inum);
                    break;
                case atom::symbol:
                    printf("sym %s",al[i]->str);
                    break;
                case atom::string:
                    printf("string %s ",al[i]->str);
                    break;
                case atom::list:
                    printf(" ( ");
                    printAstlist(al[i]->vlist);
                    printf(" ) ");
                    break;
                default:
                    printf("TYPEERR %d ",al[i]->type);
                    break;
            }
        }
    }
}

std::string eval(std::string cmd) {
    astlist vlist=split(cmd);
    printAstlist(vlist);
    printf("\n");
    return "";
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
            // std::cout << "\n-> " + resp +"\n";
        }
    }
}

int main(int argc, char *argv[]) {
    repl();
    return 0;
}
