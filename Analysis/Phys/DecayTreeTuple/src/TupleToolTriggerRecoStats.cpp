// $Id: TupleToolTriggerRecoStats.cpp,v 1.1 2009-07-30 14:54:47 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TupleToolTriggerRecoStats.h"
#include "Event/Particle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolTriggerRecoStats
//
// 2009-07-30 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolTriggerRecoStats );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolTriggerRecoStats::TupleToolTriggerRecoStats( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
  : GaudiTupleTool ( type, name , parent )
{
  declareInterface<IEventTupleTool>(this);
  m_locations.push_back("Hlt2NoCutsPions");
  m_locations.push_back("Hlt2GoodPions");
  m_locations.push_back("Hlt2Muons");
  m_locations.push_back("Hlt2Electrons");
  m_locations.push_back("Hlt2Photons");
  m_locations.push_back("Hlt2RichPIDsKaons");
  declareProperty("InputLocations",m_locations,"Locations to look at");
}
//=============================================================================
// Destructor
//=============================================================================
TupleToolTriggerRecoStats::~TupleToolTriggerRecoStats() {} 

//=============================================================================
//=============================================================================
// Fill
//=============================================================================
StatusCode TupleToolTriggerRecoStats::fill( Tuples::Tuple& tup) {
  bool test = true;
  for ( std::vector<std::string>::const_iterator l = m_locations.begin() ; l != m_locations.end() ; ++l){
    test &= tup->column("NumberOf"+*l,number<LHCb::Particles>("/Event/HLT/"+*l+"/Particles"));
  }
  return StatusCode(test) ;
} 
