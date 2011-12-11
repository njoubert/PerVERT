// Delegate memory management to stl containers.
// Insert elements into a vector (should cause automatic resizing).
// Read element from the vector.
// Free the memory when the vector goes out of scope.

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>

using namespace std;

int main()
{
  list<int> l;
  for ( unsigned int i = 0; i < 4096; ++i )
    l.push_back(i);

  for ( list<int>::iterator i = l.begin(), ie = l.end(); i != ie; ++i )
    cout << *i << endl;

  vector<int> xs;
  for ( unsigned int i = 0; i < 4096; ++i )
    xs.push_back(i);

  for ( unsigned int i = 0; i < 4096; ++i )
    cout << xs[i] << endl;

  string s;
  map<int, string> m;

  for ( unsigned int i = 0; i < 1024; ++i )
  {
    s += "x";
    m[i] = s;
  }  

  for ( map<int, string>::iterator i = m.begin(), ie = m.end(); i != ie; ++i )
    cout << (*i).first << " " << (*i).second << endl;

  return 0;
}
