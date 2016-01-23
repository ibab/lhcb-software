
// local
#include "ChargedProtoANNPIDTupleTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ChargedProtoANNPIDTupleTool
//
// 2011-02-04 : Chris Jones
//-----------------------------------------------------------------------------

using namespace ANNGlobalPID;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ChargedProtoANNPIDTupleTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoANNPIDTupleTool::ChargedProtoANNPIDTupleTool( const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent )
: ChargedProtoANNPIDToolBase ( type, name, parent )
{

  // interface
  declareInterface<IChargedProtoANNPIDTupleTool>(this);

  // Job options
  declareProperty( "Variables", m_variables =
      {
        // General event variables
        "NumProtoParticles","NumCaloHypos",
          "NumLongTracks","NumDownstreamTracks","NumUpstreamTracks",
          "NumVeloTracks","NumTTracks","NumPVs","NumSPDHits",
          "NumRich1Hits","NumRich2Hits","NumMuonTracks",
          // Tracking
          "TrackP","TrackPt","TrackChi2PerDof","TrackType","TrackHistory",
          "TrackNumDof","TrackLikelihood","TrackGhostProbability",
          "TrackMatchChi2","TrackFitMatchChi2","TrackCloneDist",
          "TrackFitVeloChi2","TrackFitVeloNDoF","TrackFitTChi2","TrackFitTNDoF",
          "TrackDOCA",
          // Combined DLLs
          "CombDLLe","CombDLLmu","CombDLLpi","CombDLLk","CombDLLp",
          // RICH
          "RichUsedAero","RichUsedR1Gas","RichUsedR2Gas",
          "RichAboveElThres","RichAboveMuThres","RichAbovePiThres",
          "RichAboveKaThres","RichAbovePrThres",
          "RichDLLe","RichDLLmu","RichDLLpi","RichDLLk","RichDLLp","RichDLLbt",
          // MUON
          "InAccMuon",
          "MuonMuLL","MuonBkgLL","MuonIsMuon","MuonIsLooseMuon","MuonNShared",
          // ECAL
          "InAccEcal","CaloChargedSpd","CaloChargedPrs","CaloChargedEcal",
          "CaloElectronMatch","CaloTrMatch","CaloEcalE","CaloEcalChi2",
          "CaloClusChi2","EcalPIDe","EcalPIDmu","CaloTrajectoryL",
          // HCAL
          "InAccHcal","CaloHcalE","HcalPIDe","HcalPIDmu",
          // PRS
          "InAccPrs","CaloPrsE","PrsPIDe",
          // SPD
          "InAccSpd","CaloSpdE",
          // BREM
          "InAccBrem","CaloNeutralSpd","CaloNeutralPrs","CaloNeutralEcal",
          "CaloBremMatch","CaloBremChi2","BremPIDe",
          // VELO
          "VeloCharge"
          }
                   );

  // Turn off Tuple printing during finalize
  setProperty( "NTuplePrint", false );

}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoANNPIDTupleTool::~ChargedProtoANNPIDTupleTool() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChargedProtoANNPIDTupleTool::initialize()
{
  const StatusCode sc = ChargedProtoANNPIDToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  m_truth = tool<Rich::Rec::MC::IMCTruthTool>( "Rich::Rec::MC::MCTruthTool",
                                               "MCTruth", this );

  // Get a vector of input accessor objects for the configured variables
  for ( const auto& i : m_variables ) { m_inputs[i] = getInput(i); }

  // return
  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedProtoANNPIDTupleTool::finalize()
{
  // Clean Up
  for ( const auto & i : m_inputs ) { delete i.second; }
  m_inputs.clear();
  // return
  return ChargedProtoANNPIDToolBase::finalize();
}

//=============================================================================

StatusCode ChargedProtoANNPIDTupleTool::fill( Tuples::Tuple& tuple,
                                              const LHCb::ProtoParticle * proto,
                                              const LHCb::ParticleID pid ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // Get track 
  const auto * track = proto->track();
  if ( !track ) return Error( "ProtoParticle is neutral!" );

  // Loop over reconstruction variables
  for ( const auto & i : m_inputs )
  {
    // get the variable and fill ntuple
    sc = sc && tuple->column( i.first, (float) i.second->value(proto) );
  }

  // PID info
  sc = sc && tuple->column( "RecoPIDcode", pid.pid() );

  // MC variables

  // First get the MCParticle, if associated
  const auto * mcPart = m_truth->mcParticle(track);
  sc = sc && tuple->column( "HasMC",          mcPart != nullptr );
  sc = sc && tuple->column( "MCParticleType", mcPart ? mcPart->particleID().pid() : 0    );
  sc = sc && tuple->column( "MCParticleP",    mcPart ? mcPart->p()                : -999 );
  sc = sc && tuple->column( "MCParticlePt",   mcPart ? mcPart->pt()               : -999 );
  sc = sc && tuple->column( "MCVirtualMass",  mcPart ? mcPart->virtualMass()      : -999 );

  // MC history flags
  bool fromB(false), fromD(false);
  // Parent MC particle
  const auto * mcParent = ( mcPart ? mcPart->mother() : nullptr );
  unsigned int iCount(0); // protect against infinite loops
  while ( mcParent && ++iCount < 99999 )
  {
    const auto & pid = mcParent->particleID();
    if ( pid.hasBottom() && mcParent->particleID().isHadron() )
    { fromB = true; }
    if ( pid.hasCharm()  && mcParent->particleID().isHadron() )
    { fromD = true; }
    mcParent = mcParent->mother();
  }
  // Save MC parent info
  sc = sc && tuple->column( "MCFromB", fromB );
  sc = sc && tuple->column( "MCFromD", fromD );

  // Get info on the MC vertex type
  const auto * mcVert = ( mcPart ? mcPart->originVertex() : nullptr );
  sc = sc && tuple->column( "MCVertexType", mcVert ? (int)mcVert->type()    : -999   );
  sc = sc && tuple->column( "MCVertexX",    mcVert ? mcVert->position().x() : -999.0 );
  sc = sc && tuple->column( "MCVertexY",    mcVert ? mcVert->position().y() : -999.0 );
  sc = sc && tuple->column( "MCVertexZ",    mcVert ? mcVert->position().z() : -999.0 );

  // return
  return sc;
}
