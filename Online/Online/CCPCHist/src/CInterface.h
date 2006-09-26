#ifndef ID_CINTERFACE
#include "hist_types.h"
#define H_IllegalID 1
extern "C"
{
  int ccpc_init();
  void *hccpc_book1(char *name, char *title, int nx, bintype xmin, bintype xmax );
  void *hccpc_profile(char *name, char *title, int nx, bintype xmin, bintype xmax );
  void *hccpc_book2(char *name, char *title, int nx, bintype xmin, bintype xmax, 
					   int ny, bintype ymin, bintype ymax );
	int fill1 (void *id, bintype x, bintype weight);
	int fill2 (void *id, bintype x,bintype y, bintype weight);
  int fillp (void *id, bintype x, bintype y);
}
#define ID_CINTERFACE
#endif