
void foo(int j) {
    int array[] = {0x1, 0x4, 0x7, 0x9, 0x14, 0x21, 0x4f, 0xff, 0x2a, 0x95};
    int a = array[j];
}

void bar() {
    foo(4);
    foo(2);
}

void bar2() {
    foo(3);
    foo(7);
}

int main() {
    /*int* array = new int[5];
    for(int i = 0; i < 5; i++)
        array[i] = i * 5;

    int a = array[2];

    delete [] array;*/
    
    //foo(6);
    bar();
    bar2();
}
