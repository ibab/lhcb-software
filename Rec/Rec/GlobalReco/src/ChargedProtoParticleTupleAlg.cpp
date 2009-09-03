
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleTupleAlg.cpp
 *
 * Implemenrtation file for algorithm ChargedProtoParticleTupleAlg
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleTupleAlg.cpp,v 1.6 2009-09-03 11:09:22 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-11-15
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "ChargedProtoParticleTupleAlg.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoParticleTupleAlg );

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
  m_truth = tool<Rich::Rec::MC::IMCTruthTool>( "Rich::Rec::MC::MCTruthTool", "MCTruth", this );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoParticleTupleAlg::execute()
{
  // Load the charged ProtoParticles
  const LHCb::ProtoParticles * protos = get<LHCb::ProtoParticles>( m_protoPath );

  // Loop over the protos
  for ( LHCb::ProtoParticles::const_iterator iP = protos->begin();
        iP != protos->end(); ++iP )
  {
    const LHCb::ProtoParticle * proto = *iP;

    // Check this is a charged track ProtoParticle
    const LHCb::Track * track = proto->track();
    if ( !track ) continue;

    // make a tuple
    Tuple tuple = nTuple("protoPtuple", "ProtoParticle PID Information");

    // reco variables

    // some track info
    tuple->column( "TrackP",          track->p()          );
    tuple->column( "TrackPt",         track->pt()         );
    tuple->column( "TrackChi2PerDof", track->chi2PerDoF() );
    tuple->column( "TrackNumDof",     track->nDoF()       );
    tuple->column( "TrackType",       track->type()       );
    tuple->column( "TrackHistory",    track->history()    );

    // rich
    //const LHCb::RichPID * rpid = proto->richPID();
    LHCb::RichPID tmpRPID;
    tmpRPID.setPidResultCode( static_cast<int>(proto->info(LHCb::ProtoParticle::RichPIDStatus,0)) );
    tuple->column( "RichDLLe",      proto->info ( LHCb::ProtoParticle::RichDLLe,  0 ) );
    tuple->column( "RichDLLmu",     proto->info ( LHCb::ProtoParticle::RichDLLmu, 0 ) );
    tuple->column( "RichDLLpi",     proto->info ( LHCb::ProtoParticle::RichDLLpi, 0 ) );
    tuple->column( "RichDLLk",      proto->info ( LHCb::ProtoParticle::RichDLLk,  0 ) );
    tuple->column( "RichDLLp",      proto->info ( LHCb::ProtoParticle:: RichDLLp,  0 ) );
    tuple->column( "RichUsedAero",  tmpRPID.usedAerogel()  );
    tuple->column( "RichUsedR1Gas", tmpRPID.usedRich1Gas() );
    tuple->column( "RichUsedR2Gas", tmpRPID.usedRich2Gas() );
    tuple->column( "RichAboveElThres", tmpRPID.electronHypoAboveThres() );
    tuple->column( "RichAboveMuThres", tmpRPID.muonHypoAboveThres() );
    tuple->column( "RichAbovePiThres", tmpRPID.pionHypoAboveThres() );
    tuple->column( "RichAboveKaThres", tmpRPID.kaonHypoAboveThres() );
    tuple->column( "RichAbovePrThres", tmpRPID.protonHypoAboveThres() );

    // muon
    //const LHCb::MuonPID * mpid = proto->muonPID();
    tuple->column( "MuonBkgLL",    proto->info ( LHCb::ProtoParticle::MuonBkgLL, 0 ) );
    tuple->column( "MuonMuLL",     proto->info ( LHCb::ProtoParticle::MuonMuLL,  0 ) );
    tuple->column( "MuonNShared",  proto->info ( LHCb::ProtoParticle::MuonNShared, 0 ) );
    tuple->column( "MuonIsMuon",   proto->hasInfo( LHCb::ProtoParticle::MuonPIDStatus ) );
    tuple->column( "MuonInAcc",    proto->info ( LHCb::ProtoParticle::InAccMuon, false ) );

    // calo
    tuple->column( "InAccSpd",   proto->info ( LHCb::ProtoParticle::InAccSpd,  false ) );
    tuple->column( "InAccPrs",   proto->info ( LHCb::ProtoParticle::InAccPrs,  false ) );
    tuple->column( "InAccEcal",  proto->info ( LHCb::ProtoParticle::InAccEcal, false ) );
    tuple->column( "InAccHcal",  proto->info ( LHCb::ProtoParticle::InAccHcal, false ) );
    tuple->column( "InAccBrem",  proto->info ( LHCb::ProtoParticle::InAccBrem, false ) );
    tuple->column( "CaloTrMatch",       proto->info ( LHCb::ProtoParticle::CaloTrMatch, 0 ) );
    tuple->column( "CaloElectronMatch", proto->info ( LHCb::ProtoParticle::CaloElectronMatch, 0 ) );
    tuple->column( "CaloBremMatch",     proto->info ( LHCb::ProtoParticle::CaloBremMatch, 0 ) );
    tuple->column( "CaloChargedSpd",    proto->info ( LHCb::ProtoParticle::CaloChargedSpd, 0 ) );
    tuple->column( "CaloChargedPrs",    proto->info ( LHCb::ProtoParticle::CaloChargedPrs, 0 ) );
    tuple->column( "CaloChargedEcal",   proto->info ( LHCb::ProtoParticle::CaloChargedEcal, 0 ) );
    tuple->column( "CaloSpdE",   proto->info ( LHCb::ProtoParticle::CaloSpdE,   0 ) );
    tuple->column( "CaloPrsE",   proto->info ( LHCb::ProtoParticle::CaloPrsE,   0 ) );
    tuple->column( "CaloEcalE",  proto->info ( LHCb::ProtoParticle::CaloEcalE,  0 ) );
    tuple->column( "CaloHcalE",  proto->info ( LHCb::ProtoParticle::CaloHcalE,  0 ) );
    tuple->column( "CaloTrajectoryL", proto->info ( LHCb::ProtoParticle::CaloTrajectoryL,  0 ) );
    tuple->column( "EcalPIDe",   proto->info ( LHCb::ProtoParticle::EcalPIDe,  0 ) );
    tuple->column( "HcalPIDe",   proto->info ( LHCb::ProtoParticle::HcalPIDe,  0 ) );
    tuple->column( "PrsPIDe",    proto->info ( LHCb::ProtoParticle::PrsPIDe,   0 ) );
    tuple->column( "BremPIDe",   proto->info ( LHCb::ProtoParticle::BremPIDe,  0 ) );
    tuple->column( "EcalPIDmu",  proto->info ( LHCb::ProtoParticle::EcalPIDmu, 0 ) );
    tuple->column( "HcalPIDmu",  proto->info ( LHCb::ProtoParticle::HcalPIDmu, 0 ) );

    // combined DLLs
    tuple->column( "CombDLLe",   proto->info ( LHCb::ProtoParticle::CombDLLe,  0 ) );
    tuple->column( "CombDLLmu",  proto->info ( LHCb::ProtoParticle::CombDLLmu, 0 ) );
    tuple->column( "CombDLLpi",  proto->info ( LHCb::ProtoParticle::CombDLLpi, 0 ) );
    tuple->column( "CombDLLk",   proto->info ( LHCb::ProtoParticle::CombDLLk,  0 ) );
    tuple->column( "CombDLLp",   proto->info ( LHCb::ProtoParticle::CombDLLp,  0 ) );

    // VeloCharge
    tuple->column( "VeloCharge", proto->info ( LHCb::ProtoParticle::VeloCharge,  0 ) );

    // MCParticle information
    const LHCb::MCParticle * mcPart = m_truth->mcParticle( track );
    tuple->column( "MCParticleType", mcPart ? mcPart->particleID().pid() : 0 );
    tuple->column( "MCParticleP",    mcPart ? mcPart->p() : -99999 );
    tuple->column( "MCParticlePt",   mcPart ? mcPart->pt() : -99999 );
    tuple->column( "MCVirtualMass",  mcPart ? mcPart->virtualMass() : -99999 );

    // write the tuple for this protoP
    tuple->write();

  } // loop over protos

  return StatusCode::SUCCESS;
}

//=============================================================================
