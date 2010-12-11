#include <cstring>


int test_FileMap_scan();
int test_FileMap_memory();
int test_FileMap_write();
int test_FileMap_read();

int test_MemMaps_read();
int test_MemMaps_write();
int test_MemMaps_sharable();

int test_Process_write();
int test_Process_read();
int test_Process_restore();

int test_thread_checkpoint();
int test_thread_restore();

int main(int argc, char** argv) {
  char *opt = "None";
  if ( argc>1        ) opt = argv[1];
  
  if      ( strncmp(opt,"m_write",  7) ==0 ) test_MemMaps_write();
  else if ( strncmp(opt,"m_read",   6) ==0 ) test_MemMaps_read();
  else if ( strncmp(opt,"m_share",  7) ==0 ) test_MemMaps_sharable();

  else if ( strncmp(opt,"f_scan",   6) ==0 ) test_FileMap_scan();
  else if ( strncmp(opt,"f_memory", 8) ==0 ) test_FileMap_memory();
  else if ( strncmp(opt,"f_write",  7) ==0 ) test_FileMap_write();
  else if ( strncmp(opt,"f_read",   6) ==0 ) test_FileMap_read();

  else if ( strncmp(opt,"p_write",  7) ==0 ) test_Process_write();
  else if ( strncmp(opt,"p_read",   6) ==0 ) test_Process_read();
  else if ( strncmp(opt,"p_restore",6) ==0 ) test_Process_restore();

  else if ( strncmp(opt,"t_checkpo",6) ==0) test_thread_checkpoint();
  else if ( strncmp(opt,"t_restore",6) ==0) test_thread_restore();

  return 1;
}
