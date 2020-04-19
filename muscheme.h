#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <map>

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
        astnode(const astnode& a) {
            type=a.type;
            size=a.size;
            val=malloc(size);
            memcpy(val,a.val,size);
            right=a.right;
            down=a.down;
        }
        astnode& operator=(const astnode& a) {
            if (this !=&a) {
                type=a.type;
                size=a.size;
                val=malloc(size);
                memcpy(val,a.val,size);
                right=a.right;
                down=a.down;
            }
            return *this;
        }
        astnode(string str) {
            type=atom::str;
            size=str.length()+1;
            val=malloc(size);
            strcpy((char *)val, str.c_str());
            std::cout << "create-str: " << str << std::endl;
            printf("ch: %s\n",(char *)val);
            right=nullptr;
            down=nullptr;
        }
        astnode(int i) {
            type=atom::inum;
            size=sizeof(int);
            val=malloc(size);
            *(int *)val=i;
            right=nullptr;
            down=nullptr;
        }
        astnode(double f) {
            type=atom::fnum;
            size=sizeof(double);
            val=malloc(size);
            *(double *)val=f;
            printf("create %f\n",*(double*)val);
            right=nullptr;
            down=nullptr;
        }
        string to_str() {
            switch (type) {
                case atom::list:
                case atom::lista:
                    return "[";
                case atom::liste:
                    return "]";
                case atom::str:
                    return "\""+string((char *)val)+"\"";
                case atom::cmt:
                    return ";"+string((char *)val);
                case atom::symbol:
                    return string((char *)val);
                case atom::inum:
                    return std::to_string((*(int *)val));
                case atom::fnum:
                    return std::to_string((*(double *)val));
                default:
                    return "???";
            }
        }
        ~astnode() {
            if (val != nullptr) free(val);
        }
    };

    std::map<string,std::vector<astnode*>> symstore;

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
            if (tok=="-") return atom::symbol;
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
        astnode *plastd=nullptr;
        astnode *plastr=nullptr;
        astnode *pastnode=nullptr;
        int ival;
        double fval;
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
                        if (plastd!=nullptr) plastd->down=pastnode;
                        if (plastr!=nullptr) plastr->right=pastnode;
                        stack.push_back(pastnode);
                        plastd=pastnode;
                        plastr=nullptr;
                        break;
                    case atom::liste:
                        if (stack.size()>0) {
                            plastr=stack.back();
                            plastd=nullptr;
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
                        ast.push_back(pastnode);
                        if (plastd!=nullptr) plastd->down=pastnode;
                        if (plastr!=nullptr) plastr->right=pastnode;
                        plastr=pastnode;
                        plastd=nullptr;
                        break;
                    case atom::fnum:
                        pastnode=new astnode();
                        pastnode->type=atom::fnum;
                        pastnode->size=sizeof(double);
                        pastnode->val=malloc(pastnode->size);
                        fval=atof(toks[i].c_str());
                        std::memcpy(pastnode->val,(const void *)&fval,sizeof(pastnode->size));
                        ast.push_back(pastnode);
                        if (plastd!=nullptr) plastd->down=pastnode;
                        if (plastr!=nullptr) plastr->right=pastnode;
                        plastr=pastnode;
                        plastd=nullptr;
                        break;
                    case atom::str:
                        pastnode=new astnode();
                        pastnode->type=atom::str;
                        pastnode->size=toks[i].length()-1;
                        if (pastnode->size<0 || toks[i][0]!='"' || toks[i][toks[i].length()-1]!='"') {
                            std::cout << "invalid string token format!" << std::endl;
                            delete pastnode;
                            break;
                        }
                        pastnode->val=malloc(pastnode->size);
                        strcpy((char *)pastnode->val,toks[i].substr(1,toks[i].length()-2).c_str());
                        ast.push_back(pastnode);
                        if (plastd!=nullptr) plastd->down=pastnode;
                        if (plastr!=nullptr) plastr->right=pastnode;
                        plastr=pastnode;
                        plastd=nullptr;
                        break; 
                    case atom::symbol:
                        pastnode=new astnode();
                        pastnode->type=atom::symbol;
                        pastnode->size=strlen(toks[i].c_str())+1;
                        pastnode->val=malloc(pastnode->size);
                        strcpy((char *)pastnode->val,toks[i].c_str());
                        ast.push_back(pastnode);
                        if (plastd!=nullptr) plastd->down=pastnode;
                        if (plastr!=nullptr) plastr->right=pastnode;
                        plastr=pastnode;
                        plastd=nullptr;
                        break;
                    default:
                        printf("Huch! %s\n",atomnames[tt].c_str());
                        val=false;
                        break;
                }
            }
            if (!val) {
                printf("Something isn't implemented yet, Can't run this.\n");
            } else {
                evalchecker(ast);
            }
        }
        
        return ast;
    }

    void printNode(astnode *past) {
        if (past==nullptr) {
            printf("(nul) ");
            return;
        }
        printf("%s %s ",atomnames[past->type].c_str(),past->to_str().c_str());
    }

    void preval(std::vector<astnode *> ast) {
        for (unsigned int i=0; i<ast.size(); i++) {
            printf("%lx ",(long)ast[i]);
            printNode(ast[i]);
            printf(" r:%lx d:%lx\n", (long)ast[i]->right, (long)ast[i]->down);
        }
    }

    void evalchecker(std::vector<astnode *> ast) {
        astnode * past;
        std::vector<astnode *> stack;
        past=ast[0];
        bool esc=false;
        preval(ast);
        while (!esc) {
            while (past==nullptr) {
                if (stack.size()>0) {
                    past=stack.back();
                    stack.pop_back();
                    printf("]<B> ");
                } else {
                    printf("<ye>\n");
                    return;
                }
            }
            //printf("%s ",atomnames[past->type].c_str());
            printNode(past);
            if (past->type==atom::list && past->down==nullptr) printf("MD! ");
            if (past->down!=nullptr && past->right!=nullptr) {
                printf("<rd> ");
                stack.push_back(past->right);
                past=past->down;
            } else {
                if (past->right!=nullptr) {
                    past=past->right;
                } else {
                    if (past->down!=nullptr) {
                        past=past->down;
                        stack.push_back(nullptr);
                        printf("<d> ");
                    } else {
                        if (stack.size()==0) {
                            esc=true;
                            printf("<xe>\n");
                        } else {
                            past=stack.back();
                            stack.pop_back();
                            printf("]<U> ");
                        }
                    }
                }
            }
        }
    }

    unsigned int astlen(astnode *past) {
        if (past==nullptr) return 0;
        unsigned int l=1;
        while (past->right != nullptr) {
            past=past->right;
            ++l;
        }
        return l;
    }

    astnode *astind(astnode *past, unsigned int ind) {
        unsigned int l=0;
        while (true) {
            if (ind==l) return past;
            if (past==nullptr) return nullptr;
            if (past->right==nullptr) return nullptr;
            past=past->right;
            ++l;
        }
    }
    
    astnode *receval(std::vector<astnode *> ast) {
        astnode *past;
        if (ast.size()<1) {
            std::cout << "ast too small, no data" << std::endl;
            return nullptr;
        }
        past=ast[0];
        return reval(past);
    }

    std::vector<astnode *>newexpr(astnode *past, std::vector<astnode *>ast) {
        astnode *p=new astnode(*past);
        while (true) {
            ast.push_back(p);
            if (p->type==atom::list) {
                std::cout << "deeper" << std::endl;
                if (p->down==nullptr) std::cout << "DOWN-null!?" << std::endl;
                ast=newexpr(p->down,ast);        
            
                if (p->right==nullptr) break;
                p=new astnode(*(p->right));
            } else {
                p->down=nullptr;
                p->right=nullptr;
                break;
            }
        }
        std::cout << "new ast vector of length: " << ast.size() << std::endl;
        evalchecker(ast);
        return ast;
    }
    
    astnode* reval(astnode *past) {
        astnode *inv=new astnode();
        astnode *res=new astnode();
        if (past==nullptr) {
            std::cout << "cannot eval nullptr" << std::endl;
            return inv;
        }
        if (past->type==atom::list) {
            return reval(past->down); // XXX!
        }
        unsigned int l=astlen(past);
        if (l<1) {
            std::cout << "reval list too short (<1)" << std::endl;
            return inv;
        }
        if (past->type!=atom::symbol) {
            if (past->type==atom::inum || past->type==atom::fnum || past->type==atom::str) return past;
            std::cout << "first param needs to be symbol: " << atomnames[past->type] << std::endl;
            return inv;
        }
        string cmd((char *)past->val);
        if (l==1) {
            if (symstore.count(cmd)) {
                astnode *p=receval(symstore[cmd]);
                //if (p->type==atom::list) p=reval(p);
                std::cout << "found: " << cmd << std::endl;
                return p; //symstore[cmd];
            } else {
                std::cout << "symbol " << past->to_str() << " not defined." << std::endl;
                return inv;
            }
        }
        if (cmd=="define") {
            std::cout << "define: l=" << l << std::endl;
            if (l!=3) return inv;
            astnode* pn=past->right;
            astnode* pv=pn->right;
            //if (pn->type==atom::list) {
            //    pn=reval(pn->down);
            //} else if (pn->type!=atom::symbol) return inv;
            //if (pv->type==atom::list) {
            //    pv=reval(pn->down);
            //}
            std::vector<astnode *>ast;
            symstore[pn->to_str()]=newexpr(pv,ast);
            return pv;
        } else if (cmd=="+" || cmd=="*" || cmd=="-" || cmd=="/") {
            if (l<3) {
                std::cout << "not enough + params" << std::endl;
                return inv;
            }
            int si=0;
            double sf=0.0;
            string ss="";
            for (int i=1; i<l; i++) {
                astnode* p=astind(past,i);
                if (p==nullptr) {
                    std::cout << "unexpected nullptr at + params" << std::endl;
                    return inv;
                }
                if (p->type==atom::list) p=reval(p);
                if (p->type==atom::symbol) {
                    if (p->type==atom::list) p=reval(p);
                    std::cout << "sym-eval "<<p->to_str() << " ->";
                    astnode *rp = new astnode(*p);
                    rp->down=nullptr;
                    rp->right=nullptr;
                    p=reval(rp);
                    std::cout << " " << p->to_str() << std::endl;
                }
                if (p->type==atom::inum) {
                    if (i==1) si=*(int *)p->val;
                    else {
                        if (cmd=="+") si+=*(int *)p->val;
                        else if (cmd=="-") si-=*(int *)p->val;
                        else if (cmd=="*") si = si * (*(int *)p->val);
                        else if (cmd=="/") si = si / (*(int *)p->val);
                    }
                }
            }
            res=new astnode(si);
            return res;
        }
        std::cout << " something is not implemented: " << past->to_str() << std::endl;
        return inv;    
    }
};

