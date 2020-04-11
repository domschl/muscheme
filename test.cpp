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

int nrand(int max_digits=8, bool posneg=true) {
    int digs[]={10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
    if (max_digits<1 || max_digits>9) max_digits=9;
    int d=rand()%max_digits;
    int modw=digs[d];
    int r=rand()%modw;
    if (posneg) {
        if (rand()%2) r=r*(-1);
    }
    return r;
}

int testnum(Muscheme &ms, int count=1000, bool verbose=false) {
    int errs=0;
    int i1,i2;
    munum a,b,c;
    for (int i=0; i<count; i++) {
        i1=nrand();
        i2=nrand();
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
        i1=nrand();
        i2=nrand();
        a=munum(i1);
        b=munum(i2);
        b1=a.mueq(a,a);
        if (!b1) {
            errs+=1;
            printf("Error: %d == %d: %s\n", i1,i1, b1?"true": "false");
        } else {
            if (verbose) printf("OK: %d == %d: %s\n", i1, i1, b1?"true": "false");
        }
        b2=b.mueq(b,b);
        if (!b2) {
            errs+=1;
            printf("Error: %d == %d: %s\n", i2,i2, b2?"true": "false");
        } else {
            if (verbose) printf("OK: %d == %d: %s\n", i2, i2, b2?"true": "false");
        }
        if (i1!=i2) {
            b1=a.mueq(a,b);
            if (b1) {
                errs+=1;
                printf("Error: %d == %d: %s\n", i1,i2, b1?"true": "false");
            } else {
                if (verbose) printf("OK: %d == %d: %s\n", i1, i2, b1?"true": "false");
            }
            b1=a.mune(a,b);
            if (!b1) {
                errs+=1;
                printf("Error: %d != %d: %s\n", i1,i2, b1?"true": "false");
            } else {
                if (verbose) printf("OK: %d != %d: %s\n", i1, i2, b1?"true": "false");
            }
        }

        b3=a.mugt(a,b);
        if ((i1>i2)!=b3) {
            printf("Error: %d > %d == %s\n",i1,i2,b3?"true":"false");
            if (!b1 || !b2) printf("EQ error!\n");
            errs+=1;
        } else {
           if (verbose) printf("OK: %d > %d == %s\n",i1,i2,b3?"true":"false");
        }
        b3=a.muge(a,b);
        if ((i1>=i2)!=b3) {
            printf("Error: %d >= %d == %s\n",i1,i2,b3?"true":"false");
            if (!b1 || !b2) printf("EQ error!\n");
            errs+=1;
        } else {
           if (verbose) printf("OK: %d >= %d == %s\n",i1,i2,b3?"true":"false");
        }
        b3=a.mult(a,b);
        if ((i1<i2)!=b3) {
            printf("Error: %d < %d == %s\n",i1,i2,b3?"true":"false");
            if (!b1 || !b2) printf("EQ error!\n");
            errs+=1;
        } else {
           if (verbose) printf("OK: %d < %d == %s\n",i1,i2,b3?"true":"false");
        }
        b3=a.mule(a,b);
        if ((i1<=i2)!=b3) {
            printf("Error: %d <= %d == %s\n",i1,i2,b3?"true":"false");
            if (!b1 || !b2) printf("EQ error!\n");
            errs+=1;
        } else {
           if (verbose) printf("OK: %d <= %d == %s\n",i1,i2,b3?"true":"false");
        }
    }
    return errs;
}

int testnummul(Muscheme &ms, int count=1000, bool verbose=false) {
    int errs=0;
    int i1,i2;
    munum a,b,c;
    for (int i=0; i<count; i++) {
        i1=nrand(4);
        i2=nrand(4);
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

void dofacs(int n=100, int verbose=false) {
    munum a;
    for (int i=-10; i<=n; i++) {
        a=a.mufac(munum(i));
        if (verbose)
            printf("fac(%d)=%s\n",i,a.str().c_str());
    }
}

int testbigfac1000(int verbose=false) {
    string result1000="402387260077093773543702433923003985719374864210714632543799910429938512398629020592044208486969404800479988610197196058631666872994808558901323829669944590997424504087073759918823627727188732519779505950995276120874975462497043601418278094646496291056393887437886487337119181045825783647849977012476632889835955735432513185323958463075557409114262417474349347553428646576611667797396668820291207379143853719588249808126867838374559731746136085379534524221586593201928090878297308431392844403281231558611036976801357304216168747609675871348312025478589320767169132448426236131412508780208000261683151027341827977704784635868170164365024153691398281264810213092761244896359928705114964975419909342221566832572080821333186116811553615836546984046708975602900950537616475847728421889679646244945160765353408198901385442487984959953319101723355556602139450399736280750137837615307127761926849034352625200015888535147331611702103968175921510907788019393178114194545257223865541461062892187960223838971476088506276862967146674697562911234082439208160153780889893964518263243671616762179168909779911903754031274622289988005195444414282012187361745992642956581746628302955570299024324153181617210465832036786906117260158783520751516284225540265170483304226143974286933061690897968482590125458327168226458066526769958652682272807075781391858178889652208164348344825993266043367660176999612831860788386150279465955131156552036093988180612138558600301435694527224206344631797460594682573103790084024432438465657245014402821885252470935190620929023136493273497565513958720559654228749774011413346962715422845862377387538230483865688976461927383814900140767310446640259899490222221765904339901886018566526485061799702356193897017860040811889729918311021171229845901641921068884387121855646124960798722908519296819372388642614839657382291123125024186649353143970137428531926649875337218940694281434118520158014123344828015051399694290153483077644569099073152433278288269864602789864321139083506217095002597389863554277196742822248757586765752344220207573630569498825087968928162753848863396909959826280956121450994871701244516461260379029309120889086942028510640182154399457156805941872748998094254742173582401063677404595741785160829230135358081840096996372524230560855903700624271243416909004153690105933983835777939410970027753472000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    munum r;
    r=r.mufac(1000);
    if (r.nom!=result1000) {
        printf("Error: wrong result for 1000!: %s instead of %s\n",r.str().c_str(),result1000.c_str());
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char *argv[]) {
    Muscheme ms;
    bool verbose=false;
    bool big=false;
    for (int i=1; i<argc; i++) {
        if (!strcmp(argv[i],"-v")) verbose=true;
        if (!strcmp(argv[i],"-b")) big=true;
        if (!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help")) {
            printf("test [-v] [-b] [-h]\n");
            printf("  -v: show all tests (verbose mode)\n");
            printf("  -b: perform large test with many samples\n");
            printf("  -h: this message\n");
            return 0;
         }
    }
    int n=100;
    if (big) n=1000000;
    int errs=testit(ms);
    errs+=testnum(ms, n, verbose);
    errs+=testnummul(ms,n, verbose);
    errs+=testcmpnum(ms,n, verbose);
    dofacs(100,verbose);
    errs+=testbigfac1000(verbose);
    if (errs==0) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("%d tests failed!\n",errs);
       return -1;
    }
}
