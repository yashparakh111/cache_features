void foo() {
	int x, y, z, i;
	for(x = 0; x < 10; x++) {
		for(y = 0; y < 10; y++) {
			continue;
		}

		for(z = 0; z < 10; z++) {
			for(int i = 0; i < 100; i++) {
				i++;
			}
		}
	}

	for(i = 0; i < 10; i++)
		continue;
}

int bar() {
	for(int i = 0; i < 10; i++)
		for(int j = 0;j < 100;j++)
			continue;
}

int main() {
	for(int i = 0; i < 10; i++) {
		foo();
	}

	for(int i = 0; i < 10; i++) {
		bar();
	}

	return 0;

}
