// Delegate memory management to stl containers.
// Insert elements into a vector (should cause automatic resizing).
// Read element from the vector.
// Free the memory when the vector goes out of scope.

#include <iostream>
#include <vector>

using namespace std;

int main()
{
  vector<int> xs;
  for ( unsigned int i = 0; i < 10; ++i )
    xs.push_back(i);

  for ( unsigned int i = 0; i < 10; ++i )
    cout << xs[i] << endl;

  return 0;
}
