#include <iostream>
#include <string>

using namespace std;

int main() {
	string str = "12|123";
	
	string::size_type sz;
	int i_dec = std::stoi(str, &sz);
	unsigned char i_dec2 = std::stoi(str.substr(sz + 1));
	cout << i_dec << endl;
	cout << (unsigned char)stoi(str.substr(sz+1)) << endl;
	return 0;
}
