// Include files 

// from Gaudi
#include "CaloUtils/CaloMomentum.h"
#include "CaloUtils/CaloAlgUtils.h"
// from Event
#include "Event/RecHeader.h"
#include "Event/CaloHypo.h"
#include "Event/CaloCluster.h"

// local
#include "NeutralIDTool.h"


using namespace LHCb;
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : NeutralIDTool
//
// 2013-07-25 : Mostafa HOBALLAH
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( NeutralIDTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NeutralIDTool::NeutralIDTool( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_estimator(NULL){
  declareInterface<INeutralIDTool>(this);
  declareProperty("MinPt", m_minPt = 75.);
}
//=============================================================================
// Destructor
//=============================================================================
NeutralIDTool::~NeutralIDTool() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode NeutralIDTool::initialize() {

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  
  // TMVA discriminant
  std::vector<std::string> inputVars;
  
  inputVars.push_back("TMath::TanH(CaloHypo_ClusterMatch/2000)");
  inputVars.push_back("CaloHypo_ToPrsE");
  inputVars.push_back("CaloHypo_E19");
  inputVars.push_back("CaloHypo_Hcal2Ecal");
  inputVars.push_back("CaloHypo_PrsE19");
  inputVars.push_back("CaloHypo_PrsE49");
  inputVars.push_back("CaloHypo_Spread");
  inputVars.push_back("CaloHypo_PrsE4Max");
  inputVars.push_back("CaloHypo_HypoPrsM");
  inputVars.push_back("CaloHypo_HypoSpdM");

  std::vector<std::string> inputVars1;
  
  inputVars1.push_back("CaloHypo_ClusterMatch");
  inputVars1.push_back("CaloHypo_ToPrsE");
  inputVars1.push_back("CaloHypo_E19");
  inputVars1.push_back("CaloHypo_Hcal2Ecal");
  inputVars1.push_back("CaloHypo_PrsE19");
  inputVars1.push_back("CaloHypo_PrsE49");
  inputVars1.push_back("CaloHypo_Spread");
  inputVars1.push_back("CaloHypo_PrsE4Max");
  inputVars1.push_back("CaloHypo_HypoPrsM");
  inputVars1.push_back("CaloHypo_HypoSpdM");



  m_reader1.reset( new ReadMLPH(inputVars) );
  m_reader0.reset( new ReadMLPE(inputVars1) );


  return StatusCode::SUCCESS;
  
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode NeutralIDTool::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;
  StatusCode sc = GaudiTool::finalize(); // must be executed first

  m_reader0.reset(0);
  m_reader1.reset(0);


  return sc;
}

//=============================================================================
// Main execution
//=============================================================================


double NeutralIDTool::isNotE(const LHCb::CaloHypo* hypo,ICaloHypoEstimator* estimator){
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug()<<"Inside NeutralID ------"<<endmsg;

  if( estimator != NULL)m_estimator=estimator;

  double pt = LHCb::CaloMomentum(hypo).pt();

  double tmva_outputE = CaloDataType::Default;
  
  if (pt>m_minPt){  

    double clmatch = 0; 
    double prse = 0; 
    double e19 = 0; 
    double hclecl = 0; 
    double prse19 = 0;    
    double prse49 = 0; 
    double sprd = 0; 
    double prse4mx = 0; 
    double prsm = 0; 
    double spdm = 0;
    


    const LHCb::CaloCluster* cluster = LHCb::CaloAlgUtils::ClusterFromHypo( hypo , false);        
    if (cluster!=0){

      Variables(hypo,clmatch,prse, e19, hclecl,prse19,prse49, sprd, prse4mx, prsm, spdm);
      
      tmva_outputE = photonDiscriminantE(clmatch, prse, e19, hclecl, prse19,prse49, sprd, prse4mx, prsm, spdm);
    
    }//cluster exists
    else{
      tmva_outputE = CaloDataType::Default; 
    }		
  } 
  else{
    tmva_outputE = CaloDataType::Default;
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug()<<"Outside range of pt"<<endmsg;
  }   
  
  return tmva_outputE;
}

double NeutralIDTool::isNotH(const LHCb::CaloHypo* hypo,ICaloHypoEstimator* estimator){
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug()<<"Inside NeutralID ------"<<endmsg;

  if( estimator != NULL)m_estimator=estimator;


  double pt = LHCb::CaloMomentum(hypo).pt();

  double tmva_outputH = CaloDataType::Default;
  
  if (pt>m_minPt){  

    double clmatch = 0; 
    double prse = 0; 
    double e19 = 0; 
    double hclecl = 0; 
    double prse19 = 0;    
    double prse49 = 0; 
    double sprd = 0; 
    double prse4mx = 0; 
    double prsm = 0; 
    double spdm = 0;

    const LHCb::CaloCluster* cluster = LHCb::CaloAlgUtils::ClusterFromHypo( hypo , false);        
    if (cluster!=0){
      Variables(hypo,clmatch,prse, e19, hclecl,prse19,prse49, sprd, prse4mx, prsm, spdm);
      
      tmva_outputH = photonDiscriminantH(clmatch, prse, e19, hclecl, prse19,prse49, sprd, prse4mx, prsm, spdm);
      
    }//cluster exists
    else{
      tmva_outputH = CaloDataType::Default; 
    }		
  } 
  else{
    tmva_outputH = CaloDataType::Default;
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug()<<"Outside range of pt"<<endmsg;
  }   
  
  return tmva_outputH;
}



double NeutralIDTool::photonDiscriminantE(double clmatch, double prse, double e19, double hclecl, double prse19,
                                         double prse49, double sprd, double prse4mx, double prsm, double spdm) {


        std::vector<double> input;
        
        input.push_back(tanh(clmatch/2000));
        input.push_back(prse);
        input.push_back(e19);
        input.push_back(hclecl);
        input.push_back(prse19);
        input.push_back(prse49);
        input.push_back(sprd);
        input.push_back(prse4mx);
        input.push_back(prsm);
        input.push_back(spdm);
               
        double valueE = -1e10;
        valueE = m_reader0->GetMvaValue(input);


        return valueE;
}




double NeutralIDTool::photonDiscriminantH(double clmatch, double prse, double e19, double hclecl, double prse19,
                                          double prse49, double sprd, double prse4mx, double prsm, double spdm) {


        std::vector<double> input;
        
        input.push_back(tanh(clmatch/2000));
        input.push_back(prse);
        input.push_back(e19);
        input.push_back(hclecl);
        input.push_back(prse19);
        input.push_back(prse49);
        input.push_back(sprd);
        input.push_back(prse4mx);
        input.push_back(prsm);
        input.push_back(spdm);
               
        double valueH = -1e10;
        valueH = m_reader1->GetMvaValue(input);


        return valueH;
}



void NeutralIDTool::Variables(const LHCb::CaloHypo* hypo,  double& clmatch, double& prse, 
                             double& e19, double& hclecl, double& prse19,double& prse49, double& sprd, double& prse4mx, 
                             double& prsm, double& spdm) {
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug()<<"Inside Variables ------"<<endmsg;

  using namespace CaloDataType;

  // load the estimator tool only when not passed in argument (to avoid circular initialization with the estimator itself)
  if( m_estimator == NULL)m_estimator = tool<ICaloHypoEstimator>("CaloHypoEstimator","CaloHypoEstimatorForNeutralID",this);

  clmatch =   m_estimator->data(hypo ,  ClusterMatch ,0.);
  prse    =   m_estimator->data(hypo ,  ToPrsE ,0.);
  e19     =   m_estimator->data(hypo ,  E19 ,0.);
  hclecl  =   m_estimator->data(hypo ,  Hcal2Ecal ,0.);
  prse19  =   m_estimator->data(hypo ,  PrsE19 ,0.);
  prse49  =   m_estimator->data(hypo ,  PrsE49 ,0.);
  sprd    =   m_estimator->data(hypo ,  Spread ,0.);
  prse4mx =   m_estimator->data(hypo ,  PrsE4Max ,0.);
  prsm    =   m_estimator->data(hypo ,  HypoPrsM ,0.);
  spdm    =   m_estimator->data(hypo ,  HypoSpdM ,0.);

}

