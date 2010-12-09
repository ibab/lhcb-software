#include "src/tests.h"

int main(int argc, char** argv) {
  char opt = 'w';
  if ( argc>1        ) opt=argv[1][0];
  if ( opt=='w'      ) test_MemMaps_write();
  else if ( opt=='r' ) test_MemMaps_read();
  else if ( opt=='s' ) test_MemMaps_sharable();
  else if ( opt=='a' ) test_FileMap_scan();
  else if ( opt=='b' ) test_FileMap_write();
  return 1;
}
