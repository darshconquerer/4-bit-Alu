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
struct halfAdderResult
{
    int sum;
    int carry;
};
halfAdderResult halfAdder(int a, int b){
    halfAdderResult res;
    int n = nandGate(a, b);
    int x = nandGate(a, n);
    int y = nandGate(b, n);
    res.sum   = nandGate(x, y);
    res.carry = nandGate(n, n);
    return res;
}
int main(){
    int a, b;
    cout << "First input(Input must be binary either 0 or 1): ";
    cin>>a;
    cout << "Second input(Input must be binary either 0 or 1): ";
    cin>>b;

    if(trueBit(a)){
        if(trueBit(b)){
        //    cout << xorGate(a, b) << endl;
        halfAdderResult r = halfAdder(a, b);
        cout << "Sum: " << r.sum << endl << "Carry: " << r.carry << endl;
        return 0;
        }
    }
cout << "Inputs must be 0 or 1" <<endl;
return 1;
}
