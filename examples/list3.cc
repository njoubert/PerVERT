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

  list<S*> ss;

  for ( unsigned int i = 0; i < size; ++i )
  {
    S* s = new S();
    s->i_ = i;
    s->d_ = i * 0.2;
    s->c_ = 'a' + i % 26;

    ss.push_back(s);
  }

  for ( list<S*>::iterator i = ss.begin(), ie = ss.end(); i != ie; ++i )
    cout << (*i)->i_ << " " << (*i)->d_ << " " << (*i)->c_ << endl;

  for ( list<S*>::iterator i = ss.begin(), ie = ss.end(); i != ie; ++i )
    delete *i;

  return 0;
}
