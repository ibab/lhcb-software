#include "CCPCHist.h"
#include <stdlib.h>
#ifdef WIN32
#include <Windows.h>
#define sleep Sleep
#else
#include <unistd.h>
#endif
int main(void /*int argc, char **argv*/)
{
  void *h[3];
  float x,y;
  hccpc_init("CC-PC_HSys");
  h[0] = hccpc_book1("Hist1","1-dim Histogram 1",100,0.0,100.0);
  h[1] = hccpc_book1("Hist2","1-dim Histogram 2",100,0.0,200.0);
  h[2] = hccpc_book1("Hist3","1-dim Histogram 3",100,0.0,300.0);
  h[3] = hccpc_book2("Hist4","2-dim Histogram",100,0.0,100.0, 10, 0.0, 100.0);

  while (1)
  {
    x=(float)rand()/RAND_MAX;
    y=(float)rand()/RAND_MAX;
		hfill1 (h[0],(bintype)100*x, (bintype)1.0);
		hfill1 (h[1],(bintype)(200*x),(bintype)1.0);
		hfill1 (h[2],(bintype)(300*x), (bintype)1.0);
		hfill2 (h[3],(bintype)(100*x), (bintype)(100*y), (bintype)1.0);
#ifdef WIN32
    int slp = 10;
    sleep(slp);
#else
    int slp = 10000;
    usleep(slp);
#endif
	}
}
