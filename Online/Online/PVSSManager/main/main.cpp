#include "unistd.h"
namespace PVSS {
  int pvss_exec_manager(int argc, char **argv);
}
int main (int argc, char** argv)  {
  return PVSS::pvss_exec_manager(argc,argv);
}
