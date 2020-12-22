#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "file.h"

/**
 * Loads a file into memory and returns a pointer to the data. 
 * 
 * Buffer is not NUL terminated.
 */

struct file_data *file_load(char *filename)
{
  char *buffer, *p;
  int bytes_read, bytes_remaining, total_bytes = 0;

  // get the file size
  if (stat(filename, &buf) == -1){
    
  } 
}