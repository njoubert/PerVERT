#include <iostream>
#include <list>

using namespace std;

struct S
{
	int x;
	double y;
	char c;
};

int main()
{
	list<int> l1;
	for ( unsigned int i = 0; i < 1024 * 16; ++i )
		l1.push_back(i);

	for ( list<int>::iterator i = l1.begin(), ie = l1.end(); i != ie; ++i )
		cout << (*i) << endl;

	list<S> l2;
	for ( unsigned int i = 0; i < 1024 * 16; ++i )
	{
		S s;
		s.x = i;
		s.y = .2 * i;
		s.c = 'a' + (i % 26);
		l2.push_back(s);
	}

	for ( list<S>::iterator i = l2.begin(), ie = l2.end(); i != ie; ++i )
		cout << (*i).x << " " << (*i).y << " " << (*i).c << endl;

	return 0;
}
		
