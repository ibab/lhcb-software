// $Id: DecayTreeTuple.cpp,v 1.17 2010-04-20 06:40:48 rlambert Exp $
// Include files

#include "boost/lexical_cast.hpp"
#include "Kernel/Escape.h"

#include <algorithm>
#include <functional>

// local
#include "DecayTreeTuple.h"

using namespace LHCb;
using namespace Gaudi ;
//-----------------------------------------------------------------------------
// Implementation file for class : DecayTreeTuple
//
// 2009-01-20 : Patrick Koppenburg based on Jérémie Borel's initial vesion
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DecayTreeTuple::DecayTreeTuple( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : DecayTreeTupleBase ( name, pSvcLocator )
{
  // fill some default value
  m_toolList.push_back( "TupleToolKinematic" );
  m_toolList.push_back( "TupleToolPid" );
  m_toolList.push_back( "TupleToolANNPID" );
  m_toolList.push_back( "TupleToolGeometry" );
  m_toolList.push_back( "TupleToolEventInfo" );
  declareProperty( "ToolList", m_toolList );
  setProperty( "TupleName", "DecayTree" );
}

//=============================================================================
// Destructor
//=============================================================================
DecayTreeTuple::~DecayTreeTuple() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DecayTreeTuple::initialize()
{
  StatusCode sc = DecayTreeTupleBase::initialize();
  if ( sc.isFailure() )
  {
    return Error( "Error from base class", sc );
  }
  sc = initializeDecays(false);
  if ( sc.isFailure() )
  {
    return Error( "Error from initializeDecays(false)" );
  }
  return sc;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode DecayTreeTuple::execute()
{
  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  ++counter("Event");

  LHCb::Particle::ConstVector heads;
  bool found = false;
  if ( useLoKiDecayFinders() ) {
    std::copy_if( i_particles().begin(), i_particles().end(), std::back_inserter(heads), std::cref(decayTree()) );
    found = !heads.empty();
  } else {
    const LHCb::Particle::ConstVector mothers( this->particles().begin(),
                                               this->particles().end()   );
    if ( mothers.empty() )
    {
      setFilterPassed(false);
      return StatusCode::SUCCESS;
    }
    if (msgLevel(MSG::VERBOSE)) verbose() << "I have " << mothers.size()
                                          << " particles to handle" << endmsg;
    StatusCode test = getDecayMatches( mothers, heads );
    found = test.isSuccess();
  }
  if ( found )
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "There is " << heads.size()
                                          << " top particles matching the decay." << endmsg;
  }
  else
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "No particle matching the decay." << endmsg;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  //don't create the ntuple if there's nothing to fill!
  Tuple tuple = nTuple( tupleName(), tupleName() );
  StatusCode test;
  if ( useLoKiDecayFinders() ) {
    test = fillTuple( tuple, heads, decayTree() );
  } else {
    test = fillTuple( tuple, heads, dkFinder() );
  }

  if ( test.isSuccess() )
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "NTuple sucessfully filled" << endmsg;
  }

  setFilterPassed( test.isSuccess() );

  // Mandatory. Set to true if event is accepted.
  return test ;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DecayTreeTuple )
