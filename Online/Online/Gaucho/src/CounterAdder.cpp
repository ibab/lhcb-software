#include "Gaucho/CounterAdder.h"

#include "Gaucho/AdderSys.h"
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
  void *p;
  unsigned long long tim = gettime();
  SerialHeader* header = ((SerialHeader*)buff);
  if (siz == 4)
  {
    //printf("No Link from %s. Update counts....\n",h->m_TargetService.c_str());
    m_received++;
    return;
  }
  if ( header->m_magic != SERIAL_MAGIC)
  {
    //printf("========> [ERROR] Serial Magic Word Missing  from connection %s\n",h->m_TargetService.c_str());
    m_received++;
    return;
  }
  size_t    expected = m_inputServicemap.size();
  long long current  = (m_IsEOR) ? header->run_number : header->ser_tim;

  if (m_reference < current)
  {
    m_time0 = tim;
    m_added++;
    m_received = 1;
    if (m_isSaver)
    {
      if (m_received == 1)
      {
        //printf("Counter Adder: Locking \n");
        Lock();
      }
    }
    ////printf ("New cycle %s... %d\n",h->m_TargetService.c_str(),m_received);
    p = Allocate(siz);
    m_reference = current;
    memset(m_buffer,0,m_buffersize);
    m_hmap.clear();
    memcpy(p,buff,siz);
    m_usedSize = siz;
    void *bend   = AddPtr(p,siz);
    void *hstart = AddPtr(p,sizeof(SerialHeader));
    DimHistbuff1 *pp = (DimHistbuff1*)hstart;
    while (pp<bend)
    {
      char *nam = (char*)AddPtr(pp,pp->nameoff);
      //printf("Counter Adder: Locking MAP\n");
      LockMap();
      m_hmap.insert(std::make_pair(nam,pp));
      //printf("Counter Adder: UNLocking MAP\n");
      UnLockMap();
      pp=(DimHistbuff1*)AddPtr(pp,pp->reclen);
    }
  }
  else if (m_reference == current)
  {
    if (m_histo != 0)
    {
      //printf("Counter Adder: Locking MonitorSvc\n");
      ObjectLock<IGauchoMonitorSvc> lock(m_monsvc);
      unsigned long long dtim = tim-m_time0;
      double ftim = dtim/1000000000;
      m_histo->fill(ftim);
    }
    //printf("Counter Adder: UNLocking MonitorSvc\n");
    MonMap hmap;
    void *bend   = AddPtr(buff,siz);
    void *hstart = AddPtr(buff,sizeof(SerialHeader));
    DimHistbuff1 *pp = (DimHistbuff1*)hstart;
    while (pp<bend)
    {
      char *nam = (char*)AddPtr(pp,pp->nameoff);
      hmap.insert(std::make_pair(nam,pp));
      pp=(DimHistbuff1*)AddPtr(pp,pp->reclen);
    }
    for (MonIter i=hmap.begin();i!=hmap.end();i++)
    {
      MonIter j = m_hmap.find(i->first);
      if (j!=m_hmap.end())
      {
        //printf("Counter Adder: Locking MAP\n");
        LockMap();
        DimHistbuff1 *sumh = (DimHistbuff1*)(j->second);
        DimHistbuff1 *srch = (DimHistbuff1*)(i->second);
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
          {
            *(float*)ps += *(float*)ph;
            break;
          }
        case C_DOUBLE:
        case C_RATE:
          {
            *(double*)ps += *(double*)ph;
            break;
          }
        default:
          {
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
        p = ReAllocate(hsiz);
        if (p!=0)
        {
          p = AddPtr(p,csiz);
          memcpy(p,srch,hsiz);
          m_usedSize += hsiz;
          char *nam = (char*)AddPtr(srch,srch->nameoff);
          m_hmap.insert(std::make_pair(nam,p));
        }
        //printf("Counter Adder: UNLocking MAP\n");
        UnLockMap();
      }
    }
    m_received++;
    m_added++;
    ////printf ("New Source %s... %d\n",h->m_TargetService.c_str(),m_received);
  }
  else
  {
    //    //printf("late update from %s\n expected %lli received %lli\n",h->m_TargetService.c_str(),m_reference,current);
    m_received++;
  }
  if (m_received >= expected)
  {
    //    //printf("Finished one cycle. Updating our service... %d %d\n", m_received,expected);
    if (m_isSaver)
    {
      //printf("Counter Adder: UNLocking\n");
      UnLock();
    }
    m_received = 0;
    ////printf(" %d %d\n", m_received,expected);
    if (m_outservice != 0)
    {
      m_outservice->Serialize();
      m_outservice->Update();
    }
    fflush(stdout);
    if (CycleFn!= 0)
    {
      (*CycleFn)(CycleCBarg, m_buffer, m_buffersize, &m_hmap, this);
    }
  }
}
