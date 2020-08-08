#include "muscheme.h"

using msc::munum;
using msc::Muscheme;
using std::string;

int testit(Muscheme &ms, bool verbose = false) {
    int errs = 0;

    std::vector<string> ints{"0",  "01", "23424",  "02020202",
                             "-1", "-0", "-923432"};
    for (auto const &it : ints) {
        if (!munum::isint(it)) {
            printf("Fail: %s is int!\n", it.c_str());
            ++errs;
        } else {
            if (verbose)
                printf("OK: %s is int.\n", it.c_str());
        }
    }
    std::vector<string> nints{"0e", "0.1", "23a424", "", "-", ".", "e"};
    for (auto const &it : nints) {
        if (munum::isint(it)) {
            printf("Fail: %s is NOT an int!\n", it.c_str());
            ++errs;
        } else {
            if (verbose)
                printf("OK: %s is not an int.\n", it.c_str());
        }
    }

    std::vector<string> rats{"1/1",  "01",   "23424/2343", "0202/0202",
                             "-1/1", "-0/0", "-923432/1",  "1/1"};
    for (auto const &it : rats) {
        if (!munum::isrational(it)) {
            printf("Fail: %s is rational!\n", it.c_str());
            ++errs;
        } else {
            if (verbose)
                printf("OK: %s is rational.\n", it.c_str());
        }
    }
    std::vector<string> nrats{"0e/",  "/0.1", "/23424", "1/",
                              "1/-1", "/",    "1/1/1",  "1.1/1"};
    for (auto const &it : nints) {
        if (munum::isrational(it)) {
            printf("Fail: %s is NOT rational!\n", it.c_str());
            ++errs;
        } else {
            if (verbose)
                printf("OK: %s is not rational.\n", it.c_str());
        }
    }

    std::vector<string> floats{
        "0",       "01",  "23424", "02020202", "-1",     "-0",
        "-923432", ".1",  "0.",    "-.1",      "-0.234", "134.3233",
        ".0e4",    "0e4", "-1e3",  "1E4",      "1E-4",   "-1.343e-32"};
    for (auto const &it : floats) {
        if (!munum::isfloat(it)) {
            printf("Fail: %s is float!\n", it.c_str());
            ++errs;
        } else {
            if (verbose)
                printf("OK: %s is float.\n", it.c_str());
        }
    }
    std::vector<string> nfloats{"0e", ".e10", "-e23a424", "",      "-",
                                ".",  "e",    "1.43.322", "1e1e1", "1e1.2"};
    for (auto const &it : nfloats) {
        if (munum::isfloat(it)) {
            printf("Fail: %s is NOT a float!\n", it.c_str());
            ++errs;
        } else {
            if (verbose)
                printf("OK: %s is not a float.\n", it.c_str());
        }
    }
    std::vector<string> strs{"\"\"", "\"asdf\"", "\"a\\\"a\"", "\"\\\"a\\\"\"",
                             "\"a a b\""};
    for (auto const &it : strs) {
        if (!ms.isstr(it)) {
            printf("Fail: %s is str!\n", it.c_str());
            ++errs;
        } else {
            if (verbose)
                printf("OK: %s is str.\n", it.c_str());
        }
    }
    std::vector<string> nstrs{"asdf", "\"asfd\"asdf\"", "\"asdf", "asdf\"", "",
                              "\"",   "\"\"\""};
    for (auto const &it : nstrs) {
        if (ms.isstr(it)) {
            printf("Fail: %s is NOT a str!\n", it.c_str());
            ++errs;
        } else {
            if (verbose)
                printf("OK: %s is not a str.\n", it.c_str());
        }
    }
    return errs;
}

int nrand(int max_digits = 8, bool posneg = true) {
    int digs[] = {10,      100,      1000,      10000,     100000,
                  1000000, 10000000, 100000000, 1000000000};
    if (max_digits < 1 || max_digits > 9)
        max_digits = 9;
    int d = rand() % max_digits;
    int modw = digs[d];
    int r = rand() % modw;
    if (posneg) {
        if (rand() % 2)
            r = r * (-1);
    }
    return r;
}

int testnum(Muscheme &ms, int count = 1000, bool verbose = false) {
    int errs = 0;
    int i1, i2;
    munum a, b, c;
    for (int i = 0; i < count; i++) {
        i1 = nrand();
        i2 = nrand();
        a = munum(i1);
        b = munum(i2);
        c = a.muadd(a, b);
        if (i1 + i2 != atoi(c.str().c_str())) {
            printf("Error: %d + %d = %d, not %s %s\n", i1, i2, i1 + i2,
                   c.str().c_str(), c.nom.c_str());
            errs += 1;
        } else {
            if (verbose)
                printf("OK: %d + %d = %d, %s\n", i1, i2, i1 + i2,
                       c.str().c_str());
        }
        c = a.musub(a, b);
        if (i1 - i2 != atoi(c.str().c_str())) {
            printf("Error: %d - %d = %d, not %s %s\n", i1, i2, i1 - i2,
                   c.str().c_str(), c.nom.c_str());
            errs += 1;
        } else {
            if (verbose)
                printf("OK: %d - %d = %d, %s\n", i1, i2, i1 - i2,
                       c.str().c_str());
        }
    }
    return errs;
}

