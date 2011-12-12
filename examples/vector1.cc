#include <cstdlib>
#include <iostream>
#include <vector>
using namespace std;

int main(int argc, char** argv)
{
  int size = atoi(argv[1]);

  vector<int> xs;

  for ( unsigned int i = 0; i < size; ++i )
    xs.push_back(i);

  for ( unsigned int i = 0; i < size; ++i )
    cout << xs[i] << endl;

  return 0;
}
