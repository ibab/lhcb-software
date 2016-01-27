
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleAddRichInfo.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleAddRichInfo
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 28/08/2009
 */
//-----------------------------------------------------------------------------

// local
#include "ChargedProtoParticleAddRichInfo.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoParticleAddRichInfo )

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
  // ProtoParticle container
  LHCb::ProtoParticles * protos = getIfExists<ProtoParticles>(m_protoPath);
  if ( !protos )
  {
    return Warning( "No existing ProtoParticle container at " +  
                    m_protoPath + " thus do nothing.",
                    StatusCode::SUCCESS );
  }
  
  // Load the RichPIDs
  const bool richSc = getRichData();
  if ( !richSc ) { return StatusCode::SUCCESS; }

  // Loop over proto particles and add RICH info
  for ( auto * proto : *protos ) { updateRICH(proto); }
  counter(m_richPath + " ==> " + m_protoPath ) += protos->size();

  // return
  return StatusCode::SUCCESS;
}

//=============================================================================
// Add RICH info to the protoparticle
//=============================================================================
void ChargedProtoParticleAddRichInfo::updateRICH( ProtoParticle * proto ) const
{
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Trying ProtoParticle " << proto->key() << endmsg;

  // Erase current RichPID information
  proto->removeRichInfo();

  // Does this track have a RICH PID result ?
  TrackToRichPID::const_iterator iR = m_richMap.find( proto->track() );
  if ( m_richMap.end() == iR ) 
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> NO associated RichPID object found" << endmsg;
    return;
  }

  // RichPID for this track is found, so save data
  const LHCb::RichPID * richPID = (*iR).second;

  // RichPID for this track is found, so save data
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << " -> Found RichPID data : DLLs = " << richPID->particleLLValues() << endmsg;

  // reference to RichPID object
  proto->setRichPID( richPID );

  // Store the raw RICH PID info
  proto->addInfo( LHCb::ProtoParticle::RichDLLe,   richPID->particleDeltaLL(Rich::Electron) );
  proto->addInfo( LHCb::ProtoParticle::RichDLLmu,  richPID->particleDeltaLL(Rich::Muon) );
  proto->addInfo( LHCb::ProtoParticle::RichDLLpi,  richPID->particleDeltaLL(Rich::Pion) );
  proto->addInfo( LHCb::ProtoParticle::RichDLLk,   richPID->particleDeltaLL(Rich::Kaon) );
  proto->addInfo( LHCb::ProtoParticle::RichDLLp,   richPID->particleDeltaLL(Rich::Proton) );
  proto->addInfo( LHCb::ProtoParticle::RichDLLbt,  richPID->particleDeltaLL(Rich::BelowThreshold) );
  proto->addInfo( LHCb::ProtoParticle::RichDLLd,   richPID->particleDeltaLL(Rich::Deuteron) );

  // Store History
  proto->addInfo( LHCb::ProtoParticle::RichPIDStatus, richPID->pidResultCode() );

}

//=============================================================================
// Loads the RICH data
//=============================================================================
bool ChargedProtoParticleAddRichInfo::getRichData()
{
  // empty the map
  m_richMap.clear();

  // Do we have any RichPID results
  const RichPIDs * richpids = getIfExists<RichPIDs>( m_richPath );
  if ( !richpids )
  {
    Warning( "No RichPIDs at '" + m_richPath +
             "' -> ProtoParticles will not be changed.", 
             StatusCode::SUCCESS, 1 ).ignore();
    return false;
  }
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Successfully loaded " << richpids->size()
            << " RichPIDs from " << m_richPath << endmsg;

  // refresh the reverse mapping
  for ( const LHCb::RichPID * pid : *richpids )
  {
    if ( pid->track() )
    {
      m_richMap[ pid->track() ] = pid;
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "RichPID key=" << pid->key() 
                  << " has Track key=" << pid->track()->key()
                  << " " << pid->track() << endmsg;
    }
    else
    {
      std::ostringstream mess;
      mess << "RichPID key=" << pid->key() << " has NULL Track pointer";
      Warning( mess.str() ).ignore();
    }
  }

  return true;
}
