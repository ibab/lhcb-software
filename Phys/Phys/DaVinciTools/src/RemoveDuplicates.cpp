// $Id: RemoveDuplicates.cpp,v 1.1 2012-10-11 11:56:34 elsasser Exp $

#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from DaVinci
#include "Kernel/ICheckOverlap.h"
#include "Kernel/HashIDs.h"

// local
#include "RemoveDuplicates.h"


//-----------------------------------------------------------------------------
/// Implementation file for class : RemoveClones
/**
 * 2011-10-11 : Christian Elsasser
 */
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RemoveDuplicates )

//=============================================================================
/// Standard constructor, initializes variables
//=============================================================================
RemoveDuplicates::RemoveDuplicates( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : 
  DaVinciAlgorithm ( name , pSvcLocator ) 
  , m_NpartIn(0)
  , m_NpartOut(0)
{ 


}
//=============================================================================
/// Destructor
//=============================================================================
RemoveDuplicates::~RemoveDuplicates() {}

//#############################################################################
/// Initialization
//#############################################################################
StatusCode RemoveDuplicates::initialize() {

  StatusCode sc = DaVinciAlgorithm::initialize();
  if (!sc) return sc;

  if (msgLevel(MSG::DEBUG)) {
    debug() << "==> Initialize" << endmsg;
  }

  return sc;

}
//#############################################################################
/// Execution
//#############################################################################
StatusCode RemoveDuplicates::execute() {

  StatusCode sc=StatusCode::SUCCESS;
  
  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  counter("Event")++;
  const LHCb::Particle::ConstVector mothers( this->particles().begin(),
                                            this->particles().end()   );
  if ( mothers.empty() )
  {
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "I have " << mothers.size()
    << " particles to handle" << endreq;

  if(!sc) return sc;
  int NpartIn=0;
  int NpartOut=0;
  
  for(LHCb::Particle::ConstVector::const_iterator i=mothers.begin(); i<mothers.end();i++ )
  {
    NpartIn++;
    bool toBeRemoved = false;
    for (LHCb::Particle::ConstVector::const_iterator j=i+1; j<mothers.end();j++ ) {
      size_t hash1 = LHCb::HashIDs::hashID(*i);
      size_t hash2 = LHCb::HashIDs::hashID(*j);
      if (msgLevel(MSG::VERBOSE)){
        verbose() << "Particle 1 ID hash: " << hash1 << endmsg;
        verbose() << "Particle 2 ID hash: " << hash2 << endmsg;
      }
      if(hash1==hash2 && i!=j){
        if (msgLevel(MSG::VERBOSE)){
          verbose() << "Particle have the same hash of ID! Remove first one from the list!" << endmsg;
        }
        toBeRemoved = true;
      }else{
        if (msgLevel(MSG::VERBOSE)){
          verbose() << "Particles do not have the same hash of ID! Still checking with others!" << endmsg;
        }
      }
    }
    if(!toBeRemoved){
      this->cloneAndMarkTree(*i);
      NpartOut++;
      if (msgLevel(MSG::VERBOSE)){
        verbose() << "Particle has no later duplicate --> Staying in TES!" << endmsg;
      }
    }
  }

  m_NpartIn+=NpartIn;
  m_NpartOut+=NpartOut;
  

  setFilterPassed(NpartOut>0);  
  return sc;

}
//#############################################################################
/// Finalization
//#############################################################################
StatusCode RemoveDuplicates::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  info() << "Filtered " << m_NpartIn << " to " << m_NpartOut 
         << ", removing " << m_NpartIn-m_NpartOut << " duplicates" << endmsg ;

  return DaVinciAlgorithm::finalize() ;
}