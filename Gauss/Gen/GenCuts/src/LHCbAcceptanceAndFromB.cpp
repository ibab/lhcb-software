// $Id: LHCbAcceptanceAndFromB.cpp,v 1.1 2007-11-26 13:44:34 jonrob Exp $
// Include files

// local
#include "LHCbAcceptanceAndFromB.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// from HepMC
#include "HepMC/GenParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LHCbAcceptanceAndFromB
//
// 22/11/2007 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( LHCbAcceptanceAndFromB )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LHCbAcceptanceAndFromB::LHCbAcceptanceAndFromB( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : LHCbAcceptance ( type, name , parent ),
    m_fromBcuts    ( NULL                ) { }

//=============================================================================
// Destructor
//=============================================================================
LHCbAcceptanceAndFromB::~LHCbAcceptanceAndFromB( ) { }

//=============================================================================
// initialize
//=============================================================================
StatusCode LHCbAcceptanceAndFromB::initialize()
{
  const StatusCode sc = LHCbAcceptance::initialize();
  if ( sc.isFailure() ) return sc;

  // load the tool to check if the signal is ultimately from a b
  m_fromBcuts = tool<IGenCutTool>( "SignalIsFromBDecay", this );

  return sc;
}

//=============================================================================
// Acceptance function
//=============================================================================
bool LHCbAcceptanceAndFromB::applyCut( ParticleVector & theParticleVector ,
                                       const HepMC::GenEvent * theGenEvent ,
                                       const LHCb::GenCollision * col ) const
{
  return ( m_fromBcuts->applyCut(theParticleVector,theGenEvent,col) &&
           LHCbAcceptance::applyCut(theParticleVector,theGenEvent,col) );
}
