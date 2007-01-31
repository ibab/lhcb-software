
//-----------------------------------------------------------------------------
/** @file ProtoParticleTupleAlg.cpp
 *
 * Implemenrtation file for algorithm ProtoParticleTupleAlg
 *
 * CVS Log :-
 * $Id: ProtoParticleTupleAlg.cpp,v 1.2 2007-01-31 18:25:12 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-11-15
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "ProtoParticleTupleAlg.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ProtoParticleTupleAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProtoParticleTupleAlg::ProtoParticleTupleAlg( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_truth       ( NULL )
{
  // Job Options
  declareProperty( "ProtoParticleLocation",
                   m_protoPath = LHCb::ProtoParticleLocation::Charged );
}

//=============================================================================
// Destructor
//=============================================================================
ProtoParticleTupleAlg::~ProtoParticleTupleAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ProtoParticleTupleAlg::initialize()
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
StatusCode ProtoParticleTupleAlg::execute()
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
    const LHCb::RichPID * rpid = proto->richPID();
    tuple->column( "RichDlle",      proto->info ( LHCb::ProtoParticle::RichDLLe,  99999 ) );
    tuple->column( "RichDllmu",     proto->info ( LHCb::ProtoParticle::RichDLLmu, 99999 ) );
    tuple->column( "RichDllpi",     proto->info ( LHCb::ProtoParticle::RichDLLpi, 99999 ) );
    tuple->column( "RichDllk",      proto->info ( LHCb::ProtoParticle::RichDLLk,  99999 ) );
    tuple->column( "RichDllp",      proto->info ( LHCb::ProtoParticle::RichDLLp,  99999 ) );
    tuple->column( "RichUsedAero",  rpid ? rpid->usedAerogel()  : false );
    tuple->column( "RichUsedR1Gas", rpid ? rpid->usedRich1Gas() : false );
    tuple->column( "RichUsedR2Gas", rpid ? rpid->usedRich2Gas() : false );
    tuple->column( "RichAboveElThres", rpid ? rpid->electronHypoAboveThres() : false );
    tuple->column( "RichAboveMuThres", rpid ? rpid->muonHypoAboveThres() : false );
    tuple->column( "RichAbovePiThres", rpid ? rpid->pionHypoAboveThres() : false );
    tuple->column( "RichAboveKaThres", rpid ? rpid->kaonHypoAboveThres() : false );
    tuple->column( "RichAbovePrThres", rpid ? rpid->protonHypoAboveThres() : false );

    // muon
    const LHCb::MuonPID * mpid = proto->muonPID();
    tuple->column( "MuonBkgLL",    proto->info ( LHCb::ProtoParticle::MuonBkgLL, 99999 ) );
    tuple->column( "MuonMuLL",     proto->info ( LHCb::ProtoParticle::MuonMuLL,  99999 ) );
    tuple->column( "MuonNShared",  proto->info ( LHCb::ProtoParticle::MuonNShared, 99999 ) );
    tuple->column( "MuonIsMuon",   mpid ? mpid->IsMuon() : false );
    tuple->column( "MuonInAcc",    mpid ? mpid->InAcceptance() : false );

    // calo
    tuple->column( "EcalPIDe",   proto->info ( LHCb::ProtoParticle::EcalPIDe,  99999 ) );
    tuple->column( "HcalPIDe",   proto->info ( LHCb::ProtoParticle::HcalPIDe,  99999 ) );
    tuple->column( "PrsPIDe",    proto->info ( LHCb::ProtoParticle::PrsPIDe,   99999 ) );
    tuple->column( "BremPIDe",   proto->info ( LHCb::ProtoParticle::BremPIDe,  99999 ) );
    tuple->column( "EcalPIDmu",  proto->info ( LHCb::ProtoParticle::EcalPIDmu, 99999 ) );
    tuple->column( "HcalPIDmu",  proto->info ( LHCb::ProtoParticle::HcalPIDmu, 99999 ) );
    tuple->column( "InAccSpd",   proto->info ( LHCb::ProtoParticle::InAccSpd,  false ) );
    tuple->column( "InAccPrs",   proto->info ( LHCb::ProtoParticle::InAccPrs,  false ) );
    tuple->column( "InAccEcal",  proto->info ( LHCb::ProtoParticle::InAccEcal,  false ) );
    tuple->column( "InAccHcal",  proto->info ( LHCb::ProtoParticle::InAccHcal,  false ) );
    tuple->column( "InAccBrem",  proto->info ( LHCb::ProtoParticle::InAccBrem,  false ) );
    tuple->column( "CaloTrMatch",       proto->info ( LHCb::ProtoParticle::CaloTrMatch,  99999 ) );
    tuple->column( "CaloElectronMatch", proto->info ( LHCb::ProtoParticle::CaloElectronMatch,  99999 ) );
    tuple->column( "CaloBremMatch",     proto->info ( LHCb::ProtoParticle::CaloBremMatch, 99999 ) );
    tuple->column( "CaloChargedSpd",    proto->info ( LHCb::ProtoParticle::CaloChargedSpd, 99999 ) );
    tuple->column( "CaloChargedPrs",    proto->info ( LHCb::ProtoParticle::CaloChargedPrs, 99999 ) );
    tuple->column( "CaloSpdE",   proto->info ( LHCb::ProtoParticle::CaloSpdE,   99999 ) );
    tuple->column( "CaloPrsE",   proto->info ( LHCb::ProtoParticle::CaloPrsE,   99999 ) );
    tuple->column( "CaloEcalE",  proto->info ( LHCb::ProtoParticle::CaloEcalE,  99999 ) );
    tuple->column( "CaloHcalE",  proto->info ( LHCb::ProtoParticle::CaloHcalE,  99999 ) );

    // combined DLLs
    tuple->column( "CombDLLe",   proto->info ( LHCb::ProtoParticle::CombDLLe,  99999 ) );
    tuple->column( "CombDLLmu",  proto->info ( LHCb::ProtoParticle::CombDLLmu, 99999 ) );
    tuple->column( "CombDLLpi",  proto->info ( LHCb::ProtoParticle::CombDLLpi, 99999 ) );
    tuple->column( "CombDLLk",   proto->info ( LHCb::ProtoParticle::CombDLLk,  99999 ) );
    tuple->column( "CombDLLp",   proto->info ( LHCb::ProtoParticle::CombDLLp,  99999 ) );

    // VeloCharge
    tuple->column( "VeloCharge", proto->info ( LHCb::ProtoParticle::VeloCharge,  99999 ) );

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
