
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleAddEcalInfo.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleAddEcalInfo
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 28/08/2009
 */
//-----------------------------------------------------------------------------

// local
#include "ChargedProtoParticleAddEcalInfo.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoParticleAddEcalInfo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoParticleAddEcalInfo::
ChargedProtoParticleAddEcalInfo( const std::string& name,
                                 ISvcLocator* pSvcLocator)
  : ChargedProtoParticleCALOBaseAlg ( name , pSvcLocator ),
    m_InEcalTable     ( NULL ),
    m_elecTrTable     ( NULL ),
    m_clusTrTable     ( NULL ),
    m_EcalChi2Table   ( NULL ),
    m_EcalETable      ( NULL ),
    m_ClusChi2Table   ( NULL ),
    m_dlleEcalTable   ( NULL ),
    m_dllmuEcalTable  ( NULL ),
    m_electron        ( NULL )
{
  // default locations from context()

  using namespace LHCb::Calo2Track;
  using namespace LHCb::CaloIdLocation;
  m_protoPath         =  LHCb::ProtoParticleLocation::Charged ;
  m_inEcalPath        =  LHCb::CaloAlgUtils::PathFromContext( context() , InEcal        );
  m_electronMatchPath =  LHCb::CaloAlgUtils::PathFromContext( context() , ElectronMatch );
  m_clusterMatchPath  =  LHCb::CaloAlgUtils::PathFromContext( context() , ClusterMatch  );
  m_ecalChi2Path      =  LHCb::CaloAlgUtils::PathFromContext( context() , EcalChi2      );
  m_ecalEPath         =  LHCb::CaloAlgUtils::PathFromContext( context() , EcalE         );
  m_clusterChi2Path   =  LHCb::CaloAlgUtils::PathFromContext( context() , ClusChi2      );
  m_ecalPIDePath      =  LHCb::CaloAlgUtils::PathFromContext( context() , EcalPIDe      );
  m_ecalPIDmuPath     =  LHCb::CaloAlgUtils::PathFromContext( context() , EcalPIDmu     );
  declareProperty("InputInEcalLocation"        , m_inEcalPath         );
  declareProperty("InputElectronMatchLocation" , m_electronMatchPath  );
  declareProperty("InputClusterMatchLocation"  , m_clusterMatchPath   );
  declareProperty("InputEcalChi2Location"      , m_ecalChi2Path       );
  declareProperty("InputEcalELocation"         , m_ecalEPath          );
  declareProperty("InputClusterChi2Location"   , m_clusterChi2Path    );
  declareProperty("InputEcalPIDeLocation"      , m_ecalPIDePath       );
  declareProperty("InputEcalPIDmuLocation"     , m_ecalPIDmuPath      );

  // ProtoParticles
  declareProperty( "ProtoParticleLocation", m_protoPath );

}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoParticleAddEcalInfo::~ChargedProtoParticleAddEcalInfo() { }

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChargedProtoParticleAddEcalInfo::initialize()
{
  const StatusCode sc = ChargedProtoParticleCALOBaseAlg::initialize();
  if ( sc.isFailure() ) return sc;

  // CaloElectron tool
  m_electron = tool<ICaloElectron>("CaloElectron","CaloElectron",this);

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoParticleAddEcalInfo::execute()
{
  // Load the ECAL data
  const bool sc = getEcalData();
  if ( !sc )
  {
    return Warning( "No ECAL data -> ProtoParticles will not be changed.", StatusCode::SUCCESS );
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
    addEcal(*iProto);
  }
  counter("EcalPIDs("+context()+") ==> " + m_protoPath )+= protos->size();

  return StatusCode::SUCCESS;
}

//=============================================================================

//=============================================================================
// Loads the Calo Ecal data
//=============================================================================
bool ChargedProtoParticleAddEcalInfo::getEcalData()
{
  const bool sc1 = loadCaloTable(m_InEcalTable  , m_inEcalPath       );
  const bool sc2 = loadCaloTable(m_elecTrTable  , m_electronMatchPath);
  const bool sc3 = loadCaloTable(m_clusTrTable  , m_clusterMatchPath );
  const bool sc4 = loadCaloTable(m_EcalChi2Table, m_ecalChi2Path);
  const bool sc5 = loadCaloTable(m_EcalETable   , m_ecalEPath);
  const bool sc6 = loadCaloTable(m_ClusChi2Table, m_clusterChi2Path);
  const bool sc7 = loadCaloTable(m_dlleEcalTable, m_ecalPIDePath);
  const bool sc8 = loadCaloTable(m_dllmuEcalTable,m_ecalPIDmuPath);

  const bool sc  = sc1 && sc2 && sc3 && sc4 && sc5 && sc6 && sc7 && sc8;
  if ( sc && msgLevel(MSG::DEBUG) ) debug() << "Ecal PID SUCCESSFULLY LOADED" << endmsg;

  return sc;
}

//=============================================================================
// Add Calo Ecal info to the protoparticle
//=============================================================================
bool ChargedProtoParticleAddEcalInfo::addEcal( LHCb::ProtoParticle * proto ) const
{
  // First remove existing ECAL info
  proto->removeCaloEcalInfo();

  // Now add new ECAL info

  LHCb::Calo2Track::ITrAccTable::Range                 aRange;
  LHCb::Calo2Track::ITrHypoTable2D::Range              hRange;
  LHCb::Calo2Track::IClusTrTable2D::InverseType::Range cRange;
  LHCb::Calo2Track::ITrEvalTable::Range                vRange;

  bool hasEcalPID = false;

  aRange = m_InEcalTable -> relations ( proto->track() ) ;
  if ( !aRange.empty() )
  {
    hasEcalPID = aRange.front().to();

    if( hasEcalPID )
    {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> The track is in Ecal acceptance"  << endmsg;
      proto->addInfo(LHCb::ProtoParticle::InAccEcal, true );

      // Get the highest weight associated electron CaloHypo (3D matching)
      hRange = m_elecTrTable ->relations ( proto->track() ) ;
      if ( !hRange.empty() ){

        const LHCb::CaloHypo* hypo = hRange.front().to();
        proto->addToCalo ( hypo );
        // CaloElectron->caloTrajectory must be after addToCalo
        if ( electronTool()->set(proto) ){
          proto->addInfo( LHCb::ProtoParticle::CaloTrajectoryL, electronTool()->caloTrajectoryL(CaloPlane::ShowerMax,"hypo") );
          proto->addInfo( LHCb::ProtoParticle::CaloEoverP, electronTool()->eOverP() );          
        }

        using namespace CaloDataType;
        proto->addInfo(LHCb::ProtoParticle::CaloChargedSpd,  m_estimator->data(hypo, HypoSpdM ) > 0 );
        proto->addInfo(LHCb::ProtoParticle::CaloChargedPrs,  m_estimator->data(hypo, HypoPrsE )   );
        proto->addInfo(LHCb::ProtoParticle::CaloChargedEcal, m_estimator->data(hypo, ClusterE )  );
        proto->addInfo(LHCb::ProtoParticle::CaloChargedID       ,  m_estimator->data(hypo, CellID )  );
        proto->addInfo(LHCb::ProtoParticle::CaloElectronMatch , hRange.front().weight() );
      }

      // Get the highest weight associated CaloCluster (2D matching)
      cRange = m_clusTrTable -> inverse() ->relations ( proto->track() ) ;
      if ( !cRange.empty() ) { proto->addInfo(LHCb::ProtoParticle::CaloTrMatch , cRange.front().weight() ); }

      // Get EcalE (intermediate) estimator
      vRange = m_EcalETable -> relations ( proto->track() ) ;
      if ( !vRange.empty() ) { proto->addInfo(LHCb::ProtoParticle::CaloEcalE ,  vRange.front().to() ); }

      // Get EcalChi2 (intermediate) estimator
      vRange = m_EcalChi2Table -> relations ( proto->track() ) ;
      if ( !vRange.empty() ) { proto->addInfo(LHCb::ProtoParticle::CaloEcalChi2,  vRange.front().to() ); }

      // Get ClusChi2 (intermediate) estimator
      vRange = m_ClusChi2Table -> relations ( proto->track() ) ;
      if ( !vRange.empty() ) { proto->addInfo(LHCb::ProtoParticle::CaloClusChi2,  vRange.front().to() ); }

      // Get Ecal DLL(e)
      vRange = m_dlleEcalTable -> relations ( proto->track() ) ;
      if ( !vRange.empty() ) { proto->addInfo(LHCb::ProtoParticle::EcalPIDe , vRange.front().to() ); }

      // Get Ecal DLL(mu)
      vRange = m_dllmuEcalTable -> relations ( proto->track() ) ;
      if ( !vRange.empty() ) { proto->addInfo(LHCb::ProtoParticle::EcalPIDmu , vRange.front().to() ); }

      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> Ecal PID : "
                  << " Chi2-3D    =" <<  proto->info(LHCb::ProtoParticle::CaloElectronMatch, -999.)
                  << " Chi2-2D    =" <<  proto->info(LHCb::ProtoParticle::CaloTrMatch, -999.)
                  << " EcalE      =" <<  proto->info(LHCb::ProtoParticle::CaloEcalE , -999.)
                  << " ClusChi2   =" <<  proto->info(LHCb::ProtoParticle::CaloClusChi2, -999.)
                  << " EcalChi2   =" <<  proto->info(LHCb::ProtoParticle::CaloEcalChi2, -999.)
                  << " Dlle (Ecal) =" <<  proto->info(LHCb::ProtoParticle::EcalPIDe, -999.)
                  << " Dllmu (Ecal) =" <<  proto->info(LHCb::ProtoParticle::EcalPIDmu, -999.)
                  << " Spd Digits " <<  proto->info(LHCb::ProtoParticle::CaloChargedSpd, 0.)
                  << " Prs Digits " <<  proto->info(LHCb::ProtoParticle::CaloChargedPrs, 0.)
                  << " Spd Digits " <<  proto->info(LHCb::ProtoParticle::CaloChargedSpd, 0.)
                  << " Ecal Cluster " <<  proto->info(LHCb::ProtoParticle::CaloChargedEcal, 0.)
                  << " TrajectoryL " <<  proto->info(LHCb::ProtoParticle::CaloTrajectoryL, 0.)
                  << endmsg;

    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is NOT in Ecal acceptance"  << endmsg;
    }
  }
  else
  {
    if ( msgLevel(MSG::VERBOSE) )verbose() << " -> No entry for that track in the Ecal acceptance table "  << endmsg;
  }

  return hasEcalPID;
}
