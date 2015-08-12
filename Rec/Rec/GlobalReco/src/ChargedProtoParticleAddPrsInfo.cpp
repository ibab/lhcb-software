
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleAddPrsInfo.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleAddPrsInfo
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 28/08/2009
 */
//-----------------------------------------------------------------------------

// local
#include "ChargedProtoParticleAddPrsInfo.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoParticleAddPrsInfo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoParticleAddPrsInfo::
ChargedProtoParticleAddPrsInfo( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : ChargedProtoParticleCALOBaseAlg ( name , pSvcLocator ),
    m_InPrsTable   ( NULL ),
    m_PrsETable    ( NULL ),
    m_dllePrsTable ( NULL )
{
  // default locations from context()

  using namespace LHCb::Calo2Track;
  using namespace LHCb::CaloIdLocation;
  using namespace LHCb::CaloAlgUtils;

  m_protoPath         = LHCb::ProtoParticleLocation::Charged ;
  m_inPrsPath         = PathFromContext( context() , InPrs   );
  m_prsEPath          = PathFromContext( context() , PrsE    );
  m_prsPIDePath       = PathFromContext( context() , PrsPIDe );

  declareProperty("ProtoParticleLocation"      , m_protoPath        );
  declareProperty("InputInPrsLocation"         , m_inPrsPath        );
  declareProperty("InputPrsELocation"          , m_prsEPath         );
  declareProperty("InputPrsPIDeLocation"       , m_prsPIDePath      );

}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoParticleAddPrsInfo::~ChargedProtoParticleAddPrsInfo() { }

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoParticleAddPrsInfo::execute()
{
  // Load the Brem data
  const bool sc = getPrsData();
  if ( !sc )
  {
    return Warning( "No BREM data -> ProtoParticles will not be changed.", StatusCode::SUCCESS );
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
    addPrs(*iProto);
  }
  counter("PrsPIDs("+context()+") ==> " + m_protoPath )+= protos->size();

  return StatusCode::SUCCESS;
}

//=============================================================================

//=============================================================================
// Loads the Calo Prs data
//=============================================================================
bool ChargedProtoParticleAddPrsInfo::getPrsData()
{
  const bool sc1 = loadCaloTable(m_InPrsTable  , m_inPrsPath);
  const bool sc2 = loadCaloTable(m_PrsETable   , m_prsEPath);
  const bool sc3 = loadCaloTable(m_dllePrsTable, m_prsPIDePath);
  const bool sc  = sc1 && sc2 && sc3;
  if ( sc && msgLevel(MSG::DEBUG) ) debug() << "PRS PID SUCCESSFULLY LOADED" << endmsg;
  return sc;
}

//=============================================================================
// Add Calo Prs info to the protoparticle
//=============================================================================
bool ChargedProtoParticleAddPrsInfo::addPrs( LHCb::ProtoParticle * proto ) const
{
  // clear PRS info
  proto->removeCaloPrsInfo();

  // Add new info

  LHCb::Calo2Track::ITrAccTable::Range                 aRange;
  LHCb::Calo2Track::ITrEvalTable::Range                vRange;

  bool hasPrsPID = false;

  aRange = m_InPrsTable -> relations ( proto->track() ) ;
  if( !aRange.empty() )
  {
    hasPrsPID = aRange.front().to();
    if( hasPrsPID )
    {
      if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is in Prs acceptance"  << endmsg;
      proto->addInfo(LHCb::ProtoParticle::InAccPrs , true );

      // Get the PrsE (intermediate) estimator
      vRange = m_PrsETable -> relations ( proto->track() ) ;
      if ( !vRange.empty() ) { proto->addInfo(LHCb::ProtoParticle::CaloPrsE,  vRange.front().to() ); }

      // Get the Prs DLL(e)
      vRange = m_dllePrsTable -> relations ( proto->track() ) ;
      if ( !vRange.empty() ) { proto->addInfo(LHCb::ProtoParticle::PrsPIDe , vRange.front().to() ); }

      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> Prs PID : "
                  << " PrsE       =" <<  proto->info(LHCb::ProtoParticle::CaloPrsE, -999.)
                  << " Dlle (Prs)  =" <<  proto->info(LHCb::ProtoParticle::PrsPIDe , -999.)
                  << endmsg;

    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is NOT in Prs acceptance"  << endmsg;
    }
  }
  else
  {
    if ( msgLevel(MSG::VERBOSE) )verbose() << " -> No entry for that track in the Prs acceptance table"  << endmsg;
  }

  return hasPrsPID;
}
