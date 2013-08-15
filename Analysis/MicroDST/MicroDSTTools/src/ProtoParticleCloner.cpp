// $Id: ProtoParticleCloner.cpp,v 1.11 2010-08-20 13:57:39 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "ProtoParticleCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProtoParticleCloner
//
// 2008-04-01 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================

ProtoParticleCloner::ProtoParticleCloner( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : base_class       ( type, name, parent ),
    m_trackCloner    ( NULL               ),
    m_caloHypoCloner ( NULL               )
{
  declareProperty("ICloneTrack",    m_trackClonerName    = "TrackCloner"    );
  declareProperty("ICloneCaloHypo", m_caloHypoClonerName = "CaloHypoCloner" );
  //setProperty( "OutputLevel", 2 );
}

//=============================================================================

StatusCode ProtoParticleCloner::initialize()
{
  const StatusCode sc = base_class::initialize();
  if ( sc.isFailure() ) return sc;

  m_trackCloner = tool<ICloneTrack>( m_trackClonerName, this->parent() );
  if ( !m_caloHypoClonerName.empty() && m_caloHypoClonerName != "NONE" )
  {
    m_caloHypoCloner = tool<ICloneCaloHypo>( m_caloHypoClonerName,
                                             this->parent() );
  }

  return sc;
}

//=============================================================================

LHCb::ProtoParticle*
ProtoParticleCloner::operator() (const LHCb::ProtoParticle* protoParticle)
{
  return this->clone(protoParticle);
}

//=============================================================================

LHCb::ProtoParticle*
ProtoParticleCloner::clone(const LHCb::ProtoParticle* protoParticle)
{
  if ( !protoParticle )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "ProtoParticle pointer is NULL !" << endmsg;
    return NULL;
  }

  if ( !protoParticle->parent() )
  {
    this->Warning( "Cannot clone a ProtoParticle with no parent!" ).ignore();
    return NULL;
  }

  // Is this object in the veto list ?
  if ( isVetoed(protoParticle) )
  {
    return const_cast<LHCb::ProtoParticle*>(protoParticle);
  }

  LHCb::ProtoParticle* protoParticleClone =
    cloneKeyedContainerItem<BasicProtoParticleCloner>(protoParticle);

  if ( protoParticleClone )
  {

    // Track
    LHCb::Track * clonedTrack = NULL;
    if ( m_trackCloner )
    {
      clonedTrack = (*m_trackCloner)(protoParticle->track());
    }
    protoParticleClone->setTrack( clonedTrack );

    // Rich PID
    if ( !isVetoed(protoParticle->richPID()) )
    {
      LHCb::RichPID* clonedRichPID =
        cloneKeyedContainerItem<RichPIDCloner>(protoParticle->richPID());
      if ( clonedRichPID )
      {
        // set the main track
        clonedRichPID->setTrack( clonedTrack );
      }
      protoParticleClone->setRichPID(clonedRichPID);
    }

    // MUON PID
    if ( !isVetoed(protoParticle->muonPID()) )
    {
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
    }

    // CALO
    protoParticleClone->clearCalo();
    const SmartRefVector<LHCb::CaloHypo> & caloHypos = protoParticle->calo();
    if ( !caloHypos.empty() )
    {
      for ( SmartRefVector<LHCb::CaloHypo>::const_iterator iCalo = caloHypos.begin();
            iCalo != caloHypos.end(); ++iCalo )
      {
        const LHCb::CaloHypo * hypo = *iCalo;
        if ( hypo )
        {
          if ( !isVetoed(hypo) )
          {
            if ( m_caloHypoCloner )
            {
              // Use full cloning tool
              LHCb::CaloHypo * hypoClone = (*m_caloHypoCloner)(*iCalo);
              // save
              if ( hypoClone ) { protoParticleClone->addToCalo(hypoClone); }
            }
            else
            {
              // Use basic Cloner
              LHCb::CaloHypo * hypoClone = cloneKeyedContainerItem<CaloHypoCloner>(hypo);
              if ( hypoClone )
              {
                // For basic Cloner, set hypo, cluster and digit smartref vectors to empty
                // as the basic cloning keeps them pointing to the originals
                hypoClone->clearHypos();
                hypoClone->clearDigits();
                hypoClone->clearClusters();
                // save the clone
                protoParticleClone->addToCalo(hypoClone);
              }
            }
          }
          else
          {
            // save the original
            protoParticleClone->addToCalo(hypo);
          }
        }
      }
    }

  }

  return protoParticleClone;
}

//=============================================================================

ProtoParticleCloner::~ProtoParticleCloner() {}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ProtoParticleCloner )

//=============================================================================
