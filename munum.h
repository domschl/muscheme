#include <iostream>
#include <vector>
#include <string>

typedef std::string string;

struct munum {
    enum munum_type {mum_valid, mum_nan, mum_inf};

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

    int cmp(munum num1, munum num2) {
        if (mueq(num1,num2)) return 0;
        if (mugr(num1,num2)) return 1;
        return -1;
    }

    munum muipadd(munum num1, munum num2) {
        int l,l1,l2;
        int c=0,n1,n2,n3;
        munum r;
        if (num1.den!=num2.den || num1.pos!=num2.pos || num1.type!=munum::mum_valid || num2.type!=munum::mum_valid) {
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
        r.nom="";
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

    munum muipsub(munum num1, munum num2) {
        int l,l1,l2;
        int c=0,n1,n2,n3,n4,c2=0;
        munum rp,rn;
        if (num1.den!=num2.den || !num1.pos || !num2.pos || num1.type!=munum::mum_valid || num2.type!=munum::mum_valid) {
            rp.to_nan();
            return rp;
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
        rp.nom="";
        rn.nom="";
        for (int i=l-1; i>=0; i--) {
            if (i<l1) n1=0; else n1=num1.nom[i-l1]-'0';
            if (i<l2) n2=0; else n2=num2.nom[i-l2]-'0';
            n3=n1-n2+10+c;
            n4=n1-n2+10+c2;
            if (i==l-1) n4 -= 1;
            c=n3/10-1;
            n3=n3%10;
            c2=n4/10-1;
            n4=n4%10;
            rp.nom=(char)(n3+'0')+rp.nom;
            rn.nom=(char)(n4+'0')+rn.nom;
        }
        rp.den=num1.den;
        rp.pos=num1.pos;
        if (c!=0) {
            for (unsigned int i=0; i<rn.nom.length(); i++) {
                rn.nom[i]='9'-(rn.nom[i]-'0');
            }
            while (rn.nom[0]=='0') rn.nom=rn.nom.substr(1);
            rn.pos=!rn.pos;
            return rn;
        }
        return rp;
    }

    munum muadd(munum num1, munum num2) {
        if (num1.pos == num2.pos) {
            return muipadd(num1,num2);
        } else {
            if (num1.pos) {
                num2.pos=true;
                return muipsub(num1,num2);
            } else {
                num1.pos=true;
                return muipsub(num2,num1);
            }
        }
    }

    munum musub(munum num1, munum num2) {
        if (num1.pos && !num2.pos) {
            num2.pos=true;
            return muipadd(num1, num2);
        }
        if (num1.pos && num2.pos) {
            return muipsub(num1,num2);
        }
        if (!num1.pos && num2.pos) {
            num2.pos=false;
            return muipadd(num1,num2);
        }
        // if (!num1.pos && !num2.pos) 
        munum r;
        num1.pos=true; num2.pos=true;
        return muipsub(num2, num1);
    }

    munum mumul( munum num1, munum num2) {
        munum acc(0),ai,ai2;
        int mi,ind;
        if (num1.den!="1" || num2.den!="1" || num1.type!=munum::mum_valid || num2.type!=munum::mum_valid) {
            acc.to_nan();
            return acc;
        }
        for (int i=0; i<num2.nom.length(); i++) {
            ind=num2.nom.length()-1-i;
            mi=num2.nom[ind]-'0';
            if (mi==0) continue;
            switch (mi) {
                case 1:
                    ai=num1;
                    break;
                case 2:
                    ai=muadd(num1, num1);
                    break;
                case 3:
                    ai=muadd(num1,num1);
                    ai=muadd(num1,ai);
                    break;
                case 4:
                    ai=muadd(num1,num1);
                    ai=muadd(ai,ai);
                    break;
                case 5:
                    ai=muadd(num1,num1);
                    ai=muadd(ai,ai);
                    ai=muadd(ai,num1);
                    break;
                case 6:
                    ai=muadd(num1,num1);
                    ai2=muadd(ai,ai);
                    ai=muadd(ai,ai2);
                    break;
                case 7:
                    ai=muadd(num1,num1);
                    ai2=muadd(ai,ai);
                    ai=muadd(ai,ai2);
                    ai=muadd(ai,num1);
                    break;
                case 8:
                    ai=muadd(num1,num1);
                    ai=muadd(ai,ai);
                    ai=muadd(ai,ai);
                    break;
                case 9:
                    ai=muadd(num1,num1);
                    ai=muadd(ai,ai);
                    ai=muadd(ai,ai);
                    ai=muadd(ai,num1);
                    break;
                default:
                    printf("Illegal numeric digit: %d\n", mi);
                    acc.to_nan();
                    return acc;
            }
            for (int j=0; j<i; j++) {
                ai.nom += "0";
            }
            acc=muadd(acc,ai);
        }
        acc.pos=(num1.pos==num2.pos);
        return acc;
    }

    munum fac(munum n) {
        munum f(1);
        return f;
    }

   bool mueq(munum num1, munum num2) {
       if (num1.type!=mum_valid || num2.type!=mum_valid) return false;
       if (num1.pos==num2.pos) {
           if (num1.nom==num2.nom) {
               if (num1.den==num2.den) return true;
               else {
                   munum n11(num1.nom), n12(num1.den), n21(num2.nom), n22(num2.den);
                   munum n1,n2;
                   n1=mumul(n11,n22);
                   n2=mumul(n12,n21);
                   if (n1.nom==n2.nom) return true;
               }
           }
       }
       return false;
   }

   bool mugr(munum num1, munum num2) {
       if (num1.type!=mum_valid || num2.type!=mum_valid) return false;
       if (num1.pos != num2.pos) return num1.pos;
       if (num1.den==num2.den) {
           if (num1.nom.length()>num2.nom.length()) return num1.pos;
           if (num1.nom.length()<num2.nom.length()) return !num1.pos;
           for (unsigned int i=0; i<num1.nom.length(); i++) {
               if (num1.nom[i]>num2.nom[i]) return num1.pos;
               if (num1.nom[i]<num2.nom[i]) return !num1.pos;
           }
           return false;
       } else {
           munum n11(num1.nom), n12(num1.den), n21(num2.nom), n22(num2.den);
           munum n1,n2;
           n1=mumul(n11,n22);
           n2=mumul(n12,n21);
           if (n1.nom.length()>n2.nom.length()) return num1.pos;
           if (n1.nom.length()<n2.nom.length()) return !num1.pos;
           for (unsigned int i=0; i<n1.nom.length(); i++) {
               if (n1.nom[i]>n2.nom[i]) return num1.pos;
               if (n1.nom[i]<n2.nom[i]) return !num1.pos;
           }
           return false;
       }
    }

    bool mule(munum num1, munum num2) {
        if (mueq(num1,num2)) return false;
        if (mugr(num1,num2)) return false;
        return true;
    }
};

