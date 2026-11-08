int nandGate(int a , int b){
    int table[2][2] = {{1, 1}, {1, 0}};
    return table[a][b];
}

bool trueBit(int a){
    if(a == 0) return true;
    if(a == 1) return true;

    return false;
}

int notGate(int a){
    return nandGate(a, a);
}
int andGate(int a, int b){
    int res = nandGate(a, b);
    return nandGate(res, res);
}
int orGate(int a, int b){
    int res1 = nandGate(a, a);
    int res2 = nandGate(b, b);

    return nandGate(res1, res2);
}
int xorGate(int a, int b){
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
HalfAdderResult halfAdder(int a, int b){
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
FullAdderResult fullAdder(int a, int b, int cinput){
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

AdderResult add4bits(fourBit a, fourBit b, int cinput){
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

AdderResult sub4bits(fourBit a, fourBit b){
    fourBit nb;
    for(int i = 0; i < 4; i++){
        nb.bit[i] = notGate(b.bit[i]);
    }
    return add4bits(a, nb, 1);
}

struct eightBit
{
    int bit[8];
};

eightBit mul4bits(fourBit a, fourBit b){
    eightBit acc= {{0, 0, 0, 0, 0, 0, 0, 0}};
    for(int i = 0; i < 4; i++){
        int pp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        for(int j = 0; j < 4; j++){
            pp[i+j] = andGate(a.bit[j], b.bit[i]);
       }
       fourBit accLow, accHigh, ppLow, ppHigh;
       for(int k = 0; k < 4; k++){
        accLow.bit[k] = acc.bit[k];
        accHigh.bit[k] = acc.bit[k+4];
        ppLow.bit[k] = pp[k];
        ppHigh.bit[k] = pp[k+4];
       }
       AdderResult low = add4bits(accLow, ppLow, 0);
       AdderResult high = add4bits(accHigh, ppHigh, low.carry);

       for(int k = 0; k < 4; k++){
            acc.bit[k] = low.sum.bit[k];
            acc.bit[k+4] = high.sum.bit[k];
       }
    }
    return acc;
}

void toBits(int n, fourBit &b){
    for(int i = 0; i < 4; i++){
        b.bit[i] = n%2;
        n = n/2;
    }
}

int toInt(fourBit n)
{
    int value = 0;
    for(int i = 3; i >= 0; i--){
        value = value * 2 + n.bit[i];
    }
    return value;
}

int toInt8(eightBit n)
{
    int value = 0;
    for(int i = 7; i >= 0; i--){
        value = value * 2 + n.bit[i];
    }
    return value;
}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define EXPORT extern "C" EMSCRIPTEN_KEEPALIVE
#else
#define EXPORT extern "C"
#endif

int packBits(const int *bits, int n){
    int v = 0;
    for(int i = n - 1; i >= 0; i--){
        v = v*2 + bits[i];
    }
    return v;
}
bool validInput(int a , int b){
    return a >= 0 && a <= 15 && b >= 0 && b <= 15;
}

EXPORT int alu_bits(int op, int x, int y){
    if(!validInput(x, y)) return -1;
    fourBit a, b;
    toBits(x, a);
    toBits(y, b);

    if(op == 0){
        AdderResult r = add4bits(a, b, 0);
        int bits[5] = {r.sum.bit[0], r.sum.bit[1], r.sum.bit[2], r.sum.bit[3], r.carry};
        return packBits(bits, 5);
    }
    if(op == 1){
        AdderResult r = sub4bits(a, b);
        return packBits(r.sum.bit, 4);
    }
    eightBit p = mul4bits(a, b);
    return packBits(p.bit, 8);
}

EXPORT int alu_value(int op, int x, int y){
    if(!validInput(x, y)) return -1;
    fourBit a, b;
    toBits(x, a);
    toBits(y, b);

    if(op == 0){
        AdderResult r = add4bits(a, b, 0);
        return toInt(r.sum) + r.carry * 16;
    }
    if(op == 1){
        AdderResult r = sub4bits(a, b);
        if(r.carry == 0){
            return toInt(r.sum) - 16;
        }
        return toInt(r.sum);

    }
    return toInt8(mul4bits(a, b));
}
EXPORT int alu_borrow(int x, int y){
    if(!validInput(x, y)) return -1;
    fourBit a, b;
    toBits(x, a);
    toBits(y, b);

    AdderResult r = sub4bits(a, b);
    return notGate(r.carry);
}