// $Id: DecayTreeTuple.cpp,v 1.8 2009-01-20 10:00:44 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "DecayTreeTuple.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DecayTreeTuple
//
// 2009-01-20 : Patrick Koppenburg based on Jérémie Borel's initial vesion
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DecayTreeTuple );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DecayTreeTuple::DecayTreeTuple( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : DecayTreeTupleBase ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
DecayTreeTuple::~DecayTreeTuple() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DecayTreeTuple::initialize() {
  StatusCode sc = DecayTreeTupleBase::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Execute
//=============================================================================
StatusCode DecayTreeTuple::execute(){
  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  const LHCb::Particle::ConstVector& mothers = desktop()->particles();
  if( mothers.empty() ){
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "I have " << mothers.size()
                                        << " particles to handle" << endreq;

  Tuple tuple = nTuple( m_tupleName,  m_tupleName );
  LHCb::Particle::ConstVector heads;
  StatusCode test = getDecayMatches( mothers, heads );
  if( test ){
    if (msgLevel(MSG::VERBOSE)) verbose() << "There is " << heads.size()
                                          << " top particles matching the decay." << endreq;
  }
  else {
    if (msgLevel(MSG::VERBOSE)) verbose() << "No particle matching the decay." << endreq;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  test = fillTuple( tuple, heads );

  if( test ){
    if (msgLevel(MSG::VERBOSE)) verbose() << "NTuple sucessfully filled" << endreq;
  }
  
  setFilterPassed(test);
  // Mandatory. Set to true if event is accepted.  
  return StatusCode::SUCCESS;
  
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DecayTreeTuple::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DecayTreeTupleBase::finalize();
}

//=============================================================================
