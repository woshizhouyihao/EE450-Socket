#include <iostream>
#include<string.h>
#include <stdio.h>
#include <map>
#include <sstream>
#include <iomanip>

using namespace std;

int main()
{	
	map<string, string> m;
	m.insert(pair<string, string>("c1asd", "0"));
	m.insert(pair<string, string>("c2s", "1"));
	m.insert(pair<string, string>("c3qqq", "0"));
	m.insert(pair<string, string>("c4sdasdas", "0"));
	m.insert(pair<string, string>("c5sasd", "1"));

	string t1 = "";
	string t2 = "";
	for(map<string,string>::iterator it=m.begin(); it != m.end(); it++) {
		if(it->second == "0")
			t1 += it->first + " ";
		else
			t2 += it->first + " ";
	}

	istringstream s1(t1);
	istringstream s2(t2);
	cout << left << setfill(' ') << setw(15) << "Server A" << "|Server B" << endl;
	string w1, w2;
	while((!s1.eof() || !s2.eof())) {
		s1 >> w1;
		s2 >> w2;
		if(s1.eof())
			w1 = " ";
		if(s2.eof())
			w2 = " ";
		if(w1 != " " || w2 != " ")
			cout << left << setfill(' ') << setw(15) << w1 << "|" << w2 << endl;
	}
	
	return 0;
}