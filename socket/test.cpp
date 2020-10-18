#include <iostream>
#include<string.h>

using namespace std;

int main()
{	
	string input;
	cin >> input;
	char ref[input.size()];
	strcpy(ref, input.c_str());
	
	cout << ref;
	return 0;
}