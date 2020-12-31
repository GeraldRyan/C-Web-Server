#include <stdio.h>

int phw()
{
  printf("Hello world\n");
  return 1;
}
int main(void)
{
  int test_data[][2] = {{10}, {10, 13}, {3, 17}, {6, 11}, {10, 5}, {9, 10}, {13}, {2, 19}, {2}, {3}, {5, 25}, {8}, {9, 22}, {5, 5}, {1, 30}, {11}, {9, 12}, {7}, {5}, {8}, {9}, {4, 30}, {9, 3}, {9}, {10}, {10}, {6, 14}, {3, 1}, {3}, {10, 11}, {8}, {2, 14}, {1}, {5}, {4}, {11, 4}, {12, 24}, {5, 18}, {13}, {7, 23}, {8}, {12}, {3, 27}, {2, 12}, {5}, {2, 9}, {13, 4}, {8, 18}, {1, 7}, {6}, {9, 29}, {8, 21}, {5}, {6, 30}, {1, 12}, {10}, {4, 15}, {7, 22}, {11, 26}, {8, 17}, {9, 29}, {5}, {3, 4}, {11, 30}, {12}, {4, 29}, {3}, {9}, {6}, {3, 4}, {1}, {10}, {3, 29}, {10, 28}, {1, 20}, {11, 13}, {3}, {3, 12}, {3, 8}, {10, 9}, {3, 26}, {8}, {7}, {5}, {13, 17}, {2, 27}, {11, 15}, {12}, {9, 19}, {2, 15}, {3, 16}, {1}, {12, 17}, {9, 1}, {6, 19}, {4}, {5}, {5}, {8, 1}, {11, 7}, {5, 2}, {9, 28}, {1}, {2, 2}, {7, 4}, {4, 22}, {7, 24}, {9, 26}, {13, 28}, {11, 26}};
  int len_td = sizeof test_data / sizeof test_data[0];
  printf("length of array is %d\n", len_td);
  int len_ptr = *(&test_data + 1) - test_data;
  printf("length of array pointer arithmatic is %d\n", len_ptr);
  int len1 = sizeof test_data[0] / sizeof test_data[0][0];
  int len2 = sizeof test_data[1] / sizeof test_data[1][0];
  int len1p = *(&test_data[0] + 1) - test_data[0];
  int len2p = *(&test_data[1] + 1) - test_data[1];
  printf("length of small tuple is %d\n", len1);
  printf("length of big tuple is %d\n", len2);
  printf("length of array pointer arithmatic is %d\n", len_ptr);
  printf("length of array small arithmatic is %d\n", len1p);
  printf("length of array big arithmatic is %d\n", len2p);

  int len = 4;
  int array[4] = {1, 2};
  for (int i = 0; i < len; i++)
  {
    printf("Array value at %d: %d\n", i, array[i]);
  }

  int r;
  int *x = phw;
  printf("phw function address: %p, a clone of it %p and its invocation %d\n", phw, x, phw());
  r = phw();
  if (0)
    printf("\nIt's true\n");
  else
    printf("\nit's false\n");
  if (r)
  {
    return 1;
  }

  return 0;
}