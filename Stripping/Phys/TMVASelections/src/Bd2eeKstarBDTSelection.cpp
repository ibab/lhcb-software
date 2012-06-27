// $Id: $
// Include files
#include "LoKi/IHybridFactory.h"

#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>

// local
#include "Bd2eeKstarBDTSelection.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Bd2eeKstarBDTSelection
//
// 2012-01-19 : Jibo He
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Bd2eeKstarBDTSelection );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Bd2eeKstarBDTSelection::Bd2eeKstarBDTSelection( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : base_class(type,name,parent),
    m_dist(0),
    m_dva(0),
    m_BDTReader(0),
    m_nVars(18),
    m_cut(-1.0),
    m_BPVIPCHI2(m_dist, 0),
    m_BPVVDCHI2(0),
    m_BPVDIRA(0)
{
  m_values = new float[m_nVars];
  
  declareProperty("BDTCut", m_cut, "BDT cut value");
  declareProperty("WeightsFile", m_weightsFile, "Weights file");
 
}
//=============================================================================
// Destructor
//=============================================================================
Bd2eeKstarBDTSelection::~Bd2eeKstarBDTSelection() {
  delete []m_values;
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode Bd2eeKstarBDTSelection::initialize() {
  
  // initialize the base class  (the first action)
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure()) return sc; 
  
  //====================================================================
  // Initialize DVAlg, etc
  //====================================================================
  m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm",
                             StatusCode::FAILURE);
  
  m_dist = m_dva->distanceCalculator();
  if( !m_dist ){
    Error("Unable to retrieve the IDistanceCalculator tool");
    return StatusCode::FAILURE;
  }

  const char* m_variables[] = { "L1_IPCHI2_OWNPV", "L2_IPCHI2_OWNPV", "Jpsi_IPCHI2_OWNPV", "Kaon_IPCHI2_OWNPV", "Pion_IPCHI2_OWNPV", "Kstar_IPCHI2_OWNPV", "B_IPCHI2_OWNPV", "L1_PT", "L2_PT", "Jpsi_PT", "Kaon_PT", "Pion_PT", "Kstar_PT", "B_PT", "Jpsi_FDCHI2_OWNPV", "Kstar_FDCHI2_OWNPV", "B_FDCHI2_OWNPV", "B_DIRA_OWNPV" };
  
  //====================================================================
  // Initialize TMVA reader 
  //====================================================================  
  //m_BDTReader = new TMVA::Reader( "!Color:!Silent" );
  m_BDTReader = new TMVA::Reader( "Silent" );
  
  TString varName;  
  for(unsigned int index = 0; index < m_nVars ; index++){    
    varName = m_variables[index] ;
    m_BDTReader->AddVariable( varName, &m_values[index] );
  }
  
  std::string methodName = "method";
  m_BDTReader->BookMVA( methodName, m_weightsFile );
  
  return StatusCode::SUCCESS;
}


//=============================================================================
// Finalize
//=============================================================================
StatusCode Bd2eeKstarBDTSelection::finalize() {

  if(m_BDTReader) delete m_BDTReader; 
  
  return GaudiTool::finalize();
  
}


//====================================================================
// BDT cut
//====================================================================
bool Bd2eeKstarBDTSelection::operator()(const LHCb::Particle* p) const 
{
  if(0 == p) {
    Error("LHCb::Particle* points to NULL, return false");
    return false ;
  }

  if(!set(p)) return false;
  
  std::string methodName = "method"; 
  
  double BDTResValue = m_BDTReader->EvaluateMVA(methodName);

  if(msgLevel(MSG::DEBUG)) debug() << " BDTG value of this candidate is: " << BDTResValue << endmsg ;
  
  if(BDTResValue > m_cut) return true;
  
  return false;  
}

//====================================================================
// All variables for BDT
//====================================================================
bool Bd2eeKstarBDTSelection::set (const LHCb::Particle* p) const{
  
  if(0 == p) return false;

  const LHCb::Particle* pJpsi  = LoKi::Child::child( p, 1 );
  const LHCb::Particle* pKstar = LoKi::Child::child( p, 2 );

  const LHCb::Particle* pL1 = LoKi::Child::child( p, 1, 1 );
  const LHCb::Particle* pL2 = LoKi::Child::child( p, 1, 2 );
  
  const LHCb::Particle* pKaon = LoKi::Child::child( p, 2, 1 );
  const LHCb::Particle* pPion = LoKi::Child::child( p, 2, 2 );
  
  if( !pJpsi || !pKstar || !pL1 || !pL2 || !pKaon || !pPion  ){
    Error("Can't get all the daughters, check the inputs!");
    return false;
  }

  if(msgLevel(MSG::DEBUG)) debug() << " Jpsi PID: " << pJpsi->particleID() 
                                   << ", Kstar PID: " << pKstar->particleID() 
                                   << ", Kaon PID: " << pKaon->particleID()
                                   << ", Pion PID: " << pPion->particleID()
                                   << endmsg;

  
  // BPV, just use the one of the mother for simplicity 
  const LHCb::VertexBase* BPV = m_dva->bestVertex(p);

  // IPCHI2
  m_BPVIPCHI2.setTool(m_dist);
  m_BPVIPCHI2.setVertex(BPV);

  m_values[0] = m_BPVIPCHI2( pL1 );
  m_values[1] = m_BPVIPCHI2( pL2 );
  m_values[2] = m_BPVIPCHI2( pJpsi );
  m_values[3] = m_BPVIPCHI2( pKaon );
  m_values[4] = m_BPVIPCHI2( pPion );
  m_values[5] = m_BPVIPCHI2( pKstar );
  m_values[6] = m_BPVIPCHI2( p );
  

  // PT
  m_values[7] = pL1->pt()/Gaudi::Units::MeV;
  m_values[8] = pL2->pt()/Gaudi::Units::MeV;
  m_values[9] = pJpsi->pt()/Gaudi::Units::MeV;
  m_values[10]= pKaon->pt()/Gaudi::Units::MeV;
  m_values[11]= pPion->pt()/Gaudi::Units::MeV; 
  m_values[12]= pKstar->pt()/Gaudi::Units::MeV; 
  m_values[13]= p->pt()/Gaudi::Units::MeV; 
  
  // FDCHI2
  m_BPVVDCHI2.setVertex( BPV );

  m_values[14] = m_BPVVDCHI2( pJpsi );
  m_values[15] = m_BPVVDCHI2( pKstar );
  m_values[16] = m_BPVVDCHI2( p );
  
  // DIRA
  m_BPVDIRA.setVertex( BPV );

  m_values[17] = m_BPVDIRA( p );

  return true;
}






