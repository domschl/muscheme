#include "muscheme.h"

typedef std::string string;

int testit(Muscheme &ms, bool verbose=false) {
    int errs=0;
    std::vector<string> ints{"0","01","23424","02020202","-1","-0","-923432"};
    for(auto const& it: ints) {
        if (!ms.isint(it)) {
            printf("Fail: %s is int!\n",it.c_str());
            ++errs;
        } else {
            if (verbose) printf("OK: %s is int.\n",it.c_str());
        }
    }
    std::vector<string> nints{"0e","0.1","23a424","","-",".","e"}; 
    for (auto const& it: nints) {
        if (ms.isint(it)) {
            printf("Fail: %s is NOT an int!\n",it.c_str());
            ++errs;
        } else {
            if (verbose) printf("OK: %s is not an int.\n",it.c_str());
        }
    }
    std::vector<string> floats{"0","01","23424","02020202","-1","-0","-923432",".1","0.","-.1","-0.234","134.3233", ".0e4", "0e4","-1e3", "1E4", "1E-4", "-1.343e-32"};
    for(auto const& it: floats) {
        if (!ms.isfloat(it)) {
            printf("Fail: %s is float!\n",it.c_str());
            ++errs;
        } else {
            if (verbose) printf("OK: %s is float.\n",it.c_str());
        }
    }
    std::vector<string> nfloats{"0e",".e10","-e23a424","","-",".","e","1.43.322","1e1e1","1e1.2"}; 
    for (auto const& it: nfloats) {
        if (ms.isfloat(it)) {
            printf("Fail: %s is NOT a float!\n",it.c_str());
            ++errs;
        } else {
            if (verbose) printf("OK: %s is not a float.\n",it.c_str());
        }
    }
   std::vector<string> strs{"\"\"", "\"asdf\"","\"a\\\"a\"","\"\\\"a\\\"\"","\"a a b\""};
    for(auto const& it: strs) {
        if (!ms.isstr(it)) {
            printf("Fail: %s is str!\n",it.c_str());
            ++errs;
        } else {
            if (verbose) printf("OK: %s is str.\n",it.c_str());
        }
    }
    std::vector<string> nstrs{"asdf","\"asfd\"asdf\"","\"asdf","asdf\"","","\"","\"\"\""};
    for (auto const& it: nstrs) {
        if (ms.isstr(it)) {
            printf("Fail: %s is NOT a str!\n",it.c_str());
            ++errs;
        } else {
            if (verbose) printf("OK: %s is not a str.\n",it.c_str());
        }
    }
    return errs;
}

int testnum(Muscheme &ms, int count=1000, bool verbose=false) {
    int errs=0;
    int i1,i2;
    munum a,b,c;
    for (int i=0; i<count; i++) {
        i1=rand()/2;
        if (rand()%2) i1=(-1)*i1;
        i2=rand()/2;
        if (rand()%2) i2=(-1)*i2;
        a=munum(i1);
        b=munum(i2);
        c=a.muadd(a,b);
        if (i1+i2!=atoi(c.str().c_str())) {
             printf("Error: %d + %d = %d, not %s %s\n",i1,i2,i1+i2,c.str().c_str(),c.nom.c_str());
             errs+=1;
        } else {
            if (verbose) printf("OK: %d + %d = %d, %s\n",i1,i2,i1+i2,c.str().c_str());
        }
        c=a.musub(a,b);
        if (i1-i2!=atoi(c.str().c_str())) {
             printf("Error: %d - %d = %d, not %s %s\n",i1,i2,i1-i2,c.str().c_str(),c.nom.c_str());
             errs+=1;
        } else {
            if (verbose) printf("OK: %d - %d = %d, %s\n",i1,i2,i1-i2,c.str().c_str());
        }
    }
    return errs;
}

int testcmpnum(Muscheme &ms, int count=1000, bool verbose=false) {
    int errs=0;
    int i1,i2;
    munum a,b;
    bool b1,b2,b3;
    for (int i=0; i<count; i++) {
        i1=rand()/2;
        if (rand()%2) i1=(-1)*i1;
        i2=rand()/2;
        if (rand()%2) i2=(-1)*i2;
        a=munum(i1);
        b=munum(i2);
        b1=a.mueq(a,a);
        b2=b.mueq(b,b);
        b3=a.mugr(a,b);
        if ((i1>i2)!=b3 || !b1 || !b2) {
             printf("Error: %d > %d == %s\n",i1,i2,b3?"true":"false");
             if (!b1 || !b2) printf("EQ error!\n");
             errs+=1;
        } else {
            if (verbose) printf("OK: %d > %d == %s\n",i1,i2,b3?"true":"false");
        }
    }
    return errs;
}

int testnummul(Muscheme &ms, int count=1000, bool verbose=false) {
    int errs=0;
    int i1,i2;
    munum a,b,c;
    for (int i=0; i<count; i++) {
        i1=rand()%10000;
        if (rand()%2) i1=(-1)*i1;
        i2=rand()%10000;
        if (rand()%2) i2=(-1)*i2;
        a=munum(i1);
        b=munum(i2);
        c=a.mumul(a,b);
        if (i1*i2!=atoi(c.str().c_str())) {
             printf("Error: %d * %d = %d, not %s %s\n",i1,i2,i1*i2,c.str().c_str(),c.nom.c_str());
             errs+=1;
        } else {
            if (verbose) printf("OK: %d * %d = %d, %s\n",i1,i2,i1*i2,c.str().c_str());
        }
    }
    return errs;
}


int main(int argc, char *argv[]) {
    Muscheme ms;
    int errs=testit(ms);
    errs+=testnum(ms, 100);
    errs+=testnummul(ms,10000,true);
    errs+=testcmpnum(ms,1000,true);
    if (errs==0) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("%d tests failed!\n",errs);
       return -1;
    }
}
