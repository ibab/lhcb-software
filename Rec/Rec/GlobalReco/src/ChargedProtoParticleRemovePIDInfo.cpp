
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleRemovePIDInfo.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleRemovePIDInfo
 *
 * @author Dmitry Golubkov
 * @date 13/03/2010
 */
//-----------------------------------------------------------------------------

// local
#include "ChargedProtoParticleRemovePIDInfo.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoParticleRemovePIDInfo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoParticleRemovePIDInfo::
ChargedProtoParticleRemovePIDInfo( const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_RemoveRichPID( false )
  , m_RemoveMuonPID( false )
  , m_RemoveCombPID( false )
{

  // context specific locations
  if ( context() == "HLT" || context() == "Hlt" )
  {
    m_protoPath  = LHCb::ProtoParticleLocation::HltCharged;
  }
  else
  {
    m_protoPath  = LHCb::ProtoParticleLocation::Charged;
  }

  // ProtoParticles
  declareProperty( "ProtoParticleLocation", m_protoPath );
  declareProperty( "RemoveRichPID", m_RemoveRichPID = true, "remove Rich PID info from the ProtoParticles"    );
  declareProperty( "RemoveMuonPID", m_RemoveMuonPID = true, "remove Muon PID info from the ProtoParticles"    );
  declareProperty( "RemoveCombPID", m_RemoveCombPID = true, "remove Combined PID info from the ProtoParticles");
}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoParticleRemovePIDInfo::~ChargedProtoParticleRemovePIDInfo() { }

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoParticleRemovePIDInfo::execute()
{
  // ProtoParticle container
  LHCb::ProtoParticles * protos = getIfExists<LHCb::ProtoParticles>(m_protoPath);
  if ( NULL == protos )
  {
    return Warning( "No existing ProtoParticle container at " +  m_protoPath + " thus do nothing.",
                    StatusCode::SUCCESS );
  }

  // Loop over proto particles
  for ( LHCb::ProtoParticles::iterator iProto = protos->begin();
        iProto != protos->end(); ++iProto )
  {
    LHCb::ProtoParticle *proto = *iProto;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Trying ProtoParticle " << proto->key() << endmsg;

    // Erase current RichPID information
    if ( m_RemoveRichPID ) proto->removeRichInfo();

    // Erase current MuonPID information
    if ( m_RemoveMuonPID ) proto->removeMuonInfo();

    // Erase current CombPID information
    if ( m_RemoveCombPID ) proto->removeCombinedInfo();

    // print full ProtoParticle content
    if ( msgLevel(MSG::VERBOSE) ) verbose() << *proto << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
