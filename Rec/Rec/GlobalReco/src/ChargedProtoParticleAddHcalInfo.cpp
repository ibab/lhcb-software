
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleAddHcalInfo.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleAddHcalInfo
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 28/08/2009
 */
//-----------------------------------------------------------------------------

// local
#include "ChargedProtoParticleAddHcalInfo.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoParticleAddHcalInfo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoParticleAddHcalInfo::
ChargedProtoParticleAddHcalInfo( const std::string& name,
                                 ISvcLocator* pSvcLocator)
  : ChargedProtoParticleCALOBaseAlg ( name , pSvcLocator ),
    m_InHcalTable    ( NULL ),
    m_HcalETable     ( NULL ),
    m_dlleHcalTable  ( NULL ),
    m_dllmuHcalTable ( NULL )
{
  // default locations from context()

  using namespace LHCb::Calo2Track;
  using namespace LHCb::CaloIdLocation;
  using namespace LHCb::CaloAlgUtils;

  m_protoPath         = LHCb::ProtoParticleLocation::Charged ;
  m_inHcalPath        = PathFromContext( context() , InHcal              );
  m_hcalEPath         = PathFromContext( context() , HcalE               );
  m_hcalPIDePath      = PathFromContext( context() , HcalPIDe            );
  m_hcalPIDmuPath     = PathFromContext( context() , HcalPIDmu           );

  declareProperty("ProtoParticleLocation"      , m_protoPath      );
  declareProperty("InputInHcalLocation"        , m_inHcalPath     );
  declareProperty("InputHcalELocation"         , m_hcalEPath      );
  declareProperty("InputHcalPIDeLocation"      , m_hcalPIDePath   );
  declareProperty("InputHcalPIDmuLocation"     , m_hcalPIDmuPath  );

}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoParticleAddHcalInfo::~ChargedProtoParticleAddHcalInfo() { }

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoParticleAddHcalInfo::execute()
{
  // Load the HCAL data
  const bool sc = getHcalData();
  if ( !sc )
  {
    return Warning( "No HCAL data -> ProtoParticles will not be changed.", StatusCode::SUCCESS );
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
    addHcal(*iProto);
  }
  counter("HcalPIDs("+context()+") ==> " + m_protoPath )+= protos->size();

  return StatusCode::SUCCESS;
}

//=============================================================================

//=============================================================================
// Loads the Calo Hcal data
//=============================================================================
bool ChargedProtoParticleAddHcalInfo::getHcalData()
{
  const bool sc1 = loadCaloTable(m_InHcalTable   , m_inHcalPath);
  const bool sc2 = loadCaloTable(m_HcalETable    , m_hcalEPath);
  const bool sc3 = loadCaloTable(m_dlleHcalTable , m_hcalPIDePath);
  const bool sc4 = loadCaloTable(m_dllmuHcalTable, m_hcalPIDmuPath);
  const bool sc  = sc1 && sc2 && sc3 && sc4;
  if ( sc && msgLevel(MSG::DEBUG) ) debug() << "HCAL PID SUCCESSFULLY LOADED" << endmsg;
  return sc;
}

//=============================================================================
// Add Calo Hcal info to the protoparticle
//=============================================================================
bool ChargedProtoParticleAddHcalInfo::addHcal( LHCb::ProtoParticle * proto ) const
{
  // clear HCAL info
  proto->removeCaloHcalInfo();

  LHCb::Calo2Track::ITrAccTable::Range                 aRange;
  LHCb::Calo2Track::ITrEvalTable::Range                vRange;

  bool hasHcalPID = false;

  aRange = m_InHcalTable -> relations ( proto->track() ) ;
  if ( !aRange.empty() )
  {
    hasHcalPID = aRange.front().to();
    if( hasHcalPID )
    {
      if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is in Hcal acceptance"  << endmsg;
      proto->addInfo(LHCb::ProtoParticle::InAccHcal, true );

      // Get the HcalE (intermediate) estimator
      vRange = m_HcalETable -> relations ( proto->track() ) ;
      if ( !vRange.empty() ) { proto->addInfo(LHCb::ProtoParticle::CaloHcalE,  vRange.front().to() ); }
      // Get the Hcal DLL(e)
      vRange = m_dlleHcalTable -> relations ( proto->track() ) ;
      if ( !vRange.empty() ) { proto->addInfo(LHCb::ProtoParticle::HcalPIDe , vRange.front().to() ); }

      // Get the Hcal DLL(mu)
      vRange = m_dllmuHcalTable -> relations ( proto->track() ) ;
      if ( !vRange.empty() ) { proto->addInfo(LHCb::ProtoParticle::HcalPIDmu , vRange.front().to() ); }

      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> Hcal PID  : "
                  << " HcalE      =" <<  proto->info(LHCb::ProtoParticle::CaloHcalE, -999.)
                  << " Dlle (Hcal) =" <<  proto->info(LHCb::ProtoParticle::HcalPIDe, -999.)
                  << " Dllmu (Hcal) =" <<  proto->info(LHCb::ProtoParticle::HcalPIDmu, -999.)
                  << endmsg;

    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is NOT in Hcal acceptance"  << endmsg;
    }
  }
  else
  {
    if ( msgLevel(MSG::VERBOSE) )verbose() << " -> No entry for that track in the Hcal acceptance table"  << endmsg;
  }

  return hasHcalPID;
}
