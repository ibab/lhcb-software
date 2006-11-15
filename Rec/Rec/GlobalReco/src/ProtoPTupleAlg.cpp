// $Id: ProtoPTupleAlg.cpp,v 1.2 2006-11-15 16:51:33 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "ProtoPTupleAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProtoPTupleAlg
//
// 2006-11-15 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ProtoPTupleAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProtoPTupleAlg::ProtoPTupleAlg( const std::string& name,
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
ProtoPTupleAlg::~ProtoPTupleAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ProtoPTupleAlg::initialize()
{
  const StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  m_truth = tool<IRichRecMCTruthTool>( "RichRecMCTruthTool", "MCTruth", this );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ProtoPTupleAlg::execute()
{

  // Load the charged ProtoParticles
  LHCb::ProtoParticles * protos = get<LHCb::ProtoParticles>( m_protoPath );

  // Loop over the protos
  for ( LHCb::ProtoParticles::const_iterator iP = protos->begin();
        iP != protos->end(); ++iP )
  {
    const LHCb::ProtoParticle * proto = *iP;

    // Check this is a charged track ProtoParticle
    const LHCb::Track * track = proto->track();
    if ( !track ) continue;

    // MCParticle
    const LHCb::MCParticle * mcPart = m_truth->mcParticle( track );

    // make a tuple
    Tuple tuple = nTuple("protoPdll", "ProtoParticle DLL Information");

    // reco variables

    // some track info
    tuple->column( "TrackP",  track->p() );
    tuple->column( "TrackPt", track->pt() );
    tuple->column( "TrackChi2PerDof", track->chi2PerDoF() );
    tuple->column( "TrackNumDof", track->nDoF() );
    tuple->column( "TrackType", track->type() );
    tuple->column( "TrackHistory", track->history() );
    
    // rich
    tuple->column( "RichDlle",  proto->info ( LHCb::ProtoParticle::RichDLLe,  0 ) );
    tuple->column( "RichDllmu", proto->info ( LHCb::ProtoParticle::RichDLLmu, 0 ) );
    tuple->column( "RichDllpi", proto->info ( LHCb::ProtoParticle::RichDLLpi, 0 ) );
    tuple->column( "RichDllk",  proto->info ( LHCb::ProtoParticle::RichDLLk,  0 ) );
    tuple->column( "RichDllp",  proto->info ( LHCb::ProtoParticle::RichDLLp,  0 ) );

    // muon
    tuple->column( "MuonBkgLL", proto->info ( LHCb::ProtoParticle::MuonBkgLL, 0 ) );
    tuple->column( "MuonMuLL",  proto->info ( LHCb::ProtoParticle::MuonMuLL,  0 ) );

    // calo
    tuple->column( "EcalPIDe",   proto->info ( LHCb::ProtoParticle::EcalPIDe,  0 ) );
    tuple->column( "HcalPIDe",   proto->info ( LHCb::ProtoParticle::HcalPIDe,  0 ) );
    tuple->column( "PrsPIDe",    proto->info ( LHCb::ProtoParticle::PrsPIDe,   0 ) );
    tuple->column( "BremPIDe",   proto->info ( LHCb::ProtoParticle::BremPIDe,  0 ) );
    tuple->column( "EcalPIDmu",  proto->info ( LHCb::ProtoParticle::EcalPIDmu, 0 ) );
    tuple->column( "HcalPIDmu",  proto->info ( LHCb::ProtoParticle::HcalPIDmu, 0 ) );

    // MCInfo
    tuple->column( "MCParticleType", mcPart ? mcPart->particleID().pid() : 0 );

    // write the tuple for this protoP
    tuple->write();

  } // loop over protos

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ProtoPTupleAlg::finalize()
{

  return GaudiTupleAlg::finalize();
}

//=============================================================================
