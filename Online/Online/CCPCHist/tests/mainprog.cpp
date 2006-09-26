#include "CCPCHist/CCPCHist.h"
#include "src/dimhist.h"

int main(int /* argc */, char** /* argv */ )
{
  //HSys hs();
  HSys &hsi=HSys::instance();
//  hs  = new HSys();
  hsi.setname("CC-PC_HSys");
  hsi.serv->autoStartOn();
  printf("%s\n",hsi.serv->getDnsNode());
  hsi.start();
  Histo *h[10];

  h[0] = new Histo("Hist1","1-dim Histogram 1",100,0.0,100.0);
  h[1] = new Histo("Hist2","1-dim Histogram 2",100,0.0,200.0);
  h[2] = new Histo("Hist3","1-dim Histogram 3",100,0.0,300.0);
  h[3] = new Histo("Hist4","1-dim Histogram 4",100,0.0,400.0);
  h[4] = new Histo("Hist5","1-dim Histogram 5",100,0.0,500.0);
  h[5] = new Histo("Hist6","1-dim Histogram 6",100,0.0,600.0);

	int i;
	for (i=0;i<100;i++)
	{
		h[0]->fill ((bintype)i, (bintype)1.0);
		h[1]->fill ((bintype)(2*i),(bintype)i);
		h[2]->fill ((bintype)(3*i));
		h[3]->fill ((bintype)(4*i));
		h[4]->fill ((bintype)(5*i));
		h[5]->fill ((bintype)(6*i));
	}
  while (1)
  {
    sleep(150);
    //for (i=0;i<5;i++)
    //{
    //  h[i]->serv->serviceHandler();
    //}
  }
}
