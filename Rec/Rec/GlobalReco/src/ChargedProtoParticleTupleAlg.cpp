
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleTupleAlg.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleTupleAlg
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-11-15
 */
//-----------------------------------------------------------------------------

// local
#include "ChargedProtoParticleTupleAlg.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoParticleTupleAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoParticleTupleAlg::
ChargedProtoParticleTupleAlg( const std::string& name,
                              ISvcLocator* pSvcLocator )
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_truth       ( NULL )
{
  // context specific locations
  if ( context() == "HLT" || context() == "Hlt" )
  {
    m_protoPath = LHCb::ProtoParticleLocation::HltCharged;
  }
  else
  {
    m_protoPath = LHCb::ProtoParticleLocation::Charged;
  }

  // Job Options
  declareProperty( "ProtoParticleLocation", m_protoPath );
}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoParticleTupleAlg::~ChargedProtoParticleTupleAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChargedProtoParticleTupleAlg::initialize()
{
  const StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  m_truth = tool<Rich::Rec::MC::IMCTruthTool>( "Rich::Rec::MC::MCTruthTool", 
                                               "MCTruth", this );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoParticleTupleAlg::execute()
{
  // Load the charged ProtoParticles
  const LHCb::ProtoParticles * protos = 
    getIfExists<LHCb::ProtoParticles>( m_protoPath );
  if ( !protos ) 
    return Warning( "No ProtoParticles at '" + m_protoPath + "'", 
                    StatusCode::SUCCESS );

  // Loop over the protos
  for ( LHCb::ProtoParticles::const_iterator iP = protos->begin();
        iP != protos->end(); ++iP )
  {
    const LHCb::ProtoParticle * proto = *iP;

    // Check this is a charged track ProtoParticle
    const LHCb::Track * track = proto->track();
    if ( !track ) continue;

    // make a tuple
    Tuple tuple = nTuple("protoPtuple","ProtoParticle PID Information");

    // Status Code for filling the ntuple
    StatusCode sc = StatusCode::SUCCESS;

    // reco variables

    // some track info
    sc = sc && tuple->column( "TrackP",          track->p()  );
    sc = sc && tuple->column( "TrackPt",         track->pt() );
    sc = sc && tuple->column( "TrackChi2PerDof", track->chi2PerDoF() );
    sc = sc && tuple->column( "TrackNumDof",     track->nDoF() );
    sc = sc && tuple->column( "TrackType",       track->type() );
    sc = sc && tuple->column( "TrackHistory",    track->history() );
    sc = sc && tuple->column( "TrackGhostProb",  track->ghostProbability() );
    sc = sc && tuple->column( "TrackLikelihood", track->likelihood() );
    sc = sc && tuple->column( "TrackCloneDist",  track->info(LHCb::Track::CloneDist,9e10) );

    // rich
    static LHCb::RichPID tmpRPID;
    tmpRPID.setPidResultCode( static_cast<int>(proto->info(LHCb::ProtoParticle::RichPIDStatus,0)) );
    sc = sc && tuple->column( "RichDLLe",      proto->info ( LHCb::ProtoParticle::RichDLLe,  0 ) );
    sc = sc && tuple->column( "RichDLLmu",     proto->info ( LHCb::ProtoParticle::RichDLLmu, 0 ) );
    sc = sc && tuple->column( "RichDLLpi",     proto->info ( LHCb::ProtoParticle::RichDLLpi, 0 ) );
    sc = sc && tuple->column( "RichDLLk",      proto->info ( LHCb::ProtoParticle::RichDLLk,  0 ) );
    sc = sc && tuple->column( "RichDLLp",      proto->info ( LHCb::ProtoParticle::RichDLLp,  0 ) );
    sc = sc && tuple->column( "RichDLLbt",     proto->info ( LHCb::ProtoParticle::RichDLLbt, 0 ) );
    sc = sc && tuple->column( "RichUsedAero",     tmpRPID.usedAerogel()  );
    sc = sc && tuple->column( "RichUsedR1Gas",    tmpRPID.usedRich1Gas() );
    sc = sc && tuple->column( "RichUsedR2Gas",    tmpRPID.usedRich2Gas() );
    sc = sc && tuple->column( "RichAboveElThres", tmpRPID.electronHypoAboveThres() );
    sc = sc && tuple->column( "RichAboveMuThres", tmpRPID.muonHypoAboveThres() );
    sc = sc && tuple->column( "RichAbovePiThres", tmpRPID.pionHypoAboveThres() );
    sc = sc && tuple->column( "RichAboveKaThres", tmpRPID.kaonHypoAboveThres() );
    sc = sc && tuple->column( "RichAbovePrThres", tmpRPID.protonHypoAboveThres() );
    sc = sc && tuple->column( "RichBestPID",      (int)tmpRPID.bestParticleID() );

    // muon
    static LHCb::MuonPID tmpMPID;
    tmpMPID.setStatus( static_cast<int>(proto->info(LHCb::ProtoParticle::MuonPIDStatus,0)) );
    sc = sc && tuple->column( "MuonBkgLL",    proto->info ( LHCb::ProtoParticle::MuonBkgLL, 0 ) );
    sc = sc && tuple->column( "MuonMuLL",     proto->info ( LHCb::ProtoParticle::MuonMuLL,  0 ) );
    sc = sc && tuple->column( "MuonNShared",  proto->info ( LHCb::ProtoParticle::MuonNShared, 0 ) );
    sc = sc && tuple->column( "MuonIsLooseMuon", tmpMPID.IsMuonLoose() );
    sc = sc && tuple->column( "MuonIsMuon",      tmpMPID.IsMuon() );
    sc = sc && tuple->column( "MuonInAcc",  proto->info ( LHCb::ProtoParticle::InAccMuon, false ) );

    // calo
    sc = sc && tuple->column( "InAccSpd",   proto->info ( LHCb::ProtoParticle::InAccSpd,  false ) );
    sc = sc && tuple->column( "InAccPrs",   proto->info ( LHCb::ProtoParticle::InAccPrs,  false ) );
    sc = sc && tuple->column( "InAccEcal",  proto->info ( LHCb::ProtoParticle::InAccEcal, false ) );
    sc = sc && tuple->column( "InAccHcal",  proto->info ( LHCb::ProtoParticle::InAccHcal, false ) );
    sc = sc && tuple->column( "InAccBrem",  proto->info ( LHCb::ProtoParticle::InAccBrem, false ) );
    sc = sc && tuple->column( "CaloTrMatch",       proto->info ( LHCb::ProtoParticle::CaloTrMatch, 0 ) );
    sc = sc && tuple->column( "CaloElectronMatch", proto->info ( LHCb::ProtoParticle::CaloElectronMatch, 0 ) );
    sc = sc && tuple->column( "CaloBremMatch",     proto->info ( LHCb::ProtoParticle::CaloBremMatch, 0 ) );
    sc = sc && tuple->column( "CaloChargedSpd",    proto->info ( LHCb::ProtoParticle::CaloChargedSpd, 0 ) );
    sc = sc && tuple->column( "CaloChargedPrs",    proto->info ( LHCb::ProtoParticle::CaloChargedPrs, 0 ) );
    sc = sc && tuple->column( "CaloChargedEcal",   proto->info ( LHCb::ProtoParticle::CaloChargedEcal, 0 ) );
    sc = sc && tuple->column( "CaloSpdE",   proto->info ( LHCb::ProtoParticle::CaloSpdE,   0 ) );
    sc = sc && tuple->column( "CaloPrsE",   proto->info ( LHCb::ProtoParticle::CaloPrsE,   0 ) );
    sc = sc && tuple->column( "CaloEcalChi2", proto->info ( LHCb::ProtoParticle::CaloEcalChi2, 0 ) );
    sc = sc && tuple->column( "CaloClusChi2", proto->info ( LHCb::ProtoParticle::CaloClusChi2, 0 ) );
    sc = sc && tuple->column( "CaloBremChi2", proto->info ( LHCb::ProtoParticle::CaloBremChi2, 0 ) );
    sc = sc && tuple->column( "CaloPrsE",   proto->info ( LHCb::ProtoParticle::CaloPrsE,   0 ) );
    sc = sc && tuple->column( "CaloEcalE",  proto->info ( LHCb::ProtoParticle::CaloEcalE,  0 ) );
    sc = sc && tuple->column( "CaloHcalE",  proto->info ( LHCb::ProtoParticle::CaloHcalE,  0 ) );
    sc = sc && tuple->column( "CaloTrajectoryL", proto->info ( LHCb::ProtoParticle::CaloTrajectoryL, 0 ) );
    sc = sc && tuple->column( "EcalPIDe",   proto->info ( LHCb::ProtoParticle::EcalPIDe,  0 ) );
    sc = sc && tuple->column( "HcalPIDe",   proto->info ( LHCb::ProtoParticle::HcalPIDe,  0 ) );
    sc = sc && tuple->column( "PrsPIDe",    proto->info ( LHCb::ProtoParticle::PrsPIDe,   0 ) );
    sc = sc && tuple->column( "BremPIDe",   proto->info ( LHCb::ProtoParticle::BremPIDe,  0 ) );
    sc = sc && tuple->column( "EcalPIDmu",  proto->info ( LHCb::ProtoParticle::EcalPIDmu, 0 ) );
    sc = sc && tuple->column( "HcalPIDmu",  proto->info ( LHCb::ProtoParticle::HcalPIDmu, 0 ) );

    // combined DLLs
    sc = sc && tuple->column( "CombDLLe",   proto->info ( LHCb::ProtoParticle::CombDLLe,  0 ) );
    sc = sc && tuple->column( "CombDLLmu",  proto->info ( LHCb::ProtoParticle::CombDLLmu, 0 ) );
    sc = sc && tuple->column( "CombDLLpi",  proto->info ( LHCb::ProtoParticle::CombDLLpi, 0 ) );
    sc = sc && tuple->column( "CombDLLk",   proto->info ( LHCb::ProtoParticle::CombDLLk,  0 ) );
    sc = sc && tuple->column( "CombDLLp",   proto->info ( LHCb::ProtoParticle::CombDLLp,  0 ) );

    // ANN PID Probabilities
    sc = sc && tuple->column( "ProbNNe",     proto->info ( LHCb::ProtoParticle::ProbNNe,  -1 ) );
    sc = sc && tuple->column( "ProbNNmu",    proto->info ( LHCb::ProtoParticle::ProbNNmu, -1 ) );
    sc = sc && tuple->column( "ProbNNpi",    proto->info ( LHCb::ProtoParticle::ProbNNpi, -1 ) );
    sc = sc && tuple->column( "ProbNNk",     proto->info ( LHCb::ProtoParticle::ProbNNk,  -1 ) );
    sc = sc && tuple->column( "ProbNNp",     proto->info ( LHCb::ProtoParticle::ProbNNp,  -1 ) );
    sc = sc && tuple->column( "ProbNNghost", proto->info ( LHCb::ProtoParticle::ProbNNghost, -1 ) );

    // VeloCharge
    sc = sc && tuple->column( "VeloCharge", proto->info ( LHCb::ProtoParticle::VeloCharge, 0 ) );

    // MCParticle information
    const LHCb::MCParticle * mcPart = m_truth->mcParticle( track );
    sc = sc && tuple->column( "MCParticleType", mcPart ? mcPart->particleID().pid() : 0 );
    sc = sc && tuple->column( "MCParticleP",    mcPart ? mcPart->p() : -99999 );
    sc = sc && tuple->column( "MCParticlePt",   mcPart ? mcPart->pt() : -99999 );
    sc = sc && tuple->column( "MCVirtualMass",  mcPart ? mcPart->virtualMass() : -99999 );

    // write the tuple for this protoP
    sc = sc && tuple->write();

    if ( sc.isFailure() ) return sc;

  } // loop over protos

  return StatusCode::SUCCESS;
}

//=============================================================================
