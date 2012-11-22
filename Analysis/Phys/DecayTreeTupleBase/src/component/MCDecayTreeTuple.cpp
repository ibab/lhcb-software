// $Id: MCDecayTreeTuple.cpp,v 1.6 2010-04-20 06:40:48 rlambert Exp $
// Include files

#include "boost/lexical_cast.hpp"
#include "Kernel/Escape.h"
// local
#include "MCDecayTreeTuple.h"

using namespace LHCb;
using namespace Gaudi ;
//-----------------------------------------------------------------------------
// Implementation file for class : MCDecayTreeTuple
//
// 2009-01-20 : Patrick Koppenburg based on JÅÈrÅÈmie Borel's initial vesion
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCDecayTreeTuple )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  MCDecayTreeTuple::MCDecayTreeTuple( const std::string& name,
                                      ISvcLocator* pSvcLocator)
    : DecayTreeTupleBase ( name , pSvcLocator )
{
  // fill some default value
  m_toolList.push_back( "MCTupleToolKinematic" );
  m_toolList.push_back( "TupleToolEventInfo" );
  declareProperty( "ToolList", m_toolList );
  setProperty( "TupleName", "MCDecayTree" );
}

//=============================================================================
// Destructor
//=============================================================================
MCDecayTreeTuple::~MCDecayTreeTuple() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode MCDecayTreeTuple::initialize() 
{
  const StatusCode sc = DecayTreeTupleBase::initialize();
  if ( sc.isFailure() ) return sc;
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return initializeDecays(true) ;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode MCDecayTreeTuple::execute()
{
  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  ++counter("Event");

  LHCb::MCParticle::ConstVector mothers ;
  const LHCb::MCParticle* head = 0 ;
  while ( mcdkFinder()->findDecay( head) )
  {
    mothers.push_back(head);
  }
  if ( mothers.empty() )
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "No mothers of decay " << headDecay() 
                                          << " found" << endreq;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "I have " << mothers.size()
                                        << " particles to handle" << endreq;

  LHCb::MCParticle::ConstVector heads;
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
  //don't create the ntuple if there's nothing to fill!
  Tuple tuple = nTuple( tupleName(), tupleName() );
  test = fillTuple( tuple, heads, mcdkFinder() );

  if( test ){
    if (msgLevel(MSG::VERBOSE)) verbose() << "NTuple sucessfully filled" << endreq;
  }

  setFilterPassed(test);
  // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;

}
