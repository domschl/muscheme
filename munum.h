#include <iostream>
#include <vector>
#include <string>

namespace msc {
using std::string;
// using std::string::NPOS;
#define NPOS (std::string::npos)

string infSymbol = "∞";

struct munum {
    enum munum_type { mum_valid, mum_nan, mum_inf };

    bool pos;
    munum_type type;
    string nom;
    string den;
    munum() {
        pos = true;
        type = mum_valid;
        nom = "0";
        den = "1";
    }

    munum(const munum &mn) {
        pos = mn.pos;
        type = mn.type;
        nom = mn.nom;
        den = mn.den;
    }

    static bool isnat(string tok) {
        if (tok.length() == 0)
            return false;
        std::string isn = "0123456789";
        for (unsigned int i = 0; i < tok.length(); i++) {
            if (isn.find(tok[i]) == NPOS)
                return false;
        }
        return true;
    }

    static bool isint(string tok) {
        if (tok.length() == 0)
            return false;
        if (tok[0] == '-')
            return isnat(tok.substr(1));
        return isnat(tok);
    }

    static bool isrational(string tok) {
        if (tok.length() == 0)
            return false;
        unsigned long p1 = tok.find('/');
        if (p1 == NPOS)
            return isint(tok);
        return isint(tok.substr(0, p1)) && isnat(tok.substr(p1 + 1));
    }

    static bool issimpfloat(string tok) {
        if (tok.length() == 0)
            return false;
        unsigned long p1 = tok.find('.');
        if (p1 != NPOS) {
            return isint(tok.substr(0, p1) + tok.substr(p1 + 1));
        } else {
            return isint(tok);
        }
    }

    static bool isfloat(string tok) {
        if (tok.length() == 0)
            return false;
        if (isint(tok))
            return true;
        unsigned long p1 = tok.find('e');
        if (p1 == NPOS)
            p1 = tok.find('E');
        if (p1 == NPOS) {
            return issimpfloat(tok);
        } else {
            return issimpfloat(tok.substr(0, p1)) && isint(tok.substr(p1 + 1));
        }
    }

    munum(string n) {
        if (isint(n)) {
            if (n.length() && (n[0] == '-')) {
                pos = false;
                type = mum_valid;
                n = n.substr(1);
            } else {
                pos = true;
                type = mum_valid;
            }
            while (n.length() > 1 && (n[0] == '0'))
                n = n.substr(1);
            nom = n;
            den = "1";
            return;
        } else {
            if (isrational(n)) {
                unsigned long p1 = n.find('/');
                if (p1 != NPOS) {
                    if (n.length() && (n[0] == '-')) {
                        pos = false;
                        type = mum_valid;
                        nom = n.substr(1, p1 - 1);
                    } else {
                        pos = true;
                        type = mum_valid;
                        nom = n.substr(0, p1);
                    }
                    den = n.substr(p1 + 1);
                }
                while (nom.length() > 1 && (nom[0] == '0'))
                    nom = nom.substr(1);
                while (den.length() > 1 && (den[0] == '0'))
                    den = den.substr(1);
                return;
            }
        }
        to_nan();
    }

    munum(int n) {
        if (n < 0) {
            pos = false;
            type = mum_valid;
            n = (-n);
        } else {
            pos = true;
            type = mum_valid;
        }
        nom = std::to_string(n);
        den = "1";
    }
    void to_nan() {
        nom = "0";
        den = "1";
        type = mum_nan;
        pos = true;
    }
    void to_inf(bool _pos = true) {
        nom = "0";
        den = "1";
        type = mum_inf;
        pos = _pos;
    }
    string str() {
        switch (type) {
        case mum_inf:
            if (pos)
                return infSymbol;
            else
                return "-" + infSymbol;
            break;
        case mum_nan:
            return "NaN";
            break;
        case mum_valid:
            string r;
            if (!pos)
                r = "-";
            else
                r = "";
            if (den == "1") {
                r += nom;
            } else {
                r += nom + "/" + den;
            }
            return r;
        }
        return ("muBAD");
    }

    static munum muipadd(munum num1, munum num2) {
        int l, l1, l2;
        int c = 0, n1, n2, n3;
        munum r;
        if (num1.den != num2.den || num1.pos != num2.pos ||
            num1.type != munum::mum_valid || num2.type != munum::mum_valid) {
            r.to_nan();
            return r;
        }
        if (num1.nom.length() > num2.nom.length()) {
            l = num1.nom.length();
            l1 = 0;
            l2 = l - num2.nom.length();
        } else {
            l = num2.nom.length();
            l1 = l - num1.nom.length();
            l2 = 0;
        }
        r.nom = "";
        for (int i = l - 1; i >= 0; i--) {
            if (i < l1)
                n1 = 0;
            else
                n1 = num1.nom[i - l1] - '0';
            if (i < l2)
                n2 = 0;
            else
                n2 = num2.nom[i - l2] - '0';
            n3 = n1 + n2 + c;
            c = n3 / 10;
            n3 = n3 % 10;
            r.nom = (char)(n3 + '0') + r.nom;
        }
        if (c != 0)
            r.nom = (char)(c + '0') + r.nom;
        r.den = num1.den;
        r.pos = num1.pos;
        return r;
    }

