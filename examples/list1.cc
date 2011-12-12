#include <cstdlib>
#include <iostream>
#include <list>
using namespace std;

int main(int argc, char** argv)
{
  int size = atoi(argv[1]);

  list<int> xs;

  for ( unsigned int i = 0; i < size; ++i )
    xs.push_back(i);

  for ( list<int>::iterator i = xs.begin(), ie = xs.end(); i != ie; ++i )
    cout << *i << endl;

  return 0;
}
