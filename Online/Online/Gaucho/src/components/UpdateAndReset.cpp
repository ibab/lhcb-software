//      ====================================================================
//      UpdateAndReset.cpp
//      --------------------------------------------------------------------
//
//      Package   : Gaucho
//
//      Author    : Eric van Herwijnen
//      Should be run as first algorithm in TopAlg
//      Checks:
//      1) If the run number has changed. If so, updateAll(true) and resetHistos
//      2) If timerelapsed flag is true. If so, updateAll(false)
//
//      ====================================================================


// Include files
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "AIDA/IHistogram.h"
#include "AIDA/IProfile1D.h"
#include "TFile.h"
#include "TH1.h"
#include "TProfile.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TSystem.h"
#include <GaudiUtils/Aida2ROOT.h>
#include "Gaucho/Misc.h"

#include "GaudiKernel/ServiceLocatorHelper.h"
#include "GaudiKernel/IDataManagerSvc.h"

#include "RTL/rtl.h"

#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "MDF/OnlineRunInfo.h"


#include "UpdateAndReset.h"
#include "MonitorSvc.h"
#include "RTL/rtl.h"


// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(UpdateAndReset)

// Constructor
//------------------------------------------------------------------------------
UpdateAndReset::UpdateAndReset(const std::string& name, ISvcLocator* ploc)
  : GaudiAlgorithm(name, ploc)
{
  declareProperty("disableMonRate", m_disableMonRate = 0);
  declareProperty("desiredDeltaTCycle", m_desiredDeltaTCycle = 20);
  declareProperty("disableReadOdin", m_disableReadOdin = 0);
  declareProperty("disableChekInTimer", m_disableChekInTimer = 0);
  declareProperty("disableChekInExecute", m_disableChekInExecute = 0);
  declareProperty("disableUpdateData", m_disableUpdateData = 0);
  declareProperty("disableResetHistos", m_disableResetHistos = 0);

  declareProperty("saveHistograms", m_saveHistograms = 0);
  declareProperty("saveSetDir", m_saveSetDir = "/hist/Savesets");
  declareProperty("saverCycle", m_saverCycle = 900);
  declareProperty("resetHistosAfterSave", m_resetHistosAfterSave = 0);
  declareProperty("abortRetroEvents",m_abortRetroEvents = true);
  declareProperty("publishSaveSetLocation",m_publishSavesetLoc=true);
  declareProperty("teste", m_teste = 100000);
  declareProperty("MyName",m_MyName=""); //partition_TaskName
  declareProperty("DoRunChangeInc",m_doRunChangeInc = false);
  declareProperty("ResetonStart",m_ResetonStart = false);
  declareProperty("ProcLimit",m_ProcLimit = 30);

  m_timerCycle = m_desiredDeltaTCycle - 1;
  m_firstExecute = true;
  m_cycleNumber=0;
  m_runNumber=0;
  gpstime=0;
  m_firstCycleNumber=0;
  m_timeStart=0;
  m_timeFirstEvInRun=0;
  m_offsetTimeFirstEvInRun=0;
  m_timeLastEvInCycle=0;
  m_offsetTimeLastEvInCycle=0;
  m_gpsTimeLastEvInCycle=0;
  m_offsetGpsTimeLastEvInCycle=0;
  m_dimSvcSaveSetLoc = 0;
  m_pGauchoMonitorSvc = 0;
  EoEInc = 0;
  m_incs = 0;
  m_eorNumber = 0;
  m_one = 1;
}


//------------------------------------------------------------------------------
StatusCode UpdateAndReset::initialize() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if( sc.isSuccess() ) msg << MSG::DEBUG << "GaudiAlgorithm Initialized" << endreq;
  else {
    msg << MSG::FATAL << "GaudiAlgorithm not Initialized" << endreq;
    return StatusCode::FAILURE;
  }
  serviceLocator()->service("IncidentSvc",m_incs,true);
  EoEInc = new EoEIncidentListener("",serviceLocator(),-1);
  EoEInc->m_proclimit = m_ProcLimit;
  m_stopdone = false;
  //const std::string& utgid = RTL::processName();
   return StatusCode::SUCCESS;
}

