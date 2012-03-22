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

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Cloning ProtoParticle " << protoParticle->key() << " in "
              << protoParticle->parent()->registry()->identifier()
              << " " << *protoParticle
              << endmsg;

  LHCb::ProtoParticle* protoParticleClone =
    cloneKeyedContainerItem<BasicProtoParticleCloner>(protoParticle);

  if ( protoParticleClone )
  {

    LHCb::Track * clonedTrack = NULL;
    if ( m_trackCloner )
    {
      clonedTrack = (*m_trackCloner)(protoParticle->track());
    }
    protoParticleClone->setTrack( clonedTrack );

    // Rich PID
    LHCb::RichPID* clonedRichPID =
      cloneKeyedContainerItem<RichPIDCloner>(protoParticle->richPID());
    if ( clonedRichPID )
    {
      // set the main track
      clonedRichPID->setTrack( clonedTrack );
    }
    protoParticleClone->setRichPID(clonedRichPID);

    // MUON PID
    LHCb::MuonPID* clonedMuonPID =
      cloneKeyedContainerItem<MuonPIDCloner>(protoParticle->muonPID());
    if ( clonedMuonPID )
    {
      // Set the main track
      clonedMuonPID->setIDTrack( clonedTrack );
      // Clone and set the Muon Track
      LHCb::Track * clonedMuonTrack = NULL;
      if ( m_trackCloner )
      {
        clonedMuonTrack = (*m_trackCloner)(protoParticle->muonPID()->muonTrack());
      }
      clonedMuonPID->setMuonTrack( clonedMuonTrack );
    }
    protoParticleClone->setMuonPID(clonedMuonPID);

    // CALO
    protoParticleClone->clearCalo();
    const SmartRefVector<LHCb::CaloHypo> & caloHypos = protoParticle->calo();
    if ( !caloHypos.empty() )
    {
      for ( SmartRefVector<LHCb::CaloHypo>::const_iterator iCalo = caloHypos.begin();
            iCalo != caloHypos.end(); ++iCalo )
      {
        // Basic Cloner
        LHCb::CaloHypo * hypoClone = cloneKeyedContainerItem<CaloHypoCloner>(*iCalo);
        if ( hypoClone )
        {
          // For basic Cloner, set hypo, cluster and digit smartref vectors to empty
          // as the basic cloning keeps them pointing to the originals
          hypoClone->clearHypos();
          hypoClone->clearDigits();
          hypoClone->clearClusters();
          // save
          protoParticleClone->addToCalo(hypoClone);
        }
      }
    }

  }

  if ( msgLevel(MSG::VERBOSE) )
  {
    if ( protoParticleClone )
    {
      verbose() << "Cloned ProtoParticle " << protoParticleClone->key() << " in "
                << protoParticleClone->parent()->registry()->identifier()
                << " " << *protoParticleClone
                << endmsg;
    }
    else
    {
      verbose() << "FAILED to clone ProtoParticle" << endmsg;
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
