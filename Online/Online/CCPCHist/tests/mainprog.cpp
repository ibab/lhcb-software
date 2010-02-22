#include "CCPCHist.h"
#ifdef WIN32
#include "windows.h"
#define sleep Sleep
#else
#include "unistd.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include "MonSys.h"
#include <string>
int main(int , char** )
{
  Histo *h[4];
  PHisto *p;
  float x,y,d;
  MonSys *s;
  s = (MonSys*)hccpc_init("CC-PC_HSys");
  s->setup("MONITOR");
  h[0] = new Histo("TELL1Mult1_$T3/L2/Q0/myTell","1-dim Histogram 1",100,0.0,100.0);
  h[1] = new Histo("TELL1Mult2_$T3/L1/Q0/myTell","1-dim Histogram 2",100,0.0,200.0);
  h[2] = new Histo("TELL1Mult3_$T3/L2/Q1/myTell","1-dim Histogram 3",100,0.0,300.0);  
	h[3] = new Histo("TELL1Mult4_$T3/L2/Q1/myTell","2-dim Histogram", 100,0.0,10.0,10,0.0,100.0);
  p    = new PHisto("TELL1Mult5_$T3/L2/Q1/myTell","Profile 1",100,0.0,10.0);
  s->start();
  while (1)
  {
    x=(float)rand()/RAND_MAX;
    y=(float)rand()/RAND_MAX;
    d=(float)rand()/RAND_MAX;
    h[0]->fill (100*x, 1.0);
    h[1]->fill (200*x);
    h[2]->fill (300*x);
    h[3]->fill(10*x, 100*y);
    p->fill(10*x,(float)(4*(x-0.5)*(x-0.5)+2*(d-0.5)));
#ifdef WIN32
    int slp = 5;
    sleep(slp);
#else
    int slp = 5000;
    usleep(slp);
#endif
  }
}
