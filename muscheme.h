#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <cstdlib>

#include "munum.h"

class Muscheme {
  public:
    enum atom {nul=0, inum, fnum, str, symbol, proc, list, lista, liste, cmt, qt};
    std::vector<std::string> atomnames{"nul","inum","fnum","str","symbol","proc","list","lista","liste","cmt","qt"};
    enum astval {valid, invalid, empty};

    typedef std::string string;
    unsigned long NPOS=std::string::npos;

    struct astnode {
        atom type;
        void *val;
        int size;
        astnode *right;
        astnode *down;
        astnode() {
            type=atom::nul;
            val=nullptr;
            size=0;
            right=nullptr;
            down=nullptr;
        }
        ~astnode() {
            if (val != nullptr) free(val);
        }
    };

    enum aststate {start, inter, token, end};

    bool isnat(string tok) {
        if (tok.length()==0) return false;
        std::string isn="0123456789";
        for (unsigned int i=0; i<tok.length(); i++) {
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
        unsigned long p1=tok.find('.');
        if (p1!=NPOS) {
            return isint(tok.substr(0,p1)+tok.substr(p1+1));
        } else {
            return isint(tok);
        }
    }
     
    bool isfloat(string tok) {
        if (tok.length()==0) return false;
        if (isint(tok)) return true;
        unsigned long p1=tok.find('e');
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
            unsigned long p=tok.substr(si,tok.length()-si-1).find('"');
            if (p==NPOS) return true;
            if (tok[p+si-1]!='\\') return false;
            si=p+si+1;
        }
    }

    enum tokstate {nil, sym, vstr, comm};

    std::vector<string> tokenize(string cmd) {
        std::vector<string> toks;
        std::vector<string> empty;
        std::vector<int> qlev;
        bool qmode=false;
        char lc=0;
        char c=0;
        int brlev=0;
        // bool islist=false;
        tokstate state=tokstate::nil;
        string subtok;
        string ws=" \t\n\r";
        string lbchars="\n\r";
        string spechars="();";
        string reschars="\"'";
        for (unsigned int i=0; i<cmd.length(); i++) {
            lc=c;
            char c=cmd[i];
            switch (state) {
                case tokstate::nil:
                    subtok="";
                    if (ws.find(c)!=NPOS) continue;
                    switch (c) {
                        case '(':
                            brlev+=1;
                            // islist=true;
                            toks.push_back("(");
                            continue;
                        case ')':
                            if (brlev==0) {
                                printf("Too many ')' after: %s",cmd.substr(0,i).c_str()); 
                                return empty;
                            } else {
                                brlev-=1;
                                if (qmode && qlev.back()==brlev) {
                                    toks.push_back(")");
                                    qlev.pop_back();
                                    if (qlev.size()==0) qmode=false;
                                }
                                // if (lc==0) islist=false;
                                toks.push_back(")");
                            }
                            continue;
                        case '"':
                            subtok="\"";
                            state=tokstate::vstr;
                            continue;
                        case '\'':
                            toks.push_back("(");
                            toks.push_back("quote");
                            qlev.push_back(brlev);
                            state=tokstate::nil;
                            qmode=true;
                            continue;
                        case ';':
                            subtok=";";
                            state=tokstate::comm;
                            continue;
                        default:
                            subtok=c;
                            state=tokstate::sym;
                            continue;
                    }
                    break;
                case tokstate::vstr:
                    subtok+=c;
                    if (c=='\"' && lc!='\\') {
                        toks.push_back(subtok);
                        state=tokstate::nil;
                        if (qmode && qlev.back()==brlev) {
                            toks.push_back(")");
                            qlev.pop_back();
                            if (qlev.size()==0) qmode=false;
                        } 
                    }
                    continue;
                case tokstate::sym:
                    if (reschars.find(c)!=NPOS) {
                        printf("Unexpected character after: %s\n",cmd.substr(0,i).c_str());
                        return empty;
                    }
                    if (ws.find(c)!=NPOS) {
                        toks.push_back(subtok);
                        state=tokstate::nil;
                        if (qmode && qlev.back()==brlev) {
                            toks.push_back(")");
                            qlev.pop_back();
                            if (qlev.size()==0) qmode=false;
                        }
                        continue;
                    }
                    if (spechars.find(c)!=NPOS) {
                        toks.push_back(subtok);
                        --i;
                        state=tokstate::nil;
                        if (qmode && qlev.back()==brlev) {
                            toks.push_back(")");
                            qlev.pop_back();
                            if (qlev.size()==0) qmode=false;
                        }
                        continue;
                    }
                    subtok+=c;
                    continue;
                case tokstate::comm:
                    if (lbchars.find(c)!=NPOS) {
                        toks.push_back(subtok);
                        state=tokstate::nil;
                    }
                    subtok+=c;
                    continue;
                default:
                    printf("Internal tokenizer error at: %s",cmd.substr(0,i).c_str());
                    return empty;
            }
        }
        if (brlev>0) {
            if (state!=tokstate::vstr)printf("Missing closing parentesis after: %s\n", cmd.c_str());
            else printf("Missing string-closing \" (and maybe: closing parentesis missing) after: %s\n", cmd.c_str());
            return empty;
        }
        switch (state) {
            case tokstate::vstr:
                printf("String expression not closed after: %s\n", cmd.c_str());
                return empty;
            case tokstate::comm:
            case tokstate::sym:
                toks.push_back(subtok);
                break;
            case tokstate::nil:
                break;
            default:
                printf("Invalid end-state %d\n",state);
                return empty;
        }
        if (qlev.size()>0) {
            for (unsigned int i=0; i<qlev.size(); i++) {
                toks.push_back(")");
            }
        }
        return toks;
    }

