#define main checkpointing_main
#include "main.cpp"
#undef main
class CheckpointRestoreWrapper;
extern CheckpointRestoreWrapper& CheckpointRestoreWrapper__instance();

int main (int argc, char** argv)  {
  CheckpointRestoreWrapper__instance();
  return checkpointing_main(argc,argv);
}
