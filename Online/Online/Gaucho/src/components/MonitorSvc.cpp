// Include files

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "Gaucho/CntrMgr.h"
#include "Gaucho/MonHist.h"
#include "Gaucho/MonSys.h"
#include "Gaucho/MonSubSys.h"
#include "Gaucho/MonCounter.h"
#include "Gaucho/Utilities.h"

#include "RTL/rtl.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

#include "TH1D.h"
#include "TProfile.h"
#include "TH2D.h"
#include "Gaucho/MonRate.h"
#include "Gaucho/RateMgr.h"
#include "Gaucho/StatusService.h"
#include "MonitorSvc.h"

using namespace std;

//namespace AIDA { class IBaseHistogram; }

// Factory for instantiation of service objects
DECLARE_SERVICE_FACTORY(MonitorSvc)

// Constructor
//Query interfaces of Interface
// @param riid       ID of Interface to be retrieved
// @param ppvUnknown Pointer to Location for interface pointer
StatusCode MonitorSvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
  if(IMonitorSvc::interfaceID().versionMatch(riid)) {
    *ppvIF = (IMonitorSvc*)this;
  }
  else if (IGauchoMonitorSvc::interfaceID().versionMatch(riid)) {
    *ppvIF = (IGauchoMonitorSvc*)this;
  }
  else if ( IIncidentListener::interfaceID().versionMatch(riid) ) {
    *ppvIF = (IIncidentListener*)this;
  }
  else if ( IUpdateableIF::interfaceID().versionMatch(riid) ) {
    *ppvIF = (IUpdateableIF*)this;
  }
  else {
    return Service::queryInterface(riid, ppvIF);
  }
  addRef();
  return StatusCode::SUCCESS;
}

MonitorSvc::MonitorSvc(const string& name, ISvcLocator* sl)
: Service(name, sl)
{
  m_started = false;
  m_MonSys =   &MonSys::m_instance();
  m_HistSubSys = 0;
  m_savetimer = 0;
  m_CntrSubSys = 0;
  m_CntrMgr = 0;
  m_RateMgr = 0;
  m_StatusService = 0;
  m_monsysrecover = 0;
  declareProperty("UniqueServiceNames", m_uniqueServiceNames = 0);
  declareProperty("disableMonRate", m_disableMonRate = 0);
  declareProperty("disableDimPropServer", m_disableDimPropServer = 0);
  declareProperty("disableDimCmdServer", m_disableDimCmdServer = 0);
  // declareProperty("disableDimRcpGaucho", m_disableDimRcpGaucho = 0);

  declareProperty("disableMonObjectsForBool", m_disableMonObjectsForBool = 1);
  declareProperty("disableMonObjectsForInt", m_disableMonObjectsForInt = 0);
  declareProperty("disableMonObjectsForLong", m_disableMonObjectsForLong = 0);
  declareProperty("disableMonObjectsForDouble", m_disableMonObjectsForDouble = 0);
  declareProperty("disableMonObjectsForString", m_disableMonObjectsForString = 1);
  declareProperty("disableMonObjectsForPairs", m_disableMonObjectsForPairs = 1);
  declareProperty("disableMonObjectsForHistos", m_disableMonObjectsForHistos = 0);

  declareProperty("disableDeclareInfoBool", m_disableDeclareInfoBool = 1);
  declareProperty("disableDeclareInfoInt", m_disableDeclareInfoInt = 0);
  declareProperty("disableDeclareInfoLong", m_disableDeclareInfoLong = 0);
  declareProperty("disableDeclareInfoDouble", m_disableDeclareInfoDouble = 0);
  declareProperty("disableDeclareInfoString", m_disableDeclareInfoString = 1);
  declareProperty("disableDeclareInfoPair", m_disableDeclareInfoPair = 1);
  declareProperty("disableDeclareInfoFormat", m_disableDeclareInfoFormat = 0);
  declareProperty("disableDeclareInfoHistos", m_disableDeclareInfoHistos = 0);
  declareProperty("maxNumCountersMonRate", m_maxNumCountersMonRate = 1000);
  declareProperty("DimUpdateInterval", m_updateInterval = 20);
  declareProperty("CounterUpdateInterval", m_CounterInterval =0);
  declareProperty("ExpandCounterServices",m_expandCounterServices=0);
  declareProperty("ExpandNameInfix",m_expandInfix="");
  declareProperty("PartitionName",m_partname="LHCb");
  declareProperty("ProcessName",m_ProcName="");
  declareProperty("ProgramName",m_ProgName="");
  declareProperty("DontResetCountersonRunChange",m_DontResetCountersonRunChange=false);
  TH1D::SetDefaultSumw2();
  TH2D::SetDefaultSumw2();
  TProfile::SetDefaultSumw2();
  m_InfoMap.clear();
}


