// $Id: PreDV.cpp,v 1.2 2004-03-16 18:49:45 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"

// from DaVinci
#include "Event/SelResult.h"

// local
#include "PreDV.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PreDV
//
// 2004-03-02 : Hugo Ruiz Perez
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PreDV>          s_factory ;
const        IAlgFactory& PreDVFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PreDV::PreDV( const std::string& name,
              ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_warned(false)
{
  declareProperty( "KeepSelHistory", m_keepSelHistory = true);
}
//=============================================================================
// Destructor
//=============================================================================
PreDV::~PreDV() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PreDV::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PreDV::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;
  setFilterPassed(true);

  std::string location = SelResultLocation::Default;

  bool needToRegister = false;
  
  if (m_keepSelHistory){
    SmartDataPtr<SelResults> existingSelRess ( eventSvc(), location);
    if(existingSelRess ) {
      if (!m_warned){
        msg << MSG::WARNING<< "SelResult container already there"  << endreq;
        msg << MSG::WARNING<< "Adding SelResult objects to existing ones"
            <<endreq;
        m_warned = true;
      }
    }
    else      {
      if (!m_warned){
        msg << MSG::INFO << "SelResult container not there yet."
            << "Creating it" << endreq;
        m_warned = true;
      }
      needToRegister = true;
    }
  }
  
  
  // If dont want to keep SelResults history or container not there yet,
  // register container in TES
  if ((!m_keepSelHistory) || (needToRegister)){
 
    SelResults* resultsToSave = new SelResults();
    
    StatusCode scRO = eventSvc()->registerObject(location,resultsToSave);
    
    if (scRO.isFailure()){
      msg << MSG::ERROR << "Cannot register Selection Result summary " 
          << endreq;
      msg << MSG::ERROR << "Location: " << location << endreq;
      return StatusCode::FAILURE;
    }
   
    msg << MSG::DEBUG << "SelResult container registered" << endreq;

  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PreDV::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