StatusCode UpdateAndReset::start()
{
  m_one = 1;

  MsgStream msg( msgSvc(), name() );
  m_utgid = RTL::processName();
  StatusCode sc;
  m_stopdone = false;
  sc = StatusCode::SUCCESS;
  /*if (m_pGauchoMonitorSvc== 0)*/ sc = serviceLocator()->service("MonitorSvc", m_pGauchoMonitorSvc, false);
  if( sc.isSuccess() ) msg << MSG::DEBUG << "Found the IGauchoMonitorSvc interface" << endreq;
  else {
    msg << MSG::FATAL << "Unable to locate the IGauchoMonitorSvc interface." << endreq;
    return StatusCode::FAILURE;
  }
  // to make the ODIN object. doesn't work.
  //updMgrSvc();


  taskName = "unknownTask";
  partName = "unknownPartition";
  instancenumber = "1";
  if (m_MyName.empty())
  {
    serviceParts = Strsplit((char*)m_utgid.c_str(), (char*)"_");
    if (3 == serviceParts->size())
    {
      partName = serviceParts->at(0);
      taskName = serviceParts->at(1);
      instancenumber = serviceParts->at(2);
      if (partName=="CALD0701")
      {
         partName="LHCb";
         if (instancenumber=="1") {taskName = "CaloDAQCalib";}
         else {taskName = "Calib"+taskName+"_"+instancenumber;}
      }
    }
    else if (4 == serviceParts->size())
    {
      partName = serviceParts->at(0);
      taskName = serviceParts->at(2);
    }
  }
  else
  {
    serviceParts = Strsplit((char*)m_utgid.c_str(), (char*)"_");
    partName = serviceParts->at(0);
    if (partName=="CALD0701")
    {
       partName="LHCb";
    }
    taskName = m_MyName;
  }
  m_infoFileStatus = "SAVESETLOCATION/......................................................";
  infoName = partName+"/"+taskName+"/SAVESETLOCATION";
  if (m_saveHistograms)
  {
    if ((m_dimSvcSaveSetLoc == 0) && m_publishSavesetLoc)
    {
      m_dimSvcSaveSetLoc = new DimService(infoName.c_str(),(char*)m_infoFileStatus.c_str());
    }
  }

  declareInfo("Tasks",m_one,"NumberOfTasks");
  sc = serviceLocator()->service("HistogramDataSvc", m_histogramSvc, true);

  if (sc.isFailure()) msg << MSG::FATAL << "Unable to locate the HistogramDataService" << endreq;
  else msg << MSG::DEBUG << "Found the HistogramDataService" << endreq;

  if (0 == m_desiredDeltaTCycle){
//    msg << MSG::WARNING << "Your algorithm is using the UpdateAndReset algrithm which update and reset data every desiredDeltaTCycle seconds. You didn't fill the desiredDeltaTCycle option in your options file, then we will consider 10 seconds as default." << endreq;
    m_desiredDeltaTCycle = 20;
  }

  if (1 == m_saveHistograms){
    div_t divresult = div (m_saverCycle, m_desiredDeltaTCycle);
    m_numCyclesToSave = divresult.quot;
    if (0 == divresult.rem ) {
    }
    else {
      m_saverCycle = divresult.quot * m_desiredDeltaTCycle;
    }
  }

  // The below part is for test
  m_runNumber = 0;
  m_deltaTRunTest = 8 * m_desiredDeltaTCycle;


 // m_runNumber = currentRunNumber().first.first;
  m_triggerConfigurationKey = currentTCK();
//  m_cycleNumber = currentCycleNumber(m_timeStart).first;
  m_timeFirstEvInRun = m_timeStart;
  m_offsetTimeFirstEvInRun = offsetToBoundary(m_cycleNumber, m_timeFirstEvInRun, true);
  m_timeLastEvInCycle = m_timeStart;
//  m_gpsTimeLastEvInCycle = gpsTime();
  m_runStatus = s_statusNoUpdated;
  m_cycleStatus = s_statusNoUpdated;

   EoEInc->setMonitorSvc(m_pGauchoMonitorSvc);
  if (0==m_disableMonRate)  m_pGauchoMonitorSvc->declareMonRateComplement(m_runNumber, m_triggerConfigurationKey, m_cycleNumber,
      m_deltaTCycle, m_offsetTimeFirstEvInRun, m_offsetTimeLastEvInCycle, m_gpsTimeLastEvInCycle);
  if (this->m_saveHistograms != 0)
  {
    m_pGauchoMonitorSvc->StartSaving(this->m_saveSetDir,partName,taskName,this->m_saverCycle,m_dimSvcSaveSetLoc );
  }
  if (m_ResetonStart)
  {
    m_pGauchoMonitorSvc->resetHistos(this);
  }
  return StatusCode::SUCCESS;
}
//------------------------------------------------------------------------------
StatusCode UpdateAndReset::execute()
{
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );

  int runno;
  unsigned int tck;
  ulonglong gps;
  EoEInc->m_startTime = time(0);
  m_pGauchoMonitorSvc->Lock();
  this->EoEInc->m_executing = true;
