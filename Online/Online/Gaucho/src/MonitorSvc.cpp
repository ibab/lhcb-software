// Include files
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IAlgTool.h"
//#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/SmartIF.h"
#include "Gaucho/CntrMgr.h"
#include "Gaucho/MonHist.h"
#include "MonitorSvc.h"
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
  declareProperty("disableDeclareInfoFormat", m_disableDeclareInfoFormat = 1);
  declareProperty("disableDeclareInfoHistos", m_disableDeclareInfoHistos = 0);
  declareProperty("maxNumCountersMonRate", m_maxNumCountersMonRate = 1000);
  declareProperty("DimUpdateInterval", m_updateInterval = 20);
  declareProperty("ExpandCounterServices",m_expandCounterServices=0);
  declareProperty("ExpandNameInfix",m_expandInfix="");
  declareProperty("PartitionName",m_partname="LHcb");
  declareProperty("ProcessName",m_ProcName="");
  declareProperty("ProgramName",m_ProgName="");
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
  MsgStream msg(msgSvc(),"MonitorSvc");
  StatusCode sc = Service::initialize();

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
  log << MSG::INFO << "Got incident from:" << inc.source() << ": " << inc.type() << endmsg;
  if ( inc.type() == "APP_RUNNING" ) {
    i_start();
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
    for (m_CntrMgr->m_counterMapIt = m_CntrMgr->m_counterMap.begin();m_CntrMgr->m_counterMapIt != m_CntrMgr->m_counterMap.end();
        m_CntrMgr->m_counterMapIt++)
    {
      nam = m_CntrMgr->m_counterMapIt->first;
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
  return Service::start();
}

StatusCode MonitorSvc::i_start()
{
//  CALLGRIND_START_INSTRUMENTATION
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::INFO << "======== MonitorSvc start() called ============= " << endmsg;
//  setProperties();
  DimServer::autoStartOff();
  if (m_CntrMgr != 0)
  {
//    printf("In STARTS Method... Counter Manager present... Closing it...\n");
    m_CntrMgr->close();
  }
  if (m_started)
  {
    m_MonSys->start();
    DimServer::autoStartOn();
    DimServer::start();
    return StatusCode::SUCCESS;
  }
  if (m_CntrSubSys != 0)
  {
    m_CntrSubSys->m_expandnames = m_expandCounterServices;
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
  m_MonSys->start();
  m_started = true;
  return StatusCode::SUCCESS;
}
StatusCode MonitorSvc::stop()
{
  if (m_CntrMgr != 0)
  {
    m_CntrMgr->open();
  }
  if ( m_MonSys )  
  {
    m_MonSys->stop();
  }
  return Service::stop();
}

//updateSvc and resetHistos methods are for fast run changes

StatusCode MonitorSvc::finalize()
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::DEBUG << "MonitorSvc Destructor" << endmsg;

  if ( m_incidentSvc )
  {
    m_incidentSvc->removeListener(this);
    m_incidentSvc->release();
    m_incidentSvc = 0;
  }
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
  deletePtr(m_HistSubSys);
  deletePtr(m_CntrSubSys);
  deletePtr(m_CntrMgr);
  deletePtr(m_savetimer);
  msg << MSG::DEBUG << "finalized successfully" << endmsg;

  dim_unlock();
  return Service::finalize();
}

void MonitorSvc::setRunNo(int runno)
{
  m_runno = runno;
  m_MonSys->setRunNo(runno);
}

void MonitorSvc::declareInfo(const string& name, const bool&  ,
                             const string& , const IInterface* owner)
{
  string oname = infoOwnerName(owner);
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::ERROR << "declareInfo bool not implemented " << name << endmsg;
  return;
}
//
void MonitorSvc::declareInfo(const string& name, const int&  var,
                             const string& desc, const IInterface* owner)
{
  string oname = infoOwnerName(owner);
  if (m_started)
  {
//    printf("Delcare Info called after start for Name %s\n",name.c_str());
  }
  if (0 != m_disableDeclareInfoInt) return;
  MsgStream msg(msgSvc(),"MonitorSvc");
  if (name.find("COUNTER_TO_RATE") != string::npos)
  {
    string newName = extract("COUNTER_TO_RATE", name);
    if ( 0 == m_disableMonRate)
    {
      if (m_CntrMgr == 0)
      {
        m_CntrMgr = new CntrMgr(msgSvc(),"MonitorSvc",0);
      }
      m_CntrMgr->addCounter(oname+"/"+newName,desc,var);
      m_InfoMap.insert(pair<string,void*>(oname+"/"+newName,(void*)m_CntrMgr));
      if (m_HistSubSys == 0)
      {
        m_HistSubSys = new MonSubSys(m_updateInterval);
        m_HistSubSys->m_type = MONSUBSYS_Histogram;
      }
      addRate(m_HistSubSys,m_CntrMgr);
    }
    else
    {
      msg << MSG::INFO << "Counter "<< newName << " can not be declared because MonRate process is disabled." << endmsg;
    }
    return;
  }
  MonCounter *cnt = new MonCounter((char*)(oname+"/"+name).c_str(),(char*)desc.c_str(),(int*)&var);
  if (m_CntrSubSys == 0)
  {
    m_CntrSubSys = new MonSubSys(m_updateInterval);
    m_CntrSubSys->m_type = MONSUBSYS_Counter;
  }
  m_InfoMap.insert(pair<string,void*>(oname+"/"+name,(void*)m_CntrSubSys));
  m_CntrSubSys->addObj(cnt);
}

void MonitorSvc::declareInfo(const string& name, const long&  var,
                             const string& desc, const IInterface* owner)
{
  string oname = infoOwnerName(owner);
  if (m_started)
  {
//    printf("Delcare Info called after start for Name %s\n",name.c_str());
  }
  if (0 != m_disableDeclareInfoLong) return;
  MsgStream msg(msgSvc(),"MonitorSvc");
  if (name.find("COUNTER_TO_RATE") != string::npos)
  {
    string newName = extract("COUNTER_TO_RATE", name);
    if ( 0 == m_disableMonRate)
    {
      if (m_CntrMgr == 0)
      {
        m_CntrMgr = new CntrMgr(msgSvc(),"MonitorSvc",0);
      }
      m_InfoMap.insert(pair<string,void*>(oname+"/"+newName,(void*)m_CntrMgr));
      m_CntrMgr->addCounter(oname+"/"+newName,desc,var);
      if (m_HistSubSys == 0)
      {
        m_HistSubSys = new MonSubSys(m_updateInterval);
        m_HistSubSys->m_type = MONSUBSYS_Histogram;
//        m_MonSys->addSubSys(m_HistSubSys);
//        m_HistSubSys->setup((char*)"Histos");
//        m_HistSubSys->start();
      }
      addRate(m_HistSubSys,m_CntrMgr);
    }
    else
    {
      msg << MSG::INFO << "Counter "<< newName << " can not be declared because MonRate process is disabled." << endmsg;
    }
    return;
  }

  MonCounter *cnt = new MonCounter((char*)(oname+"/"+name).c_str(),(char*)desc.c_str(),(long long*)&var);
  if (m_CntrSubSys == 0)
  {
    m_CntrSubSys = new MonSubSys(m_updateInterval);
    m_CntrSubSys->m_type = MONSUBSYS_Counter;
//    m_MonSys->addSubSys(m_CntrSubSys);
//    m_CntrSubSys->setup((char*)"Counter");
//    m_CntrSubSys->start();
  }
  m_InfoMap.insert(pair<string,void*>(oname+"/"+name,(void*)m_CntrSubSys));
  m_CntrSubSys->addObj(cnt);
}
void MonitorSvc::declareInfo(const string& name, const double& var,
                             const string& desc, const IInterface* owner)
{
  if (m_started)
  {
//    printf("Delcare Info called after start for Name %s\n",name.c_str());
  }
  string oname = infoOwnerName(owner);
  if (0 != m_disableDeclareInfoDouble) return;
  MsgStream msg(msgSvc(),"MonitorSvc");
  if (name.find("COUNTER_TO_RATE") != string::npos)
  {
    string newName = extract("COUNTER_TO_RATE", name);
    if ( 0 == m_disableMonRate)
    {
      if (m_CntrMgr == 0)
      {
        m_CntrMgr = new CntrMgr(msgSvc(),"MonitorSvc",0);
      }
      m_InfoMap.insert(pair<string,void*>(oname+"/"+newName,(void*)m_CntrMgr));
      m_CntrMgr->addCounter(oname+"/"+newName,desc,var);
      if (m_HistSubSys == 0)
      {
        m_HistSubSys = new MonSubSys(m_updateInterval);
        m_HistSubSys->m_type = MONSUBSYS_Histogram;
      }
      addRate(m_HistSubSys,m_CntrMgr);
    }
    else
    {
      msg << MSG::INFO << "Counter "<< newName << " can not be declared because MonRate process is disabled." << endmsg;
    }
    return;
  }
  MonCounter *cnt = new MonCounter((char*)(oname+"/"+name).c_str(),(char*)desc.c_str(),(double*)&var);
  if (m_CntrSubSys == 0)
  {
    m_CntrSubSys = new MonSubSys(m_updateInterval);
    m_CntrSubSys->m_type = MONSUBSYS_Counter;
  }
  m_InfoMap.insert(pair<string,void*>(oname+"/"+name,(void*)m_CntrSubSys));
  m_CntrSubSys->addObj(cnt);
}
void MonitorSvc::addRate(MonSubSys *ss, CntrMgr *cm)
{
//  MsgStream msg(msgS(),"MonitorSvc");
  MonHist *h = (MonHist*)ss->findobj("COUNTERRATE");
  if (h == 0)
  {
    h = new MonHist(msgSvc(),"COUNTERRATE",cm);
    ss->addObj(h);
  }
  return;
}
void MonitorSvc::declareInfo(const string& name, const string& ,
                             const string& , const IInterface* owner)
{
  string oname = infoOwnerName(owner);
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::ERROR << "declareInfo for strings not implemented" + name << endmsg;
  return;
}
void MonitorSvc::declareInfo(const string& name, const pair<double,double>& ,
                             const string& , const IInterface* owner)
{
  string oname = infoOwnerName(owner);
  if (0 != m_disableDeclareInfoPair) {};

  if (m_started)
  {
//    printf("Delcare Info called after start for Name %s\n",name.c_str());
  }
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::ERROR << "MonitorSvc doesn't have support for declaring pairs " << name << endmsg;
  return;
}
//
void MonitorSvc::declareInfo(const string& name, const string& format, const void * var,
    int size, const string& desc, const IInterface* owner)
{
//  if (0 != m_disableDeclareInfoFormat) return;
  string oname = infoOwnerName(owner);

  if (m_started)
  {
//    printf("Delcare Info called after start for Name %s\n",name.c_str());
  }
  if (0 != m_disableDeclareInfoFormat) return;
  MsgStream msg(msgSvc(),"MonitorSvc");
  MonCounter *cnt = new MonCounter((char*)(oname+"/"+name).c_str(),(char*)desc.c_str(),format, (void*)var,size);
  if (m_CntrSubSys == 0)
  {
    m_CntrSubSys = new MonSubSys(m_updateInterval);
    m_CntrSubSys->m_type = MONSUBSYS_Counter;
//    m_MonSys->addSubSys(m_CntrSubSys);
//    m_CntrSubSys->setup((char*)"Counter");
//    m_CntrSubSys->start();
  }
  m_InfoMap.insert(pair<string,void*>(oname+"/"+name,(void*)m_CntrSubSys));
  m_CntrSubSys->addObj(cnt);
  return;
}
//
void MonitorSvc::declareInfo(const string& name, const StatEntity& var,
                             const string& desc, const IInterface* owner)
{
	  string oname = infoOwnerName(owner);
  if (m_started)
  {
//    printf("Delcare Info called after start for Name %s\n",name.c_str());
  }
  MsgStream msg(msgSvc(),"MonitorSvc");
   msg << MSG::INFO << "=========================== DeclareInfo for StatEntity "<< name << endmsg;
  if (name.find("COUNTER_TO_RATE") != string::npos)
  {
    string newName = extract("COUNTER_TO_RATE", name);
    //make newName unique!
    if ( 0 == m_disableMonRate)
    {
      if (m_CntrMgr == 0)
      {
        m_CntrMgr = new CntrMgr(msgSvc(),"MonitorSvc",0);
      }
      m_InfoMap.insert(pair<string,void*>(oname+"/"+newName,(void*)m_CntrMgr));
      m_CntrMgr->addCounter(oname+"/"+newName,desc,var);
      if (m_HistSubSys == 0)
      {
        m_HistSubSys = new MonSubSys(m_updateInterval);
        m_HistSubSys->m_type = MONSUBSYS_Histogram;
      }
      addRate(m_HistSubSys,m_CntrMgr);
    }
    else
    {
      msg << MSG::INFO << "Counter "<< newName << " can not be declared because MonRate process is disabled." << endmsg;
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


void MonitorSvc::declareInfo(const string& name, const AIDA::IBaseHistogram* var,
                             const string& , const IInterface* owner)
{
  string oname = infoOwnerName(owner);
  MsgStream msg(msgSvc(),"MonitorSvc");
 // msg << MSG::INFO << "m_disableDeclareInfoHistos : " << m_disableDeclareInfoHistos << endmsg;

  if (m_started)
  {
//    printf("Declare Info (Histogram) called after start for Name %s\n",name.c_str());
  }
  if (0 != m_disableDeclareInfoHistos)
  {
    msg << MSG::INFO << "m_disableDeclareInfoHistos DISABLED " << endmsg;
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
    msg << MSG::ERROR << "Unknown histogram type. Source " << name << endmsg;
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
    if (name.find(oname) == string::npos)
    {
      hnam = oname+"/"+name;
    }
    else
    {
      hnam = name;
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

  if ( 0 == m_disableMonRate)
  {
    if (m_CntrMgr == 0)
    {
      m_CntrMgr = new CntrMgr(msgSvc(),"MonitorSvc",0);
    }
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


bool MonitorSvc::registerName(const string& name, const IInterface* owner)
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::DEBUG << "registerName: " << name << endmsg;
  m_InfoNamesMapIt = m_InfoNamesMap.find(owner);
  string ownerName = infoOwnerName(owner);
  if( m_InfoNamesMapIt != m_InfoNamesMap.end()) {
    pair<set<string>::iterator,bool> p = (*m_InfoNamesMapIt).second.insert(name);
    if( p.second) {
      msg << MSG::DEBUG << "Declaring info: Owner: " << ownerName << " Name: " << name << endmsg;
    }
    else
    { // Insertion failed: Name already exists
      msg << MSG::ERROR << "Already existing info " << name << " from owner " << ownerName << " not published" << endmsg;
      return false;
    }
  }
  else { // Create a new set for this algo and insert name
    msg << MSG::DEBUG << "registerName: creating new map for owner: "<< ownerName << " name: " << name << endmsg;
    m_InfoNamesMap[owner]=set<string>();
    m_InfoNamesMap[owner].insert(name);
  }
  return true;
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
