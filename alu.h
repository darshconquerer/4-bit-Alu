#pragma once

inline int nandGate(int a , int b){
    int table[2][2] = {{1, 1}, {1, 0}};
    return table[a][b];
}

inline int notGate(int a){
    return nandGate(a, a);
}
inline int andGate(int a, int b){
    int res = nandGate(a, b);
    return nandGate(res, res);
}
inline int orGate(int a, int b){
    int res1 = nandGate(a, a);
    int res2 = nandGate(b, b);

    return nandGate(res1, res2);
}
inline int xorGate(int a, int b){
    int res1 = nandGate(a, b);
    int res2 = nandGate(a, res1);
    int res3 = nandGate(b, res1);
    int output = nandGate(res2, res3);

    return output;
}
struct HalfAdderResult
{
    int sum;
    int carry;
};
inline HalfAdderResult halfAdder(int a, int b){
    HalfAdderResult res;
    int n = nandGate(a, b);
    int x = nandGate(a, n);
    int y = nandGate(b, n);
    res.sum   = nandGate(x, y);
    res.carry = nandGate(n, n);
    return res;
}
struct FullAdderResult{
    int sum;
    int carry;
};
inline FullAdderResult fullAdder(int a, int b, int cinput){
    FullAdderResult res;
    HalfAdderResult h1= halfAdder(a, b);
    HalfAdderResult h2 = halfAdder(h1.sum, cinput);
    res.sum = h2.sum;
    res.carry = orGate(h1.carry, h2.carry);

    return res;
}
struct fourBit{
    int bit[4];
};
struct AdderResult{
    fourBit sum;
    int carry;
};

inline AdderResult add4bits(fourBit a, fourBit b, int cinput){
    AdderResult result;
    int carry = cinput;

    for(int i = 0; i < 4; i++){
        FullAdderResult fa = fullAdder(a.bit[i], b.bit[i], carry);
        result.sum.bit[i] = fa.sum;
        carry = fa.carry;
    }
    result.carry = carry;
    return result;
}

inline AdderResult sub4bits(fourBit a, fourBit b){
    fourBit nb;
    for(int i = 0; i < 4; i++){
        nb.bit[i] = notGate(b.bit[i]);
    }
    return add4bits(a, nb, 1);
}

struct MultiplierResult
{
    fourBit product;
    int overflow;
};

inline MultiplierResult mul4bits(fourBit a, fourBit b){
    int acc[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    for(int i = 0; i < 4; i++){
        int pp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        for(int j = 0; j < 4; j++){
            pp[i+j] = andGate(a.bit[j], b.bit[i]);
       }
       fourBit accLow, accHigh, ppLow, ppHigh;
       for(int k = 0; k < 4; k++){
        accLow.bit[k] = acc[k];
        accHigh.bit[k] = acc[k+4];
        ppLow.bit[k] = pp[k];
        ppHigh.bit[k] = pp[k+4];
       }
       AdderResult low = add4bits(accLow, ppLow, 0);
       AdderResult high = add4bits(accHigh, ppHigh, low.carry);

       for(int k = 0; k < 4; k++){
            acc[k] = low.sum.bit[k];
            acc[k+4] = high.sum.bit[k];
       }
    }
    MultiplierResult result;
    for(int k = 0; k < 4; k++){
        result.product.bit[k] = acc[k];
    }

    int over = acc[4];
    over = orGate(over, acc[5]);
    over = orGate(over, acc[6]);
    over = orGate(over, acc[7]);
    result.overflow = over;

    return result;
}

inline void toBits(int n, fourBit &b){
    for(int i = 0; i < 4; i++){
        b.bit[i] = n%2;
        n = n/2;
    }
}

inline int toInt(fourBit n)
{
    int value = 0;
    for(int i = 3; i >= 0; i--){
        value = value * 2 + n.bit[i];
    }
    return value;
}

inline void compute(int op, const int *bA, const int *bB, fourBit &out, int &flag){
    fourBit A, B;
    for(int i = 0; i < 4; i++){
        A.bit[i] = bA[i];
        B.bit[i] = bB[i];

        if(op == 0){
            AdderResult r = add4bits(A, B, 0);
            out = r.sum;
            flag = r.carry;
        }
        else if(op == 1){
            AdderResult r = sub4bits(A, B);
            out = r.sum;
            flag = r.carry;
        }
        else{
            MultiplierResult r = mul4bits(A, B);
            out = r.product;
            flag = r.overflow;
        }
    }
}