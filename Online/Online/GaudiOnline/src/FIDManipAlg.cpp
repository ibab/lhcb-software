#include "MDF/MDFIO.h"
#include "Event/RawEvent.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataIncident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiOnline/FIDManipAlg.h"
#include "GaudiOnline/FIDManipulator.h"

#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IRegistry.h" 

using namespace LHCb;
using namespace std;

/// Standard constructor
FIDManipAlg::FIDManipAlg(const string& nam, ISvcLocator* pSvc) 
  : Algorithm(nam,pSvc), m_printCnt(0), m_incidentSvc(0), m_fid(-1,"")
{
  declareProperty("Action",        m_action=DUMP);
  declareProperty("BankLocation",  m_location=RawEventLocation::Default);    
  declareProperty("OutputLocation",m_outputLocation=RawEventLocation::Default);    
  declareProperty("DataType",      m_type=MDFIO::MDF_RECORDS);
  declareProperty("PrintFreq",     m_printFreq=100);
  declareProperty("Debug",         m_debug=0);
  declareProperty("Incident",      m_incidentName="NEW_INPUT_FID");
}

/// IInterface implementation : queryInterface
StatusCode FIDManipAlg::queryInterface(const InterfaceID& riid, void** ppIf)   {
  if ( IIncidentListener::interfaceID().versionMatch(riid) ) {
    *ppIf = (IIncidentListener*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Algorithm::queryInterface(riid, ppIf);
}

/// Algorithm initialization
StatusCode FIDManipAlg::initialize() {
  m_printCnt = 0;
  StatusCode sc = service("IncidentSvc",m_incidentSvc);
  if ( !sc.isSuccess() ) {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Failed to access the IncidentSvc." << endreq;
    return sc;
  }
  m_incidentSvc->addListener(this,m_incidentName);
  return StatusCode::SUCCESS;
}

/// Algorithm finalization
StatusCode FIDManipAlg::finalize() {
  if ( m_incidentSvc ) {
    m_incidentSvc->removeListener(this);
    m_incidentSvc = 0;
  }
  return StatusCode::SUCCESS;
}

/// Incident handler implemenentation: Inform that a new incident has occured
void FIDManipAlg::handle(const Incident& inc)    {
  if ( inc.type() == "NEW_INPUT_FID" )  {
    ContextIncident<int>* i=(ContextIncident<int>*)&inc;
    m_fid.first = i->tag();
    m_fid.second = i->source();
  }
}

/// Algorithm execution
StatusCode FIDManipAlg::execute() {
  FIDManipulator manip(m_location,m_type,msgSvc(),eventSvc());
  if ( m_debug > 0 ) manip.setDebug(m_debug!=0);

  switch ( m_action )  {
    case UPDATE_DST: {
      std::pair<LHCb::RawBank*,void*> p = manip.getBank();
      if ( p.first )   {
	FIDManipulator o_manip(m_outputLocation,MDFIO::MDF_NONE,msgSvc(),eventSvc());
	FileIdInfo* info = p.first->begin<FileIdInfo>();
	return o_manip.updateDstAddress(info);
      }
      return StatusCode::FAILURE;
    }
    case COPY: {
      std::pair<LHCb::RawBank*,void*> p = manip.getBank();
      if ( p.first ) {
	FIDManipulator o_manip(m_outputLocation,MDFIO::MDF_NONE,msgSvc(),eventSvc());
	FileIdInfo* info = p.first->begin<FileIdInfo>();
	o_manip.updateDstAddress(info);
	return o_manip.add(info);
      }
      return StatusCode::FAILURE;
    }
    case ADD2:      {
      SmartDataPtr<DataObject> ptr(eventSvc(),m_location);
      if ( ptr ) {
	IRegistry*  reg = ptr->registry();
	if ( reg )  {
	  IOpaqueAddress* padd = reg->address();
	  if ( padd ) {
	    m_fid.first  = 1234;
	    m_fid.second = padd->par()[0];
	    return manip.add(m_fid.first,m_fid.second);
	  }
	}
      }
      return StatusCode::FAILURE;
    }
    case ADD:      {
      return manip.add(m_fid.first,m_fid.second);
    }
    case REMOVE:  {
      return manip.remove();
    }
    case DUMP:
      if ( ++m_printCnt == m_printFreq ) {
	m_printCnt = 0;
	return manip.print();
      }
      return StatusCode::SUCCESS;
    default:
      return StatusCode::FAILURE;
  }
}


#include "GaudiKernel/AlgFactory.h" 
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(FIDManipAlg)

