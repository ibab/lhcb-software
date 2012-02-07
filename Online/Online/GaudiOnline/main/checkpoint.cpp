#define main checkpointing_main
#include "main.cpp"
#undef main
extern void CheckpointRestoreWrapper__init_instance(int argc, char** argv);

int main (int argc, char** argv)  {
  volatile int *x = new int; //force loading of tcmalloc
  *x = 0;
  CheckpointRestoreWrapper__init_instance(argc, argv);
  return checkpointing_main(argc,argv);
}
