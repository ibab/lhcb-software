#include "src/CInterface.h"
#include <Windows.h>

int main(int argc, char **argv)
{
  void *h[6];
	int i;
  hccpc_init("CC-PC_HSys");
  h[0] = hccpc_book1("Hist1","1-dim Histogram 1",100,0.0,100.0);
  h[1] = hccpc_book1("Hist2","1-dim Histogram 2",100,0.0,200.0);
  h[2] = hccpc_book1("Hist3","1-dim Histogram 3",100,0.0,300.0);
  h[3] = hccpc_book1("Hist4","1-dim Histogram 4",100,0.0,400.0);
  h[4] = hccpc_book1("Hist5","1-dim Histogram 5",100,0.0,500.0);
  h[5] = hccpc_book1("Hist6","1-dim Histogram 6",100,0.0,600.0);

	for (i=0;i<100;i++)
	{
		hfill1 (h[0],(bintype)i, (bintype)1.0);
		hfill1 (h[1],(bintype)(2*i),(bintype)i);
		hfill1 (h[2],(bintype)(3*i), (bintype)1.0);
		hfill1 (h[3],(bintype)(4*i), (bintype)1.0);
		hfill1 (h[4],(bintype)(5*i), (bintype)1.0);
		hfill1 (h[5],(bintype)(6*i), (bintype)1.0);
	}
  while (1)
  {
    Sleep(150);
  }
}