//  printf("+++++++++++++++++++++++++++++ UPDATE AND RESET Monitor System LockED\n");
  getEventChar(runno,tck,gps,m_L0EvtNr);
  EoEInc->m_RunNr = runno;
  EoEInc->m_EvtNr=m_L0EvtNr;
  if (m_runNumber == 0)
  {
//    printf ("================================> FIRST EVENT ... Run number %d\n",runno);
    m_runNumber = runno;
    m_triggerConfigurationKey = tck;
    m_pGauchoMonitorSvc->setRunNo(runno);

  }
  if (runno > m_runNumber)
  {
//    printf("+_+_+_+_+_+_+_+_+_+_+_++_+_+_ Different Run!!! \n");
    m_pGauchoMonitorSvc->UnLock();
    m_pGauchoMonitorSvc->updateSvc("this",m_runNumber,this);
    m_pGauchoMonitorSvc->Lock();
    if ( 1 == m_saveHistograms )
    {
       //calling finalize - don't need to reset, they probably don't exist anymore
       m_eorNumber=m_runNumber;
       m_pGauchoMonitorSvc->UnLock();
       manageTESHistos(false, true, true, true);
       m_pGauchoMonitorSvc->Lock();
    }
    m_triggerConfigurationKey = tck;
    if (this->m_doRunChangeInc)
    {
      this->m_incs->fireIncident(Incident(name(),IncidentType::EndRun));
    }
    m_pGauchoMonitorSvc->resetHistos( this  );
    m_pGauchoMonitorSvc->setRunNo(runno);
    m_runNumber = runno;
  }
  else if (runno < m_runNumber)
  {
    if (m_abortRetroEvents)
    {
      m_incs->fireIncident(Incident(name(),IncidentType::AbortEvent));
      return StatusCode::SUCCESS;
    }
  }

  m_one = 1;
  m_gpsTimeLastEvInCycle = (double)gps;
