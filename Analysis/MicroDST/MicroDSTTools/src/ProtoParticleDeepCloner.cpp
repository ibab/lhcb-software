// $Id: ProtoParticleDeepCloner.cpp,v 1.1 2010-08-20 13:56:03 jpalac Exp $
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
#include <MicroDST/ICloneCaloHypo.h>
#include <MicroDST/ICloneMuonPID.h>

// local
#include "ProtoParticleDeepCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProtoParticleDeepCloner
//
// 2008-04-01 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProtoParticleDeepCloner::ProtoParticleDeepCloner( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  :
  base_class           ( type, name , parent ),
  m_trackCloner        ( NULL ),
  m_caloHypoCloner     ( NULL ),
  m_trackClonerName    ( "TrackCloner"    ),
  m_caloHypoClonerName ( "CaloHypoCloner" )
{
  declareProperty("ICloneTrack", m_trackClonerName);
  declareProperty("ICloneCaloHypo", m_caloHypoClonerName);
}

//=============================================================================

StatusCode ProtoParticleDeepCloner::initialize()
{
  const StatusCode sc = base_class::initialize();
  if ( sc.isFailure() ) return sc;

  m_trackCloner    = tool<ICloneTrack>    ( m_trackClonerName,    this->parent() );
  m_caloHypoCloner = tool<ICloneCaloHypo> ( m_caloHypoClonerName, this->parent() );

  return sc;
}

//=============================================================================

LHCb::ProtoParticle* ProtoParticleDeepCloner::operator() (const LHCb::ProtoParticle* protoParticle)
{
  return this->clone(protoParticle);
}

//=============================================================================

LHCb::ProtoParticle* ProtoParticleDeepCloner::clone(const LHCb::ProtoParticle* protoParticle)
{
  if ( !protoParticle ) return NULL;

  LHCb::ProtoParticle* protoParticleClone =
    cloneKeyedContainerItem<BasicProtoParticleCloner>(protoParticle);

  if ( !protoParticleClone ) return NULL;

  if ( m_trackCloner )
  {
    protoParticleClone->setTrack( (*m_trackCloner)(protoParticle->track()) );
  }

  // Rich PID
  LHCb::RichPID* clonedRichPID =
    cloneKeyedContainerItem<RichPIDCloner>(protoParticle->richPID());
  if ( clonedRichPID )
  {
    // set the main track
    clonedRichPID->setTrack( protoParticleClone->track() );
  }
  protoParticleClone->setRichPID(clonedRichPID);

  // MUON PID
  LHCb::MuonPID* clonedMuonPID =
    cloneKeyedContainerItem<MuonPIDCloner>(protoParticle->muonPID());
  if ( clonedMuonPID )
  {
    // Set the main track
    clonedMuonPID->setIDTrack( protoParticleClone->track() );

    // Clone and set the Muon Track
    clonedMuonPID->setMuonTrack( (*m_trackCloner)(protoParticle->muonPID()->muonTrack()) );
  }
  protoParticleClone->setMuonPID(clonedMuonPID);

  // CALO
  protoParticleClone->clearCalo();
  const SmartRefVector<LHCb::CaloHypo> & caloHypos = protoParticle->calo();
  if ( !caloHypos.empty() )
  {
    for ( SmartRefVector<LHCb::CaloHypo>::const_iterator iCalo = caloHypos.begin();
          iCalo != caloHypos.end(); ++iCalo)
    {
      // Deep cloning
      LHCb::CaloHypo * hypoClone = (*m_caloHypoCloner)(*iCalo);
      // save
      if ( hypoClone ) protoParticleClone->addToCalo(hypoClone);
    }
  }

  return protoParticleClone;
}

//=============================================================================
// Destructor
//=============================================================================
ProtoParticleDeepCloner::~ProtoParticleDeepCloner() {}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ProtoParticleDeepCloner )
