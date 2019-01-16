void foo() {
	int x, y, z, i;
	for(x = 0; x < 2; x++) {
		for(y = 0; y < 2; y++) {
			continue;
		}

		for(z = 0; z < 2; z++) {
			for(int i = 0; i < 1; i++) {
				i++;
			}
		}
	}

	for(i = 0; i < 3; i++)
		continue;
}

int bar() {
	for(int i = 0; i < 4; i++)
		for(int j = 0;j < 1;j++) {
			int a = i + j;
		}
}

int main() {
	for(int i = 0; i < 2; i++) {
		foo();
	}

	for(int i = 0; i < 2; i++) {
		bar();
	}

	return 0;

}