    static munum muipsub(munum num1, munum num2) {
        int l, l1, l2;
        int c = 0, n1, n2, n3, n4, c2 = 0;
        munum rp, rn;
        if (num1.den != num2.den || !num1.pos || !num2.pos ||
            num1.type != munum::mum_valid || num2.type != munum::mum_valid) {
            rp.to_nan();
            return rp;
        }
        if (num1.nom.length() > num2.nom.length()) {
            l = num1.nom.length();
            l1 = 0;
            l2 = l - num2.nom.length();
        } else {
            l = num2.nom.length();
            l1 = l - num1.nom.length();
            l2 = 0;
        }
        rp.nom = "";
        rn.nom = "";
        for (int i = l - 1; i >= 0; i--) {
            if (i < l1)
                n1 = 0;
            else
                n1 = num1.nom[i - l1] - '0';
            if (i < l2)
                n2 = 0;
            else
                n2 = num2.nom[i - l2] - '0';
            n3 = n1 - n2 + 10 + c;
            n4 = n1 - n2 + 10 + c2;
            if (i == l - 1)
                n4 -= 1;
            c = n3 / 10 - 1;
            n3 = n3 % 10;
            c2 = n4 / 10 - 1;
            n4 = n4 % 10;
            rp.nom = (char)(n3 + '0') + rp.nom;
            rn.nom = (char)(n4 + '0') + rn.nom;
        }
        rp.den = num1.den;
        rp.pos = num1.pos;
        if (c != 0) {
            for (unsigned int i = 0; i < rn.nom.length(); i++) {
                rn.nom[i] = '9' - (rn.nom[i] - '0');
            }
            while (rn.nom[0] == '0')
                rn.nom = rn.nom.substr(1);
            rn.pos = !rn.pos;
            return rn;
        }
        while (rp.nom[0] == '0' && rp.nom.length() > 1)
            rp.nom = rp.nom.substr(1);
        return rp;
    }

    static munum muadd(munum num1, munum num2) {
        bool fact = false;
        munum res;
        if (num1.den != "1" || num2.den != "1") {
            printf("(+0) %s %s\n", num1.str().c_str(), num2.str().c_str());
            munum d1;
            d1 = num1;
            printf("(d1) %s\n", d1.str().c_str());
            num1 = mumul(num1, munum(num2.den + '/' + num2.den), false);
            printf("(+1pre) %s %s\n", num1.str().c_str(), d1.str().c_str());
            num2 = mumul(num2, munum(d1.den + '/' + d1.den), false);
            printf("(+1) %s %s\n", num1.str().c_str(), num2.str().c_str());
            fact = true;
        }
        if (num1.pos == num2.pos) {
            res = muipadd(num1, num2);
            if (fact)
                res = mufactor(res);
            return res;
        } else {
            if (num1.pos) {
                num2.pos = true;
                res = muipsub(num1, num2);
                if (fact)
                    res = mufactor(res);
                return res;
            } else {
                num1.pos = true;
                res = muipsub(num2, num1);
                if (fact)
                    res = mufactor(res);
                return res;
            }
        }
    }

    static munum musub(munum num1, munum num2) {
        bool fact = false;
        munum res;
        if (num1.den != "1" || num2.den != "1") {
            printf("(+0) %s %s\n", num1.str().c_str(), num2.str().c_str());
            munum d1;
            d1 = num1;
            printf("(d1) %s\n", d1.str().c_str());
            num1 = mumul(num1, munum(num2.den + '/' + num2.den), false);
            printf("(+1pre) %s %s\n", num1.str().c_str(), d1.str().c_str());
            num2 = mumul(num2, munum(d1.den + '/' + d1.den), false);
            printf("(+1) %s %s\n", num1.str().c_str(), num2.str().c_str());
            fact = true;
        }
        if (num1.pos && !num2.pos) {
            num2.pos = true;
            res = muipadd(num1, num2);
            if (fact)
                res = mufactor(res);
            return res;
        }
        if (num1.pos && num2.pos) {
            res = muipsub(num1, num2);
            if (fact)
                res = mufactor(res);
            return res;
        }
        if (!num1.pos && num2.pos) {
            num2.pos = false;
            res = muipadd(num1, num2);
            if (fact)
                res = mufactor(res);
            return res;
        }
        // if (!num1.pos && !num2.pos)
        munum r;
        num1.pos = true;
        num2.pos = true;
        res = muipsub(num2, num1);
        if (fact)
            res = mufactor(res);
        return res;
    }

