// Include files 

// from Gaudi
#include "Event/VeloDigit.h"

// local
#include "VeloDigit2MCHitLinker.h"
#include "VeloTruthTool.h"

// Event model
#include "Event/VeloDigit.h"
#include "Event/MCVeloFE.h"
#include "Event/MCHit.h"

// Linker
#include "Linker/LinkerWithKey.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloDigit2MCHitLinker
//
// 2006-02-18 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloDigit2MCHitLinker )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloDigit2MCHitLinker::VeloDigit2MCHitLinker( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_asctName ( LHCb::VeloDigitLocation::Default + "2MCHits" ),
    m_inputDigits ( LHCb::VeloDigitLocation::Default ),
    m_inputFEs ( LHCb::MCVeloFELocation::Default )
{

}
//=============================================================================
// Destructor
//=============================================================================
VeloDigit2MCHitLinker::~VeloDigit2MCHitLinker() {}
//=============================================================================

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloDigit2MCHitLinker::execute() {
  bool isDebug = msgLevel(MSG::DEBUG);
  //
  if(isDebug) debug() << "==> Execute" << endmsg;
  //
  LHCb::VeloDigits* digits;
  if(!exist<LHCb::VeloDigits>(m_inputDigits)){
    error()<< "There is no VeloDigits at " << m_inputDigits <<endmsg;
    return ( StatusCode::FAILURE );
  }else{
    digits=get<LHCb::VeloDigits>(m_inputDigits);
  }
  LHCb::MCVeloFEs* mcFEs=get<LHCb::MCVeloFEs>(m_inputFEs);
  //
  LinkerWithKey<LHCb::MCHit, LHCb::VeloDigit> myLink(evtSvc(),
                                                    msgSvc(), asctName());  
  //
  if(isDebug) debug()<< " ==> relations table has been created " <<endmsg;
  // link the VeloDigits to MCHits
  LHCb::VeloDigits::const_iterator digIt;
  for(digIt=digits->begin(); digIt!=digits->end(); digIt++){
    std::map<LHCb::MCHit*,double> relations;
    StatusCode sc=VeloTruthTool::associateToTruth((*digIt), relations, mcFEs);
    if(sc){
      //
      if(isDebug) debug()<< " ==> after the associator" <<endmsg;
      std::map<LHCb::MCHit*,double>::const_iterator relIt;
      for (relIt=relations.begin(); relIt!=relations.end(); relIt++){
        SmartRef<LHCb::MCHit> hit=(*relIt).first;
        // create relation weight
        double weight=(*relIt).second;
        // create the table
        myLink.link((*digIt), hit, weight);
      }
    }else{
      if(isDebug) debug()<< " ==> Empty table - no associations made! " <<endmsg;
    }
  } // loop clusters
  //
  return StatusCode::SUCCESS;
}
//=============================================================================
