void foo(int j) {
    int array[10];
    for(int i = 0; i < 10; i++)
        array[i] = 12134;

    //return array[5];
}

void bar() {
    foo(1);
    //foo(2);
}

void bar2() {
    foo(3);
    foo(10);
}

int main() {
    /*int* array = new int[5];
    for(int i = 0; i < 5; i++)
        array[i] = i * 5;

    int a = array[2];

    delete [] array;*/
    
    foo(6);
    //bar();
    //bar2();
}
