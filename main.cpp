#include <iostream>
using namespace std;

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

struct MultiplierResult
{
    fourBit product;
    int overflow;
};

MultiplierResult mul4bits(fourBit a, fourBit b){
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

bool readBits(fourBit &n, const char *name){
    cout << "Enter " << name << " in 4 bits: ";
    for(int i = 3; i >= 0; i--){
        cin >> n.bit[i];
        if(!trueBit(n.bit[i])){
            return false;
        }
    }
    return true;
}

void print(fourBit n){
    for(int i = 3; i >= 0; i--){
        cout << n.bit[i];
    }
}

int main(){
    fourBit a, b;

    if(!readBits(a, "A") || !readBits(b, "B")){
        cout << "Inputs must be 0 or 1" << endl;
        return 1;
    }

    MultiplierResult r = mul4bits(a, b);


    cout << "Product bits:";
    print(r.product);
    cout << endl;
    cout << "Overflow: " << r.overflow << endl;

    return 0;
}