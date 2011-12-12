// Allocates, reads, writes, and frees from within function calls

int memfuckfest(int x)
{
  int* xs = new int[10];
  xs[0] = x;
  int ret = xs[0];
  delete [] xs; 

  return ret;
}

int foo1()
{
  return memfuckfest(1);
}

int foo2()
{
  return memfuckfest(2);
}

int foo3()
{
  return memfuckfest(3);
}

int main()
{
  foo1();
  foo2();
  foo3();

  return 0;
}

