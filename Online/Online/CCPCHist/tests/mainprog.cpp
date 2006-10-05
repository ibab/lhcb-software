#include "CCPCHist/CCPCHist.h"
#ifdef WIN32
#include "windows.h"
#define sleep Sleep
#else
#include "unistd.h"
#endif
#include <stdio.h>
#include <stdlib.h>
int main(int /* argc */, char** /* argv */ )
{
  //HSys hs();
  Histo *h[10];
  PHisto *p;

  //HSys &hsi=HSys::instance();
  //hsi.autoinit("CC-PC_HSys");
  hccpc_init("CC-PC_HSys");
  h[0] = new Histo("Hist1","1-dim Histogram 1",100,0.0,100.0);
  h[1] = new Histo("Hist2","1-dim Histogram 2",100,0.0,200.0);
  h[2] = new Histo("Hist3","1-dim Histogram 3",100,0.0,300.0);
  h[3] = new Histo("Hist4","1-dim Histogram 4",100,0.0,400.0);
  h[4] = new Histo("Hist5","1-dim Histogram 5",100,0.0,500.0);
  h[5] = new Histo("Hist6","1-dim Histogram 6",100,0.0,600.0);
  p    = new PHisto("Phist1","Profile 1",100,0.0,10.0);
	int i;
	for (i=0;i<100;i++)
	{
    float x=(float)rand()/RAND_MAX;
    float d=(float)rand()/RAND_MAX;
		h[0]->fill ((bintype)100*x, (bintype)1.0);
		h[1]->fill ((bintype)(200*x),(bintype)i);
		h[2]->fill ((bintype)(300*x));
		h[3]->fill ((bintype)(400*x));
		h[4]->fill ((bintype)(500*x));
		h[5]->fill ((bintype)(600*x));
    p->fill((bintype)(10.0*x),(bintype)(x*x+(d-0.5)*2.0));
	}
  while (1)
  {
    sleep(150);
  }
}
