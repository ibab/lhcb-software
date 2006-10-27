#include "CCPCHist.h"
#ifdef WIN32
#include "windows.h"
#define sleep Sleep
#else
#include "unistd.h"
#endif
#include <stdio.h>
#include <stdlib.h>
int main(int , char** )
{
  Histo *h[4];
  PHisto *p;
  float x,y,d;
  hccpc_init("CC-PC_HSys");
  h[0] = new Histo("Hist1","1-dim Histogram 1",100,0.0,100.0);
  h[1] = new Histo("Hist2","1-dim Histogram 2",100,0.0,200.0);
  h[2] = new Histo("Hist3","1-dim Histogram 3",100,0.0,300.0);
  h[3] = new Histo("Hist7","2-dim Histogram", 100,0.0,10.0,10,0.0,100.0);
  p    = new PHisto("Phist1","Profile 1",100,0.0,10.0);
  while (1)
	{
    x=(float)rand()/RAND_MAX;
    y=(float)rand()/RAND_MAX;
    d=(float)rand()/RAND_MAX;
		h[0]->fill (100*x, 1.0);
		h[1]->fill (200*x);
		h[2]->fill (300*x);
		h[3]->fill(10*x, 100*y);
    p->fill(10*x,4*(x-0.5)*(x-0.5)+2*(d-0.5));
#ifdef WIN32
    int slp = 5;
    sleep(slp);
#else
    int slp = 5000;
    usleep(slp);
#endif
	}
}
