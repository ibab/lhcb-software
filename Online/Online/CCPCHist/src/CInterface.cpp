#include "CInterface.h"
#include "dimhist.h"

  int hccpc_init(char *nam)
  {
    static int inited=0;
    if (inited == 0)
    {
      HSys &hsi=HSys::instance();
      hsi.setname(nam);
      hsi.serv->autoStartOn();
      hsi.start();
      inited =1;
    }
    return 0;
  }
  void *hccpc_book1(char *name, char *title, int nx, bintype xmin, bintype xmax )
  {
    Histo *h;
    h = new Histo(name,title,nx,xmin,xmax);
    return h;
  }
  void *hccpc_profile(char *name, char *title, int nx, bintype xmin, bintype xmax )
  {
    PHisto *h;
    h = new PHisto(name,title,nx,xmin,xmax);
    return h;
  }
  void *hccpc_book2(char *name, char *title, int nx, bintype xmin, bintype xmax, 
					   int ny, bintype ymin, bintype ymax )
  {
    Histo *h;
    h = new Histo(name,title,nx,xmin,xmax, ny, ymin,ymax);
    return h;
  }
	int hfill1 (void *id, bintype x, bintype weight)
  {
    Histo *h = (Histo *)id;
    if (h->type == H_1DIM)
    {
      return h->fill(x, weight);
    }
    return H_IllegalID;
  }
	int hfill2 (void *id, bintype x,bintype y, bintype weight)
  {
    Histo *h = (Histo *)id;
    if (h->type == H_2DIM)
    {
      return h->fill(x, y, weight);
    }
    return H_IllegalID;
  }
  int hfillp (void *id, bintype x, bintype y)
  {
    PHisto *h = (PHisto *)id;
    if (h->type == H_PROFILE)
    {
      return h->fill(x, y);
    }
    return H_IllegalID;
  }

