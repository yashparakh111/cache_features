//#include <iostream>
//using namespace std;

int Fib(int n) {
    if(n <= 2) return 1;
    return Fib(n - 1) + Fib(n - 2);
}

int main() {
    int a = Fib(5);
    //cout << a << endl;
    return 0;
}