//  printf("GPS Time %f\n",m_gpsTimeLastEvInCycle);
//  m_pGauchoMonitorSvc->UnLock();
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode UpdateAndReset::stop() {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "finalizing...." << endreq;
//  printf("======================UpdateAndReset STOP called\n");
  if ( 1 == m_saveHistograms )
  {
     m_eorNumber=m_runNumber;
     manageTESHistos(false, true, true, true);
     if ( 0 != m_pGauchoMonitorSvc )
     {
       m_pGauchoMonitorSvc->StopSaving();
     }
  }
  else if ( 0 != m_pGauchoMonitorSvc )
  {
//    m_pGauchoMonitorSvc->StopUpdate();
//    printf("======================UpdateAndReset Updating EOR service \n");
//    this->m_pGauchoMonitorSvc->Lock();
    this->m_pGauchoMonitorSvc->updateSvc( "this" , m_runNumber,this  );
    if (!m_ResetonStart)
    {
      m_pGauchoMonitorSvc->resetHistos(0);
    }
//    this->m_pGauchoMonitorSvc->UnLock();
//    printf("======================UpdateAndReset Updating EOR DONE...... \n");
    m_one = 0;
    m_pGauchoMonitorSvc->release();
    m_pGauchoMonitorSvc = 0;
  }
  m_stopdone = true;
  return StatusCode::SUCCESS;
}
StatusCode UpdateAndReset::restart()
{
  return StatusCode::SUCCESS;
}
StatusCode UpdateAndReset::finalize() {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "finalizing...." << endreq;
//  if (m_stopdone)
//  {
//    return StatusCode::SUCCESS;
//  }
//  else if ( 1 == m_saveHistograms )
//  {
//     m_eorNumber=m_runNumber;
//     manageTESHistos(false, false, true, true);
//  }
//  else if ( 0 != m_pGauchoMonitorSvc )
//  {
//    this->m_pGauchoMonitorSvc->Lock();
//    this->m_pGauchoMonitorSvc->updateSvc( "this" , m_runNumber,this  );
//    this->m_pGauchoMonitorSvc->UnLock();
//    m_pGauchoMonitorSvc->release();
//    m_pGauchoMonitorSvc = 0;
//  }
  if (m_dimSvcSaveSetLoc != 0)
  {
    delete m_dimSvcSaveSetLoc;
    m_dimSvcSaveSetLoc = 0;
  }
  if (EoEInc != 0) delete EoEInc;
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
//void UpdateAndReset::retrieveRunNumber(int runNumber, ulonglong gpsTime) {
////------------------------------------------------------------------------------
//
//  m_runNumber = runNumber;
//  m_runStatus = s_statusNoUpdated;
//  m_timeFirstEvInRun = gpsTime;
//  m_offsetTimeFirstEvInRun = offsetToBoundary(currentCycleNumber(m_timeFirstEvInRun).first, m_timeFirstEvInRun, true);
//}

//------------------------------------------------------------------------------
// std::pair<int, bool> UpdateAndReset::currentRunNumber() {
void UpdateAndReset::getEventChar(int &runno, unsigned int &tck, ulonglong &gps,long long int &L0EvtNr)
//------------------------------------------------------------------------------
{
  MsgStream msg( msgSvc(), name() );

  //msg << MSG::DEBUG<< "Reading ODIN Bank. " <<endreq;

  if( exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default ) )
  {
     m_rawEvt= get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
     LHCb::RawBank::BankType i = LHCb::RawBank::BankType(16); // 16 is Odin bank ID...
     std::string bname = LHCb::RawBank::typeName(i);
     std::string::size_type odinfound=bname.find("ODIN",0);

     if (odinfound!=std::string::npos)
     {
      const std::vector<LHCb::RawBank*>& b =m_rawEvt->banks(i);
      if ( b.size() > 0 )
      {
        std::vector<LHCb::RawBank*>::const_iterator itB;
       // int k = 0;
        for( itB = b.begin(); itB != b.end(); itB++ )
        {
           //if ( ((k++)%4) == 0 ) info() << endmsg << "  " ;
           const LHCb::RawBank* r = *itB;
    // const LHCb::OnlineRunInfo* ori = 0;
           const LHCb::OnlineRunInfo* ori=r->begin<LHCb::OnlineRunInfo>();
           tck=ori->TCK;
           gps=ori->GPSTime;
           runno=ori->Run;
           L0EvtNr=ori->L0ID;
        }
      }
    }
    else
    {
       msg << MSG::DEBUG << "ODIN bank not found at location '" << rootInTES() << LHCb::RawEventLocation::Default << endreq;;
    }
  }
  else
  {
     msg << MSG::DEBUG << "rawEvent not found at location '" << rootInTES() << LHCb::RawEventLocation::Default << endreq;
  }
}
//------------------------------------------------------------------------------

unsigned int UpdateAndReset::currentTCK() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  unsigned int triggerConfigurationKey=0;

  //msg << MSG::DEBUG<< "Reading ODIN Bank. " <<endreq;

  if (0 == m_disableReadOdin) {
    triggerConfigurationKey=tck;
  }
  else {
  }
  return triggerConfigurationKey;
}


//------------------------------------------------------------------------------
void UpdateAndReset::retrieveCycleNumber(int cycleNumber) {
//------------------------------------------------------------------------------
  // In order to avoid conflict, the cycle number is updated here and only here
  m_cycleNumber = cycleNumber;
  m_cycleStatus = s_statusNoUpdated;
}

double UpdateAndReset::offsetToBoundary(int cycleNumber, ulonglong time, bool inferior){
  if (inferior) {
    ulonglong timeIniCycle = ((ulonglong) cycleNumber)*((ulonglong)(m_desiredDeltaTCycle*1000000));
    return ((double)(time - timeIniCycle));
  }
  else {
    ulonglong timeEndCycle = ((ulonglong) (cycleNumber+1))*((ulonglong)(m_desiredDeltaTCycle*1000000));
    return ((double)(time - timeEndCycle));
  }
/*    if (inferior) {
    double timeIniCycle = cycleNumber*m_desiredDeltaTCycle*1000000;
    return ((double)((double)time - timeIniCycle));
  }
  else {
    double timeEndCycle = (cycleNumber+1)*m_desiredDeltaTCycle*1000000;
    return ((double)((double)time - timeEndCycle));
  }*/
}


bool UpdateAndReset::isSaveCycle(int m_cycleNumber) {
  if (m_cycleNumber == m_firstCycleNumber) return false;
  div_t divresult = div (m_cycleNumber - m_firstCycleNumber, m_numCyclesToSave);
  if (0 != divresult.rem) return false;
  return true;
}

