#include <stdio.h>
#include <stdlib.h>
#define DEBUG 1

typedef struct
{
  int current_stack_size;
  int *stack;
  int *top_of_stack_ptr;
  int *leading;
} MinStack;

/** initialize your data structure here. */

MinStack *minStackCreate()
{
  MinStack *stack = malloc(sizeof(MinStack));
  stack->current_stack_size = 0;
  stack->stack = malloc(sizeof(int));
  stack->top_of_stack_ptr = stack->stack;
  stack->leading = stack->stack;
  return stack;
}

void minStackPush(MinStack *obj, int x)
{
  if (obj->current_stack_size == 0)
  {
    *(obj->stack) = x;
    obj->current_stack_size++;
    obj->leading += sizeof(int);
    return;
  }
  else if(obj->current_stack_size > 0){
    obj->current_stack_size++;
    obj->stack = realloc(obj->stack, sizeof(int) * (obj->current_stack_size+1));
    *(obj->leading) = x;
    obj->leading += sizeof(int);
    obj->top_of_stack_ptr += sizeof(int);
  }
  // printf("Memory address of stack %p and top of stack pointer %p\n", obj->stack, obj->top_of_stack_ptr);
  // printf("New Memory address of stack %p and top of stack pointer %p\n", obj->stack, obj->top_of_stack_ptr);
}

void minStackPop(MinStack *obj)
{
  obj->current_stack_size--;
  obj->stack = realloc(obj->stack, obj->current_stack_size); // automatically 'deletes' top
  obj->top_of_stack_ptr = obj->top_of_stack_ptr - sizeof(int);
}

int minStackTop(MinStack *obj)
{ // pikachu
  int top = *obj->top_of_stack_ptr;
  return top;
}

int minStackGetMin(MinStack *obj)
{ // pikachu
  // int running_min;
  // if (DEBUG)

  //   printf("Running Get Min.\n");
  //   printf("deferencing object stack at base %d\n", *(obj->stack));

  // for (int i = 0; i < 3; i++)
  // {
  //   printf("i is %d\n", i);
  //   if (i = 0)
  //   {
  //     running_min = *(obj->stack);
  //   }
  //   // if (*(obj->stack + (i * sizeof(int))) < running_min)
  //   // {
  //   //   running_min = *(obj->stack + (i * sizeof(int)));
  //   // }
  //   // if (DEBUG){
  //   //   printf("i:%d, running_min: %d", i, running_min);}
  // }
  return 0;
}

void minStackFree(MinStack *obj)
{
  free(obj->stack);
  free(obj);
}

void print_state(MinStack *stack, char *op)
{
  printf("Current Stack Size: %d\n", stack->current_stack_size);
  printf("Stack Size:%d Last Operation: %s\n", stack->current_stack_size, op);
  for (int i = 0; i < stack->current_stack_size; i++)
  {
    // int elevator = sizeof(int) *i;
    int value = 1; //*((stack->stack) + sizeof(int) * i);
    printf("%d:%d\n", i, value);
  }
  return;
}

int main(void)
{
  MinStack *stack = minStackCreate();
  minStackPush(stack, -2);
  print_state(stack, "pushed -2");
  minStackPush(stack, 0);
  print_state(stack, "pushed 0");
  minStackPush(stack, -3);
  print_state(stack, "pushed -3");
  int min = minStackGetMin(stack);
  print_state(stack, "got min expect -3");
  printf("Min from last op %d\n", min);
  minStackPop(stack);
  print_state(stack, "popped");
  int top = minStackTop(stack);
  print_state(stack, "Looked at top");
  printf("Top from last op %d\n", top);
  minStackGetMin(stack);
  print_state(stack, "got min expect ??");

  return 0;
}
/**
 * Your MinStack struct will be instantiated and called as such:
 * MinStack* obj = minStackCreate();
 * minStackPush(obj, x);
 
 * minStackPop(obj);
 
 * int param_3 = minStackTop(obj);
 
 * int param_4 = minStackGetMin(obj);
 
 * minStackFree(obj);
*/