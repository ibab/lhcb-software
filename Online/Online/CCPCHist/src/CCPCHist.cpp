#include "iCCPCHist.h"
#include "CCPCHist.h"
Histo::Histo(char *name, char *title, int nx, float xmin, float xmax )
  {
    m_h = new CCPCHisto(this, name, title, nx,xmin,xmax);
  }
//Constructor for 2-dim histogram
  Histo::Histo(char *name, char *title, int nx, float xmin, float xmax, 
             int ny, float ymin, float ymax )
  {
    m_h = new CCPCHisto(this, name, title, nx, xmin, xmax,ny,ymin,ymax);
  }
//Constructor generic histogram
  Histo::Histo()
{
  m_h = new CCPCHisto(this);
}
  Histo::~Histo()
  {
    delete m_h;
  }
  //int Histo::setname ( char* name)
  //{
  //  return h->setname(name);
  //}
  void Histo::clear(void)
  {
    m_h->clear();
  }
  int Histo::put (float *from)
  {
    return m_h->put(from);
  }
  int Histo::put (double *from)
  {
    return m_h->put(from);
  }
  int Histo::putnents (int ne) 
  {
    return m_h->putnents(ne);
  }
  int Histo::get (float *to)
  {
    return m_h->get(to);
  }
  int Histo::geterr (float *to)
  {
    return m_h->geterr(to);
  }
  int Histo::get (double *to)
  {
    return m_h->get(to);
  }
  int Histo::geterr (double *to)
  {
    return m_h->geterr(to);
  }
  int Histo::getnents()
  {
    return m_h->getnents();
  }

  int Histo::fill (float x)
  {
    return m_h->fill(x);
  }
  int Histo::fill (float x, float weight)
  {
    return m_h->fill(x, weight);
  }
  int Histo::fill (float x,float y, float weight)
  {
    return m_h->fill(x, y, weight);
  }
   PHisto::PHisto(char *name, char *title, int nx, float xmin, float xmax )
    {
      m_h = new CCPCPHisto((Histo*)this, name, title, nx, xmin, xmax);
    }
  PHisto::~PHisto()
  {
    delete m_h;
  }
  int PHisto::fill(float x, float y)
  {
    return m_h->fill(x,y);
  }
  int PHisto::getsums (float *to) 
  {
    return m_h->getsums(to);
  }
  int PHisto::getsum2s(float *to )
  {
    return m_h->getsum2s(to);
  }
  int PHisto::getsums (double *to) 
  {
    return m_h->getsums(to);
  }
  int PHisto::getsum2s(double *to )
  {
    return m_h->getsum2s(to);
  }
  int PHisto::getentries(float *to)
  {
    return m_h->getentries(to);
  }
  int PHisto::getentries(double *to)
  {
    return m_h->getentries(to);
  }

  void PHisto::clear(void)
  {
    return m_h->clear();
  }
  void *hccpc_book1(char *name, char *title, int nx, float xmin, float xmax )
  {
    void *h;
    h = new CCPCHisto(name,title,nx,xmin,xmax);
    return h;
  }
  void *hccpc_profile(char *name, char *title, int nx, float xmin, float xmax )
  {
    CCPCPHisto *h;
    h = new CCPCPHisto(name,title,nx,xmin,xmax);
    return h;
  }
  void *hccpc_book2(char *name, char *title, int nx, float xmin, float xmax, 
             int ny, float ymin, float ymax )
  {
    void *h;
    h = new CCPCHisto(name,title,nx,xmin,xmax, ny, ymin,ymax);
    return h;
  }
  int hfill1 (void *id, float x, float weight)
  {
    CCPCHisto *h = (CCPCHisto *)id;
    if (h->type() == H_1DIM)
    {
      return h->fill(x, weight);
    }
    return H_IllegalID;
  }
  int hputf (void *id, float* buf)
  {
    CCPCHisto *h = (CCPCHisto *)id;
    return h->put(buf);
  }
  int hputd (void *id, double* buf)
  {
    CCPCHisto *h = (CCPCHisto *)id;
    return h->put(buf);
  }
  int hfill2 (void *id, float x,float y, float weight)
  {
    CCPCHisto *h = (CCPCHisto *)id;
    if (h->type() == H_2DIM)
    {
      return h->fill(x, y, weight);
    }
    return H_IllegalID;
  }
  int hfillp (void *id, float x, float y)
  {
    CCPCPHisto *h = (CCPCPHisto *)id;
    if (h->type() == H_PROFILE)
    {
      return h->fill(x, y);
    }
    return H_IllegalID;
  }

