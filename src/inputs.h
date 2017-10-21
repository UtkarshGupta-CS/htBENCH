#ifndef INPUT_H

// struct to pass args to various modules
typedef struct arg_struct
{
  int argc;
  char **argv;
  int reqCounter;
  int startTime;
} arg_struct;

#endif // !INPUT_H