MonitorSvc::~MonitorSvc()
{
  deletePtr(m_HistSubSys);
  deletePtr(m_CntrSubSys);
  deletePtr(m_CntrMgr);
}

StatusCode MonitorSvc::initialize()
{
  m_StatusService = 0;
  MsgStream msg(msgSvc(),"MonitorSvc");
  StatusCode sc = Service::initialize();
  if (m_CounterInterval == 0) m_CounterInterval = m_updateInterval;

  if ( 0 != m_disableMonRate)
  {
    msg << MSG::DEBUG << "MonRate process is disabled." << endmsg;
  }
  sc = serviceLocator()->service("IncidentSvc",m_incidentSvc,true);
  if( !sc.isSuccess() ) {
    msg << MSG::FATAL << "Service [IncidentSvc] not found" << endmsg;
    return sc;
  }
  m_incidentSvc->addListener(this,"APP_RUNNING");
  m_incidentSvc->addListener(this,"APP_STOPPED");
  return sc;
}

/// Incident handler implemenentation: Inform that a new incident has occured
void MonitorSvc::handle(const Incident& inc) {
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "MonitorSvc: Got incident from:" << inc.source() << ": " << inc.type() << endmsg;
  if ( inc.type() == "APP_RUNNING" ) {
    i_start().ignore();
  }
  else if ( inc.type() == "APP_STOPPED" ) {
    i_stop().ignore();
  }
}

void MonitorSvc::undeclareInfo( const string& name, const IInterface*  owner)
{
  map<string,void*>::iterator it;
  string oname = infoOwnerName(owner);
  string newName;
  if (name.find("COUNTER_TO_RATE") != string::npos)
  {
    newName = extract("COUNTER_TO_RATE", name);
    newName = oname+"/"+newName;
    it = m_InfoMap.find(newName);
    if (it != m_InfoMap.end())
    {
       CntrMgr* cm = (CntrMgr*)it->second;
       cm->removeCounter(newName);
    }
    newName = extract("COUNTER_TO_RATE", name);
    newName = oname+"/R"+newName;
    it = m_InfoMap.find(newName);
    if (it != m_InfoMap.end())
    {
       RateMgr* rm = (RateMgr*)it->second;
       MonRateBase *p=rm->removeRate(newName);
       m_CntrSubSys->removeObj(p);
       delete p;
    }
    return;
  }
  newName = oname +"/"+name;
  it = m_InfoMap.find(newName);
  if (it != m_InfoMap.end())
  {
     MonSubSys* ss = (MonSubSys*)it->second;
     MonObj *o = ss->findobj(newName.c_str());
     ss->removeObj(o);
     delete o;
  }
  return;
}