void UpdateAndReset::manageTESHistos (bool list, bool reset, bool save, bool isFromEndOfRun) {
  MsgStream msg( msgSvc(), name() );
  IRegistry* object = rootObject();
  int level = 0;
  std::vector<std::string> idList;
  msg << MSG::DEBUG << "managing histos list " << list << " reset " << reset << " save " << save << " endofrun " << isFromEndOfRun << endreq;
  TFile *f=0;
  m_infoFileStatus = "......this is the file name were we will save histograms...........";
  char timestr[64];
  char year[5];
  char month[3];
  char day[3];
  time_t rawTime=time(NULL);
  struct tm* timeInfo = localtime(&rawTime);
  ::strftime(timestr, sizeof(timestr),"%Y%m%dT%H%M%S", timeInfo);
  ::strftime(year, sizeof(year),"%Y", timeInfo);
  ::strftime(month, sizeof(month),"%m", timeInfo);
  ::strftime(day, sizeof(day),"%d", timeInfo);


  if (save)
  {
     //std::string dirName = m_saveSetDir + "/" + year + "/" + partName + "/" + taskName;
     //add the month and day to avoid too many files per year
     std::string dirName = m_saveSetDir + "/" + year + "/" + partName + "/" + taskName + "/" + month + "/" + day;
     void *dir = gSystem->OpenDirectory(dirName.c_str());
     if ((dir == 0) && (save)) {
     gSystem->mkdir(dirName.c_str(),true);
    }
    std::string tmpfile="";
    //add runnumber to saveset name

    if (m_runNumber != 0)
    {
       std::string runNumberstr;
       std::stringstream outstr;
       if (isFromEndOfRun) outstr << m_eorNumber;
       else outstr << m_runNumber;
       runNumberstr=outstr.str();
       if (isFromEndOfRun) tmpfile = dirName + "/" + taskName + "-" + runNumberstr + "-" + timestr + "-EOR.root";
       else tmpfile = dirName + "/" + taskName + "-" + runNumberstr + "-" + timestr + ".root";
    }
    else
    {
       if (isFromEndOfRun)  tmpfile = dirName + "/" + taskName + "-0-" + timestr + "-EOR.root";
       else tmpfile = dirName + "/" + taskName + "-0-" + timestr + ".root";
    }
   // std::string tmpfile = dirName + "/" + taskName + "-" + timestr + ".root";
   // if (isFromEndOfRun) tmpfile = dirName + "/" + taskName + "-" + timestr + "-EOR.root";
      msg << MSG::DEBUG << "updating infofile status" << endreq;
    m_infoFileStatus.replace(0, m_infoFileStatus.length(), tmpfile);


 //   msg << MSG::INFO << "We will save histograms in file " << m_infoFileStatus << endreq;
    f = new TFile(m_infoFileStatus.c_str(),"create");
  }
  if (f!=0)
  {
    if(! f->IsZombie())
    {
      histogramIdentifier(object, idList, reset, save, level, (TDirectory*) f);
      if (save)
      {
        f->Close();
        delete f;f=0;
      }

    }
    else
    {
      std::string errorTmpfile = "Error Saving Data => Zombie File..!!!!!";
      m_infoFileStatus.replace(0, m_infoFileStatus.length(), errorTmpfile);
      msg << MSG::DEBUG << "error opening file "<< m_infoFileStatus << ". Closing it." << endreq;
      if (f->IsOpen()) {f->Close();}
      delete f;f=0;
    }
    if (m_publishSavesetLoc)
    {
      m_dimSvcSaveSetLoc->updateService((char*)m_infoFileStatus.c_str());
    }
  }
  else
  {
     //f=0 because should also be able to reset without saving
     histogramIdentifier(object, idList, reset, save, level, (TDirectory*) f);
  }

}

