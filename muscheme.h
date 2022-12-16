#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <map>

#include "munum.h"
namespace msc {

string fnSymbol = "⒡";

class Muscheme {
  public:
    enum atom { nul = 0,
                mnum,
                str,
                symbol,
                proc,
                list,
                lista,
                liste,
                cmt,
                qt };
    std::vector<std::string> atomnames{"nul", "mnum", "str", "symbol",
                                       "proc", "list", "lista", "liste",
                                       "cmt", "qt"};
    enum astval { valid,
                  invalid,
                  empty };

    typedef std::string string;
    // unsigned long NPOS=std::string::npos;

    struct astnode {
        atom type;
        void *val;
        int size;
        astnode *right;
        astnode *down;
        astnode() {
            type = atom::nul;
            val = nullptr;
            size = 0;
            right = nullptr;
            down = nullptr;
        }
        astnode(const astnode &a) {
            type = a.type;
            size = a.size;
            val = malloc(size);
            memcpy(val, a.val, size);
            right = a.right;
            down = a.down;
        }
        astnode &operator=(const astnode &a) {
            if (this != &a) {
                type = a.type;
                size = a.size;
                val = malloc(size);
                memcpy(val, a.val, size);
                right = a.right;
                down = a.down;
            }
            return *this;
        }
        astnode(string str) {
            type = atom::str;
            size = str.length() + 1;
            val = malloc(size);
            strcpy((char *)val, str.c_str());
            std::cout << "create-str: " << str << std::endl;
            printf("ch: %s\n", (char *)val);
            right = nullptr;
            down = nullptr;
        }
        astnode(const munum &n) {
            type = atom::mnum;
            val = new munum(n);
            size = sizeof(*(munum *)val);
            // printf("create %s\n", ((munum *)val)->str().c_str());
            right = nullptr;
            down = nullptr;
        }
        string to_str() {
            switch (type) {
            case atom::list:
            case atom::lista:
                return "[";
            case atom::liste:
                return "]";
            case atom::str:
                return "\"" + string((char *)val) + "\"";
            case atom::cmt:
                return ";" + string((char *)val);
            case atom::symbol:
                return string((char *)val);
            case atom::mnum:
                return ((munum *)val)->strH();
            default:
                return "???";
            }
        }
        ~astnode() {
            if (val != nullptr)
                free(val);
        }
    };

    std::map<string, std::vector<astnode *>> symstore;

    enum aststate { start,
                    inter,
                    token,
                    end };

    bool isstr(string tok) {  // handling for '/'?! TBD.
        if (tok.length() < 2)
            return false;
        if (tok[0] != '"' || tok[tok.length() - 1] != '"')
            return false;
        int si = 1;
        while (true) {
            unsigned long p = tok.substr(si, tok.length() - si - 1).find('"');
            if (p == NPOS)
                return true;
            if (tok[p + si - 1] != '\\')
                return false;
            si = p + si + 1;
        }
    }

    enum tokstate { nil,
                    sym,
                    vstr,
                    comm };
    enum parse_state { ok,
                       failure,
                       incomplete };