    static munum mumulnat(munum num1, munum num2) {
        munum acc(0), ai, ai2;
        int mi, ind;
        if (num1.den != "1" || num2.den != "1" ||
            num1.type != munum::mum_valid || num2.type != munum::mum_valid) {
            acc.to_nan();
            printf("cant mul %s %s\n", num1.str().c_str(), num2.str().c_str());
            return acc;
        }
        for (unsigned int i = 0; i < num2.nom.length(); i++) {
            ind = num2.nom.length() - 1 - i;
            mi = num2.nom[ind] - '0';
            if (mi == 0)
                continue;
            switch (mi) {
            case 1:
                ai = num1;
                break;
            case 2:
                ai = muadd(num1, num1);
                break;
            case 3:
                ai = muadd(num1, num1);
                ai = muadd(num1, ai);
                break;
            case 4:
                ai = muadd(num1, num1);
                ai = muadd(ai, ai);
                break;
            case 5:
                ai = muadd(num1, num1);
                ai = muadd(ai, ai);
                ai = muadd(ai, num1);
                break;
            case 6:
                ai = muadd(num1, num1);
                ai2 = muadd(ai, ai);
                ai = muadd(ai, ai2);
                break;
            case 7:
                ai = muadd(num1, num1);
                ai2 = muadd(ai, ai);
                ai = muadd(ai, ai2);
                ai = muadd(ai, num1);
                break;
            case 8:
                ai = muadd(num1, num1);
                ai = muadd(ai, ai);
                ai = muadd(ai, ai);
                break;
            case 9:
                ai = muadd(num1, num1);
                ai = muadd(ai, ai);
                ai = muadd(ai, ai);
                ai = muadd(ai, num1);
                break;
            default:
                printf("Illegal numeric digit: %d\n", mi);
                acc.to_nan();
                return acc;
            }
            for (unsigned int j = 0; j < i; j++) {
                ai.nom += "0";
            }
            acc = muadd(acc, ai);
        }
        acc.pos = (num1.pos == num2.pos);
        return acc;
    }

    static munum mumul(munum num1, munum num2, bool bFactor=true) {
        munum res = munum(0), n1, n2;
        if (num1.type != munum::mum_valid || num2.type != munum::mum_valid) {
            res.to_nan();
            return res;
        }
        n1 = munum(num1.nom);
        n2 = munum(num2.nom);
        res.nom = mumulnat(n1, n2).nom;
        n1 = munum(num1.den);
        n2 = munum(num2.den);
        res.den = mumulnat(n1, n2).nom;
        res.pos = (num1.pos == num2.pos);
        if (bFactor) res = mufactor(res);
        return res;
    }

    static munum mufac(munum n) {
        munum f(1);
        if (n.type != mum_valid || mult(n, munum(0)) || n.den != "1") {
            f.to_nan();
            return (f);
        }
        if (mueq(n, munum(0)))
            return munum(1);
        while (true) {
            f = mumulnat(f, n);
            n = musub(n, munum(1));
            if (mule(n, munum(1)))
                return f;
        }
    }

    static bool mueq(munum num1, munum num2) {
        if (mucmp(num1, num2) == 0)
            return true;
        else
            return false;
    }

    static bool mune(munum num1, munum num2) {
        int c = mucmp(num1, num2);
        if (c == -1 || c == 1)
            return true;
        else
            return false;
    }

    static int mucmp(munum num1, munum num2) {
        if (num1.type != mum_valid || num2.type != mum_valid)
            return -2;
        if (num1.pos != num2.pos)
            return num1.pos ? 1 : -1;
        if (num1.den == num2.den) {
            if (num1.nom.length() > num2.nom.length())
                return num1.pos ? 1 : -1;
            if (num1.nom.length() < num2.nom.length())
                return num1.pos ? -1 : 1;
            for (unsigned int i = 0; i < num1.nom.length(); i++) {
                if (num1.nom[i] > num2.nom[i])
                    return num1.pos ? 1 : -1;
                if (num1.nom[i] < num2.nom[i])
                    return num1.pos ? -1 : 1;
            }
            return 0;
        } else {
            munum n11(num1.nom), n12(num1.den), n21(num2.nom), n22(num2.den);
            munum n1, n2;
            n1 = mumulnat(n11, n22);
            n2 = mumulnat(n12, n21);
            if (n1.nom.length() > n2.nom.length())
                return num1.pos ? 1 : -1;
            if (n1.nom.length() < n2.nom.length())
                return !num1.pos ? -1 : 1;
            for (unsigned int i = 0; i < n1.nom.length(); i++) {
                if (n1.nom[i] > n2.nom[i])
                    return num1.pos ? 1 : -1;
                if (n1.nom[i] < n2.nom[i])
                    return num1.pos ? -1 : 1;
            }
            return 0;
        }
    }

