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

  vector<S*> ss;

  for ( unsigned int i = 0; i < size; ++i )
  {
    S* s = new S();
    s->i_ = i;
    s->d_ = i * 0.2;
    s->c_ = 'a' + i % 26;

    ss.push_back(s);
  }

  for ( unsigned int i = 0; i < size; ++i )
    cout << ss[i]->i_ << " " << ss[i]->d_ << " " << ss[i]->c_ << endl;

  for ( unsigned int i = 0; i < size; ++i )
    delete ss[i];

  return 0;
}