    std::vector<string> tokenize(string cmd, parse_state *pstate) {
        std::vector<string> toks;
        std::vector<string> empty;
        std::vector<int> qlev;
        bool qmode = false;
        char lc = 0;
        char c = 0;
        int brlev = 0;
        // bool islist=false;
        tokstate state = tokstate::nil;
        string subtok;
        string ws = " \t\n\r";
        string lbchars = "\n\r";
        string spechars = "();";
        string reschars = "\"'";
        for (unsigned int i = 0; i < cmd.length(); i++) {
            lc = c;
            char c = cmd[i];
            switch (state) {
            case tokstate::nil:
                subtok = "";
                if (ws.find(c) != NPOS)
                    continue;
                switch (c) {
                case '(':
                    brlev += 1;
                    // islist=true;
                    toks.push_back("(");
                    continue;
                case ')':
                    if (brlev == 0) {
                        printf("Too many ')' after: %s",
                               cmd.substr(0, i).c_str());
                        if (pstate)
                            *pstate = parse_state::failure;
                        return empty;
                    } else {
                        brlev -= 1;
                        if (qmode && qlev.back() == brlev) {
                            toks.push_back(")");
                            qlev.pop_back();
                            if (qlev.size() == 0)
                                qmode = false;
                        }
                        // if (lc==0) islist=false;
                        toks.push_back(")");
                    }
                    continue;
                case '"':
                    subtok = "\"";
                    state = tokstate::vstr;
                    continue;
                case '\'':
                    toks.push_back("(");
                    toks.push_back("quote");
                    qlev.push_back(brlev);
                    state = tokstate::nil;
                    qmode = true;
                    continue;
                case ';':
                    subtok = ";";
                    state = tokstate::comm;
                    continue;
                default:
                    subtok = c;
                    state = tokstate::sym;
                    continue;
                }
                break;
            case tokstate::vstr:
                subtok += c;
                if (c == '\"' && lc != '\\') {
                    toks.push_back(subtok);
                    state = tokstate::nil;
                    if (qmode && qlev.back() == brlev) {
                        toks.push_back(")");
                        qlev.pop_back();
                        if (qlev.size() == 0)
                            qmode = false;
                    }
                }
                continue;
            case tokstate::sym:
                if (reschars.find(c) != NPOS) {
                    printf("Unexpected character after: %s\n",
                           cmd.substr(0, i).c_str());
                    if (pstate)
                        *pstate = parse_state::failure;
                    return empty;
                }
                if (ws.find(c) != NPOS) {
                    toks.push_back(subtok);
                    state = tokstate::nil;
                    if (qmode && qlev.back() == brlev) {
                        toks.push_back(")");
                        qlev.pop_back();
                        if (qlev.size() == 0)
                            qmode = false;
                    }
                    continue;
                }
                if (spechars.find(c) != NPOS) {
                    toks.push_back(subtok);
                    --i;
                    state = tokstate::nil;
                    if (qmode && qlev.back() == brlev) {
                        toks.push_back(")");
                        qlev.pop_back();
                        if (qlev.size() == 0)
                            qmode = false;
                    }
                    continue;
                }
                subtok += c;
                continue;
            case tokstate::comm:
                if (lbchars.find(c) != NPOS) {
                    toks.push_back(subtok);
                    state = tokstate::nil;
                }
                subtok += c;
                continue;
            default:
                printf("Internal tokenizer error at: %s",
                       cmd.substr(0, i).c_str());
                if (pstate)
                    *pstate = parse_state::failure;
                return empty;
            }
        }
        if (brlev > 0) {
            if (state != tokstate::vstr)
                printf("Missing closing parentesis after: %s\n", cmd.c_str());
            else
                printf("Missing string-closing \" (and maybe: closing "
                       "parentesis missing) after: %s\n",
                       cmd.c_str());
            if (pstate)
                *pstate = parse_state::incomplete;
            return empty;
        }
        switch (state) {
        case tokstate::vstr:
            printf("String expression not closed after: %s\n", cmd.c_str());
            if (pstate)
                *pstate = parse_state::incomplete;
            return empty;
        case tokstate::comm:
        case tokstate::sym:
            toks.push_back(subtok);
            break;
        case tokstate::nil:
            break;
        default:
            printf("Invalid end-state %d\n", state);
            if (pstate)
                *pstate = parse_state::failure;
            return empty;
        }
        if (qlev.size() > 0) {
            for (unsigned int i = 0; i < qlev.size(); i++) {
                toks.push_back(")");
            }
        }
        if (pstate)
            *pstate = parse_state::ok;
        return toks;
    }

    atom getTokType(std::string tok) {
        std::string num = "-.0123456789";
        if (tok == ")") {
            return atom::liste;
        }
        if (tok == "(") {
            return atom::lista;
        }
        if (tok[0] == ';') {
            return atom::cmt;
        }
        if (tok[0] == '"') {
            if (isstr(tok))
                return atom::str;
            else {
                printf("Invalid string token: %s\n", tok.c_str());
                return atom::nul;
            }
        }
        if (num.find(tok[0]) != NPOS) {
            if (munum::isrational(tok))
                return atom::mnum;
            if (munum::isfloat(tok))
                return atom::mnum;
            if (tok == "-")
                return atom::symbol;
            printf("Invalid number token: %s\n", tok.c_str());
            return atom::nul;
        }
        if (tok[0] == '\'')
            return atom::qt;
        return atom::symbol;
    }

