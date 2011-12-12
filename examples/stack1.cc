#include <iostream>
using namespace std;

void touchheap(int xx)
{
  int* x = new int;
  *x = xx;
  int ret = *x;
  delete x; 

  cout << ret << endl;
}

void foo0() { touchheap(0); }
void foo1() { touchheap(1); }
void foo2() { touchheap(2); }
void foo3() { touchheap(3); }
void foo4() { touchheap(4); }
void foo5() { touchheap(5); }
void foo6() { touchheap(6); }
void foo7() { touchheap(7); }
void foo8() { touchheap(8); }
void foo9() { touchheap(9); }

int main()
{
  foo0();
  foo1();
  foo2();
  foo3();
  foo4();
  foo5();
  foo6();
  foo7();
  foo8();
  foo9();

  return 0;
}

