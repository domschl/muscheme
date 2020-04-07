#include <iostream>
#include <vector>
#include <string>

typedef std::string string;

enum munum_type {mum_valid, mum_nan, mum_inf};

struct munum {
    bool pos;
    munum_type type;
    string nom;
    string den;
    munum() {
        pos=true;
        type=mum_valid;
        nom="0";
        den="1";
    }
    munum(string n) {
        if (n.length() && (n[0]=='-')) {
            pos=false;
            type=mum_valid;
            n=n.substr(1);
        } else {
            pos=true;
            type=mum_valid;
        }
        nom=n;
        den="1";
    }
    munum(int n) {
        if (n<0) {
            pos=false;
            type=mum_valid;
            n=(-n);
        } else {
            pos=true;
            type=mum_valid;
        }
        nom=std::to_string(n);
        den="1";
    }
    void to_nan() {
        nom="0";
        den="1";
        type=mum_nan;
        pos=true;
    }
    string str() {
        switch (type) {
            case mum_inf:
                if (pos) return "INF";
                else return "-INF";
                break;
            case mum_nan:
                return "NaN";
                break;
            case mum_valid:
                string r;
                if (!pos) r="-"; else r="";
                if (den=="1") {
                     r+=nom;
                } else {
                     r+=nom+"/"+den;
                }
                return r;
        }
        return("muBAD");
    }
};

munum muipadd(munum num1, munum num2) {
    int l,l1,l2;
    int c=0,n1,n2,n3;
    munum r;
    if (num1.den!=num2.den || num1.pos!=num2.pos || num1.type!=mum_valid || num2.type!=mum_valid) {
        r.to_nan();
        return r;
    }
    if (num1.nom.length()>num2.nom.length()) {
        l=num1.nom.length();
        l1=0;
        l2=l-num2.nom.length();
    } else { 
        l=num2.nom.length();
        l1=l-num1.nom.length();
        l2=0;
    }
    for (int i=l-1; i>=0; i--) {
        if (i<l1) n1=0; else n1=num1.nom[i-l1]-'0';
        if (i<l2) n2=0; else n2=num2.nom[i-l2]-'0';
        n3=n1+n2+c;
        c=n3/10;
        n3=n3%10;
        r.nom=(char)(n3+'0')+r.nom;
    }
    if (c!=0) r.nom=(char)(c+'0')+r.nom;
    r.den=num1.den;
    r.pos=num1.pos;
    return r;
}

munum musub(munum num1, munum num2) {
    int l,l1,l2;
    int c=0,n1,n2,n3;
    munum r;
    if (num1.den!=num2.den || num1.pos!=num2.pos) {
        r.to_nan();
        return r;
    }
    if (num1.nom.length()>num2.nom.length()) {
        l=num1.nom.length();
        l1=0;
        l2=l-num2.nom.length();
    } else { 
        l=num2.nom.length();
        l1=l-num1.nom.length();
        l2=0;
    }
    for (int i=l-1; i>=0; i--) {
        if (i<l1) n1=0; else n1=num1.nom[i-l1]-'0';
        if (i<l2) n2=0; else n2=num2.nom[i-l2]-'0';
        n3=n1+n2+c;
        c=n3/10;
        n3=n3%10;
        r.nom=(char)(n3+'0')+r.nom;
    }
    if (c!=0) r.nom=(char)(c+'0')+r.nom;
    r.den=num1.den;
    r.pos=num1.pos;
    return r;
}


