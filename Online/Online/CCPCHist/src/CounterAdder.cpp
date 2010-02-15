#include "CounterAdder.h"
#include "hist_types.h"
#include "HistService.h"
#include "HistServer.h"
#include "HistRPC.h"
static int mpty;
typedef std::pair<std::string, void*> HistPair;
extern "C"
{
  void BufferAdder(void *tis, void *buff, int siz, HistInfo *h);
}
void CounterAdder::add(void *buff, int siz, HistInfo *h)
{
  void *p;
  p = Allocate(siz);
  m_expected = m_inputServicemap.size();
  long long cltime;
  cltime = *(long long*)buff;
  //printf("%s Last update %d Client Time %d\n",h->m_TargetService.c_str(),update_time,cltime);
  if (m_reference < cltime)
  {
    m_received = 1;
    //printf ("New cycle %s... %d\n",h->m_TargetService.c_str(),m_received);
    m_reference = cltime;
    memset(m_buffer,0,m_buffersize);
    m_hmap.clear();
    memcpy(p,buff,siz);
    m_usedSize = siz;
    void *bend = AddPtr(p,siz);
    void *hstart = AddPtr(p,sizeof(long long));
    DimBuffBase *pp = (DimBuffBase*)hstart;
    while (pp<bend)
    {
      char *nam = (char*)AddPtr(pp,pp->nameoff);
      std::string nams =nam;
      m_hmap.insert(HistPair(nams,pp));
      if (m_expand)
      {
        OUTServiceDescr *outs;
        outs = findOUTService(nams);
        if (outs != 0)
        {
          outs->m_buffer = pp;
          outs->m_buffersize = pp->reclen;
        }
        else
        {
          outs = new OUTServiceDescr((char*)nams.c_str(),pp->type,(char*)m_name.c_str());
          outs->m_buffer = pp;
          outs->m_buffersize = pp->reclen;
          this->m_outputServicemap.insert(OUTServicePair(nams,outs));
        }
      }
      pp=(DimBuffBase*)AddPtr(pp,pp->reclen);
    }
  }
  else if (m_reference == cltime)
  {
    HistMap hmap;
    void *bend = AddPtr(buff,siz);
    void *hstart = AddPtr(buff,sizeof(long long));
    DimBuffBase *pp = (DimBuffBase*)hstart;
    while (pp<bend)
    {
      char *nam = (char*)AddPtr(pp,pp->nameoff);
      hmap.insert(HistPair(std::string(nam),pp));
      pp=(DimBuffBase*)AddPtr(pp,pp->reclen);
    }
    for (HistIter i=hmap.begin();i!=hmap.end();i++)
    {
      HistIter j = m_hmap.find(i->first);
      if (j!=m_hmap.end())
      {
        DimBuffBase *sumh = (DimBuffBase*)(j->second);
        DimBuffBase *srch = (DimBuffBase*)(i->second);
        switch(srch->type)
        {
        case C_INT:
        case C_LONGLONG:
          {
            long long *ps,*ph;
            ps = (long long*)AddPtr(sumh,sumh->dataoff);
            ph = (long long*)AddPtr(srch,srch->dataoff);
            //printf("adding %lli to %lli",*ph,*ps);
            *ps += *ph;
            //printf(" result %lli\n",*ps);
            break;
          }
        case C_FLOAT:
        case C_DOUBLE:
          {
            double *ps,*ph;
            ps = (double*)AddPtr(sumh,sumh->dataoff);
            ph = (double*)AddPtr(srch,srch->dataoff);
            //printf("adding %f to %f\n",*ph, *ps);
            *ps += *ph;
            break;
          }
        }
      }
      else
      {
        DimBuffBase *srch = (DimBuffBase*)(i->second);
        char *nam = (char*)AddPtr(srch,srch->nameoff);
        printf("unseen Counter Name %s\n",nam);
        int csiz = m_usedSize;
        int hsiz;
        void *p;
        hsiz = srch->reclen;
        p = ReAllocate(hsiz);
        if (p!=0)
        {
          p = AddPtr(p,csiz);
          memcpy(p,srch,hsiz);
          char *nam = (char*)AddPtr(srch,srch->nameoff);
          m_hmap.insert(HistPair(std::string(nam),p));
        }
      }
    }
    m_received++;
    //printf ("New Source %s... %d\n",h->m_TargetService.c_str(),m_received);
  }
  else
  {
    printf("late update from %s\n expected %lli received %lli\n",h->m_TargetService.c_str(),m_reference,cltime);
    m_received++;
  }
  if (m_received >= m_expected)
  {
    printf("Finished one cycle. Updating our service... %d %d\n", m_received,m_expected);
    m_received = 0;
    //printf(" %d %d\n", m_received,m_expected);
    m_outservice->Updater();
    if (m_expand)
    {
      OUTServIter j;
      for (j= m_outputServicemap.begin();j!=m_outputServicemap.end();j++)
      {
        OUTServiceDescr *sd = j->second;
        sd->m_svc->Updater();
      }
    }
  }
}
CounterAdder::CounterAdder(char * taskname, char *myName, char *serviceName, bool expand):DimInfo("DIS_DNS/SERVER_LIST","DEAD")
{
  m_taskname = std::string(taskname);
  m_buffersize = 0;
  m_buffer = 0;
  m_usedSize = 0;
  m_reference =0;
  m_serviceName = serviceName;
  m_expand = expand;
  hist_prefix="_MON_";
  if (getenv("HIST_SRCNODE") != 0)
  {
    m_srcnode = "@";
    m_srcnode = m_srcnode+getenv("HIST_SRCNODE");
    m_srcnode = m_srcnode+".";
  }
  else
  {
    m_srcnode = "";
  }
  if(getenv("HIST_OUTDNS") != 0)
  {
    DimServer::setDnsNode(getenv("HIST_OUTDNS"));
  }
  else
  {
    DimServer::setDnsNode("localhost");
  }
  char *nodename;
#ifdef WIN32
  nodename  = getenv("COMPUTERNAME");
#else
  nodename  = getenv("HOSTNAME");
#endif
  if (nodename != 0)
  {
    m_name= std::string(nodename)+hist_prefix;
      //std::string("_HIST_");
    //if (strlen(myName) != 0)
    {
      m_name = m_name + std::string(myName)+ std::string("_");
    }
    //else
    //{
    //  m_name = m_name + std::string("_");
    //}
    m_name = m_name +
      std::string(taskname)+
      std::string("_0");
  }
  DimServer::autoStartOn();
  DimServer::start(m_name.c_str());
  std::string servc = m_name+serviceName+"Data";
  m_outservice = new HistService(servc.c_str(),"C",(void*)&mpty, 4, &m_buffer, &m_usedSize);
  std::string nam = m_name+serviceName+std::string("Command");
  m_ser = new AddSerializer((ObjMap*)&m_hmap);
  m_rpc = new HistRPC(m_ser,(char*)nam.c_str(), "I:1;C","C");
}
void CounterAdder::infoHandler(void)
{
  char *input;
  int siz;
  input = (char*)getData();
  siz = getSize();
  if (strcmp(input,"DEAD") == 0)
  {
    printf("DNS Down!!!!\n");
    return;
  }
  if (input[0] == '+')
  {
    std::string serv(&input[1]);
    std::string servc;
    std::string tskname,tgen;
    TaskName(serv,tskname,tgen);
    if (tskname != "")
    {
      ServiceName(serv,servc);
      INServiceDescr *sd = new INServiceDescr((char*)(servc.c_str()),
        new HistInfo((char*)(servc.c_str()), &BufferAdder,this));
      printf("Server: %s service: %s\n",serv.c_str(),servc.c_str());
      m_inputServicemap.insert(INServicePair(servc,sd));
      printf("%s: Adding service %s\n",m_name.c_str(),servc.c_str());
    }
  }
  else if (input[0] == '-' || input[0] == '!')
  {
    std::string serv(&input[1]);
    std::string servc;
    std::string tskname,tgen;
    TaskName(serv,tskname,tgen);
    if (tskname != "")
    {
      INServIter i;
      ServiceName(serv,servc);
      i = m_inputServicemap.find(servc);
      if (i != m_inputServicemap.end())
      {
        INServiceDescr *sd = i->second;
        delete sd->m_Info;
        delete sd;
        m_inputServicemap.erase(i);
        printf("%s: Removing service %s\n",m_name.c_str(),servc.c_str());
      }
      else
      {
        printf("Service %s not found in map\n",servc.c_str());
      }
    }
  }
  else
  {
    char *tok;
    tok = strtok(input,"|");
    while (tok != 0)
    {
      std::string serv(tok);
      std::string servc;
      std::string tskname,tgen;
      TaskName(serv,tskname,tgen);
      if (tskname != "")
      {
        ServiceName(serv,servc);
        INServiceDescr *sd = new INServiceDescr((char*)(servc.c_str()),
          new HistInfo((char*)(servc.c_str()),&BufferAdder,this));
        printf("Server: %s service: %s\n",serv.c_str(),servc.c_str());
        m_inputServicemap.insert(INServicePair(servc,sd));
        printf("%s: Adding service %s\n",m_name.c_str(),servc.c_str());
      }
      tok = strtok(0,"|");
    }
  }
}