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

  S** ss = new S*[size];

  for ( unsigned int i = 0; i < size; ++i )
  {
    S* s = new S();
    s->i_ = i;
    s->d_ = i * 0.2;
    s->c_ = 'a' + i % 26;

    ss[i] = s;
  }

  for ( unsigned int i = 0; i < size; ++i )
    cout << ss[i]->i_ << " " << ss[i]->d_ << " " << ss[i]->c_ << endl;

  for ( unsigned int i = 0; i < size; ++i )
    delete ss[i];

  delete [] ss;

  return 0;
}
