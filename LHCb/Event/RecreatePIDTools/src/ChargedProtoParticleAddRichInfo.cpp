
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleAddRichInfo.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleAddRichInfo
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleAddRichInfo.cpp,v 1.5 2010-03-09 18:53:02 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 28/08/2009
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "ChargedProtoParticleAddRichInfo.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoParticleAddRichInfo );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoParticleAddRichInfo::
ChargedProtoParticleAddRichInfo( const std::string& name,
                                 ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

  // context specific locations
  if ( context() == "HLT" || context() == "Hlt" )
  {
    m_richPath   = LHCb::RichPIDLocation::HLT;
    m_protoPath  = LHCb::ProtoParticleLocation::HltCharged;
  }
  else
  {
    m_richPath   = LHCb::RichPIDLocation::Offline;
    m_protoPath  = LHCb::ProtoParticleLocation::Charged;
  }

  // RICH data
  declareProperty( "InputRichPIDLocation", m_richPath );

  // ProtoParticles
  declareProperty( "ProtoParticleLocation", m_protoPath );

}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoParticleAddRichInfo::~ChargedProtoParticleAddRichInfo() { }

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoParticleAddRichInfo::execute()
{
  // Load the RichPIDs
  const bool richSc = getRichData();
  if ( !richSc ) { return StatusCode::SUCCESS; }

  // ProtoParticle container
  if ( !exist<ProtoParticles>(m_protoPath) )
  {
    return Warning( "No existing ProtoParticle container at " +  m_protoPath + " thus do nothing.",
                    StatusCode::SUCCESS );
  }
  LHCb::ProtoParticles * protos = get<ProtoParticles>(m_protoPath);

  // Loop over proto particles
  for ( ProtoParticles::iterator iProto = protos->begin();
        iProto != protos->end(); ++iProto )
  {
    // replace the RICh information
    updateRICH(*iProto);

  }
  counter(m_richPath + " ==> " + m_protoPath )+= protos->size();

  return StatusCode::SUCCESS;
}

//=============================================================================

//=============================================================================
// Loads the RICH data
//=============================================================================
bool ChargedProtoParticleAddRichInfo::getRichData()
{
  // empty the map
  m_richMap.clear();

  // Do we have any RichPID results
  if ( !exist<RichPIDs>(m_richPath) )
  {
    Warning( "No RichPIDs at '" + m_richPath +
             "' -> ProtoParticles will not be changed.", StatusCode::SUCCESS, 1 ).ignore();
    return false;
  }

  // yes, so load them
  const RichPIDs * richpids = get<RichPIDs>( m_richPath );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully loaded " << richpids->size()
            << " RichPIDs from " << m_richPath << endmsg;
  }

  // refresh the reverse mapping
  for ( RichPIDs::const_iterator iR = richpids->begin();
        iR != richpids->end(); ++iR )
  {
    m_richMap[ (*iR)->track() ] = *iR;
  }

  return true;
}

//=============================================================================
// Add RICH info to the protoparticle
//=============================================================================
void ChargedProtoParticleAddRichInfo::updateRICH( ProtoParticle * proto ) const
{
  // Erase current RichPID information
  proto->removeRichInfo();

  // Does this track have a RICH PID result ?
  TrackToRichPID::const_iterator iR = m_richMap.find( proto->track() );
  if ( m_richMap.end() == iR ) return;

  const LHCb::RichPID * richPID = (*iR).second;

  // RichPID for this track is found, so save data
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << " -> Found RichPID data : DLLs = " << richPID->particleLLValues() << endmsg;
  }

  // reference to RichPID object
  proto->setRichPID( richPID );

  // Store the raw RICH PID info
  proto->addInfo( LHCb::ProtoParticle::RichDLLe,   richPID->particleDeltaLL(Rich::Electron) );
  proto->addInfo( LHCb::ProtoParticle::RichDLLmu,  richPID->particleDeltaLL(Rich::Muon) );
  proto->addInfo( LHCb::ProtoParticle::RichDLLpi,  richPID->particleDeltaLL(Rich::Pion) );
  proto->addInfo( LHCb::ProtoParticle::RichDLLk,   richPID->particleDeltaLL(Rich::Kaon) );
  proto->addInfo( LHCb::ProtoParticle::RichDLLp,   richPID->particleDeltaLL(Rich::Proton) );
  proto->addInfo( LHCb::ProtoParticle::RichDLLbt,  richPID->particleDeltaLL(Rich::BelowThreshold) );

  // Store History
  proto->addInfo( LHCb::ProtoParticle::RichPIDStatus, richPID->pidResultCode() );

}
