// $Id: DecayTreeTuple.cpp,v 1.14 2009-01-22 09:27:33 pkoppenb Exp $
// Include files 


#include "boost/lexical_cast.hpp" 
#include "Kernel/Escape.h"
// local
#include "DecayTreeTuple.h"

using namespace LHCb;
using namespace Gaudi ;
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
  declareProperty( "TupleName", m_tupleName="DecayTree" );
  // fill some default value
  m_toolList.push_back( "TupleToolKinematic" );
  m_toolList.push_back( "TupleToolPid" );
  m_toolList.push_back( "TupleToolGeometry" );
  m_toolList.push_back( "TupleToolEventInfo" );
  
  declareProperty( "ToolList", m_toolList );

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
  if (!sc) err() << "Error from base class" << endmsg ;
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  sc =  initializeDecays(false);
  if (!sc) err() << "Error from initializeDecays(false)" << endmsg ;
  return sc ;
  
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
StatusCode DecayTreeTuple::fillTuple( Tuples::Tuple& tuple
                                      , const Particle::ConstVector& heads ){
  Particle::ConstVector::const_iterator pit = heads.begin();
  Particle::ConstVector row;

  int nCandidates=0;
  StatusCode test = StatusCode::FAILURE;

  for( ; heads.end()!=pit; ++pit ){
    test = StatusCode::FAILURE;
    row.clear();
    m_dkFinder->decayMembers( *pit, row );
    row.insert( row.begin(), *pit ); // must insert the head as it cant be flagged.

    if( fillParticles( tuple, row ) ){
      tuple->column( "nCandidate", nCandidates ).ignore();
      ++nCandidates;
    } else {
      Warning("Failed to fill a candidate. Will skip it.").ignore();
      continue;
    }

    test = fillEventRelatedVariables( tuple );

    if( test ){
      test = tuple->write();
    } else {
      Warning("Failed to fill some variable, will skip this candidate.");
    }
  }


  return test;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode DecayTreeTuple::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DecayTreeTupleBase::finalize();
}
