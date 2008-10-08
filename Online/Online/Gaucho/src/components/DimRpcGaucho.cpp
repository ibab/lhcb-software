#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/SmartIF.h"
#include "DimRpcGaucho.h"
#include "TFile.h"
#include "TH1.h"
#include "TProfile.h"
#include <GaudiUtils/Aida2ROOT.h>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram.h"
#include "AIDA/IProfile1D.h"
#include  <time.h>

DimRpcGaucho::DimRpcGaucho(std::string name, ISvcLocator* svcLocator) : DimRpc(name.c_str(),"C", "C")
 , m_svcLocator(svcLocator)
 , m_msgSvc(0)
 , m_histogramSvc(0)
{
  StatusCode sc = m_svcLocator->service("MessageSvc", m_msgSvc);
  MsgStream msg(m_msgSvc, "DimRpcGaucho");  
  
  if (sc.isFailure()) msg << MSG::FATAL << "Error locating MessageSvc." << endreq;
  else msg << MSG::DEBUG << "MessageSvc located." << endreq;
  
  sc = m_svcLocator->service("HistogramDataSvc", m_histogramSvc, true); 
  
  if (sc.isFailure()) msg << MSG::FATAL << "Unable to locate the HistogramDataService" << endreq;
  else msg << MSG::DEBUG << "Found the HistogramDataService" << endreq;

  sc = m_svcLocator->service("ApplicationMgr", m_applicationMgr);
  
  if (sc.isFailure()) msg << MSG::FATAL << "Unable to locate the ApplicationMgr" << endreq;
  else msg << MSG::DEBUG << "Found ApplicationMgr"<< endreq;

  SmartIF<IAlgManager> algManager ( IID_IAlgManager, m_applicationMgr);
  if( algManager.isValid() ) {
    m_algManager=algManager;
    msg << MSG::DEBUG << "Found the AlgoritmFactory" << endreq;
  }
  else msg << MSG::FATAL << "Unable to locate the AlgorithmFactory" << endreq;
  
  DimServer::start(name.c_str());
}

DimRpcGaucho::~DimRpcGaucho() {
  if (m_histogramSvc) m_histogramSvc->release();
}

void DimRpcGaucho::rpcHandler() {
  MsgStream msg(m_msgSvc, "DimRpcGaucho");  
  std::string rpcCommand = getString();

  msg << MSG::INFO << "Received rpcCommand " << rpcCommand << endreq;
  
  bool list = false;
  bool reset = false;
  bool save = false;
  
  if (rpcCommand.compare("list_histos") == 0) {
    list = true;
    manageHistos (list, reset, save);
  }  
  else if (rpcCommand.compare("reset_histos") == 0) {
    reset = true;
    manageHistos (list, reset, save);
  }
  else if (rpcCommand.compare("save_histos") == 0) {
    save = true;
    manageHistos (list, reset, save);
  }
  else if (rpcCommand.compare("reset_save_histos") == 0) {
    reset = true;
    save = true;
    manageHistos (list, reset, save);
  }
  
}

void DimRpcGaucho::manageHistos (bool list, bool reset, bool save) {
  MsgStream msg(m_msgSvc, "DimRpcGaucho");
  IRegistry* object = rootObject();
  int level = 0;
  std::vector<std::string> idList;
  
  std::string saveDir = "~/w0/";
  
  TFile *f=0;
  std::string fileName = "......this is the file name were we will save histograms...........";
  char timestr[64];
  time_t rawTime=time(NULL);
  struct tm* timeInfo = localtime(&rawTime);
  ::strftime(timestr, sizeof(timestr),"%Y%m%dT%H%M%S", timeInfo);
  
  std::string tmpfile = saveDir + "GroupName" + "-" + timestr + ".root";  
  
  fileName.replace(0, fileName.length(), tmpfile);
  msg << MSG::INFO << "SaverSvc will save histograms in file " << fileName << endreq;

  if (save)  f = new TFile(fileName.c_str(),"create");
  
  histogramIdentifier(object, idList, reset, save, level);
  
  if (0 == idList.size()) msg << MSG::INFO << "No histogram found" << endreq;
  
  if (save) {
    f->Close();
    delete f;f=0;
  }
  
  if ((list)&&(0 != idList.size())) {
    msg << MSG::INFO << "Printing identified histograms/profiles " << endreq;
    for (std::vector<std::string>::iterator it = idList.begin(); it != idList.end(); it++){
      msg << MSG::INFO << "    " << (*it) << endreq;
    }
  }
}

void DimRpcGaucho::histogramIdentifier(IRegistry* object, std::vector<std::string> &idList, bool reset, bool save, int &level){
  MsgStream msg(m_msgSvc, "DimRpcGaucho");  
  std::vector<IRegistry*> leaves;
  std::vector<IRegistry*>::const_iterator  it;
  
  msg << MSG::INFO << "Looking for histos in object " << object->identifier() << ", level  " << level << endreq;
  
  SmartIF<IDataManagerSvc> dataManagerSvc(m_histogramSvc);
  if (!dataManagerSvc) {
    msg << MSG::WARNING << "    Unable to go to the transient store. " << endreq;
    return;
  }
  
  StatusCode sc = dataManagerSvc->objectLeaves(object, leaves);
  if (sc.isFailure()) msg << MSG::WARNING << "    No histograms found on the transient store." << endreq;
      
  for ( it=leaves.begin(); it != leaves.end(); it++ ) {
    const std::string& id = (*it)->identifier();
    //msg << MSG::DEBUG << "    Object found: " << id << endreq;
    
    DataObject* dataObject;
    sc = m_histogramSvc->retrieveObject(id, dataObject);
    if (sc.isFailure()) {
      msg << MSG::INFO << "Could not retrieve object from TES " << endreq;
      continue;
    }

    IHistogram* histogram = dynamic_cast<AIDA::IHistogram*> (dataObject);
    //TH1* hRoot = dynamic_cast<TH1*> (dataObject);
    if ( 0 != histogram) {
//    if ( 0 != hRoot) {
      //msg << MSG::DEBUG << ", title = " << histogram->title() << endreq;
      if (save) {
        TH1* hRoot = (TH1*) Gaudi::Utils::Aida2ROOT::aida2root(histogram);
        hRoot->Write();
      }
      if (reset) histogram->reset();
//      if (reset) hRoot->Reset();
      idList.push_back(id);
      continue;
    }
    IProfile1D* profile = dynamic_cast<AIDA::IProfile1D*> (dataObject);
    //TProfile* hProf = dynamic_cast<TProfile*> (dataObject);
    if (0 != profile) {
//    if (0 != hProf) {
      //msg << MSG::DEBUG << ", title = " << profile->title() << endreq;
      if (save)  {
        TProfile* hRoot = (TProfile*) Gaudi::Utils::Aida2ROOT::aida2root(profile);
        hRoot->Write();
      }
      if (reset) profile->reset();
//      if (reset) hProf->Reset();
      idList.push_back(id);
      continue;
    }
    
    int newLevel = level + 1;
    if (newLevel >= 10) continue;
    histogramIdentifier(*it, idList, reset, save, newLevel);
  }
} 

IRegistry* DimRpcGaucho::rootObject(){
  MsgStream msg(m_msgSvc, "DimRpcGaucho");  
  std::string  path;
  SmartDataPtr<DataObject> smartDataPtr(m_histogramSvc, path);
  msg << MSG::INFO << "root identifier : " << smartDataPtr->registry()->identifier() << endreq;
  return smartDataPtr->registry();
}
