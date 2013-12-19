// $Id: $
// Include files
#include "LoKi/IHybridFactory.h"

#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>

// DTF
#include "DecayTreeFitter/Fitter.h"

// local
#include "Bc2JpsiHBDTSelection.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Bc2JpsiHBDTSelection
//
// 2013-11-01 : Jibo He
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Bc2JpsiHBDTSelection )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Bc2JpsiHBDTSelection::Bc2JpsiHBDTSelection( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : base_class(type,name,parent),
    m_dist(0),
    m_dva(0),
    m_BDTReader(0),
    m_nVars(12),
    m_cut(-1.0),
    m_BPVIPCHI2(m_dist, 0)
{
  m_values = new float[m_nVars];

  declareProperty("BDTCut", m_cut, "BDT cut value");
  declareProperty("WeightsFile", m_weightsFile, "Weights file");

}
//=============================================================================
// Destructor
//=============================================================================
Bc2JpsiHBDTSelection::~Bc2JpsiHBDTSelection() {
  delete []m_values;
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode Bc2JpsiHBDTSelection::initialize() {

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

  const char* m_variables[] = { "sqrt(MuP_IPCHI2_OWNPV)", "sqrt(MuM_IPCHI2_OWNPV)", "sqrt(Jpsi_IPCHI2_OWNPV)", "sqrt(H_IPCHI2_OWNPV)", "sqrt(B_IPCHI2_OWNPV)", "MuP_PT", "MuM_PT", "Jpsi_PT", "H_PT", "B_PVFit_decayLength[0]", "B_PVFit_ctau[0]", "B_PVFit_chi2[0]" };
  
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
StatusCode Bc2JpsiHBDTSelection::finalize() {

  if(m_BDTReader) delete m_BDTReader;

  return GaudiTool::finalize();

}


//====================================================================
// BDT cut
//====================================================================
bool Bc2JpsiHBDTSelection::operator()(const LHCb::Particle* p) const
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
bool Bc2JpsiHBDTSelection::set (const LHCb::Particle* p) const{

  if(0 == p) return false;

  const LHCb::Particle* pJpsi = LoKi::Child::child( p, 1 );
  const LHCb::Particle* pPion = LoKi::Child::child( p, 2 );

  const LHCb::Particle* pL1 = LoKi::Child::child( p, 1, 1 );
  const LHCb::Particle* pL2 = LoKi::Child::child( p, 1, 2 );

  if( !pJpsi || !pPion || !pL1 || !pL2 ){
    Error("Can't get all the daughters, check the inputs!");
    return false;
  }

  if(msgLevel(MSG::DEBUG)) debug() << " Jpsi PID: "  << pJpsi->particleID()
                                   << ", L1 PID: "   << pL1->particleID()
                                   << ", L2 PID: "   << pL2->particleID()
                                   << ", Pion PID: " << pPion->particleID()
                                   << endmsg;

  // IPCHI2
  m_BPVIPCHI2.setTool(m_dist);

  // B 
  const LHCb::VertexBase* BPV = m_dva->bestVertex(p);  
  m_BPVIPCHI2.setVertex(BPV);
  double B_IPCHI2 = m_BPVIPCHI2( p ) ;

  const LHCb::VertexBase* aPV = NULL;
  
  // L1
  aPV = m_dva->bestVertex( pL1 );
  m_BPVIPCHI2.setVertex(aPV);
  double L1_IPCHI2 = m_BPVIPCHI2( pL1 ) ;

  // L2
  aPV = m_dva->bestVertex( pL2 );
  m_BPVIPCHI2.setVertex(aPV);
  double L2_IPCHI2 = m_BPVIPCHI2( pL2 ) ;  
  
  // Jpsi
  aPV = m_dva->bestVertex( pJpsi );
  m_BPVIPCHI2.setVertex(aPV);
  double Jpsi_IPCHI2 = m_BPVIPCHI2( pJpsi ) ;  

  // Pion
  aPV = m_dva->bestVertex( pPion );
  m_BPVIPCHI2.setVertex(aPV);
  double Pion_IPCHI2 = m_BPVIPCHI2( pPion ) ;


  // protection, just in case 
  if(    L1_IPCHI2 <0 
      || L2_IPCHI2 <0 
      || Jpsi_IPCHI2 <0
      || Pion_IPCHI2  <0
      || B_IPCHI2 < 0 ) {    

    Error( "IPCHI2<0! Something must be wrong!" );
    return false ;
  }
   
  m_values[0] = (float)sqrt( L1_IPCHI2 );
  m_values[1] = (float)sqrt( L2_IPCHI2 );
  m_values[2] = (float)sqrt( Jpsi_IPCHI2 );
  m_values[3] = (float)sqrt( Pion_IPCHI2 );
  m_values[4] = (float)sqrt( B_IPCHI2 );


  // PT
  m_values[5] = (float)pL1->pt()/Gaudi::Units::MeV;
  m_values[6] = (float)pL2->pt()/Gaudi::Units::MeV;
  m_values[7] = (float)pJpsi->pt()/Gaudi::Units::MeV;
  m_values[8] = (float)pPion->pt()/Gaudi::Units::MeV;


  // DTF variables
  // with PV constraint
  DecayTreeFitter::Fitter m_fitter( *p, *BPV );

  // J/psi mass constraint
  m_fitter.setMassConstraint(LHCb::ParticleID(443));

  m_fitter.fit();

  if( DecayTreeFitter::Fitter::Success == m_fitter.status() ){

    const Gaudi::Math::ParticleParams* m_fitParams = m_fitter.fitParams( p ) ;

    Gaudi::Math::ValueWithError m_tau = m_fitParams->ctau() ;
    Gaudi::Math::ValueWithError m_decayLength = m_fitParams->decayLength();

    double myChi2 = m_fitter.chiSquare();
    double myCtau = m_tau.value();
    double myDL   = m_decayLength.value();
    
    m_values[9]  = (float)myDL ;
    m_values[10] = (float)myCtau ;
    m_values[11] = (float)myChi2 ;

  } else {

    return false ;
    
  } 
  
  return true;
}