    std::vector<astnode *> parse(string cmd, parse_state *pstate) {
        std::vector<astnode *> ast;
        std::vector<astnode *> stack;
        parse_state tokstate;
        std::vector<string> toks = tokenize(cmd, &tokstate);
        astnode *plastd = nullptr;
        astnode *plastr = nullptr;
        astnode *pastnode = nullptr;
        int ival;
        double fval;
        bool val = true;

        if (tokstate != parse_state::ok) {
            if (pstate)
                *pstate = tokstate;
            return ast;
        }
        for (auto const &tok : toks) {
            int t = getTokType(tok);
            if (t == atom::nul)
                val = false;
            printf("[%s](%s) ", tok.c_str(), atomnames[t].c_str());
        }
        printf("\n");
        if (!val) {
            if (pstate)
                *pstate = parse_state::failure;
            printf("INVALID\n");
            return ast;
        } else {
            for (unsigned int i = 0; i < toks.size(); i++) {
                atom tt = getTokType(toks[i]);
                switch (tt) {
                case atom::lista:
                    pastnode = new astnode();
                    pastnode->type = atom::list;
                    ast.push_back(pastnode);
                    if (plastd != nullptr)
                        plastd->down = pastnode;
                    if (plastr != nullptr)
                        plastr->right = pastnode;
                    stack.push_back(pastnode);
                    plastd = pastnode;
                    plastr = nullptr;
                    break;
                case atom::liste:
                    if (stack.size() > 0) {
                        plastr = stack.back();
                        plastd = nullptr;
                        stack.pop_back();
                    } else {
                        printf("AST stack corruption!\n");
                    }
                    break;
                case atom::mnum:
                    pastnode = new astnode(munum(toks[i]));
                    ast.push_back(pastnode);
                    if (plastd != nullptr)
                        plastd->down = pastnode;
                    if (plastr != nullptr)
                        plastr->right = pastnode;
                    plastr = pastnode;
                    plastd = nullptr;
                    break;
                case atom::str:
                    pastnode = new astnode();
                    pastnode->type = atom::str;
                    pastnode->size = toks[i].length() - 1;
                    if (pastnode->size < 0 || toks[i][0] != '"' ||
                        toks[i][toks[i].length() - 1] != '"') {
                        std::cout << "invalid string token format!"
                                  << std::endl;
                        delete pastnode;
                        break;
                    }
                    pastnode->val = malloc(pastnode->size);
                    strcpy((char *)pastnode->val,
                           toks[i].substr(1, toks[i].length() - 2).c_str());
                    ast.push_back(pastnode);
                    if (plastd != nullptr)
                        plastd->down = pastnode;
                    if (plastr != nullptr)
                        plastr->right = pastnode;
                    plastr = pastnode;
                    plastd = nullptr;
                    break;
                case atom::symbol:
                    pastnode = new astnode();
                    pastnode->type = atom::symbol;
                    pastnode->size = strlen(toks[i].c_str()) + 1;
                    pastnode->val = malloc(pastnode->size);
                    strcpy((char *)pastnode->val, toks[i].c_str());
                    ast.push_back(pastnode);
                    if (plastd != nullptr)
                        plastd->down = pastnode;
                    if (plastr != nullptr)
                        plastr->right = pastnode;
                    plastr = pastnode;
                    plastd = nullptr;
                    break;
                default:
                    printf("Huch! %s\n", atomnames[tt].c_str());
                    val = false;
                    break;
                }
            }
            if (!val) {
                printf("Something isn't implemented yet, Can't run this.\n");
            } else {
                evalchecker(ast);
            }
        }
        if (pstate)
            *pstate = parse_state::ok;
        return ast;
    }

    void printNode(astnode *past) {
        if (past == nullptr) {
            printf("(nul) ");
            return;
        }
        printf("%s %s ", atomnames[past->type].c_str(), past->to_str().c_str());
    }

    void preval(std::vector<astnode *> ast) {
        for (unsigned int i = 0; i < ast.size(); i++) {
            printf("%lx ", (long)ast[i]);
            printNode(ast[i]);
            printf(" r:%lx d:%lx\n", (long)ast[i]->right, (long)ast[i]->down);
        }
    }

