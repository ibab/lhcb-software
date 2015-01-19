#define main checkpointing_main
#include "gentest.cpp"
#undef main
extern void CheckpointRestoreWrapper__init_instance(int argc, char** argv);

int main (int argc, char** argv)  {
  CheckpointRestoreWrapper__init_instance(argc, argv);
  return checkpointing_main(argc,argv);
}
