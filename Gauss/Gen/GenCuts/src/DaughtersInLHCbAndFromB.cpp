// $Id: DaughtersInLHCbAndFromB.cpp,v 1.1 2007-11-26 13:44:34 jonrob Exp $
// Include files

// local
#include "DaughtersInLHCbAndFromB.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// Kernel
#include "Kernel/ParticleID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaughtersInLHCbAndFromB
//
// 22/11/2007 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( DaughtersInLHCbAndFromB )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaughtersInLHCbAndFromB::DaughtersInLHCbAndFromB( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : DaughtersInLHCb ( type, name , parent ),
    m_fromBcuts     ( NULL                ) { }

//=============================================================================
// Destructor
//=============================================================================
DaughtersInLHCbAndFromB::~DaughtersInLHCbAndFromB( ) { }

//=============================================================================
// initialize
//=============================================================================
StatusCode DaughtersInLHCbAndFromB::initialize()
{
  const StatusCode sc = DaughtersInLHCb::initialize();
  if ( sc.isFailure() ) return sc;
  
  // load the tool to check if the signal is ultimately from a b
  m_fromBcuts = tool<IGenCutTool>( "SignalIsFromBDecay", this );

  return sc;
}

//=============================================================================
// Acceptance function
//=============================================================================
bool DaughtersInLHCbAndFromB::applyCut( ParticleVector & theParticleVector ,
                                        const HepMC::GenEvent * theEvent ,
                                        const LHCb::GenCollision * theHardInfo )
  const
{
  return ( m_fromBcuts->applyCut(theParticleVector,theEvent,theHardInfo) && 
           DaughtersInLHCb::applyCut(theParticleVector,theEvent,theHardInfo) );
}