    void evalchecker(std::vector<astnode *> ast) {
        astnode *past;
        std::vector<astnode *> stack;
        past = ast[0];
        bool esc = false;
        preval(ast);
        while (!esc) {
            while (past == nullptr) {
                if (stack.size() > 0) {
                    past = stack.back();
                    stack.pop_back();
                    printf("]<B> ");
                } else {
                    printf("<ye>\n");
                    return;
                }
            }
            // printf("%s ",atomnames[past->type].c_str());
            printNode(past);
            if (past->type == atom::list && past->down == nullptr)
                printf("MD! ");
            if (past->down != nullptr && past->right != nullptr) {
                printf("<rd> ");
                stack.push_back(past->right);
                past = past->down;
            } else {
                if (past->right != nullptr) {
                    past = past->right;
                } else {
                    if (past->down != nullptr) {
                        past = past->down;
                        stack.push_back(nullptr);
                        printf("<d> ");
                    } else {
                        if (stack.size() == 0) {
                            esc = true;
                            printf("<xe>\n");
                        } else {
                            past = stack.back();
                            stack.pop_back();
                            printf("]<U> ");
                        }
                    }
                }
            }
        }
    }

    void printexpr(astnode *past) {
        std::vector<astnode *> stack;
        bool esc = false;
        // preval(ast);
        while (!esc) {
            while (past == nullptr) {
                if (stack.size() > 0) {
                    past = stack.back();
                    stack.pop_back();
                    printf("m)");
                } else {
                    // printf("e)\n");
                    return;
                }
            }
            // printf("%s ",atomnames[past->type].c_str());
            // printf("{%s %s}", atomnames[past->type].c_str(),
            //       past->to_str().c_str());
            printf("%s ", past->to_str().c_str());
            // if (past->type == atom::list &&
            // past->down == nullptr)
            //    printf("MD! ");
            if (past->down != nullptr && past->right != nullptr) {
                //    printf("<rd> ");
                stack.push_back(past->right);
                past = past->down;
            } else {
                if (past->right != nullptr) {
                    past = past->right;
                } else {
                    if (past->down != nullptr) {
                        past = past->down;
                        stack.push_back(nullptr);
                        printf("(");
                        // printf("<d> ");
                    } else {
                        if (stack.size() == 0) {
                            esc = true;
                            printf(")\n");
                        } else {
                            past = stack.back();
                            stack.pop_back();
                            printf("x)");  //]<U> ");
                        }
                    }
                }
            }
        }
    }

    unsigned int astlen(astnode *past) {
        if (past == nullptr)
            return 0;
        unsigned int l = 1;
        while (past->right != nullptr) {
            past = past->right;
            ++l;
        }
        return l;
    }

    astnode *astind(astnode *past, unsigned int ind) {
        unsigned int l = 0;
        while (true) {
            if (ind == l)
                return past;
            if (past == nullptr)
                return nullptr;
            if (past->right == nullptr)
                return nullptr;
            past = past->right;
            ++l;
        }
    }

    astnode *receval(std::vector<astnode *> ast) {
        astnode *past;
        if (ast.size() < 1) {
            std::cout << "ast too small, no data" << std::endl;
            return nullptr;
        }
        past = ast[0];
        astnode *p = reval(past);
        // if (p!=nullptr) p=new astnode(*p);
        return p;
    }

    std::vector<astnode *> newexpr(astnode *psrcast, astnode *plrast,
                                   astnode *pldast, bool isBase,
                                   std::vector<astnode *> &ast) {
        // std::vector<astnode *> ast(_ast);
        astnode *p = new astnode(*psrcast);
        if (pldast != nullptr)
            pldast->down = p;
        if (plrast != nullptr)
            plrast->right = p;
        while (true) {
            ast.push_back(p);
            if (p->type == atom::list) {
                // std::cout << "deeper" << std::endl;
                // if (p->down==nullptr) std::cout << "DOWN-null!?" <<
                // std::endl;
                std::cout << "cur ast vector of length: " << ast.size()
                          << std::endl;
                ast = newexpr(psrcast->down, nullptr, p, false, ast);
                if (psrcast->right == nullptr)
                    break;
                astnode *pn = new astnode(*(psrcast->right));
                psrcast = psrcast->right;
                p->right = pn;
                pn->down = nullptr;
                pn->right = nullptr;
                p = pn;
            } else {
                if (isBase) {
                    p->down = nullptr;
                    p->right = nullptr;
                    break;
                } else {
                    if (psrcast->right == nullptr)
                        break;
                    astnode *pn = new astnode(*(psrcast->right));
                    psrcast = psrcast->right;
                    p->right = pn;
                    pn->right = nullptr;
                    pn->down = nullptr;
                    p = pn;
                    continue;
                }
            }
        }
        std::cout << "new ast vector of length: " << ast.size() << std::endl;
        evalchecker(ast);
        return ast;
    }

