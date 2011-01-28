#include "Gaucho/HistAdder.h"
#include "Gaucho/ObjService.h"
#include "Gaucho/MonServer.h"
#include "Gaucho/ObjRPC.h"
#include "Gaucho/MonHist.h"
#include "Gaucho/RootHists.h"
#include "Gaucho/RateService.h"
#include "dis.hxx"
#include "TDirectory.h"
#include <vector>
#include "string.h"
#include "Gaucho/Utilities.h"
static int mpty;
typedef std::pair<std::string, void*> HistPair;
extern "C"
{
  void BufferAdder(void *tis, void *buff, int siz, MonInfo *h);
}
void HistAdder::add(void *buff, int siz, MonInfo *)
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
  m_expected = m_inputServicemap.size();
  long long cltime;
  SerialHeader* header= ((SerialHeader*)buff);
  cltime = header->ser_tim;
//  printf("Received size %d Header size %d\n",siz,header->buffersize);
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
  m_RateBuff = 0;
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
      m_hmap.insert(HistPair(nams,pp));
      UnLockMap();
//      printf("Name: %s Nentries %i\n",nam,(int)pp->nentries);
//      if (m_expandRate)
//      {
//        OUTServiceDescr *outs;
//        outs = findOUTService(nams);
//        if (outs != 0)
//        {
//          outs->m_buffer = pp;
//          outs->m_buffersize = pp->reclen;
//        }
//        else
//        {
//          outs = new OUTServiceDescr((char*)nams.c_str(),pp->type,(char*)m_name.c_str());
//          outs->m_buffer = pp;
//          outs->m_buffersize = pp->reclen;
//          this->m_outputServicemap.insert(OUTServicePair(nams,outs));
//        }
//      }
      if ((MONTYPE)pp->type == H_RATE)
      {
        m_RateBuff = pp;
      }
      pp=(DimHistbuff1*)AddPtr(pp,pp->reclen);
    }
  }
  else if (m_reference == current)
  {
//    printf("Adding %s\n",h->m_TargetService.c_str());
    HistMap hmap;
    hmap.clear();
    void *bend = AddPtr(buff,siz);
    void *hstart = AddPtr(buff,sizeof(SerialHeader));
    DimHistbuff1 *pp = (DimHistbuff1*)hstart;
    while (pp<bend)
    {
      char *nam = (char*)AddPtr(pp,pp->nameoff);
      hmap.insert(HistPair(std::string(nam),pp));
      pp=(DimHistbuff1*)AddPtr(pp,pp->reclen);
    }
    for (HistIter i=hmap.begin();i!=hmap.end();i++)
    {
      HistIter j = m_hmap.find(i->first);
      if (j!=m_hmap.end())
      {
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
        //printf("Histogram %s Number of Entries %f\n",(char*)AddPtr(sumh,sumh->nameoff),sumh->nentries);
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
        case H_PROFILE:
        case H_RATE:
          {
            ndble = 3*(srch->nxbin+2);
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
          m_hmap.insert(HistPair(std::string(nam),p));
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
      m_outservice->Updater();
    }
    if (m_expandRate)
    {
#define MAX_SERVICE_SIZE 128
      std::string nnam;
      if (m_RateBuff != 0)
      {
        MonHist h;
//        MyTProfile *p;
//        if (m_isSaver)
//        {
//          Lock();
//        }
//        p = (MyTProfile*)h.de_serialize(m_RateBuff,"RATE");
//        if (m_isSaver)
//        {
//          UnLock();
//        }
//        printf("Array Size %d Block Size %d",p->fN,h.BlockSize());
//        p->Print("range");
        DimHistbuff1 *pdiff;

        if (m_oldProf !=0)
        {
          int ndble = (m_RateBuff->nxbin+2);
          pdiff = (DimHistbuff1*)malloc(m_RateBuff->reclen);
          double *obincont  = (double*)AddPtr(AddPtr(m_oldProf,m_oldProf->dataoff),0*(m_oldProf->nxbin+2)*sizeof(double));
          double *nbincont  = (double*)AddPtr(AddPtr(m_RateBuff,m_RateBuff->dataoff),0*(m_RateBuff->nxbin+2)*sizeof(double));
          double *dbincont  = (double*)AddPtr(AddPtr(pdiff,m_RateBuff->dataoff),0*(m_RateBuff->nxbin+2)*sizeof(double));
          double *ocont  = (double*)AddPtr(AddPtr(m_oldProf,m_oldProf->dataoff),1*(m_oldProf->nxbin+2)*sizeof(double));
          double *ncont  = (double*)AddPtr(AddPtr(m_RateBuff,m_RateBuff->dataoff),1*(m_RateBuff->nxbin+2)*sizeof(double));
          double *dcont  = (double*)AddPtr(AddPtr(pdiff,m_RateBuff->dataoff),1*(m_RateBuff->nxbin+2)*sizeof(double));
          std::vector<char*> labvec;
          labvec.clear();
          labvec.push_back(0);
          {
            int i;
            int nbin=m_oldProf->nxbin;
            char *labs = (char*)AddPtr(m_oldProf,m_oldProf->xlaboff);
            char *lab = labs;
            for (i = 1; i < (nbin+1) ; ++i)
            {
              labvec.push_back(lab);
              lab = (char*)AddPtr(lab,strlen(lab)+1);
            }
          }
          for (int i = 9;i<ndble;i++)
          {
            dbincont[i] = nbincont[i]-obincont[i];
            dcont[i] = ncont[i]-ocont[i];
//            printf("%d\t%f = %f - %f\n\t%f = %f - %f\n",i,dbincont[i], nbincont[i], obincont[i],dcont[i], ncont[i], ocont[i]);
          }
          OUTServiceDescr *outs;
          std::string nams;
          int i;
          int dim;
          double rate;
          double denom;
//          printf("Time Bin %f\n",p->GetBinContent(8));
//          denom  = p->GetBinContent(8)/1.0e09;
//          denom = dbincont[8] == 0 ? 0 : dcont[8]/dbincont[8]/1.0e09;
          denom = ncont[8]/m_added/1.0e09;
          dim = m_RateBuff->nxbin;
          std::string sname;
          for (i=9;i<dim;i++)
          {
            nams  = labvec[i];
            outs = findOUTService(nams);
            RateService *s;
            if (outs != 0)
            {
              s = (RateService*)outs->m_svc;
            }
            else
            {
              DimServer::autoStartOn();
              sname = m_NamePrefix+nams;
              if (sname.size() < MAX_SERVICE_SIZE)
              {
                s= new RateService(sname.c_str(),(char*)"D:2;C");
                outs = new OUTServiceDescr((char*)nams.c_str(),(void*)s);
                this->m_outputServicemap.insert(OUTServicePair(nams,outs));
              }
              else
              {
                continue;
              }
            }
//            rate = dbincont[i] == 0 ? 0 : dcont[i]/dbincont[i]/denom;
            double c = dcont[i];
            rate = c/denom;
//            printf("%d %d %s %f %f\n",dim,i,nams.c_str(), pdiff->GetBinContent(i),rate);
            s->m_data.d1 = rate;
            s->m_data.d2 = c;
            int upsiz = 2*sizeof(s->m_data.d1)+strlen(s->m_data.c);
            s->Updater(upsiz);
          }
          delete pdiff;
        }
        else
        {
          m_oldProf = (DimHistbuff1*)malloc(m_RateBuff->reclen);
        }
      }
      m_added = 0;
      memcpy(m_oldProf,m_RateBuff,m_RateBuff->reclen);
    }
    if (CycleFn!= 0)
    {
      (*CycleFn)(CycleCBarg, m_buffer, m_buffersize, &m_hmap, this);
    }
  }
}
HistAdder::HistAdder(char * taskname, char *myName, char *serviceName)
{
  std::string svc_prefix("/Histos/");
  m_taskname = std::string(taskname);
  m_buffersize = 0;
  m_buffer = 0;
  m_usedSize = 0;
  m_reference =0;
  m_serviceName = svc_prefix +std::string(serviceName);
  m_expandRate = false;
  m_srcnode = "";
  m_outdns = "localhost";
  m_MyName = myName;
  m_taskname = taskname;
  m_lockid = 0;
  m_maplock = 0;
  m_IsEOR = false;
  m_DNSInfo = 0;
  m_TaskHandler = new HAdderTaskInfoHandler(this);
  m_ServHandler = new HAdderServInfoHandler(this);
  m_oldProf = 0;
  m_added = 0;
}
HistAdder::~HistAdder()
{
  if (m_DNSInfo != 0)
  {
    delete m_DNSInfo;
  }
  if (m_outservice  != 0)
  {
    delete m_outservice;
  }
  if (m_ser !=0 )
  {
    delete m_ser;
  }
  if (m_rpc !=0 )
  {
    delete m_rpc;
  }
}
void HistAdder::Configure()
{
  std::string nodename;
  nodename = RTL::nodeNameShort();
  if (nodename != "")
  {
    toLowerCase(nodename);
    m_name= std::string("MON_");
    //if (strlen(myName) != 0)
    {
      m_name = m_name + std::string(m_MyName)+ std::string("_");
    }
    //else
    //{
    //  m_name = m_name + std::string("_");
    //}
    std::string tsknam = m_taskname.substr(m_taskname.find("_")+1);
    m_name = m_name + tsknam+ std::string("_0");
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
  m_DNSInfo = new DimInfo((char*)"DIS_DNS/SERVER_LIST",(char*)"DEAD", m_TaskHandler);
  DimServer::setDnsNode(m_outdns.c_str());
  m_outsvcname = m_name+m_serviceName;
  lib_rtl_create_lock(0,&m_maplock);
//  printf("================Out Service Name %s %s %s\n",m_name.c_str(),m_taskname.c_str(),m_outsvcname.c_str());
//  m_outservice = new ObjService(m_outsvcname.c_str(),(char*)"C",(void*)&mpty, 4, &m_buffer, &m_usedSize);
  std::string nam = m_name+std::string("/Histos/HistCommand");
  m_ser = new AddSerializer((ObjMap*)&m_hmap);
  m_rpc = new ObjRPC(m_ser,(char*)nam.c_str(), (char*)"I:1;C",(char*)"C", this->m_maplock, 0/*this->m_lockid*/);
}
void HistAdder::ServiceHandler(DimInfo *myInfo)
{
  char *input;
  int siz;
  input = (char*)myInfo->getData();
  siz = myInfo->getSize();
  if (strcmp(input,"DEAD") == 0)
  {
//    printf("Service List Service died...\n");
    return;
  }
//  printf("New Message from Service List Service %s\n",input);
//  printf("infoname: %s data: %s\n",myInfo->getName(),input);
  if (input[0] == '+')
  {
    dyn_string *svcs = Strsplit(input,(char*)"|");
    std::string servc(&(svcs->at(0)[1]));
    delete svcs;
    bool status;
    status = boost::regex_search(servc,m_serviceexp);
    if (status)
    {
      INServIter i;
      i = m_inputServicemap.find(servc);
      if (i == m_inputServicemap.end())
      {
//        printf("new INService %s\n",servc.c_str());
        TaskServIter k;
        std::string taskn=std::string(myInfo->getName());
        taskn = taskn.substr(0,taskn.find("/SERVICE_LIST"));
        k = m_TaskServiceMap.find(taskn);
        if (k != m_TaskServiceMap.end())
        {
          k->second->m_svcname = servc;
        }
        INServiceDescr *sd = new INServiceDescr((char*)(servc.c_str()),
            new MonInfo((char*)(servc.c_str()), &BufferAdder,this));
//        printf("Service: %s\n",servc.c_str());
        m_inputServicemap.insert(INServicePair(servc,sd));
//        printf("%s: Adding service @%s@\n",m_name.c_str(),servc.c_str());
        if (m_inputServicemap.size() == 1)
        {
          m_outservice = new ObjService(m_outsvcname.c_str(),(char*)"C",(void*)&mpty, 4, &m_buffer, &m_usedSize);
        }
      }
      else
      {
//        printf("Duplicate service %s. DNS restart? \n",servc.c_str());
      }
    }
  }
  else if (input[0] == '-' || input[0] == '!')
  {
    dyn_string *svcs = Strsplit(input,(char*)"|");
    std::string servc(&(svcs->at(0)[1]));
    delete svcs;
    INServIter i;
    i = m_inputServicemap.find(servc);
    if (i != m_inputServicemap.end())
    {
      TaskServIter k;
      std::string taskn=std::string(myInfo->getName());
      taskn = taskn.substr(0,taskn.find("/SERVICE_LIST"));
      k = m_TaskServiceMap.find(taskn);
      if (k != m_TaskServiceMap.end())
      {
        k->second->m_svcname = "";
      }
      INServiceDescr *sd = i->second;
      delete sd->m_Info;
      delete sd;
      m_inputServicemap.erase(i);
//      printf("%s: Removing service %s\n",m_name.c_str(),servc.c_str());
      if (m_inputServicemap.empty())
      {
//        printf ("No more client... Deleteing our output service...\n");
        delete m_outservice;
        m_outservice = 0;
      }
    }
    else
    {
//      printf("Service %s not found in map\n",servc.c_str());
    }
  }
  else
  {
    dyn_string *svcs = Strsplit(input,(char*)"\n");
    for (unsigned int j=0;j<svcs->size();j++)
    {
      std::string servc(svcs->at(j));
      dyn_string *svc = Strsplit((char*)servc.c_str(),(char*)"|");
      bool status;
      status = boost::regex_search(svc->at(0),m_serviceexp);
      if (status)
      {
        INServIter i;
        i = m_inputServicemap.find(svc->at(0));
        if (i == m_inputServicemap.end())
        {
          servc = svc->at(0);
          TaskServIter k;
          std::string taskn=std::string(myInfo->getName());
          taskn = taskn.substr(0,taskn.find("/SERVICE_LIST"));
          k = m_TaskServiceMap.find(taskn);
          if (k != m_TaskServiceMap.end())
          {
            k->second->m_svcname = servc;
          }
          INServiceDescr *sd = new INServiceDescr((char*)(servc.c_str()),
          new MonInfo((char*)(svc->at(0).c_str()),&BufferAdder,this));
//          printf("Service: %s\n",svc->at(0).c_str());
          m_inputServicemap.insert(INServicePair(svc->at(0).c_str(),sd));
//          printf("%s: Adding service @%s@\n",m_name.c_str(),servc.c_str());
          if (m_inputServicemap.size() == 1)
          {
//            printf ("First client for adding... Creating our output service...\n");
            m_outservice = new ObjService(m_outsvcname.c_str(),(char*)"C",(void*)&mpty, 4, &m_buffer, &m_usedSize);
          }
        }
        else
        {
//          printf("Duplicate service %s. DNS restart? \n",servc.c_str());
        }
      }
    }
  }
}
void HistAdder::TaskHandler(void)
{
  char *input;
  int siz;
  input = (char*)m_DNSInfo->getData();
  siz = m_DNSInfo->getSize();
  if (strcmp(input,"DEAD") == 0)
  {
//    printf("DNS Down!!!!\n");
    return;
  }
//  printf("New Message from DNS %s\n",input);
  if (input[0] == '+')
  {
    std::string serv(&input[1]);
    std::string tskname,tgen;
    std::string servc;
    TaskName(serv,tskname,tgen);
    if (tskname != "")
    {
      TaskServIter i;
      i = m_TaskServiceMap.find(tskname);
      if (i == m_TaskServiceMap.end())
      {
        servc = tskname+std::string("/SERVICE_LIST");
        DimInfo *slist = new DimInfo(servc.c_str(),(char *)"DEAD",m_ServHandler);
        TaskSvcDescr *ts = new TaskSvcDescr(tskname,slist);
        m_TaskServiceMap.insert(TaskServicePair(tskname,ts));
//        printf("Listening to Task %s\n",tskname.c_str());
      }
      else
      {
//        printf("Duplicate Task %s. DNS restart? \n",tskname.c_str());
      }
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
      TaskServIter i;
      i = m_TaskServiceMap.find(tskname);
      if (i != m_TaskServiceMap.end())
      {
        std::string service;
        INServIter k,l;
        service = i->second->m_svcname;
//        printf("Task %s died. Removing service @%s@\n",tskname.c_str(),service.c_str());
        k = m_inputServicemap.find(service);
        if (k != m_inputServicemap.end())
        {
          delete k->second->m_Info;
          delete k->second;
          m_inputServicemap.erase(k);
          if (m_inputServicemap.empty())
          {
//            printf ("No more client... Deleteing our output service...\n");
            delete m_outservice;
            m_outservice = 0;
          }
        }
        else
        {
//          printf("Very Funny... service @%s@ (%d)is not in service map...\n",service.c_str(),(int)service.size());
//          printf("map contents...\n");
//          int indx=0;
//          for (k=m_inputServicemap.begin(); k!= m_inputServicemap.end();k++)
//          {
//            printf("Index %d key: @%s@ (%d) @%s@\n",indx,k->first.c_str(),(int)k->first.size(),k->second->m_serviceName.c_str());
//            indx++;
//          }
        }
        TaskSvcDescr *ts = i->second;
        delete ts->m_diminfo;
        delete ts;
        m_TaskServiceMap.erase(i);
//        printf("%s: Removing task %s\n",m_name.c_str(),tskname.c_str());
//        if (m_inputServicemap.empty())
//        {
//          printf("no more clients... Exiting volontarily....\n");
//          exit(0);
//        }
      }
      else
      {
//        printf("Task %s not found in map\n",tskname.c_str());
      }
      INServIter k;
      std::string srvc=tskname+m_serviceName;
      k = m_inputServicemap.find(srvc);
      if (k != m_inputServicemap.end())
      {
        INServiceDescr *is = k->second;
        m_inputServicemap.erase(k);
        delete is->m_Info;
        delete is;
        if (m_inputServicemap.empty())
        {
//          printf ("No more client... Deleteing our output service...\n");
          delete m_outservice;
          m_outservice = 0;
        }
      }
    }
  }
  else
  {
    dyn_string *servers = Strsplit(input,(char*)"|");
    dyn_string::iterator i;
    for (i=servers->begin();i!=servers->end();i++)
    {
      std::string serv = *i;
      std::string tskname,tgen;
      std::string servc;
      TaskName(serv,tskname,tgen);
      if (tskname != "")
      {
        TaskServIter i;
        i = m_TaskServiceMap.find(tskname);
        if (i == m_TaskServiceMap.end())
        {
          servc = tskname+std::string("/SERVICE_LIST");
          DimInfo *slist = new DimInfo(servc.c_str(),(char *)"DEAD",m_ServHandler);
          TaskSvcDescr *ts = new TaskSvcDescr(tskname,slist);
          m_TaskServiceMap.insert(TaskServicePair(tskname,ts));
//          printf("Listening to Task %s\n",tskname.c_str());
        }
        else
        {
//          printf("Duplicate Task %s. DNS restart? \n",tskname.c_str());
        }
      }
    }
  }
}
