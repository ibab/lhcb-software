#include "Gaucho/CounterAdder.h"
#include "Gaucho/ObjService.h"
#include "Gaucho/MonServer.h"
#include "Gaucho/ObjRPC.h"
#include "Gaucho/RootHists.h"
#include "Gaucho/RateService.h"
#include "dis.hxx"
#include "TDirectory.h"
#include <vector>
#include "string.h"
#include "Gaucho/Utilities.h"
#include "Gaucho/AdderSys.h"
//static int mpty;
typedef std::pair<std::string, void*> MonPair;


//DimInfo *CounterAdder::gg_DNSInfo =0;
//std::vector<MonAdder*> CounterAdder::gg_AdderList;
//HAdderTaskInfoHandler CounterAdder::gg_TaskHandler;
//HAdderServInfoHandler CounterAdder::gg_ServHandler;



extern "C"
{
  void BufferAdder(void *tis, void *buff, int siz, MonInfo *h);
}
CounterAdder::CounterAdder(char * taskname, char *myName, char *serviceName)
{
  std::string svc_prefix("/Counter/");
  m_taskname = std::string(taskname);
  m_buffersize = 0;
  m_buffer = 0;
  m_usedSize = 0;
  m_reference =-1;
  m_serviceName = svc_prefix +std::string(serviceName);
  m_expandRate = false;
  m_srcnode = "";
  m_outdns = "localhost";
  m_MyName = myName;
  m_taskname = taskname;
  m_lockid = 0;
  m_maplock = 0;
  m_IsEOR = false;
//  g_DNSInfo = 0;
//  m_TaskHandler = new HAdderTaskInfoHandler();
//  m_ServHandler = new HAdderServInfoHandler();
//  m_oldProf = 0;
  m_added = 0;
  m_noRPC = false;
  m_ser = 0;
  m_RPCser = 0;
  AdderSys::Instance().gg_AdderList.push_back(this);
  m_type = ADD_COUNTER;
}
CounterAdder::~CounterAdder()
{
  for (TaskServIter i = m_TaskServiceMap.begin();i!= m_TaskServiceMap.end();i++)
  {
    delete i->second->m_diminfo;
    delete i->second;
  }
  m_TaskServiceMap.clear();
  for (INServIter i = m_inputServicemap.begin();i!= m_inputServicemap.end();i++)
  {
    delete i->second->m_Info;
    delete i->second;
  }
  m_inputServicemap.clear();

  if (m_outservice  != 0)
  {
    delete m_outservice;
  }
  if (m_ser !=0 )
  {
    delete m_ser;
  }
  if (m_RPCser !=0 )
  {
    delete m_RPCser;
  }
  if (m_rpc !=0 )
  {
    delete m_rpc;
  }

}
void CounterAdder::Configure()
{
  std::string nodename;
  nodename = RTL::nodeNameShort();
  if (nodename != "")
  {
    toLowerCase(nodename);
    m_name= std::string("MON_");
    //if (strlen(myName) != 0)
    {
      m_name = m_name + std::string(m_MyName);//+ std::string("_");
    }
    //else
    //{
    //  m_name = m_name + std::string("_");
    //}
//    std::string tsknam = m_taskname.substr(m_taskname.find("_")+1);
//    m_name = m_name + tsknam+ std::string("_0");
  }
  if (m_srcnode == "")
  {
    if (nodename.find("hlt"))
    {
      std::string rackrow = nodename.substr(3,1); //size of "hlt"
      std::string rack_node = nodename.substr(4);
      if (rack_node.size() == 4)
      {
        m_srcnode = nodename;
      }
      else if (rack_node.size() == 2 )
      {
        m_srcnode = nodename;
      }
      else
      {
        m_srcnode = nodename.substr(0,3);
      }
    }
  }
  m_serviceexp = boost::regex(m_servicePattern.c_str(),boost::regex_constants::icase);
  m_taskexp = boost::regex(m_taskPattern.c_str(),boost::regex_constants::icase);
//  if (AdderSys::Instance().gg_DNSInfo ==0) AdderSys::Instance().gg_DNSInfo = new DimInfo((char*)"DIS_DNS/SERVER_LIST",(char*)"DEAD", &gAdderSys::Instance().g_TaskHandler);
//  DimServer::setDnsNode(m_outdns.c_str());
  m_outsvcname = m_name+m_serviceName;
  lib_rtl_create_lock(0,&m_maplock);
//  printf("================Out Service Name %s %s %s\n",m_name.c_str(),m_taskname.c_str(),m_outsvcname.c_str());
//  m_outservice = new ObjService(m_outsvcname.c_str(),(char*)"C",(void*)&mpty, 4, &m_buffer, &m_usedSize);
  std::string nam = m_name+std::string("/Counter/HistCommand");
  m_ser = new AddSerializer((ObjMap*)&m_hmap);
  m_rpc = 0;
  if (!m_noRPC)
  {
    m_RPCser = new AddSerializer((ObjMap*)&m_hmap);
    m_rpc = new ObjRPC(m_RPCser,(char*)nam.c_str(), (char*)"I:1;C",(char*)"C", this->m_maplock, 0/*this->m_lockid*/);
  }
}
/*

 void HAdderServInfoHandler::infoHandler(void)
{
  char *input;
  int siz;
  input = (char*)this->itsService->getData();
  siz = this->itsService->getSize();
//  printf("Service Info Handler %s. Input :\n%s\n",this->itsService->getName(),input);
  std::vector<MonAdder*>::iterator i;
  for (i=MonAdder::gg_AdderList.begin();i!= MonAdder::gg_AdderList.end();i++)
  {
    CounterAdder *p;
    p = (CounterAdder*)*i;
    p->ServiceHandler(itsService,input,siz);
  }
//    m_Hadder->TaskHandler();
//    m_Hadder->ServiceHandler(itsService);
}

void HAdderTaskInfoHandler::infoHandler(void)
{
  char *input;
  int siz;
  input = (char*)this->itsService->getData();
//  printf("Server Info Handler. Input :%s\n",input);
  siz = this->itsService->getSize();
  std::vector<MonAdder*>::iterator i;
  for (i=MonAdder::gg_AdderList.begin();i!=MonAdder::gg_AdderList.end();i++)
  {
    CounterAdder *p;
    p = (CounterAdder*)*i;
    p->TaskHandler(input,siz);
  }
//    m_Hadder->TaskHandler();
}
*/
void CounterAdder::add(void *buff, int siz, MonInfo *h)
{
  void *p;
  int n1d,n2d,nprof,nrate;
  n1d=0;
  n2d=0;
  nprof=0;
  nrate=0;
  if (siz == 4)
  {
//    printf("No Link from %s. Update counts....\n",h->m_TargetService.c_str());
    m_received++;
    return;
  }
  if (((SerialHeader*)buff)->m_magic != SERIAL_MAGIC)
  {
    printf("========> [ERROR] Serial Magic Word Missing  from connection %s\n",h->m_TargetService.c_str());
    m_received++;
    return;
  }
  m_expected = m_inputServicemap.size();
  long long cltime;
  SerialHeader* header= ((SerialHeader*)buff);
  cltime = header->ser_tim;
//  printf("Received size %d Header size %d from %s\n",siz,header->buffersize,h->getName());
  long long runno = ((SerialHeader*)buff)->run_number;
  long long current;
  if (m_IsEOR)
  {
    current = runno;
  }
  else
  {
    current = cltime;
  }
//  printf("%s Last update %lld Client Time %lld run number %lld %d %d\n",h->m_TargetService.c_str(),m_reference,cltime,runno,m_received,m_expected);
//  m_RateBuff = 0;
  if (m_reference < current)
  {
    m_added++;
    m_received++;// = 1;
    if (m_isSaver)
    {
      if (m_received == 1)
      {
        Lock();
      }
    }
    //printf ("New cycle %s... %d\n",h->m_TargetService.c_str(),m_received);
    p = Allocate(siz);
    m_reference = current;
    memset(m_buffer,0,m_buffersize);
    m_hmap.clear();
    memcpy(p,buff,siz);
    m_usedSize = siz;
    void *bend = AddPtr(p,siz);
    void *hstart = AddPtr(p,sizeof(SerialHeader));
    DimHistbuff1 *pp = (DimHistbuff1*)hstart;
    while (pp<bend)
    {
      char *nam = (char*)AddPtr(pp,pp->nameoff);
//      printf("Histogram Name: %s\n",nam);
      std::string nams =nam;
      LockMap();
      m_hmap.insert(MonPair(nams,pp));
      UnLockMap();
      pp=(DimHistbuff1*)AddPtr(pp,pp->reclen);
    }
  }
  else if (m_reference == current)
  {
//    printf("Adding %s\n",h->m_TargetService.c_str());
    MonMap hmap;
    hmap.clear();
    void *bend = AddPtr(buff,siz);
    void *hstart = AddPtr(buff,sizeof(SerialHeader));
    DimHistbuff1 *pp = (DimHistbuff1*)hstart;
    while (pp<bend)
    {
      char *nam = (char*)AddPtr(pp,pp->nameoff);
      hmap.insert(MonPair(std::string(nam),pp));
      pp=(DimHistbuff1*)AddPtr(pp,pp->reclen);
    }
    for (MonIter i=hmap.begin();i!=hmap.end();i++)
    {
      MonIter j = m_hmap.find(i->first);
      if (j!=m_hmap.end())
      {
        LockMap();
        double *ps, *ph;
        DimHistbuff1 *sumh = (DimHistbuff1*)(j->second);
        DimHistbuff1 *srch = (DimHistbuff1*)(i->second);
        ps = (double*)AddPtr(sumh,sumh->dataoff);
        ph = (double*)AddPtr(srch,srch->dataoff);
        switch(srch->type)
        {
        case C_INT:
          {
            *(long long *)ps += *(long long*)ph;
//            printf("Counter %s value %lld sum now %lld\n",i->first.c_str(),*(long long*)ph,*(long long *)ps);
            break;
          }
        case C_LONGLONG:
          {
            *(long long*)ps += *(long long*)ph;
//            printf("Counter %s value %lld sum now %lld\n",i->first.c_str(),*(long long*)ph,*(long long*)ps);
            break;
          }
        case C_FLOAT:
          {
            *(float*)ps += *(float*)ph;
            break;
          }
        case C_DOUBLE:
          {
            *(double*)ps += *(double*)ph;
            break;
          }
        default:
          {
            break;
          }
        }
        UnLockMap();
      }
      else
      {
//        printf("New Histogram... re-allocating...\n");
        int csiz = m_usedSize;
        int hsiz;
        DimHistbuff1 *srch = (DimHistbuff1*)(i->second);
        void *p;
        hsiz = srch->reclen;
        LockMap();
        p = ReAllocate(hsiz);
        if (p!=0)
        {
          p = AddPtr(p,csiz);
          memcpy(p,srch,hsiz);
          m_usedSize += hsiz;
          char *nam = (char*)AddPtr(srch,srch->nameoff);
          m_hmap.insert(MonPair(std::string(nam),p));
        }
        UnLockMap();
      }
    }
    m_received++;
    m_added++;
    //printf ("New Source %s... %d\n",h->m_TargetService.c_str(),m_received);
  }
  else
  {
//    printf("late update from %s\n expected %lli received %lli\n",h->m_TargetService.c_str(),m_reference,current);
    m_received++;
  }
  if (m_received >= m_expected)
  {
//    printf("Finished one cycle. Updating our service... %d %d\n", m_received,m_expected);
    if (m_isSaver)
    {
      UnLock();
    }
    m_received = 0;
    //printf(" %d %d\n", m_received,m_expected);
    if (m_outservice != 0)
    {
      m_outservice->Serialize();
      m_outservice->Update();
    }
//    if (m_expandRate)
//    {
//#define MAX_SERVICE_SIZE 128
//      std::string nnam;
//      if (m_RateBuff != 0)
//      {
//        MonHist h;
////        MyTProfile *p;
////        if (m_isSaver)
////        {
////          Lock();
////        }
////        p = (MyTProfile*)h.de_serialize(m_RateBuff,"RATE");
////        if (m_isSaver)
////        {
////          UnLock();
////        }
////        printf("Array Size %d Block Size %d",p->fN,h.BlockSize());
////        p->Print("range");
//        DimHistbuff1 *pdiff;
//
//        if (m_oldProf !=0)
//        {
//          int ndble = (m_RateBuff->nxbin+2);
//          pdiff = (DimHistbuff1*)malloc(m_RateBuff->reclen);
//          double *obincont  = (double*)AddPtr(AddPtr(m_oldProf,m_oldProf->dataoff),0*(m_oldProf->nxbin+2)*sizeof(double));
//          double *nbincont  = (double*)AddPtr(AddPtr(m_RateBuff,m_RateBuff->dataoff),0*(m_RateBuff->nxbin+2)*sizeof(double));
//          double *dbincont  = (double*)AddPtr(AddPtr(pdiff,m_RateBuff->dataoff),0*(m_RateBuff->nxbin+2)*sizeof(double));
//          double *ocont  = (double*)AddPtr(AddPtr(m_oldProf,m_oldProf->dataoff),1*(m_oldProf->nxbin+2)*sizeof(double));
//          double *ncont  = (double*)AddPtr(AddPtr(m_RateBuff,m_RateBuff->dataoff),1*(m_RateBuff->nxbin+2)*sizeof(double));
//          double *dcont  = (double*)AddPtr(AddPtr(pdiff,m_RateBuff->dataoff),1*(m_RateBuff->nxbin+2)*sizeof(double));
//          std::vector<char*> labvec;
//          labvec.clear();
//          labvec.push_back(0);
//          {
//            int i;
//            int nbin=m_oldProf->nxbin;
//            char *labs = (char*)AddPtr(m_oldProf,m_oldProf->xlaboff);
//            char *lab = labs;
//            for (i = 1; i < (nbin+1) ; ++i)
//            {
//              labvec.push_back(lab);
//              lab = (char*)AddPtr(lab,strlen(lab)+1);
//            }
//          }
//          for (int i = 9;i<ndble;i++)
//          {
//            dbincont[i] = nbincont[i]-obincont[i];
//            dcont[i] = ncont[i]-ocont[i];
////            printf("%d\t%f = %f - %f\n\t%f = %f - %f\n",i,dbincont[i], nbincont[i], obincont[i],dcont[i], ncont[i], ocont[i]);
//          }
//          OUTServiceDescr *outs;
//          std::string nams;
//          int i;
//          int dim;
//          double rate;
//          double denom;
////          printf("Time Bin %f\n",p->GetBinContent(8));
////          denom  = p->GetBinContent(8)/1.0e09;
////          denom = dbincont[8] == 0 ? 0 : dcont[8]/dbincont[8]/1.0e09;
//          denom = ncont[8]/m_added/1.0e09;
//          dim = m_RateBuff->nxbin;
//          std::string sname;
//          for (i=9;i<dim;i++)
//          {
//            nams  = labvec[i];
//            outs = findOUTService(nams);
//            RateService *s;
//            if (outs != 0)
//            {
//              s = (RateService*)outs->m_svc;
//            }
//            else
//            {
//              DimServer::autoStartOn();
//              sname = m_NamePrefix+nams;
//              if (sname.size() < MAX_SERVICE_SIZE)
//              {
//                s= new RateService(sname.c_str(),(char*)"D:2;C");
//                outs = new OUTServiceDescr((char*)nams.c_str(),(void*)s);
//                this->m_outputServicemap.insert(OUTServicePair(nams,outs));
//              }
//              else
//              {
//                continue;
//              }
//            }
////            rate = dbincont[i] == 0 ? 0 : dcont[i]/dbincont[i]/denom;
//            double c = dcont[i];
//            rate = c/denom;
////            printf("%d %d %s %f %f\n",dim,i,nams.c_str(), pdiff->GetBinContent(i),rate);
//            s->m_data.d1 = rate;
//            s->m_data.d2 = c;
//            int upsiz = 2*sizeof(s->m_data.d1)+strlen(s->m_data.c);
//            s->Updater(upsiz);
//          }
//          delete pdiff;
//        }
//        else
//        {
//          m_oldProf = (DimHistbuff1*)malloc(m_RateBuff->reclen);
//        }
//      }
//      m_added = 0;
//      memcpy(m_oldProf,m_RateBuff,m_RateBuff->reclen);
//    }
    if (CycleFn!= 0)
    {
      (*CycleFn)(CycleCBarg, m_buffer, m_buffersize, &m_hmap, this);
    }
  }
}
