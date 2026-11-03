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

    AdderResult r = add4bits(a, b, 0);


    cout << "Sum:";
    print(r.sum);
    cout << endl;
    cout << "Carry: " << r.carry << endl;

    return 0;
}