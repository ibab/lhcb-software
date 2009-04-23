// $Id: ChargedParticleMakerBase.cpp,v 1.1 2009-04-23 10:39:31 pkoppenb Exp $
// Include files

#include "GaudiKernel/DeclareFactoryEntries.h"

// from EventSys
#include "Event/Particle.h"
#include "Event/Vertex.h"

// local
#include "ChargedParticleMakerBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ChargedParticleMakerBase
//
// 2009-04-21 P. Koppenburg
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedParticleMakerBase::ChargedParticleMakerBase( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : ParticleMakerBase  ( name , pSvcLocator ) 
  , m_p2s()
  ,  m_trSel   ( NULL )
{
  setProperty( "AvoidSelResult", "true").ignore();
  setProperty( "DecayDescriptor", "\"null\"").ignore();
}
//=========================================================================
//  
//=========================================================================
StatusCode ChargedParticleMakerBase::initialize ( ) {
  StatusCode sc = ParticleMakerBase::initialize();
  m_p2s = tool<IParticle2State>("Particle2State"); // not private
  // get an instance of the track selector
  m_trSel = tool<ITrackSelector>( "TrackSelector", "TrackSelector", this );
  return sc;
}

//=============================================================================
// Destructor
//=============================================================================
ChargedParticleMakerBase::~ChargedParticleMakerBase() {};