/** Undeclare monitoring information
    @param owner Owner identifier of the monitoring information
*/
void MonitorSvc::undeclareAll( const IInterface*  owner)
{
  string oname = owner ? infoOwnerName(owner) : "-ALL-";
  vector<string> strings;
  string nam;
  strings.clear();
  if (m_CntrMgr != 0)
  {
    CntrMgr::counterMapIt it;
    for (it = m_CntrMgr->m_counterMap.begin();it != m_CntrMgr->m_counterMap.end();
        it++)
    {
      nam = it->first;
      if (owner==0 || nam.find(oname.c_str()) != string::npos)
      {
        strings.push_back(nam);
      }
    }
    for (unsigned int i=0;i<strings.size();i++)
    {
      m_CntrMgr->removeCounter(strings[i]);
    }
  }
  strings.clear();
  if (m_RateMgr != 0)
  {
    m_RateMgr->removeRateAll(m_CntrSubSys);
  }
  vector<MonObj*> v;
  if (m_HistSubSys != 0)
  {
    ObjMap::iterator it;
    for (it=m_HistSubSys->m_Objmap.begin();it!=m_HistSubSys->m_Objmap.end();it++)
    {
      MonObj *o = it->second;
      string s = o->name();
      if (0==owner || s.find(oname.c_str()) != string::npos)
      {
        v.push_back(o);;
      }
    }
    for (unsigned int i=0;i<v.size();i++)
    {
      m_HistSubSys->removeObj(v[i]);
      delete v[i];
    }
  }
  v.clear();
  if (m_CntrSubSys != 0)
  {
    ObjMap::iterator it;
    for (it=m_CntrSubSys->m_Objmap.begin();it!=m_CntrSubSys->m_Objmap.end();it++)
    {
      MonObj *o = it->second;
      string s = o->name();
      if (0==owner || s.find(oname.c_str()) != string::npos)
      {
        v.push_back(o);;
      }
    }
    for (unsigned int i=0;i<v.size();i++)
    {
      m_CntrSubSys->removeObj(v[i]);
      delete v[i];
    }
  }
}


StatusCode MonitorSvc::start()
{
  if (m_StatusService == 0)m_StatusService = new StatusService(this);

  return Service::start();
}

StatusCode MonitorSvc::i_start()
{
//  CALLGRIND_START_INSTRUMENTATION
  MsgStream msg(msgSvc(),"MonitorSvc");
  m_i_startState = true;
  msg << MSG::DEBUG << "======== MonitorSvc start() called ============= " << endmsg;
//  setProperties();
//  dis_set_debug_on();
  DimServer::autoStartOff();
  if (m_MonSys != &MonSys::m_instance())
  {
    m_MonSys = &MonSys::m_instance();
    m_monsysrecover++;
  }
  if (m_CntrMgr != 0)
  {
//    //printf("In STARTS Method... Counter Manager present... Closing it...\n");
    m_CntrMgr->close();
  }
  if (m_started)
  {
    m_MonSSysSize = m_MonSys->Subsyslist->size();
    m_MonSys->start();
    DimServer::autoStartOn();
//    DimServer::start();
    return StatusCode::SUCCESS;
  }
  if (m_CntrSubSys != 0)
  {
    m_CntrSubSys->m_expandnames = m_expandCounterServices;
    m_CntrSubSys->m_dontclear = m_DontResetCountersonRunChange;
  }
  m_utgid = RTL::processName();
  if (m_ProcName == "")
  {
    m_ProcName = m_utgid;
  }
  if (m_expandCounterServices)
  {
    StringReplace(m_expandInfix,(char*)"<part>",m_partname);
    StringReplace(m_expandInfix,(char*)"<proc>",m_ProcName);
    StringReplace(m_expandInfix,(char*)"<program>",m_ProgName);
    if (m_CntrSubSys != 0)
    {
      m_CntrSubSys->m_expandInfix = m_expandInfix;
    }
  }
  if (m_CntrMgr != 0)
  {
    m_CntrMgr->close();
  }
  if (m_MonSys     != 0) m_MonSys->setup((char*)m_utgid.c_str());
  if (m_CntrSubSys != 0) m_CntrSubSys->setup((char*)"Counter",m_expandCounterServices);
  if (m_HistSubSys != 0) m_HistSubSys->setup((char*)"Histos");
  if (m_HistSubSys != 0)
  {
    if (m_CntrMgr != 0)
    {
      MonHist *h = (MonHist*)m_HistSubSys->findobj("COUNTERRATE");
      if (h == 0)
      {
        h = new MonHist(msgSvc(),"COUNTERRATE",m_CntrMgr);
        m_HistSubSys->addObj(h);
      }
      h->makeCounters();
    }
  }
  DimServer::autoStartOn();
  DimServer::start();
  m_MonSSysSize = m_MonSys->Subsyslist->size();
  m_MonSys->start();
  m_started = true;
  return StatusCode::SUCCESS;
}

StatusCode MonitorSvc::i_stop()
{
//  DimLock l;
  m_i_startState = false;
  if (m_CntrMgr != 0)
  {
    m_CntrMgr->open();
  }
  StopUpdate();
  return StatusCode::SUCCESS;
}
void MonitorSvc::StopUpdate()
{
  if ( m_MonSys )
  {
    m_MonSys->stop();
  }
  StopSaving();
}

