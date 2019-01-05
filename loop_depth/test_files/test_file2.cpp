void foo() {
	int x, y, z, i;
	for(x = 0; x < 10; x++) {
		for(y = 0; y < 10; y++) {
			continue;
		}

		for(z = 0; z < 10; z++) {
			continue;
		}
	}

	for(i = 0; i < 10; i++)
		continue;
}

int main() {
	for(int i = 0; i < 10; i++) {
		foo();
	}

	return 0;

}
