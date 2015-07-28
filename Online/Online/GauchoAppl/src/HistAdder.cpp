#include "GauchoAppl/HistAdder.h"
#include "Gaucho/ObjService.h"
#include "Gaucho/MonServer.h"
#include "Gaucho/ObjRPC.h"
#include "Gaucho/MonHist.h"
#include "Gaucho/RootHists.h"
#include "GauchoAppl/RateService.h"
#include "dis.hxx"
#include "TDirectory.h"
#include <vector>
#include "string.h"
#include "Gaucho/Utilities.h"
#include "GauchoAppl/AdderSys.h"
typedef std::pair<std::string, void*> HistPair;

HistAdder::HistAdder(char *myName, char *serviceName)
{
  m_serviceName = std::string("/Histos/") + serviceName;
  m_MyName      = myName;
  m_type        = ADD_HISTO;
  m_oldProf     = 0;
  m_updated = false;
  AdderSys::Instance().gg_AdderList.push_back(this);
}

HistAdder::~HistAdder()
{
  Adderlist_t::iterator g=std::find( AdderSys::Instance().gg_AdderList.begin(),AdderSys::Instance().gg_AdderList.end(),this);
  if (g != AdderSys::Instance().gg_AdderList.end())
  {
    AdderSys::Instance().gg_AdderList.erase(g);
  }
}

void HistAdder::add(void *buff, int siz, MonInfo *h)
{
    addBuffer(buff,siz,h);
}
void HistAdder::addBuffer(void *buff, int siz,MonInfo *HTsk)
{
//    //printf("Adding %s\n",h->m_TargetService.c_str());
  MonMap hmap;
  hmap.clear();
  void *bend = AddPtr(buff,siz);
  void *hstart = AddPtr(buff,sizeof(SerialHeader));
  DimHistbuff1 *pp = (DimHistbuff1*)hstart;
  while (pp<bend)
  {
    if (pp->reclen == 0) break;
    char *nam = (char*)AddPtr(pp,pp->nameoff);
    hmap.insert(HistPair(std::string(nam),pp));
    pp=(DimHistbuff1*)AddPtr(pp,pp->reclen);
  }
  for (MonIter i=hmap.begin();i!=hmap.end();i++)
  {
    MonIter j = m_hmap.find(i->first);
    if (j!=m_hmap.end())
    {
      //printf("HistAdder Locking MAP\n");
      LockMap();
      double *ps, *ph;
      DimHistbuff1 *sumh = (DimHistbuff1*)(j->second);
      DimHistbuff1 *srch = (DimHistbuff1*)(i->second);
      ps = (double*)AddPtr(sumh,sumh->dataoff);
      ph = (double*)AddPtr(srch,srch->dataoff);
      if (srch->type == (int)C_STATENT)
      {
#define MIN(X,Y) ((X) < (Y) ?  (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ?  (X) : (Y))
        DimStatBuff *s=(DimStatBuff*)srch;
        DimStatBuff *d=(DimStatBuff*)sumh;
        d->nentries += s->nentries;
        d->m_sumw += s->m_sumw;
        d->m_sumw2 += s->m_sumw2;
        d->m_min = MIN(d->m_min,s->m_min);
        d->m_min = MAX(d->m_max,s->m_max);
        //printf("HistAdder UNLocking MAP\n");
        UnLockMap();
        continue;
      }
      sumh->nentries += srch->nentries;
      sumh->m_sumw  += srch->m_sumw;
      sumh->m_sumw2 += srch->m_sumw2;
      sumh->m_sumwx += srch->m_sumwx;
      sumh->m_sumwx2  += srch->m_sumwx2;
      sumh->m_sumwx3  += srch->m_sumwx3;
      sumh->m_sumwx4  += srch->m_sumwx4;
      sumh->m_sumwy += srch->m_sumwy;
      sumh->m_sumwy2  += srch->m_sumwy2;
      sumh->m_sumwy3  += srch->m_sumwy3;
      sumh->m_sumwy4  += srch->m_sumwy4;
      ////printf("Histogram %s Number of Entries %f\n",(char*)AddPtr(sumh,sumh->nameoff),sumh->nentries);
      int ndble;
      switch(srch->type)
      {
      case H_1DIM:
        {
          ndble = 2*(srch->nxbin+2);
          break;
        }
      case H_2DIM:
        {
          //2*(m_nx+2)*(m_ny+2)
          DimHistbuff2 *s = (DimHistbuff2*)srch;
          ndble = 2*(s->nxbin+2)*(s->nybin+2);
          break;
        }
      case H_3DIM:
        {
          //2*(m_nx+2)*(m_ny+2)
          DimHistbuff3 *s = (DimHistbuff3*)srch;
          ndble = 2*(s->nxbin+2)*(s->nybin+2)*(s->nzbin+2);
          break;
        }
      case H_PROFILE:
      case H_RATE:
        {
          ndble = 4*(srch->nxbin+2);
          break;
        }
      case H_2DPROFILE:
        {
          DimHistbuff3 *s = (DimHistbuff3*)srch;
          ndble = 4*(s->nxbin+2)*(s->nybin+2);
          break;
        }
      default:
        {
          ndble = 0;
          break;
        }
      }
      int icpy;
      switch(srch->type)
      {
      case H_RATE:
        {
          m_RateBuff = sumh;
          for (icpy=0;icpy<5;icpy++)
          {
            ps[icpy]+=ph[icpy];
          }
          for (icpy=8;icpy<ndble;icpy++)
          {
            ps[icpy]+=ph[icpy];
          }
          break;
        }
      default:
        {
          for (icpy=0;icpy<ndble;icpy++)
          {
            ps[icpy]+=ph[icpy];
          }
          break;
        }
      }
      //printf("HistAdder UNLocking MAP\n");
      UnLockMap();
    }
    else
    {
      printf("[WARNING] New Histogram..'%s' from '%s reference set %s'. re-allocating...\n",
          i->first.c_str(),HTsk->getName(),m_firstSource.c_str());
      int csiz = m_usedSize;
      int hsiz;
      DimHistbuff1 *srch = (DimHistbuff1*)(i->second);
      void *p;
      hsiz = srch->reclen;
      //printf("HistAdder Locking MAP\n");
      LockMap();
      p = ReAllocate(hsiz);
      if (p!=0)
      {
        p = AddPtr(p,csiz);
        Memcpy(p,srch,hsiz);
        m_usedSize += hsiz;
        char *nam = (char*)AddPtr(srch,srch->nameoff);
        if (strcmp(nam,i->first.c_str()) != 0)
        {
          printf("[ERROR] Name Mismatch in new histogram (%s)and Map (%s)\n",nam,i->first.c_str());
        }
        m_hmap.insert(HistPair(std::string(nam),p));
      }
      //printf("HistAdder UNLocking MAP\n");
      UnLockMap();
    }
  }
  m_received++;
  m_added++;
  ////printf ("New Source %s... %d\n",h->m_TargetService.c_str(),m_received);
}

void HistAdder::Update()
{
  if (m_isSaver)
  {
    if (m_locked)
    {
      //printf("HistAdder UNLocking\n");
      m_locked = false;
      UnLock();
    }
  }
  ////printf(" %d %d\n", m_received,m_m_expected);
  if (m_outservice != 0)
  {
    m_outservice->Serialize();
    m_outservice->Update();
    m_updated = true;
  }
  fflush(stdout);
}