//updateSvc and resetHistos methods are for fast run changes

StatusCode MonitorSvc::finalize()
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::DEBUG << "MonitorSvc Finalizer" << endmsg;

  if ( m_incidentSvc )
  {
    m_incidentSvc->removeListener(this);
    m_incidentSvc->release();
    m_incidentSvc = 0;
  }
  //printf("MonitorSvc: Locking DIM\n");
  dim_lock();
  if (m_started)
  {
    m_started = false;
    if (m_CntrMgr != 0)
    {
      m_CntrMgr->open();
    }
    m_MonSys->stop();
  }
  dim_unlock();
  deletePtr(m_HistSubSys);
  deletePtr(m_CntrSubSys);
  deletePtr(m_RateMgr);
  deletePtr(m_CntrMgr);
  deletePtr(m_savetimer);
  deletePtr(m_StatusService);
  msg << MSG::DEBUG << "finalized successfully" << endmsg;

  //printf("MonitorSvc: UNLocking DIM\n");
  return Service::finalize();
}

void MonitorSvc::setRunNo(int runno)
{
  m_runno = runno;
  m_MonSys->setRunNo(runno);
}

void MonitorSvc::addRate(MonSubSys *ss, CntrMgr *cm)
{
  MonHist *h = (MonHist*)ss->findobj("COUNTERRATE");
  if (h == 0)
  {
    h = new MonHist(msgSvc(),"COUNTERRATE",cm);
    ss->addObj(h);
  }
  return;
}

template<class T> void MonitorSvc::i_declareCounter(const string& nam, const T&  var,
                             const string& desc, const IInterface* owner)
{
  MsgStream msg(msgSvc(),name());
  string oname = infoOwnerName(owner);
  if (nam.find("COUNTER_TO_RATE") != string::npos)
  {
    string newName = extract("COUNTER_TO_RATE", nam);
    if ( 0 == m_disableMonRate)
    {
//      if (m_CntrMgr == 0)
//      {
//        m_CntrMgr = new CntrMgr(msgSvc(),"MonitorSvc",0);
//      }
//      m_CntrMgr->addCounter(oname+"/"+newName,desc,var);
//      m_InfoMap.insert(pair<string,void*>(oname+"/"+newName,(void*)m_CntrMgr));
//      if (m_HistSubSys == 0)
//      {
//        m_HistSubSys = new MonSubSys(m_updateInterval);
//        m_HistSubSys->m_type = MONSUBSYS_Histogram;
//      }
//      addRate(m_HistSubSys,m_CntrMgr);


      if (m_RateMgr == 0)
      {
        m_RateMgr = new RateMgr(msgSvc(),"MonitorSvc",0);
      }
      std::string nname;
      nname = "R_"+oname+"/"+newName;
      MonRate<T> *mr = new MonRate<T>(nname,desc,var);
      m_RateMgr->addRate(nname,*mr);
      m_InfoMap.insert(pair<string,void*>(nname,(void*)m_RateMgr));
      if (m_CntrSubSys == 0)
      {
        m_CntrSubSys = new MonSubSys(m_CounterInterval);
        m_CntrSubSys->m_type = MONSUBSYS_Counter;
      }
      m_CntrSubSys->addRateMgr(m_RateMgr);
      m_CntrSubSys->addObj(mr);
    }
    else
    {
      msg << MSG::DEBUG << "Counter "<< newName << " can not be declared because MonRate process is disabled." << endmsg;
    }
    return;
  }
  MonCounter *cnt = new MonCounter((char*)(oname+"/"+nam).c_str(),(char*)desc.c_str(),(T*)&var);
  if (m_CntrSubSys == 0)
  {
    m_CntrSubSys = new MonSubSys(m_CounterInterval);
    m_CntrSubSys->m_type = MONSUBSYS_Counter;
  }
  m_CntrSubSys->addObj(cnt);
  m_InfoMap.insert(pair<string,void*>(oname+"/"+nam,(void*)m_CntrSubSys));
  if (m_RateMgr == 0)
  {
    m_RateMgr = new RateMgr(msgSvc(),"MonitorSvc",0);
  }
  std::string nname;
  nname = "R_"+oname+"/"+nam;
  MonRate<T> *mr = new MonRate<T>(nname,desc,var);
  m_RateMgr->addRate(nname,*mr);
  m_InfoMap.insert(pair<string,void*>(nname,(void*)m_RateMgr));
  m_CntrSubSys->addRateMgr(m_RateMgr);
  m_CntrSubSys->addObj(mr);
}

