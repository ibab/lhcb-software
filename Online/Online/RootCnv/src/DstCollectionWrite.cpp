// $Id: DstCollectionWrite.cpp,v 1.1 2010-03-03 14:32:00 frankb Exp $
//	====================================================================
//  DstCollection.Write.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#define EVTCOLLECTION_WRITE_CPP

// Framework include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// Example related include files
#include "DstCollectionWrite.h"
#include "Event/GenCollision.h"

#include <cmath>

DECLARE_ALGORITHM_FACTORY(DstCollectionWrite);

/**  Algorithm parameters which can be set at run time must be declared.
     This should be done in the constructor.
*/
DstCollectionWrite::DstCollectionWrite(const std::string& name, ISvcLocator* pSvcLocator)
: Algorithm(name, pSvcLocator),  m_evtTupleSvc(0)
{
}

// Standard destructor
DstCollectionWrite::~DstCollectionWrite()   {
}

StatusCode DstCollectionWrite::initialize()   {
  StatusCode status = service("EvtTupleSvc", m_evtTupleSvc);
  if ( status.isSuccess() )   {
    // Third: An event collection N tuple
    NTuplePtr nt(m_evtTupleSvc,"/NTUPLES/EvtColl/Dir1/Collection");
    if ( !nt )    {    // Check if already booked      
      nt = m_evtTupleSvc->book ("/NTUPLES/EvtColl/Dir1/Collection", CLID_ColumnWiseTuple, "Hello World");
      if ( nt )    {
        // Add an index column
        status = nt->addItem ("Address",m_evtAddrColl);
        status = nt->addItem ("Collisions", m_numColl);
        status = nt->addItem ("PtHat",      m_ptHat);
      }
      else    {   // did not manage to book the N tuple....
        return StatusCode::FAILURE;
      }
    }
    else  {  // Just reconnect to existing items
      MsgStream log(msgSvc(), name());
      log << MSG::INFO;
      status = nt->item ("Collisions",  m_numColl);
      log << "N-Tuple collumn attached 'Collisions':" << status.getCode() << endmsg;
      status = nt->item ("", m_ptHat );
      log << "N-Tuple collumn attached 'PtHat':" << status.getCode() << endmsg;
      status = nt->item ("Address",m_evtAddrColl );
      log << "N-Tuple collumn attached 'Address':" << status.getCode() << endmsg;
    }
  }
  return status;
}

// Event callback
StatusCode DstCollectionWrite::execute() {
  MsgStream log(msgSvc(), name());
  SmartDataPtr<DataObject> evtRoot(eventSvc(),"/Event");
  SmartDataPtr<LHCb::GenCollisions> evt(eventSvc(),"/Event/Gen/Collisions");
  m_ptHat = 0.;
  m_numColl = 0;
  if ( evt != 0 )    {
    double pt=0.;
    for(LHCb::GenCollisions::const_iterator i=evt->begin(); i!=evt->end();++i) {
      pt += (*i)->ptHat();
    }
    m_ptHat = pt;
    m_numColl = evt->size();
    m_evtAddrColl = evtRoot->registry()->address();
    return m_evtTupleSvc->writeRecord("/NTUPLES/EvtColl/Dir1/Collection");
  }
  m_evtAddrColl = evtRoot->registry()->address();
  return StatusCode::SUCCESS;
}