int testcmpnum(Muscheme &ms, int count = 1000, bool verbose = false) {
    int errs = 0;
    int i1, i2;
    munum a, b;
    bool b1, b2, b3;
    for (int i = 0; i < count; i++) {
        i1 = nrand();
        i2 = nrand();
        a = munum(i1);
        b = munum(i2);
        b1 = a.mueq(a, a);
        if (!b1) {
            errs += 1;
            printf("Error: %d == %d: %s\n", i1, i1, b1 ? "true" : "false");
        } else {
            if (verbose)
                printf("OK: %d == %d: %s\n", i1, i1, b1 ? "true" : "false");
        }
        b2 = b.mueq(b, b);
        if (!b2) {
            errs += 1;
            printf("Error: %d == %d: %s\n", i2, i2, b2 ? "true" : "false");
        } else {
            if (verbose)
                printf("OK: %d == %d: %s\n", i2, i2, b2 ? "true" : "false");
        }
        if (i1 != i2) {
            b1 = a.mueq(a, b);
            if (b1) {
                errs += 1;
                printf("Error: %d == %d: %s\n", i1, i2, b1 ? "true" : "false");
            } else {
                if (verbose)
                    printf("OK: %d == %d: %s\n", i1, i2, b1 ? "true" : "false");
            }
            b1 = a.mune(a, b);
            if (!b1) {
                errs += 1;
                printf("Error: %d != %d: %s\n", i1, i2, b1 ? "true" : "false");
            } else {
                if (verbose)
                    printf("OK: %d != %d: %s\n", i1, i2, b1 ? "true" : "false");
            }
        }

        b3 = a.mugt(a, b);
        if ((i1 > i2) != b3) {
            printf("Error: %d > %d == %s\n", i1, i2, b3 ? "true" : "false");
            if (!b1 || !b2)
                printf("EQ error!\n");
            errs += 1;
        } else {
            if (verbose)
                printf("OK: %d > %d == %s\n", i1, i2, b3 ? "true" : "false");
        }
        b3 = a.muge(a, b);
        if ((i1 >= i2) != b3) {
            printf("Error: %d >= %d == %s\n", i1, i2, b3 ? "true" : "false");
            if (!b1 || !b2)
                printf("EQ error!\n");
            errs += 1;
        } else {
            if (verbose)
                printf("OK: %d >= %d == %s\n", i1, i2, b3 ? "true" : "false");
        }
        b3 = a.mult(a, b);
        if ((i1 < i2) != b3) {
            printf("Error: %d < %d == %s\n", i1, i2, b3 ? "true" : "false");
            if (!b1 || !b2)
                printf("EQ error!\n");
            errs += 1;
        } else {
            if (verbose)
                printf("OK: %d < %d == %s\n", i1, i2, b3 ? "true" : "false");
        }
        b3 = a.mule(a, b);
        if ((i1 <= i2) != b3) {
            printf("Error: %d <= %d == %s\n", i1, i2, b3 ? "true" : "false");
            if (!b1 || !b2)
                printf("EQ error!\n");
            errs += 1;
        } else {
            if (verbose)
                printf("OK: %d <= %d == %s\n", i1, i2, b3 ? "true" : "false");
        }
    }
    return errs;
}

int testnummul(Muscheme &ms, int count = 1000, bool verbose = false) {
    int errs = 0;
    int i1, i2;
    munum a, b, c;
    for (int i = 0; i < count; i++) {
        i1 = nrand(4);
        i2 = nrand(4);
        a = munum(i1);
        b = munum(i2);
        c = a.mumul(a, b);
        if (i1 * i2 != atoi(c.str().c_str())) {
            printf("Error: %d * %d = %d, not %s %s\n", i1, i2, i1 * i2,
                   c.str().c_str(), c.nom.c_str());
            errs += 1;
        } else {
            if (verbose)
                printf("OK: %d * %d = %d, %s\n", i1, i2, i1 * i2,
                       c.str().c_str());
        }
    }
    return errs;
}

