#include <string>
#include <iostream>
#include <vector>
#include <cassert>

enum atom {nul, inum, fnum, str, symbol, proc, list};
enum astval {valid, invalid, empty};

typedef std::string string;
auto NPOS=std::string::npos;

struct astnode {
    atom type;
    void *val;
    int size;
    astnode *right;
    astnode *down;
};

enum aststate {start, inter, token, end};

bool isnat(string tok) {
    if (tok.length()==0) return false;
    std::string isn="0123456789";
    for (int i=0; i<tok.length(); i++) {
       if (isn.find(tok[i])==NPOS) return false;
    }
   return true; 
}

bool isint(string tok) {
    if (tok.length()==0) return false;
    if (tok[0]=='-') return isnat(tok.substr(1));
    return isnat(tok);
}

bool issimpfloat(string tok) {
    if (tok.length()==0) return false;
    int p1=tok.find('.');
    if (p1!=NPOS) {
        return isint(tok.substr(0,p1)+tok.substr(p1+1));
    } else {
        return isint(tok);
    }
}
 
bool isfloat(string tok) {
    if (tok.length()==0) return false;
    if (isint(tok)) return true;
    int p1=tok.find('e');
    if (p1==NPOS) p1=tok.find('E');
    if (p1==NPOS) {
        return issimpfloat(tok);
    } else {
        return issimpfloat(tok.substr(0,p1)) && isint(tok.substr(p1+1));
    }
}

bool isstr(string tok) {  // handling for '/'?! TBD.
    if (tok.length()<2) return false;
    if (tok[0]!='"' || tok[tok.length()-1]!='"') return false;
    int si=1;
    while (true) {
        int p=tok.substr(si,tok.length()-si-1).find('"');
        if (p==NPOS) return true;
        if (tok[p+si-1]!='\\') return false;
        si=p+si+1;
    }
}

enum tokstate {nil, sym, vstr, quot, comm};

std::vector<string> tokenize(string cmd) {
    std::vector<string> toks;
    char lc=0;
    int brlev=0;
    bool islist=false;
    tokstate state=tokstate::nil;
    string subtok;
    for (int i=0; i<cmd.length(); i++) {
        char c=cmd[i];
        switch (state) {
            case tokstate::nil:
                subtok="";
                switch (c) {
                    case '(':
                        lc+=1;
                        islist=true;
                        toks.push_back("(");
                        break;
                    case ')':
                        if (lc==0) {
                            printf("Invalid ')': %s",cmd.substr(0,i).c_str()); 
                            std::vector<string> empty;
                            return empty;
                        } else {
                            lc-=1;
                            if (lc==0) islist=false;
                            toks.push_back(")");
                        }
                        continue;
                    case '"':
                        subtok="\"";
                        state=tokstate::vstr;
                        continue;
                    case '\'':
                        subtok="\'";
                        state=tokstate::quot;
                        break;
                    case ';':
                        subtok=";";
                        state=tokstate::comm;
                        break;
                    default:
                        subtok=c;
                        state=tokstate::sym;
                        break;
                }
                break;
            default:
                printf("Internal tokenizer error at: %s",cmd.substr(0,i).c_str());
                std::vector<string> empty;
                return empty;
        }
    }
    return toks;
}

bool parse(string cmd) {
    return false;
}

int testit() {
    int errs=0;
    std::vector<string> ints{"0","01","23424","02020202","-1","-0","-923432"};
    for(auto const& it: ints) {
        if (!isint(it)) {
            printf("Fail: %s is int!\n",it.c_str());
            ++errs;
        } else {
            printf("OK: %s is int.\n",it.c_str());
        }
    }
    std::vector<string> nints{"0e","0.1","23a424","","-",".","e"}; 
    for (auto const& it: nints) {
        if (isint(it)) {
            printf("Fail: %s is NOT an int!\n",it.c_str());
            ++errs;
        } else {
            printf("OK: %s is not an int.\n",it.c_str());
        }
    }
    std::vector<string> floats{"0","01","23424","02020202","-1","-0","-923432",".1","0.","-.1","-0.234","134.3233", ".0e4", "0e4","-1e3", "1E4", "1E-4", "-1.343e-32"};
    for(auto const& it: floats) {
        if (!isfloat(it)) {
            printf("Fail: %s is float!\n",it.c_str());
            ++errs;
        } else {
            printf("OK: %s is float.\n",it.c_str());
        }
    }
    std::vector<string> nfloats{"0e",".e10","-e23a424","","-",".","e","1.43.322","1e1e1","1e1.2"}; 
    for (auto const& it: nfloats) {
        if (isfloat(it)) {
            printf("Fail: %s is NOT a float!\n",it.c_str());
            ++errs;
        } else {
            printf("OK: %s is not a float.\n",it.c_str());
        }
    }
   std::vector<string> strs{"\"\"", "\"asdf\"","\"a\\\"a\"","\"\\\"a\\\"\"","\"a a b\""};
    for(auto const& it: strs) {
        if (!isstr(it)) {
            printf("Fail: %s is str!\n",it.c_str());
            ++errs;
        } else {
            printf("OK: %s is str.\n",it.c_str());
        }
    }
    std::vector<string> nstrs{"asdf","\"asfd\"asdf\"","\"asdf","asdf\"","","\"","\"\"\""};
    for (auto const& it: nstrs) {
        if (isstr(it)) {
            printf("Fail: %s is NOT a str!\n",it.c_str());
            ++errs;
        } else {
            printf("OK: %s is not a str.\n",it.c_str());
        }
    }
    return errs;
}

int main(int argc, char *argv[]) {
    int errs=testit();
    if (errs==0) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("%d tests failed!\n",errs);
        return -1;
    }
}
