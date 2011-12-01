// Allocates ten integers on the heap.
// Reads all ten.
// Writes all ten.

int main()
{
  int* xs = new int[10];

  for ( unsigned int i = 0; i < 10; ++i )
    xs[i] = i;

  int count = 0;
  for ( unsigned int i = 0; i < 10; ++i )
    count += xs[i];

  delete [] xs;

  return count;
}