    astnode *reduce(astnode *p, bool *alloced) {
        *alloced = false;
        astnode *inv;
        astnode *pn;
        if (p == nullptr) {
            std::cout << "unexpected nullptr at + params" << std::endl;
            inv = new astnode();
            *alloced = true;
            return inv;
        }
        if (p->type == atom::list) {
            pn = reval(p, false);
            //*alloced=true;
            return pn;
        }
        if (p->type == atom::symbol) {
            std::cout << "sym-eval " << p->to_str() << " ->";
            astnode *rp = new astnode(*p);
            rp->down = nullptr;
            rp->right = nullptr;
            pn = reval(rp);
            *alloced = true;
            delete rp;
            std::cout << " " << p->to_str() << std::endl;
            return pn;
        }
        return p;
    }

    astnode *reval(astnode *past, bool multi = true) {
        astnode *inv, *res;
        if (past == nullptr) {
            std::cout << "cannot eval nullptr" << std::endl;
            inv = new astnode();
            return inv;
        }
        if (past->type == atom::list) {
            if (past->right == nullptr || !multi) {
                return reval(past->down);
            } else {
                astnode *p = reval(past->down);
                if (p != nullptr) {
                    if (p->val != nullptr) {
                        free(p->val);
                        p->val = nullptr;
                    }
                    delete p;
                }
                return reval(past->right);
            }
        }
        unsigned int l = astlen(past);
        if (l < 1) {
            std::cout << "reval list too short (<1)" << std::endl;
            inv = new astnode();
            return inv;
        }
        if (past->type != atom::symbol) {
            if (past->type == atom::mnum || past->type == atom::str)
                return past;
            std::cout << "first param needs to be symbol: "
                      << atomnames[past->type] << std::endl;
            inv = new astnode();
            return inv;
        }
        string cmd((char *)past->val);
        if (l == 1) {
            if (symstore.count(cmd)) {
                astnode *p = receval(symstore[cmd]);
                std::cout << "found: " << cmd << std::endl;
                p = new astnode(*p);
                return p;
            } else {
                std::cout << "symbol " << past->to_str() << " not defined."
                          << std::endl;
                inv = new astnode();
                return inv;
            }
        }
        if (cmd == "defn") {
            std::cout << "function defn, l=" << l << std::endl;
            if (l != 4) {
                std::cout << "defn needs 3 params" << std::endl;
                inv = new astnode();
                return inv;
            }
            astnode *pfnn = past->right;
            astnode *ppar = pfnn->right;
            astnode *pdef = ppar->right;
            std::vector<astnode *> ast;
            if (pfnn->type != atom::symbol) {
                std::cout << "defn fnname vars fndef, fnname not a symbol: " << pfnn->to_str() << std::endl;
                inv = new astnode();
                return inv;
            }
            string fnname = fnSymbol + pfnn->to_str();
            if (symstore.count(fnname)) {
                // XXX This would be a place to prevent mutability.
                freesym(fnname);
            }
            std::cout << "Creating fn " << fnname << std::endl;
            symstore[fnname] = newexpr(ppar, nullptr, nullptr, true, ast);
            // std::cout << "defn isn't implemented yet" << std::endl;
            return nullptr;
        }
        if (cmd == "define") {
            std::cout << "define: l=" << l << std::endl;
            if (l != 3) {
                std::cout << "define needs 2 params" << std::endl;
                inv = new astnode();
                return inv;
            }
            astnode *pn = past->right;
            astnode *pv = pn->right;
            std::vector<astnode *> ast;
            if (pn->type != atom::symbol) {
                std::cout << "define symbol expr, not a symbol: " << pn->to_str() << std::endl;
                inv = new astnode();
                return inv;
            }
            if (symstore.count(pn->to_str())) {
                // XXX This would be the place to prevent mutability.
                freesym(pn->to_str());
            }
            symstore[pn->to_str()] = newexpr(pv, nullptr, nullptr, true, ast);
            return nullptr;
        } else if (cmd == "set") {
            if (l != 3) {
                std::cout << "set needs 2 params" << std::endl;
                inv = new astnode();
                return inv;
            }
            astnode *pn = past->right;
            if (pn->type != atom::symbol) {
                std::cout << "set symbol expr, not a symbol: " << pn->to_str() << std::endl;
                inv = new astnode();
                return inv;
            }
            astnode *pv = pn->right;
            std::vector<astnode *> ast;
            if (symstore.count(pn->to_str())) {
                // XXX This would be the place to prevent mutability.
                freesym(pn->to_str());
            }
            astnode *res = reval(pv, false);
            symstore[pn->to_str()] = newexpr(res, nullptr, nullptr, true, ast);
            return nullptr;
        } else if (cmd == "+" || cmd == "*" || cmd == "-" || cmd == "/" ||
                   cmd == "%" || cmd == "^") {
            if (l < 3) {
                std::cout << "not enough + params" << std::endl;
                inv = new astnode();
                return inv;
            }
            munum si(0), si2(0);
            // double sf=0.0;
            // string ss="";
            bool bF = false;
            for (unsigned int i = 1; i < l; i++) {
                // printf("i: %d\n", i);
                astnode *p = astind(past, i);
                if (p == nullptr) {
                    std::cout << "unexpected nullptr at + params" << std::endl;
                    inv = new astnode();
                    return inv;
                }
                p = reduce(p, &bF);
                if (p->type == atom::mnum) {
                    if (i == 1) {
                        si = *(munum *)p->val;
                        // printf("1. %s\n", si.str().c_str());
                    } else {
                        si2 = *(munum *)p->val;
                        // printf("ni. %s\n", si2.str().c_str());
                        if (cmd == "+")
                            si += si2;
                        else if (cmd == "-")
                            si -= si2;
                        else if (cmd == "*")
                            si *= si2;
                        else if (cmd == "/")
                            si /= si2;
                        else if (cmd == "%")
                            si %= si2;
                        else if (cmd == "^")
                            si ^= si2;
                    }
                }
                if (bF)
                    delete p;
            }
            res = new astnode(si);
            return res;
        } else if (cmd == "==" || cmd == "!=" || cmd == ">" || cmd == ">=" ||
                   cmd == "<" || cmd == "<=") {
            if (l != 3) {
                std::cout << "compare needs 2 params" << std::endl;
                inv = new astnode();
                return inv;
            }
            int si = 0;
            int nps = 0;
            // double sf=0.0;
            // string ss="";
            bool bF = false;
            munum pars[2];
            for (unsigned int i = 1; i < l; i++) {
                astnode *p = astind(past, i);
                std::cout << "prered: ";
                printNode(p);
                p = reduce(p, &bF);
                std::cout << ", postred: ";
                printNode(p);
                std::cout << std::endl;
                if (p->type == atom::mnum) {
                    pars[i - 1] = munum(*(munum *)p->val);
                    nps += 1;
                }
                if (i == 2) {
                    if (nps != 2) {
                        si = 0;
                        std::cout
                            << "invalid parameters in compare, nps: " << nps
                            << std::endl;
                    } else {
                        std::cout << "cond " << pars[0] << cmd << " "
                                  << pars[1];
                        if (cmd == "==")
                            si = (pars[0] == pars[1]);
                        else if (cmd == "!=")
                            si = (pars[0] != pars[1]);
                        else if (cmd == ">=")
                            si = (pars[0] >= pars[1]);
                        else if (cmd == "<=")
                            si = (pars[0] <= pars[1]);
                        else if (cmd == ">")
                            si = (pars[0] > pars[1]);
                        else if (cmd == "<")
                            si = (pars[0] < pars[1]);
                    }
                    std::cout << " => " << si << std::endl;
                }
                if (bF)
                    delete p;
            }
            res = new astnode(si);
            return res;
        } else if (cmd == "if") {
            if (l != 3 && l != 4) {
                std::cout << " if needs 2 or 3 params" << std::endl;
                inv = new astnode();
                return inv;
            }
            bool bF = false;
            astnode *p1 = astind(past, 1);
            astnode *p2 = astind(past, 2);
            astnode *p3 = nullptr;
            if (l == 4)
                p3 = astind(past, 3);
            astnode *cond = reduce(p1, &bF);
            if (cond == nullptr || cond->type != atom::mnum) {
                astnode *inv = new astnode();
                return inv;
            }
            astnode *res = nullptr;
            munum ifc = munum(*(munum *)cond->val);
            std::cout << "cond: " << ifc << std::endl;
            if (ifc.type == munum::mum_valid && ifc.nom != "0") {
                std::cout << "true" << std::endl;
                res = reval(p2, false);
            } else if (l == 4) {
                std::cout << "false" << std::endl;
                res = reval(p3, false);
            }
            return res;
        } else if (cmd == "eval") {
            if (l != 2) {
                std::cout << " eval needs 1 param" << std::endl;
                inv = new astnode();
                return inv;
            }
            bool bF = false;
            astnode *p1 = astind(past, 1);
            astnode *res0 = reduce(p1, &bF);
            astnode *res = new astnode(*res0);
            if (bF)
                delete res0;
            res->right = nullptr;
            res->down = nullptr;
            return res;
        } else if (cmd == "fac") {
            if (l != 2) {
                std::cout << " fac needs 1 param" << std::endl;
                inv = new astnode();
                return inv;
            }
            munum si;
            bool bF = false;
            astnode *p = astind(past, 1);
            if (p == nullptr) {
                std::cout << "unexpected nullptr at fac params" << std::endl;
                inv = new astnode();
                return inv;
            }
            p = reduce(p, &bF);
            if (p->type == atom::mnum) {
                si = *(munum *)p->val;
                si = munum::mufac(si);
            }
            if (bF)
                delete p;
            res = new astnode(si);
            return res;
        } else if (cmd == "float") {
            if (l != 2) {
                std::cout << " float needs 1 param" << std::endl;
                inv = new astnode();
                return inv;
            }
            munum si;
            bool bF = false;
            astnode *p = astind(past, 1);
            if (p == nullptr) {
                std::cout << "unexpected nullptr at float params" << std::endl;
                inv = new astnode();
                return inv;
            }
            p = reduce(p, &bF);
            if (p->type == atom::mnum) {
                si = *(munum *)p->val;
                double fi = (double)si;
                std::cout << fi << std::endl;
            }
            if (bF)
                delete p;
            res = new astnode(si);
            return res;
        } else if (cmd == "quote" || cmd == "car" || cmd == "cdr") {
            if (l != 2) {
                std::cout << cmd << " needs 1 param" << std::endl;
                inv = new astnode();
                return inv;
            }
            astnode *p = astind(past, 1);
            if (p == nullptr) {
                std::cout << "unexpected nullptr at float params" << std::endl;
                inv = new astnode();
                return inv;
            }
            if (cmd == "quote") {
                // res = new astnode(*p);
                // std::cout << astlen(res) << " is new list" << std::endl;
                return p;
            } else {
                std::cout << "NOT IMPLEMENTED! (car/cdr)" << std::endl;
            }

            // p = reduce(p, &bF);
            // if (bF)
            //    delete p;
            // res = new astnode(si);
            // return res;
        }
        string fnname = fnSymbol + cmd;
        if (symstore.count(fnname) > 0) {
            std::vector<astnode *> v = symstore[fnname];
            std::cout << "eval of " << fnname << " not yet implemented." << std::endl;
            return nullptr;
        }

        std::cout << " something is not implemented: " << past->to_str()
                  << std::endl;
        inv = new astnode();
        return inv;
    }

    void freesym(string sym, bool delentry = true) {
        if (!symstore.count(sym))
            return;
        for (auto b : symstore[sym]) {
            // astnode *p=b;
            if (b->val != nullptr) {
                if (b->type == atom::mnum) {
                    delete (munum *)(b->val);
                } else {
                    free(b->val);
                }
                b->val = nullptr;
            }
            delete b;
        }
        if (delentry)
            symstore.erase(symstore.find(sym));
    }

    void freesyms() {
        std::cout << "Freeing " << symstore.size() << " entries" << std::endl;
        for (auto a : symstore) {
            freesym(a.first, false);
        }
    }
};  // namespace msc

}  // namespace msc