void MonitorSvc::i_unsupported(const string& nam, const type_info& typ, const IInterface* owner)
{
  MsgStream msg(msgSvc(),name());
  string oname = infoOwnerName(owner);
  string typ_name = System::typeinfoName(typ);
  msg << MSG::ERROR << "declareInfo(" << typ_name << ") not implemented " << oname << "/" << nam << endmsg;
  return;
}

//
void MonitorSvc::declareInfo(const string& name, const int&  var,
                             const string& desc, const IInterface* owner)
{
  if (0 == m_disableDeclareInfoInt) this->i_declareCounter(name,var,desc,owner);
}

void MonitorSvc::declareInfo(const string& name, const long&  var,
                             const string& desc, const IInterface* owner)
{
  if (0 == m_disableDeclareInfoLong) this->i_declareCounter(name,var,desc,owner);
}

void MonitorSvc::declareInfo(const string& name, const double& var,
                             const string& desc, const IInterface* owner)
{
  this->i_declareCounter(name,var,desc,owner);
}

void MonitorSvc::declareInfo(const string& name, const bool& var,
                             const string&, const IInterface* owner)
{
  this->i_unsupported(name,typeid(var),owner);
}

void MonitorSvc::declareInfo(const string& name, const string& var,
                             const string&, const IInterface* owner)
{
  this->i_unsupported(name,typeid(var),owner);
}

void MonitorSvc::declareInfo(const string& name, const pair<double,double>& var,
                             const string&, const IInterface* owner)
{
  this->i_unsupported(name,typeid(var),owner);
}

