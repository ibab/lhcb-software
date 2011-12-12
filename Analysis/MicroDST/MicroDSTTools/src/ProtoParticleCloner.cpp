// $Id: ProtoParticleCloner.cpp,v 1.11 2010-08-20 13:57:39 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from LHCb
#include "Event/ProtoParticle.h"
#include "Event/Track.h"
#include "Event/MuonPID.h"
#include "Event/RichPID.h"

// from MicroDST
#include <MicroDST/ICloneTrack.h>

// local
#include "ProtoParticleCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProtoParticleCloner
//
// 2008-04-01 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProtoParticleCloner::ProtoParticleCloner( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  :
  base_class        ( type, name , parent ),
  m_trackCloner     ( NULL                ),
  m_trackClonerName ( "TrackCloner"       )
{
  declareProperty("ICloneTrack", m_trackClonerName);
}

//=============================================================================

StatusCode ProtoParticleCloner::initialize()
{
  const StatusCode sc = base_class::initialize();
  if ( sc.isFailure() ) return sc;

  m_trackCloner = tool<ICloneTrack>( m_trackClonerName, this->parent() );

  return sc;
}

//=============================================================================

LHCb::ProtoParticle* ProtoParticleCloner::operator() (const LHCb::ProtoParticle* protoParticle)
{
  return this->clone(protoParticle);
}

//=============================================================================

LHCb::ProtoParticle* ProtoParticleCloner::clone(const LHCb::ProtoParticle* protoParticle)
{
  if ( !protoParticle ) return NULL;

  LHCb::ProtoParticle* protoParticleClone =
    cloneKeyedContainerItem<BasicProtoParticleCloner>(protoParticle);

  if ( !protoParticleClone ) return NULL;

  if ( m_trackCloner )
  {
    protoParticleClone->setTrack( (*m_trackCloner)(protoParticle->track()) );
  }

  LHCb::RichPID* clonedRichPID =
    cloneKeyedContainerItem<RichPIDCloner>(protoParticle->richPID());
  if ( clonedRichPID )
  {
    // set the main track
    clonedRichPID->setTrack( protoParticleClone->track() );
  }

  // set the RichPID in the cloned ProtoParticle
  protoParticleClone->setRichPID(clonedRichPID);

  LHCb::MuonPID* clonedMuonPID =
    cloneKeyedContainerItem<MuonPIDCloner>(protoParticle->muonPID());
  if ( clonedMuonPID )
  {
    // Set the main track
    clonedMuonPID->setIDTrack( protoParticleClone->track() );

    // Clone and set the Muon Track
    clonedMuonPID->setMuonTrack( (*m_trackCloner)(protoParticle->muonPID()->muonTrack()) );
  }

  // Set the Muon PID in the cloned ProtoParticle
  protoParticleClone->setMuonPID(clonedMuonPID);

  const SmartRefVector<LHCb::CaloHypo> & caloHypos = protoParticle->calo();
  if ( !caloHypos.empty() )
  {
    protoParticleClone->clearCalo();
    for ( SmartRefVector<LHCb::CaloHypo>::const_iterator iCalo = caloHypos.begin();
          iCalo != caloHypos.end(); ++iCalo )
    {
      LHCb::CaloHypo* hypoClone = cloneKeyedContainerItem<CaloHypoCloner>(*iCalo);
      if (hypoClone) protoParticleClone->addToCalo(hypoClone);
    }
  }

  return protoParticleClone;
}

//=============================================================================
// Destructor
//=============================================================================
ProtoParticleCloner::~ProtoParticleCloner() {}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ProtoParticleCloner )