    atom getTokType(std::string tok) {
        std::string num="-.0123456789";
        if (tok==")") {
            return atom::liste;
        }
        if (tok=="(") {
            return atom::lista;
        }
        if (tok[0]==';') {
            return atom::cmt;
        }
        if (tok[0]=='"') {
            if (isstr(tok)) return atom::str;
            else {
                printf("Invalid string token: %s\n",tok.c_str());
                return atom::nul;
            }
        }
        if (num.find(tok[0])!=NPOS) {
            if (isint(tok)) return atom::inum;
            if (isfloat(tok)) return atom::fnum;
            printf("Invalid number token: %s\n",tok.c_str());
            return atom::nul;
        }
        if (tok[0]=='\'') return atom::qt;
        return atom::symbol;
    }

    std::vector<astnode *> parse(string cmd) {
        std::vector<astnode *> ast;
        std::vector<astnode *> stack;
        std::vector<string> toks=tokenize(cmd);
        astnode *plast=nullptr;
        astnode *pastnode=nullptr;
        int ival;
        bool val=true;
        for (auto const& tok: toks) {
            int t=getTokType(tok);
            if (t==atom::nul) val=false;
            printf("[%s](%s) ",tok.c_str(),atomnames[t].c_str());
        }
        printf("\n");
        if (!val) {
            printf("INVALID\n");
            return ast;
        } else {
            for (unsigned int i=0; i<toks.size(); i++) {
                atom tt=getTokType(toks[i]);
                switch (tt) {
                    case atom::lista:
                        pastnode=new astnode();
                        pastnode->type=atom::list;
                        ast.push_back(pastnode);
                        if (plast!=nullptr) {
                            plast->down=pastnode;
                        }
                        stack.push_back(plast);
                        plast=pastnode;
                        break;
                    case atom::liste:
                        if (stack.size()>0) {
                            plast=stack.back();
                            stack.pop_back();
                        } else {
                            printf("AST stack corruption!\n");
                        }
                        break;
                    case atom::inum:
                        pastnode=new astnode();
                        pastnode->type=atom::inum;
                        pastnode->size=sizeof(int);
                        pastnode->val=malloc(pastnode->size);
                        ival=atoi(toks[i].c_str());
                        std::memcpy(pastnode->val,(const void *)&ival,sizeof(pastnode->size));
                        break;
                    default:
                        printf("Huch! %s\n",atomnames[tt].c_str());
                        break;
                }
            }
        }
        
        return ast;
    }
};