    static bool mugt(munum num1, munum num2) {
        if (mucmp(num1, num2) == 1)
            return true;
        else
            return false;
    }

    static bool muge(munum num1, munum num2) {
        int c = mucmp(num1, num2);
        if (c == 1 || c == 0)
            return true;
        else
            return false;
    }

    static bool mult(munum num1, munum num2) {
        if (mucmp(num1, num2) == -1)
            return true;
        else
            return false;
    }

    static bool mule(munum num1, munum num2) {
        int c = mucmp(num1, num2);
        if (c == -1 || c == 0)
            return true;
        else
            return false;
    }

    static munum mugcd(munum num1, munum num2) {
        munum r(0);
        if (num1.den != "1" || num2.den != "1" || !num1.pos || !num2.pos ||
            num1.type != munum::mum_valid || num2.type != munum::mum_valid) {
            r.to_nan();
            return r;
        }
        munum t;
        while (mune(num2, munum(0))) {
            t = num2;
            num2 = mumod(num1, num2);
            num1 = t;
        }
        return num1;
    }

    static munum mufactor(munum num1) {
        printf("factor: %s\n", num1.str().c_str());
        if (num1.den == "0") {
            num1.to_inf(num1.pos);
            return num1;
        }
        munum n1(num1.nom), n2(num1.den);
        munum gcd = mugcd(n1, n2);
        n1 = mudivnat(n1, gcd);
        n2 = mudivnat(n2, gcd);
        num1.nom = n1.nom;
        num1.den = n2.nom;
        return num1;
    }

    static std::vector<munum> muipdivmod(munum num1, munum num2) {
        munum r(0), q(0);
        int dl;
        std::vector<munum> res(2);
        if (num1.den != num2.den || num1.pos != num2.pos ||
            num1.type != munum::mum_valid || num2.type != munum::mum_valid) {
            r.to_nan();
            q.to_nan();
            res[0] = q;
            res[1] = r;
            return res;
        }
        munum n1, n2;
        n1 = num1, n2 = num2;
        dl = n1.nom.length() - n2.nom.length();
        while (true) {
            // printf("%s %s\n",q.nom, r.nom);
            if (mugt(n2, n1) || dl < 0) {
                r = n1;
                res[0] = q;
                res[1] = r;
                return res;
            }
            string pad = "";
            for (int i = 0; i < dl; i++)
                pad += "0";
            munum sb(n2.nom + pad);
            while (muge(n1, sb)) {
                n1 = musub(n1, sb);
                q = muadd(q, munum("1" + pad));
            }
            --dl;
        }
        res[0] = q;
        res[1] = r;
        return res;
    }

    static std::vector<munum> mudivmod(munum num1, munum num2) {
        munum q(0), r(0);
        std::vector<munum> res(2);
        if (num1.type != munum::mum_valid || num2.type != munum::mum_valid) {
            q.to_nan();
            r.to_nan();
            res[0] = q;
            res[1] = r;
            return res;
        }
        bool s1, s2;
        s1 = num1.pos;
        s2 = num2.pos;
        num1.pos = true;
        num2.pos = true;
        munum n1(num1.nom), n2(num2.nom);
        res = muipdivmod(n1, n2);
        if (s1 != s2) {
            if (res[0].nom != "0")
                res[0].pos = false;
        }
        if (!s1) {
            res[1].pos = false;
        }
        return res;
    }

    static munum mudivnat(munum num1, munum num2) {
        munum q(0), r(0);
        std::vector<munum> res;
        if (num1.type != munum::mum_valid || num2.type != munum::mum_valid) {
            q.to_nan();
            r.to_nan();
            res[0] = q;
            res[1] = r;
            return res[0];
        }
        res = mudivmod(num1, num2);
        return res[0];
    }

    static munum mudiv(munum num1, munum num2) {
        munum res;
        if (num2.nom == "0") {
            if (num1.nom == "0") {
                res.to_nan();
                return res;
            } else {
                res.to_inf(num1.pos);
                return res;
            }
        }
        munum inv = num2;
        inv.nom = num2.den;
        inv.den = num2.nom;
        return mumul(num1, inv);
    }

    static munum mumod(munum num1, munum num2) {
        munum q(0), r(0);
        std::vector<munum> res;
        if (num1.type != munum::mum_valid || num2.type != munum::mum_valid) {
            q.to_nan();
            r.to_nan();
            res[0] = q;
            res[1] = r;
            return res[1];
        }
        res = mudivmod(num1, num2);
        return res[1];
    }
};
}  // namespace msc