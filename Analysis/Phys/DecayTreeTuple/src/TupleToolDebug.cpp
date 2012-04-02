// $Id: TupleToolDebug.cpp,v 1.1 2010-06-02 12:01:44 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolDebug.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include <Event/RecVertex.h>

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolDebug
//
// 2010-06-02 : P. Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolDebug )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TupleToolDebug::TupleToolDebug( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
    : TupleToolBase ( type, name , parent )
                                //, m_dva(0)
{
  declareInterface<IEventTupleTool>(this);
}

//=============================================================================
//=============================================================================

StatusCode TupleToolDebug::fill( Tuples::Tuple& tuple )
{
  const std::string prefix=fullName();
  const unsigned int maxPV = 100 ;
  bool test=true;

  std::vector<double> deb ;
  deb.push_back(1.);
  deb.push_back(2.);
  deb.push_back(3.);
  test &= tuple->farray( prefix+"DEBUG", deb, prefix+"DEBUG3",  maxPV );

  return StatusCode(test);
  //  return StatusCode::SUCCESS;
}