int testmuipdiv(Muscheme &ms, int count = 1000, bool verbose = false,
                bool posneg = true) {
    int errs = 0;
    int i1, i2, pi2, dv, md, mdp;
    munum a, b, pb, q, r;
    std::vector<munum> res, resp;
    for (int i = 0; i < count; i++) {
        i1 = nrand(8, posneg);
        i2 = nrand(8, posneg);
        pi2 = i2;
        a = munum(i1);
        b = munum(i2);
        pb = munum(pi2);
        if (i2 == 0) {
            if (verbose)
                printf("Skipping /0 tests for now.\n");
            continue;
        }
        res = a.mudivmod(a, b);
        dv = i1 / i2;
        if (dv != atoi(res[0].str().c_str())) {
            printf("Error %d / %d = %d, not %s\n", i1, i2, dv,
                   res[0].str().c_str());
            errs += 1;
        } else {
            if (verbose)
                printf("OK %d / %d = %d, %s\n", i1, i2, dv,
                       res[0].str().c_str());
        }
        resp = a.mudivmod(a, pb);
        mdp = i1 % pi2;
        md = i1 % i2;
        if (mdp != atoi(resp[1].str().c_str())) {
            printf("Error %d mod %d = %d, not %s\n", i1, pi2, mdp,
                   resp[1].str().c_str());
            errs += 1;
        } else {
            if (verbose)
                printf("OK %d mod %d = %d, %s\n", i1, pi2, mdp,
                       resp[1].str().c_str());
        }

        q = a.mudivnat(a, b);
        r = a.mumod(a, b);
        if (q.str() != res[0].str()) {
            printf("Error %d div %d = %d, not %s,%s\n", i1, i2, dv,
                   q.str().c_str(), res[0].str().c_str());
            errs += 1;
        } else {
            if (verbose)
                printf("OK %d div %d = %d, %s\n", i1, i2, dv, q.str().c_str());
        }
        if (r.str() != res[1].str()) {
            printf("Error %d mod %d = %d, not %s,%s\n", i1, i2, md,
                   r.str().c_str(), res[1].str().c_str());
            errs += 1;
        } else {
            if (verbose)
                printf("OK %d mod %d = %d, %s\n", i1, i2, md, r.str().c_str());
        }
    }
    return errs;
}

void dofacs(int n = 100, int verbose = false) {
    munum a;
    for (int i = -10; i <= n; i++) {
        a = a.mufac(munum(i));
        if (verbose)
            printf("fac(%d)=%s\n", i, a.str().c_str());
    }
}

int testbigfac1000(int verbose = false) {
    string result1000 =
        "4023872600770937735437024339230039857193748642107146325437999104299385"
        "1239862902059204420848696940480047998861019719605863166687299480855890"
        "1323829669944590997424504087073759918823627727188732519779505950995276"
        "1208749754624970436014182780946464962910563938874378864873371191810458"
        "2578364784997701247663288983595573543251318532395846307555740911426241"
        "7474349347553428646576611667797396668820291207379143853719588249808126"
        "8678383745597317461360853795345242215865932019280908782973084313928444"
        "0328123155861103697680135730421616874760967587134831202547858932076716"
        "9132448426236131412508780208000261683151027341827977704784635868170164"
        "3650241536913982812648102130927612448963599287051149649754199093422215"
        "6683257208082133318611681155361583654698404670897560290095053761647584"
        "7728421889679646244945160765353408198901385442487984959953319101723355"
        "5566021394503997362807501378376153071277619268490343526252000158885351"
        "4733161170210396817592151090778801939317811419454525722386554146106289"
        "2187960223838971476088506276862967146674697562911234082439208160153780"
        "8898939645182632436716167621791689097799119037540312746222899880051954"
        "4441428201218736174599264295658174662830295557029902432415318161721046"
        "5832036786906117260158783520751516284225540265170483304226143974286933"
        "0616908979684825901254583271682264580665267699586526822728070757813918"
        "5817888965220816434834482599326604336766017699961283186078838615027946"
        "5955131156552036093988180612138558600301435694527224206344631797460594"
        "6825731037900840244324384656572450144028218852524709351906209290231364"
        "9327349756551395872055965422874977401141334696271542284586237738753823"
        "0483865688976461927383814900140767310446640259899490222221765904339901"
        "8860185665264850617997023561938970178600408118897299183110211712298459"
        "0164192106888438712185564612496079872290851929681937238864261483965738"
        "2291123125024186649353143970137428531926649875337218940694281434118520"
        "1580141233448280150513996942901534830776445690990731524332782882698646"
        "0278986432113908350621709500259738986355427719674282224875758676575234"
        "4220207573630569498825087968928162753848863396909959826280956121450994"
        "8717012445164612603790293091208890869420285106401821543994571568059418"
        "7274899809425474217358240106367740459574178516082923013535808184009699"
        "6372524230560855903700624271243416909004153690105933983835777939410970"
        "0277534720000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000";
    munum r;
    r = r.mufac(1000);
    if (r.nom != result1000) {
        printf("Error: wrong result for 1000!: %s instead of %s\n",
               r.str().c_str(), result1000.c_str());
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char *argv[]) {
    Muscheme ms;
    bool verbose = false;
    bool big = false;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-v"))
            verbose = true;
        if (!strcmp(argv[i], "-b"))
            big = true;
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            printf("test [-v] [-b] [-h]\n");
            printf("  -v: show all tests (verbose mode)\n");
            printf("  -b: perform large test with many samples\n");
            printf("  -h: this message\n");
            return 0;
        }
    }
    int n = 100;
    if (big)
        n = 1000000;
    int errs = testit(ms);
    errs += testnum(ms, n, verbose);
    errs += testcmpnum(ms, n, verbose);
    errs += testnummul(ms, n, verbose);
    errs += testmuipdiv(ms, n, verbose, true);
    dofacs(100, verbose);
    errs += testbigfac1000(verbose);
    if (errs == 0) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("%d tests failed!\n", errs);
        return -1;
    }
}