//
void MonitorSvc::declareInfo(const string& name, const string& format, const void * var,
    int size, const string& desc, const IInterface* owner)
{
//  if (0 != m_disableDeclareInfoFormat) return;
  string oname = infoOwnerName(owner);
  if (format.empty() || (size == 0)) return;

  if (m_started)
  {
    //printf("Delcare Info called after start for Name %s\n",name.c_str());
  }
  if (0 != m_disableDeclareInfoFormat) return;
  MsgStream msg(msgSvc(),"MonitorSvc");
  MonCounter *cnt = new MonCounter((char*)(oname+"/"+name).c_str(),(char*)desc.c_str(),format, (void*)var,size);
  if (m_CntrSubSys == 0)
  {
    m_CntrSubSys = new MonSubSys(m_CounterInterval);
    m_CntrSubSys->m_type = MONSUBSYS_Counter;
  }
  m_InfoMap.insert(pair<string,void*>(oname+"/"+name,(void*)m_CntrSubSys));
  m_CntrSubSys->addObj(cnt);
  if (m_RateMgr == 0)
  {
    m_RateMgr = new RateMgr(msgSvc(),"MonitorSvc",0);
  }
  std::string nname;
  nname = "R_"+oname+"/"+name;
  char fch = format[0];
  fch = tolower(fch);
  MonRateBase *mr;
  switch (fch)
  {
    case 'x':
    {
      mr = new MonRateA<long long*>(nname,desc,format,(long long *)var,size);
      break;
    }
    case 'i':
    {
      mr = new MonRateA<int*>(nname,desc,format,(int*)var,size);
      break;
    }
    case 'f':
    {
      mr = new MonRateA<float*>(nname,desc,format,(float*)var,size);
      break;
    }
    case 'd':
    {
      mr = new MonRateA<double*>(nname,desc,format,(double*)var,size);
      break;
    }
    default:
    {
      return;
    }
  }
  m_RateMgr->addRate(nname,*mr);
  m_InfoMap.insert(pair<string,void*>(nname,(void*)m_RateMgr));
  m_CntrSubSys->addRateMgr(m_RateMgr);
  m_CntrSubSys->addObj(mr);
  return;
}
//
void MonitorSvc::declareInfo(const string& name, const StatEntity& var,
                             const string& desc, const IInterface* owner)
{
    string oname = infoOwnerName(owner);
  if (m_started)
  {
    printf("Delcare Info called after start for Name %s\n",name.c_str());
  }
  MsgStream msg(msgSvc(),"MonitorSvc");
   msg << MSG::DEBUG << "=========================== DeclareInfo for StatEntity "<< name << endmsg;
  if (name.find("COUNTER_TO_RATE") != string::npos)
  {
    string newName = extract("COUNTER_TO_RATE", name);
    //make newName unique!
    if ( 0 == m_disableMonRate)
    {
//      if (m_CntrMgr == 0)
//      {
//        m_CntrMgr = new CntrMgr(msgSvc(),"MonitorSvc",0);
//      }
//      m_InfoMap.insert(pair<string,void*>(oname+"/"+newName,(void*)m_CntrMgr));
//      m_CntrMgr->addCounter(oname+"/"+newName,desc,var);
//      if (m_HistSubSys == 0)
//      {
//        m_HistSubSys = new MonSubSys(m_updateInterval);
//        m_HistSubSys->m_type = MONSUBSYS_Histogram;
//      }
//      addRate(m_HistSubSys,m_CntrMgr);


      if (m_RateMgr == 0)
      {
        m_RateMgr = new RateMgr(msgSvc(),"MonitorSvc",0);
      }
//
//      This is a real hack as some jerk has declared the member variables in StatEntity as private...
//
      long *se_ent = (long*)AddPtr(&var,0);
      double *se_flg = (double*)AddPtr(&var,8);
      std::string nname;
      nname = "R_"+oname+"/"+newName+"-n_entries";
      MonRate<long long> *mr1 = new MonRate<long long>(nname,desc,*se_ent);
      m_RateMgr->addRate(nname,*mr1);
      m_InfoMap.insert(pair<string,void*>(nname,(void*)m_RateMgr));
      if (m_CntrSubSys == 0)
      {
        m_CntrSubSys = new MonSubSys(m_CounterInterval);
        m_CntrSubSys->m_type = MONSUBSYS_Counter;
      }
      m_CntrSubSys->addRateMgr(m_RateMgr);
      m_CntrSubSys->addObj(mr1);
      nname = "R_"+oname+"/"+newName+"-flag";
      MonRate<double> *mr2 = new MonRate<double>(nname,desc,*se_flg);
      m_RateMgr->addRate(nname,*mr2);
      m_InfoMap.insert(pair<string,void*>(nname,(void*)m_RateMgr));
      m_CntrSubSys->addObj(mr2);
    }
    else
    {
      msg << MSG::DEBUG << "Counter "<< newName << " can not be declared because MonRate process is disabled." << endmsg;
    }
    return;
  }
  else
  {
    if (m_HistSubSys == 0)
    {
      m_HistSubSys = new MonSubSys(m_updateInterval);
      m_HistSubSys->m_type = MONSUBSYS_Histogram;
    }
    MonHist *m = new MonHist(msgSvc(),oname+"/"+name,desc,&var);
    m_InfoMap.insert(pair<string,void*>(oname+"/"+name,(void*)m_HistSubSys));
    m_HistSubSys->addObj(m);
  }
  return;
}
void MonitorSvc::declareInfo(const string& nam, const AIDA::IBaseHistogram* var,
                             const string& , const IInterface* owner)
{
  MsgStream msg(msgSvc(),name());
  string oname = infoOwnerName(owner);
 // msg << MSG::INFO << "m_disableDeclareInfoHistos : " << m_disableDeclareInfoHistos << endmsg;

  if (m_started)
  {
    msg << MSG::INFO << "Declare Info (Histogram) called after start for Name " << nam << endmsg;
  }
  if (0 != m_disableDeclareInfoHistos)
  {
    msg << MSG::DEBUG << "m_disableDeclareInfoHistos DISABLED " << endmsg;
    return;
  }
  if( 0 != dynamic_cast<const AIDA::IProfile1D* >(var) )
  {
  }
  else if( 0 != dynamic_cast<const AIDA::IHistogram1D * >(var) )
  {
  }
  else if( 0 != dynamic_cast<const AIDA::IHistogram2D * >(var) )
  {
  }
  else
  {
    msg << MSG::ERROR << "Unknown histogram type. Source " << nam << endmsg;
    return;
  }
  if (m_HistSubSys == 0)
  {
    m_HistSubSys = new MonSubSys(m_updateInterval);
    m_HistSubSys->m_type = MONSUBSYS_Histogram;
 }

  MonHist *mhist;
  string hnam;
  if (m_disableMonObjectsForHistos == 0)
  {
//    isMonObject = true;
    if (nam.find(oname) == string::npos)
    {
      hnam = oname+"/"+nam;
    }
    else
    {
      hnam = nam;
    }
    mhist = new MonHist(msgSvc(),hnam,var);
    m_InfoMap.insert(make_pair(hnam,m_HistSubSys));
    m_HistSubSys->addObj(mhist);
  }
  else
  {
    msg << MSG::ERROR << "MonitorSvc doesn't have support for declaring histograms/profiles without using MonObjects " << endmsg;
    return;
  }

}