void UpdateAndReset::histogramIdentifier(IRegistry* object, std::vector<
  std::string> &idList, bool reset, bool save, int &level, TDirectory* rootdir)
{
  MsgStream msg(msgSvc(), name());
  std::vector<IRegistry*> leaves;
  std::vector<IRegistry*>::const_iterator it;
  Bool_t dirstat = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);
  try
  {
    //     msg << MSG::INFO << "Looking for histos in object " << object->identifier() << ", level  " << level << endreq;
    SmartIF<IDataManagerSvc> dataManagerSvc(m_histogramSvc);
    if (!dataManagerSvc)
    {
      //       msg << MSG::WARNING << "    Unable to go to the transient store. " << endreq;
      TH1::AddDirectory(dirstat);
      return;
    }

    StatusCode sc = dataManagerSvc->objectLeaves(object, leaves);
    if (sc.isFailure())
    {
      //        msg << MSG::WARNING << "    No histograms found on the transient store." << endreq;
      TH1::AddDirectory(dirstat);
      return;
    }

    for (it = leaves.begin(); it != leaves.end(); it++)
    {
      const std::string& id = (*it)->identifier();
      if (rootdir != 0)
        rootdir->cd();
      //       msg << MSG::DEBUG << "    Object found: " << id << endreq;

      DataObject* dataObject;
      sc = m_histogramSvc->retrieveObject(id, dataObject);
      if (sc.isFailure())
      {
        //         msg << MSG::WARNING << "Could not retrieve object from TES " << endreq;
        continue;
      }

      IHistogram* histogram = dynamic_cast<AIDA::IHistogram*> (dataObject);
      if (0 != histogram)
      {
        if (save)
        {
          TH1* hRoot = (TH1*) Gaudi::Utils::Aida2ROOT::aida2root(histogram);
          dyn_string *HistoFullName = Strsplit((char*) hRoot->GetName(),(char*) "/");
          gDirectory->Cd("/");
          for (unsigned int i=0;i<HistoFullName->size()-1;i++)
          {
            TKey *k;
            k = gDirectory->GetKey(HistoFullName->at(i).c_str());
            if (k == 0)
            {
              gDirectory->mkdir(HistoFullName->at(i).c_str());
            }
            gDirectory->Cd(HistoFullName->at(i).c_str());
          }
          hRoot->Write(HistoFullName->at(HistoFullName->size() - 1).c_str());
          //         msg << MSG::INFO << ", saving name=" << hRoot->GetName() << " directory="
          //          << (hRoot->GetDirectory() ? hRoot->GetDirectory()->GetName() : "none") <<endreq;
          // should we reset on the root level?
          // if (reset) hRoot->Reset();
        }
        // msg << MSG::DEBUG << "Resetting histogram" << endreq;
        if (reset)
        {
          // msg << MSG::INFO << "Resetting histogram" << endreq;
          histogram->reset();
        }
        idList.push_back(id);
        continue;
      }
      IProfile1D* profile = dynamic_cast<AIDA::IProfile1D*> (dataObject);
      if (0 != profile)
      {
        if (save)
        {
          TProfile* hRoot = (TProfile*) Gaudi::Utils::Aida2ROOT::aida2root(
              profile);
          dyn_string *HistoFullName = Strsplit((char*) hRoot->GetName(),
              (char*) "/");
          gDirectory->Cd("/");
          for (unsigned int i=0;i<HistoFullName->size()-1;i++)
          {
            TKey *k;
            k = gDirectory->GetKey(HistoFullName->at(i).c_str());
            if (k == 0)
            {
              gDirectory->mkdir(HistoFullName->at(i).c_str());
            }
            gDirectory->Cd(HistoFullName->at(i).c_str());
          }
          hRoot->Write(HistoFullName->at(HistoFullName->size() - 1).c_str());
          //should we reset at the root level?
          //if (reset) hRoot->Reset();
        }
        if (reset)
        {
          // msg << MSG::DEBUG << "Resetting profile" << endreq;
          profile->reset();
        }
        idList.push_back(id);
        continue;
      }

      // not an histogram: must be a directory: create corresponding TDirectory
      dyn_string *rootDirs = Strsplit((char*) id.c_str(), (char*) "/");
      TDirectory* newdir = rootdir;
      if (NULL != newdir)
      {
        newdir = rootdir->mkdir(rootDirs->at(rootDirs->size() - 1).c_str());
        newdir->cd();
      }
      int newLevel = level + 1;
      if (newLevel >= 10)
        continue;
      histogramIdentifier(*it, idList, reset, save, newLevel, newdir);
    }
  }
  catch (const std::exception &ex)
  {
    msg << MSG::WARNING << "histogramidentifier std::exception: " << ex.what()
        << endreq;
  }
  TH1::AddDirectory(dirstat);
}

IRegistry* UpdateAndReset::rootObject(){
  MsgStream msg( msgSvc(), name() );
  std::string  path;
  SmartDataPtr<DataObject> smartDataPtr(m_histogramSvc, path);
 // msg << MSG::DEBUG << "root identifier : " << smartDataPtr->registry()->identifier() << endreq;
  return smartDataPtr->registry();
}



