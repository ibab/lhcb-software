#include "GauchoAppl/CounterAdder.h"

#include "GauchoAppl/AdderSys.h"
#include "Gaucho/Utilities.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "AIDA/IHistogram1D.h"

CounterAdder::CounterAdder(const char *myName, const char *serviceName)
{
  m_serviceName = std::string("/Counter/") + serviceName;
  m_MyName      = myName;
  m_type        = ADD_COUNTER;
  AdderSys::Instance().gg_AdderList.push_back(this);
}

CounterAdder::~CounterAdder()
{
}
void CounterAdder::add(void *buff, int siz, MonInfo *h)
{
  addBuffer(buff,siz,h);
}
void CounterAdder::addBuffer(void *buff, int siz, MonInfo *minfo)
{
  //printf("Counter Adder: UNLocking MonitorSvc\n");
  MonMap hmap;
  void *bend   = AddPtr(buff,siz);
  void *hstart = AddPtr(buff,sizeof(SerialHeader));
  DimHistbuff1 *pp = (DimHistbuff1*)hstart;
  while (pp<bend)
  {
    if (pp->reclen == 0) break;
    if (!offsetinBounds(pp,pp->reclen,pp->nameoff))
    {
      printf("+++ Counter Adder: Bad value of name offset (%d). Corrupted record? Source %s\n",pp->nameoff,minfo->getName());
      break;
    }
    char *nam = (char*)AddPtr(pp,pp->nameoff);
    char pnam[2048];
    if (!offsetinBounds(pp,pp->reclen,pp->dataoff))
    {
      printf("+++ Counter Adder: Bad value of data offset (%d [%0x] > %d). Corrupted record for %s?\n",pp->dataoff,pp->dataoff,pp->reclen,nam);
      break;
    }
    int limit = 2048;
    int nlen = strnlen(nam,limit);
    if (nlen == limit)
    {
      strncpy(pnam,nam,limit-1);
      pnam[sizeof(pnam)-1] = 0;
      printf("+++ Counter Adder: Very large name string. First 2048 characters %s out of %d. Skipping record...\n",pnam,pp->namelen);
      break;
    }
    hmap.insert(std::make_pair(nam,pp));
    pp=(DimHistbuff1*)AddPtr(pp,pp->reclen);
  }
  int count =0;
  for (MonIter i=hmap.begin();i!=hmap.end();i++)
  {
    std::string IFirst = i->first;
    MonObj *ISecond = (MonObj*)i->second;
    MonIter j = m_hmap.find(i->first);
    if (j!=m_hmap.end())
    {
      //printf("Counter Adder: Locking MAP\n");
      ++count;
      LockMap();
      std::string JFirst = j->first;
      DimHistbuff1 *JSecond = (DimHistbuff1*)j->second;
      DimHistbuff1 *sumh = (DimHistbuff1*)(j->second);
      DimHistbuff1 *srch = (DimHistbuff1*)(i->second);
      if (!offsetinBounds(sumh,sumh->reclen,sumh->dataoff))
      {
        printf ("Bad Data offset in sum record %s Loop Record number %d\n",j->first.c_str(),count);
        UnLockMap();
        break;
      }
      double *ps = (double*)AddPtr(sumh,sumh->dataoff);
      double *ph = (double*)AddPtr(srch,srch->dataoff);
      switch(srch->type)
      {
        case C_INT:
          {
            *(long long *)ps += *(long long*)ph;
            break;
          }
        case C_LONGLONG:
          {
            *(long long*)ps += *(long long*)ph;
            break;
          }
        case C_FLOAT:
        case C_RATEFLOAT:
          {
            *(float*)ps += *(float*)ph;
            break;
          }
        case C_DOUBLE:
        case C_RATEDOUBLE:
          {
            *(double*)ps += *(double*)ph;
            break;
          }
        case C_INTSTAR:
        {
          int n = (srch->reclen - srch->dataoff)/sizeof(int);
          int indx;
          int *sum = (int*)ps;
          int *src = (int*)ph;
          for (indx = 0;indx<n;indx++)
          {
            sum[indx]+=src[indx];
          }
          break;
        }
        case C_FLOATSTAR:
        {
          int n = (srch->reclen - srch->dataoff)/sizeof(float);
          int indx;
          float *sum = (float*)ps;
          float *src = (float*)ph;
          for (indx = 0;indx<n;indx++)
          {
            sum[indx]+=src[indx];
          }
          break;
        }
        case C_DOUBLESTAR:
        case C_RATEDOUBLESTAR:
        {
          int n = (srch->reclen - srch->dataoff)/sizeof(double);
          int indx;
          double *sum = (double*)ps;
          double *src = (double*)ph;
          for (indx = 0;indx<n;indx++)
          {
            sum[indx]+=src[indx];
          }
          break;
        }
        case C_LONGSTAR:
        {
          int n = (srch->reclen - srch->dataoff)/sizeof(long);
          int indx;
          long *sum = (long*)ps;
          long *src = (long*)ph;
          char txt[10000];
          sprintf(txt,"%p %p, %p %p",(void*)IFirst.c_str(),(void*)ISecond,(void*)JFirst.c_str(),(void*)JSecond);
          for (indx = 0;indx<n;indx++)
          {
            sum[indx]+=src[indx];
          }
          sprintf(txt,"%p %p, %p %p",(void*)IFirst.c_str(),(void*)ISecond,(void*)JFirst.c_str(),(void*)JSecond);
          break;
        }
        default:
          {
            printf("Unknown data type to add...%x\n",srch->type);
            break;
          }
      }
      //printf("Counter Adder: UNLocking MAP\n");
      UnLockMap();
    }
    else
    {
      DimHistbuff1 *srch = (DimHistbuff1*)(i->second);
      int csiz = m_usedSize;
      int hsiz = srch->reclen;
      //printf("Counter Adder: Locking MAP\n");
      LockMap();
      void *p = ReAllocate(hsiz);
      if (p!=0)
      {
        p = AddPtr(p,csiz);
        Memcpy(p,srch,hsiz);
        m_usedSize += hsiz;
        if (offsetinBounds(srch,srch->reclen,srch->nameoff))
        {
          char *nam = (char*)AddPtr(srch,srch->nameoff);
          m_hmap.insert(std::make_pair(nam,p));
        }
        else
        {
          printf("ERROR: +++++++++++ Bad Name Offset (%d, %0x) for Counter %s\n",srch->nameoff,srch->nameoff,IFirst.c_str());
        }
      }
      //printf("Counter Adder: UNLocking MAP\n");
      UnLockMap();
    }
  }
  m_received++;
  m_added++;
}
void CounterAdder::Update()
{
  if (m_isSaver)
  {
    //printf("Counter Adder: UNLocking\n");
    if (m_locked)
    {
      //printf("HistAdder UNLocking\n");
      m_locked = false;
      UnLock();
    }
  }
  ////printf(" %d %d\n", m_received,expected);
  if (m_outservice != 0)
  {
    m_outservice->Serialize();
    m_outservice->Update();
  }
  fflush(stdout);
//    if (CycleFn!= 0)
//    {
//      (*CycleFn)(CycleCBarg, m_buffer, m_buffersize, &m_hmap, this);
//    }
}
