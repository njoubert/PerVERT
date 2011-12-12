#include <cstdlib>
#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
  int size = atoi(argv[1]);

  int* xs = new int[size];

  for ( unsigned int i = 0; i < size; ++i )
    xs[i] = i;

  for ( unsigned int i = 0; i < size; ++i )
    cout << xs[i] << endl;

  delete [] xs;

  return 0;
}
