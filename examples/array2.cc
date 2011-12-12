#include <cstdlib>
#include <iostream>
using namespace std;

struct S
{
  int i_;
  double d_;
  char c_;
};

int main(int argc, char** argv)
{
  int size = atoi(argv[1]);
  
  S* ss = new S[size];

  for ( unsigned int i = 0; i < size; ++i )
  {
    ss[i].i_ = i;
    ss[i].d_ = i * 0.2;
    ss[i].c_ = 'a' + i % 26;
  }

  for ( unsigned int i = 0; i < size; ++i )
    cout << ss[i].i_ << " " << ss[i].d_ << " " << ss[i].c_ << endl;

  delete [] ss;

  return 0;
}
