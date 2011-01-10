
//-----------------------------------------------------------------------------
/** @file ChargedProtoANNPIDBase.cpp
 *
 * Implementation file for algorithm ChargedProtoANNPIDTrainingTuple
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2010-03-09
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "ChargedProtoANNPIDTrainingTuple.h"

//-----------------------------------------------------------------------------

using namespace ANNGlobalPID;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoANNPIDTrainingTuple );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoANNPIDTrainingTuple::
ChargedProtoANNPIDTrainingTuple( const std::string& name,
                                 ISvcLocator* pSvcLocator )
  : ChargedProtoANNPIDBase ( name , pSvcLocator ),
    m_truth                ( NULL )
{
  using namespace boost::assign;
  declareProperty( "Variables",
                   m_variablesS = list_of
                   // General event variables
                   ("NumProtoParticles")("NumCaloHypos")
                   // Tracking
                   ("TrackP")("TrackPt")("TrackChi2PerDof")("TrackType")("TrackHistory")
                   ("TrackNumDof")("TrackLikelihood")("TrackGhostProbability")
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
}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoANNPIDTrainingTuple::~ChargedProtoANNPIDTrainingTuple() { }

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChargedProtoANNPIDTrainingTuple::initialize()
{
  const StatusCode sc = ChargedProtoANNPIDBase::initialize();
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
// Main execution
//=============================================================================
StatusCode ChargedProtoANNPIDTrainingTuple::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  // Load the charged ProtoParticles
  LHCb::ProtoParticles * protos = get<LHCb::ProtoParticles>( m_protoPath );

  // Loop over ProtoParticles
  for ( LHCb::ProtoParticles::const_iterator iP = protos->begin();
        iP != protos->end(); ++iP )
  {
    // Check this is a charged track ProtoParticle
    const LHCb::Track * track = (*iP)->track();
    if ( !track ) continue;

    // make a tuple
    Tuple tuple = nTuple("annInputs", "ProtoParticle PID Information for ANN Training");

    // Loop over reconstruction variables
    StringInputs::const_iterator inputS = m_variablesS.begin();
    IntInputs::const_iterator    inputI = m_variablesI.begin();
    for ( ; inputS != m_variablesS.end() && inputI != m_variablesI.end(); ++inputS, ++inputI )
    {
      // get the variable and fill ntuple
      sc = sc && tuple->column( *inputS, getInput(*iP,*inputI) );
    }

    // MC variables
    const LHCb::MCParticle * mcPart = m_truth->mcParticle( track );
    sc = sc && tuple->column( "MCParticleType", mcPart ? mcPart->particleID().pid() : 0 );
    sc = sc && tuple->column( "MCParticleP",    mcPart ? mcPart->p() : -999 );
    sc = sc && tuple->column( "MCParticlePt",   mcPart ? mcPart->pt() : -999 );
    sc = sc && tuple->column( "MCVirtualMass",  mcPart ? mcPart->virtualMass() : -999 );

    // write the tuple for this protoP
    sc = sc && tuple->write();

  }

  return sc;
}

//=============================================================================