void MonitorSvc::declareMonRateComplement( int& runNumber, unsigned int& tck, int& cycleNumber, double& deltaT, double& offsetTimeFirstEvInRun, double& offsetTimeLastEvInCycle, double& offsetGpsTimeLastEvInCycle)
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::DEBUG << "Inside declareMonRateComplement" << endmsg;

  if ( 0 == m_disableMonRate && 0!=m_CntrMgr)
  {
    m_CntrMgr->addComplement(&runNumber, &tck, &cycleNumber, &deltaT, &offsetTimeFirstEvInRun, &offsetTimeLastEvInCycle, &offsetGpsTimeLastEvInCycle);
    if (m_HistSubSys == 0)
    {
      m_HistSubSys = new MonSubSys(m_updateInterval);
      m_HistSubSys->m_type = MONSUBSYS_Histogram;
    }
    m_HistSubSys->m_updateTimer->setExtLastDelta((unsigned long long *)&deltaT);
  }
  else  msg << MSG::INFO << "Complement of MonRate was not declared because MonRate process is disabled." << endmsg;

}

string MonitorSvc::extract(const string mascara, string value){
  if (value.find(mascara) != string::npos) {
    value.erase(0, mascara.size()+1);
    value.erase(value.size() - 1, 1);
  }
  return value;
}

string MonitorSvc::infoOwnerName( const IInterface* owner )
{
  const IAlgorithm* ownerAlg = dynamic_cast<const IAlgorithm*>(owner);
  if( 0 != ownerAlg ) // It's an algorithm
    return ownerAlg->name();
  const IAlgTool* ownerTool = dynamic_cast<const IAlgTool*>(owner);
  if( 0 != ownerTool ) // It's a  tool
    return ownerTool->name();
  const IService* ownerSvc  = dynamic_cast<const IService*>(owner);
  if( 0 != ownerSvc ) // It's a service
    return ownerSvc->name();
  // NULL pointer or Unknown interface:
  return "";
}


void MonitorSvc::updateSvc( const string& , int runno, const IInterface*  )
{
  m_MonSys->EORUpdate(runno);
}
void MonitorSvc::updatePerSvc(unsigned long ref)
{
  m_MonSys->Update(ref);
}
void MonitorSvc::resetHistos( const IInterface*  )
{
  m_MonSys->Clear();
}

void MonitorSvc::Lock(void)
{
  m_MonSys->Lock();
}

void MonitorSvc::UnLock(void)
{
  m_MonSys->unLock();
}

void MonitorSvc::StartSaving(string &dir, string &part,string &task, int period, DimService *sav)
{
  if (m_savetimer == 0)
  {
    m_savetimer = new TaskSaveTimer(m_HistSubSys,period);
    m_savetimer->setPartName(part);
    m_savetimer->setRootDir(dir);
    m_savetimer->setTaskName(task);
    m_savetimer->m_filenamesvc = sav;
    m_savetimer->setMonSys(m_MonSys);
  }
  m_savetimer->Start();
}

void MonitorSvc::StopSaving()
{
  if (m_savetimer!= 0)
  {
    m_savetimer->Stop();
  }
}
