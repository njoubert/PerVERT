#include <cstdlib>
#include <iostream>
#include <map>
using namespace std;

int main(int argc, char** argv)
{
  int size = atoi(argv[1]);

  map<int, int> xs;

  for ( unsigned int i = 0; i < size; ++i )
    xs[i] = i;

  for ( unsigned int i = 0; i < size; ++i )
    cout << xs[i] << endl;

  return 0;
}
