// $Id: PreDV.cpp,v 1.5 2004-11-15 07:28:06 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
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
  : GaudiAlgorithm ( name , pSvcLocator )
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

  StatusCode sc = GaudiAlgorithm::initialize() ;
  
  debug() << "==> Initialise" << endreq;

  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PreDV::execute() {

  debug() << "==> Execute" << endreq;
  setFilterPassed(true);

  std::string location = SelResultLocation::Default;

  bool needToRegister = false;
  
  if (m_keepSelHistory){
    if ( exist<SelResult>(location) ){
      if (!m_warned){
        warning() << "SelResult container already there"  << endreq;
        warning() << "Adding SelResult objects to existing ones"
            <<endreq;
        m_warned = true;
      }
    }
    else      {
      if (!m_warned){
        info() << "SelResult container not there yet." 
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
    
    //    StatusCode scRO = eventSvc()->registerObject(location,resultsToSave);
    StatusCode scRO = put(resultsToSave,location);
    
    if (scRO.isFailure()){
      err() << "Cannot register Selection Result summary " 
          << endreq;
      err() << "Location: " << location << endreq;
      return StatusCode::FAILURE;
    }
   
    debug() << "SelResult container registered" << endreq;

  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PreDV::finalize() {

  debug() << "==> Finalize" << endreq;

  return GaudiAlgorithm::finalize() ;
}

//=============================================================================
