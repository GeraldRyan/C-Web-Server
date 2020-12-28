#include <stdio.h>

int phw()
{
  printf("Hello world\n");
  return 1;
}
int main(void)
{
  int r;
  int *x = phw;
  printf("phw function address: %p, a clone of it %p and its invocation %d\n", phw, x, phw());
  r = phw();
  if (r)
  {
    return 1;
  }
  return 0;
}