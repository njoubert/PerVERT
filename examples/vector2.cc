#include <cstdlib>
#include <iostream>
#include <vector>
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

  vector<S> ss;

  for ( unsigned int i = 0; i < size; ++i )
  {
    ss.push_back(S());

    ss[i].i_ = i;
    ss[i].d_ = i * 0.2;
    ss[i].c_ = 'a' + i % 26;
  }

  for ( unsigned int i = 0; i < size; ++i )
    cout << ss[i].i_ << " " << ss[i].d_ << " " << ss[i].c_ << endl;

  return 0;
}
