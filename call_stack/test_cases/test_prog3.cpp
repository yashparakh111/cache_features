#include <iostream>
using namespace std;

int Fib(int* array, int n){
    if(array[n - 1] == 0) {
        array[n - 1] = Fib(array, n-1);
    }

    if(array[n - 2] == 0) {
        array[n - 2] = Fib(array, n-2);
    }

    return array[n-1] + array[n-2];
}

int main() {
    int n = 10;
    int array[n];
    array[0] = 1;
    array[1] = 1;

    int fib_n = Fib(array, n - 1);
    cout << fib_n << endl;

    return 0;
}
