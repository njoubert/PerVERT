#include <cstdlib>
#include <iostream>
#include <list>
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

  list<S> ss;

  for ( unsigned int i = 0; i < size; ++i )
  {
    ss.push_back(S());

    ss.back().i_ = i;
    ss.back().d_ = i * 0.2;
    ss.back().c_ = 'a' + i % 26;
  }

  for ( list<S>::iterator i = ss.begin(), ie = ss.end(); i != ie; ++i )
    cout << i->i_ << " " << i->d_ << " " << i->c_ << endl;

  return 0;
}
