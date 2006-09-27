#ifndef ID_CINTERFACE
#include "hist_types.h"
#define H_IllegalID 1
#ifdef __cplusplus
extern "C"{
#endif
  int hccpc_init(char *);
  void *hccpc_book1(char *name, char *title, int nx, bintype xmin, bintype xmax );
  void *hccpc_profile(char *name, char *title, int nx, bintype xmin, bintype xmax );
  void *hccpc_book2(char *name, char *title, int nx, bintype xmin, bintype xmax, 
					   int ny, bintype ymin, bintype ymax );
	int hfill1 (void *id, bintype x, bintype weight);
	int hfill2 (void *id, bintype x,bintype y, bintype weight);
  int hfillp (void *id, bintype x, bintype y);
#ifdef __cplusplus
}
#endif
#define ID_CINTERFACE
#endif