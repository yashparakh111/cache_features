//#include <iostream>
//using namespace std;

int Fib(int n) {
    if(n <= 2) return 1;
    return Fib(n - 1) + Fib(n - 2);
}

int main() {
    int n = 6;
    int a = Fib(n);
    //cout << a << endl;
    return 0;
}
