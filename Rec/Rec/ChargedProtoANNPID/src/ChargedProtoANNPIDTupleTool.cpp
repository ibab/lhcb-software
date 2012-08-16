
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

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
  : ChargedProtoANNPIDToolBase ( type, name, parent ),
    m_truth                    ( NULL )
{

  // interface
  declareInterface<IChargedProtoANNPIDTupleTool>(this);

  // Job options
  declareProperty( "Variables",
                   m_variablesS = boost::assign::list_of
                   // General event variables
                   ("NumProtoParticles")
                   // Tracking
                   ("TrackP")("TrackPt")("TrackChi2PerDof")("TrackType")("TrackHistory")
                   ("TrackNumDof")("TrackLikelihood")("TrackGhostProbability")
                   ("TrackMatchChi2")("TrackFitMatchChi2")("TrackCloneDist")
                   ("TrackFitVeloChi2")("TrackFitVeloNDoF")("TrackFitTChi2")("TrackFitTNDoF")
                   // Combined DLLs
                   ("CombDLLe")("CombDLLmu")("CombDLLpi")("CombDLLk")("CombDLLp")
                   // RICH
                   ("RichUsedAero")("RichUsedR1Gas")("RichUsedR2Gas")
                   ("RichAboveElThres")("RichAboveMuThres")("RichAbovePiThres")
                   ("RichAboveKaThres")("RichAbovePrThres")
                   ("RichDLLe")("RichDLLmu")("RichDLLpi")("RichDLLk")("RichDLLp")("RichDLLbt")
                   // MUON
                   ("InAccMuon")
                   ("MuonMuLL")("MuonBkgLL")("MuonIsMuon")("MuonIsLooseMuon")("MuonNShared")
                   // ECAL
                   ("InAccEcal")("CaloChargedSpd")("CaloChargedPrs")("CaloChargedEcal")
                   ("CaloElectronMatch")("CaloTrMatch")("CaloEcalE")("CaloEcalChi2")
                   ("CaloClusChi2")("EcalPIDe")("EcalPIDmu")("CaloTrajectoryL")
                   // HCAL
                   ("InAccHcal")("CaloHcalE")("HcalPIDe")("HcalPIDmu")
                   // PRS
                   ("InAccPrs")("CaloPrsE")("PrsPIDe")
                   // SPD
                   ("InAccSpd")("CaloSpdE")
                   // BREM
                   ("InAccBrem")("CaloNeutralSpd")("CaloNeutralPrs")("CaloNeutralEcal")
                   ("CaloBremMatch")("CaloBremChi2")("BremPIDe")
                   // VELO
                   ("VeloCharge")
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

  // get int IDs (faster lookup than with the string IDs..)
  m_variablesI = variableIDs( m_variablesS );
  if ( m_variablesI.size() != m_variablesS.size() )
  {
    return Error( "Error in defining the ntuple entries" );
  }

  return sc;
}

//=============================================================================

StatusCode ChargedProtoANNPIDTupleTool::fill( const LHCb::ProtoParticle * proto,
                                              const LHCb::ParticleID pid ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // Check this is a charged track ProtoParticle
  const LHCb::Track * track = proto->track();
  if ( !track ) return sc;

  // make a tuple
  Tuple tuple = nTuple("annInputs", "ProtoParticle PID Information for ANN Training");

  // Loop over reconstruction variables
  StringInputs::const_iterator inputS = m_variablesS.begin();
  IntInputs::const_iterator    inputI = m_variablesI.begin();
  for ( ; inputS != m_variablesS.end() && inputI != m_variablesI.end(); ++inputS, ++inputI )
  {
    // get the variable and fill ntuple
    sc = sc && tuple->column( *inputS, (float)getInput(proto,*inputI) );
  }

  // MC variables
  const LHCb::MCParticle * mcPart = m_truth->mcParticle(track);
  sc = sc && tuple->column( "MCParticleType", mcPart ? mcPart->particleID().pid() : 0 );
  sc = sc && tuple->column( "MCParticleP",    mcPart ? mcPart->p()  : -999 );
  sc = sc && tuple->column( "MCParticlePt",   mcPart ? mcPart->pt() : -999 );
  sc = sc && tuple->column( "MCVirtualMass",  mcPart ? mcPart->virtualMass() : -999 );

  // PID info
  sc = sc && tuple->column( "RecoPIDcode", pid.pid() );

  // write the tuple for this ProtoParticle
  sc = sc && tuple->write();

  // return
  return sc;
}
