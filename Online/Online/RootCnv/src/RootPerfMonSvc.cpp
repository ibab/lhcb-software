// $Id: RootPerfMonSvc.cpp,v 1.12 2010-09-27 15:43:53 frankb Exp $
//====================================================================
//  RootPerfMonSvc implementation
//--------------------------------------------------------------------
//
//  Description: Implementation of the ROOT data storage
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/DataIncident.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/System.h"
#include "RootCnv/RootPerfMonSvc.h"
#include "RootUtils.h"

#include "TDirectory.h"
#include "TSystem.h"
#include "TBranch.h"
#include "TMap.h"

using namespace std;
using namespace Gaudi;
typedef const string& CSTR;

#define S_OK   StatusCode::SUCCESS
#define S_FAIL StatusCode::FAILURE

// Standard constructor
RootPerfMonSvc::RootPerfMonSvc(CSTR nam, ISvcLocator* svc)
  : Service( nam, svc), m_incidentSvc(0)
{
  declareProperty("IOPerfStats", m_ioPerfStats);
  declareProperty("Streams",     m_setStreams);
  declareProperty("BasketSize",  m_basketSize);
  declareProperty("BufferSize",  m_bufferSize);
  declareProperty("SplitLevel",  m_splitLevel);
}

// Standard destructor
RootPerfMonSvc::~RootPerfMonSvc() {
}

// Small routine to issue exceptions
StatusCode RootPerfMonSvc::error(CSTR msg)  {
  if ( m_log ) {
    log() << MSG::ERROR << "Error: " << msg << endmsg;
    return S_FAIL;
  }
  MsgStream m(msgSvc(),name());
  m << MSG::ERROR << "Error: " << msg << endmsg;
  return S_FAIL;
}

// Initialize the Db data persistency service
StatusCode RootPerfMonSvc::initialize()  {
  string cname;
  StatusCode status = Service::initialize();
  if ( !status.isSuccess() )
    return error("Failed to initialize Service base class.");
  m_log = new MsgStream(msgSvc(),name());
  if( !(status=service("IncidentSvc", m_incidentSvc)).isSuccess() )
    return error("Unable to localize interface from service:IncidentSvc");

  m_incidentSvc->addListener(this, IncidentType::BeginEvent, 1, false, false);
  m_incidentSvc->addListener(this, "NEW_STREAM", 1, false, false);
  m_incidentSvc->addListener(this, "CONNECTED_OUTPUT", 1, false, false);

  if (m_ioPerfStats.empty())
    return error("Performance monitoring file IOPerfStats was not defined.");

  TDirectory::TContext ctxt(0);
  if (!(m_perfFile = new TFile(m_ioPerfStats.c_str(),"RECREATE")))
    return error("Could not create ROOT file.");

  if (!(m_perfTree = new TTree("T", "performance measurement")))
    return error("Could not create tree.");

  m_perfTree->Branch("utime", &m_utime, "utime/l");
  m_perfTree->Branch("stime", &m_stime, "stime/l");
  m_perfTree->Branch("vsize", &m_vsize, "vsize/l");
  m_perfTree->Branch("rss", &m_rss, "rss/L");
  m_perfTree->Branch("time", &m_time, "time/L");
  m_eventNumber = 0;
  m_perfTree->Branch("event_number", &m_eventNumber, "event_number/L");
  m_perfTree->Branch("event_type", &m_eventType, "event_type/I");

  if (m_setStreams.empty())
	  m_setStreams = "undefined";
  if (m_basketSize.empty())
	  m_basketSize = "undefined";
  if (m_bufferSize.empty())
	  m_bufferSize = "undefined";
  if (m_splitLevel.empty())
	  m_splitLevel = "undefined";

  TMap *map = new TMap();
  map->Add(new TObjString("streams"), new TObjString(m_setStreams.c_str()));
  map->Add(new TObjString("basket_size"), new TObjString(m_basketSize.c_str()));
  map->Add(new TObjString("buffer_size"), new TObjString(m_bufferSize.c_str()));
  map->Add(new TObjString("split_level"), new TObjString(m_splitLevel.c_str()));
  map->Write("params", TObject::kSingleKey);
  return S_OK;
}

void RootPerfMonSvc::record(EventType eventType) {
  SysProcStat data;
  m_eventType = eventType;
  m_utime = (ULong_t)data.utime;
  m_stime = (ULong_t)data.stime;
  m_vsize = (ULong_t)data.vsize;
  m_rss   = (Long_t)data.rss;
  m_time  = (Long_t)data.time;
  m_perfTree->Fill();
}

void RootPerfMonSvc::handle(const Incident& incident) {
  std::string t = incident.type();
  if ( !t.compare(IncidentType::BeginEvent) ) {
    m_eventNumber++;
    record(EVENT);
    return;
  }
  if ( !t.compare("CONNECTED_OUTPUT") ) {
    m_outputs.insert(incident.source());
  }
}

// Stop the performance monitoring service
StatusCode RootPerfMonSvc::stop() {
  char text[64];
  record(FSR);
  TMap *map = new TMap();
  for(set<string>::const_iterator i=m_outputs.begin(); i!=m_outputs.end();++i) {
    const char* fn = (*i).c_str();
    Long_t id, siz, flags, tim;
    if ( 0 == gSystem->GetPathInfo(fn,&id,&siz,&flags,&tim) ) {
      ::sprintf(text,"%ld",siz);
      map->Add(new TObjString(fn), new TObjString(text));
    }
  }
  TDirectory::TContext ctxt(m_perfFile);
  map->Write("Outputs", TObject::kSingleKey);
  return S_OK;
}

// Finalize the performance monitoring service
StatusCode RootPerfMonSvc::finalize()    {
  record(FSR);
  log() << MSG::INFO;
  deletePtr(m_log);
  releasePtr(m_incidentSvc);

  m_perfFile->Write();
  m_perfFile->Close();
  deletePtr(m_perfFile);

  return Service::finalize();
}
