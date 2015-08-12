
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleAddSpdInfo.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleAddSpdInfo
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 28/08/2009
 */
//-----------------------------------------------------------------------------

// local
#include "ChargedProtoParticleAddSpdInfo.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoParticleAddSpdInfo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoParticleAddSpdInfo::
ChargedProtoParticleAddSpdInfo( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : ChargedProtoParticleCALOBaseAlg ( name , pSvcLocator ),
    m_InSpdTable ( NULL ),
    m_SpdETable  ( NULL )
{
  // default locations from context()

  using namespace LHCb::Calo2Track;
  using namespace LHCb::CaloIdLocation;
  using namespace LHCb::CaloAlgUtils;

  m_protoPath   = LHCb::ProtoParticleLocation::Charged ;
  m_inSpdPath   = PathFromContext( context() , InSpd  );
  m_spdEPath    = PathFromContext( context() , SpdE   );

  declareProperty("ProtoParticleLocation"      , m_protoPath       );
  declareProperty("InputInSpdLocation"         , m_inSpdPath       );
  declareProperty("InputSpdELocation"          , m_spdEPath        );

}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoParticleAddSpdInfo::~ChargedProtoParticleAddSpdInfo() { }

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoParticleAddSpdInfo::execute()
{
  // Load the Brem data
  const bool sc = getSpdData();
  if ( !sc )
  {
    return Warning( "No CALO SPD data -> ProtoParticles will not be changed.", StatusCode::SUCCESS );
  }

  // ProtoParticle container
  LHCb::ProtoParticles * protos = getIfExists<LHCb::ProtoParticles>(m_protoPath);
  if ( NULL == protos )
  {
    if( msgLevel(MSG::DEBUG) ) debug() << "No existing ProtoParticle container at "
                                       <<  m_protoPath<<" thus do nothing."<<endmsg;
    return StatusCode::SUCCESS;  
  }

  // Loop over proto particles
  for ( LHCb::ProtoParticles::iterator iProto = protos->begin();
        iProto != protos->end(); ++iProto )
  {
    // replace the muon information
    addSpd(*iProto);
  }
  counter("SpdPIDs("+context()+") ==> " + m_protoPath )+= protos->size();

  return StatusCode::SUCCESS;
}

//=============================================================================

//=============================================================================
// Loads the Calo Spd data
//=============================================================================
bool ChargedProtoParticleAddSpdInfo::getSpdData()
{
  const bool sc1 = loadCaloTable(m_InSpdTable , m_inSpdPath);
  const bool sc2 = loadCaloTable(m_SpdETable  , m_spdEPath );
  const bool sc  = sc1 && sc2;
  if ( sc && msgLevel(MSG::DEBUG) ) debug() << "SPD PID SUCCESSFULLY LOADED" << endmsg;
  return sc;
}

//=============================================================================
// Add Calo Spd info to the protoparticle
//=============================================================================
bool ChargedProtoParticleAddSpdInfo::addSpd( LHCb::ProtoParticle * proto ) const
{
  // clear SPD info
  proto->removeCaloSpdInfo();

  LHCb::Calo2Track::ITrAccTable::Range      aRange;
  LHCb::Calo2Track::ITrEvalTable::Range     vRange;

  bool hasSpdPID = false;

  aRange = m_InSpdTable -> relations ( proto->track() ) ;
  if ( !aRange.empty() )
  {
    hasSpdPID = aRange.front().to();
    if( hasSpdPID )
    {
      if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is in Spd acceptance"  << endmsg;
      proto->addInfo(LHCb::ProtoParticle::InAccSpd , true );

      // Get the PrsE (intermediate) estimator
      vRange = m_SpdETable -> relations ( proto->track() ) ;
      if ( !vRange.empty() ) { proto->addInfo(LHCb::ProtoParticle::CaloSpdE,  vRange.front().to() ); }

      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> Spd PID : "
                  << " SpdE       =" <<  proto->info(LHCb::ProtoParticle::CaloSpdE, -999.)
                  << endmsg;

    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> The track is NOT in Spd acceptance"  << endmsg;
    }
  }
  else
  {
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> No entry for that track in the Spd acceptance table"  << endmsg;
  }

  return hasSpdPID;
}
