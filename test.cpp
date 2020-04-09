#include "muscheme.h"

typedef std::string string;

int testit(Muscheme &ms) {
    int errs=0;
    std::vector<string> ints{"0","01","23424","02020202","-1","-0","-923432"};
    for(auto const& it: ints) {
        if (!ms.isint(it)) {
            printf("Fail: %s is int!\n",it.c_str());
            ++errs;
        } else {
            printf("OK: %s is int.\n",it.c_str());
        }
    }
    std::vector<string> nints{"0e","0.1","23a424","","-",".","e"}; 
    for (auto const& it: nints) {
        if (ms.isint(it)) {
            printf("Fail: %s is NOT an int!\n",it.c_str());
            ++errs;
        } else {
            printf("OK: %s is not an int.\n",it.c_str());
        }
    }
    std::vector<string> floats{"0","01","23424","02020202","-1","-0","-923432",".1","0.","-.1","-0.234","134.3233", ".0e4", "0e4","-1e3", "1E4", "1E-4", "-1.343e-32"};
    for(auto const& it: floats) {
        if (!ms.isfloat(it)) {
            printf("Fail: %s is float!\n",it.c_str());
            ++errs;
        } else {
            printf("OK: %s is float.\n",it.c_str());
        }
    }
    std::vector<string> nfloats{"0e",".e10","-e23a424","","-",".","e","1.43.322","1e1e1","1e1.2"}; 
    for (auto const& it: nfloats) {
        if (ms.isfloat(it)) {
            printf("Fail: %s is NOT a float!\n",it.c_str());
            ++errs;
        } else {
            printf("OK: %s is not a float.\n",it.c_str());
        }
    }
   std::vector<string> strs{"\"\"", "\"asdf\"","\"a\\\"a\"","\"\\\"a\\\"\"","\"a a b\""};
    for(auto const& it: strs) {
        if (!ms.isstr(it)) {
            printf("Fail: %s is str!\n",it.c_str());
            ++errs;
        } else {
            printf("OK: %s is str.\n",it.c_str());
        }
    }
    std::vector<string> nstrs{"asdf","\"asfd\"asdf\"","\"asdf","asdf\"","","\"","\"\"\""};
    for (auto const& it: nstrs) {
        if (ms.isstr(it)) {
            printf("Fail: %s is NOT a str!\n",it.c_str());
            ++errs;
        } else {
            printf("OK: %s is not a str.\n",it.c_str());
        }
    }
    return errs;
}

int testnum(Muscheme &ms) {
    int errs=0;
    int i1,i2;
    munum a,b,c;
    for (int i=0; i<1000000; i++) {
        i1=rand()/2;
        i2=rand()/2;
        a=munum(i1);
        b=munum(i2);
        c=muipadd(a,b);
        if (i1+i2!=atoi(c.str().c_str())) {
             printf("Error: %d+%d=%d, not %s %s\n",i1,i2,i1+i2,c.str().c_str(),c.nom.c_str());
             errs+=1;
        } else {
             printf("OK: %d+%d=%d, %s\n",i1,i2,i1+i2,c.str().c_str());
        }
        c=muipsub(a,b);
        if (i1-i2!=atoi(c.str().c_str())) {
             printf("Error: %d-%d=%d, not %s %s\n",i1,i2,i1-i2,c.str().c_str(),c.nom.c_str());
             errs+=1;
        } else {
             printf("OK: %d-%d=%d, %s\n",i1,i2,i1-i2,c.str().c_str());
        }
    }
    return errs;
}

int main(int argc, char *argv[]) {
    Muscheme ms;
    int errs=testit(ms);
    errs+=testnum(ms);
    if (errs==0) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("%d tests failed!\n",errs);
       return -1;
    }
}
