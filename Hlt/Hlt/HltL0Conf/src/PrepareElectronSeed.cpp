// $id:  $

// // from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// from Event
#include "Event/CaloDigit.h"

// from Det
#include "CaloDet/DeCalorimeter.h"

// from CaloUtils
#include "CaloUtils/CellNeighbour.h"

//local
#include "PrepareElectronSeed.h"

// Declaration of the Tool Factory
// static const  ToolFactory<PrepareElectronSeed>          s_factory ;
// const        IToolFactory& PrepareElectronSeedFactory = s_factory ;


DECLARE_TOOL_FACTORY( PrepareElectronSeed );


// Standard Constructor
PrepareElectronSeed::PrepareElectronSeed(const std::string& type, 
                                         const std::string& name, 
                                         const IInterface* parent) 
  : GaudiTool( type, name, parent )
    
{
  // Declare the algorithm's properties which can be set at run time
    declareInterface<IPrepareCaloSeed>(this);

  
}

// Standard Destructor -------------------------------------
PrepareElectronSeed::~PrepareElectronSeed() {}

//-------------------------------------
StatusCode PrepareElectronSeed::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return sc;
  }
  // always() << " Initialize PrepareElectronSeed" << endmsg;

// Retrieve the ECAL detector element
  m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  
  // set det for calo tool
  m_neighbour.setDet ( m_ecal );

  m_debugInfo = false;//@ja put in job options
  

  return StatusCode::SUCCESS;
}

StatusCode PrepareElectronSeed::prepareSeed( const LHCb::L0CaloCandidate& eL0Cand,
                                             LHCb::State& seedStatePos,
                                             LHCb::State& seedStateNeg )
{

  if(m_debugInfo) debug()<<"PrepareElectronSeed::execute!"<<endmsg;
  
  //===================================================================================================
  // electrons items
  e_et=0.;
  e_x1=0.;e_x2=0.;e_x3=0.;e_x4=0.;
  e_y1=0.;e_y2=0.;e_y3=0.;e_y4=0.;
  e_e1=0.;e_e2=0.;e_e3=0.;e_e4=0.;
  e_s1=0.;e_s2=0.;e_s3=0.;e_s4=0.;
  //===================================================================================================

//===================================================================================================
  // get the calodigits 
  const LHCb::CaloDigits* calodigits =get<LHCb::CaloDigits>(LHCb::CaloDigitLocation::Ecal);
  if(m_debugInfo){
    verbose() << endmsg;
    verbose() << " n digits ( ECAL) = " << calodigits->size() << endmsg;
    verbose() << endmsg;
  }
  
  // get candidates cells x, y, energy and size for electron candidate
  StatusCode sc = SetCandPosAndE( eL0Cand , calodigits ,
                                  e_x1, e_x2, e_x3, e_x4,
                                  e_y1, e_y2, e_y3, e_y4,
                                  e_e1, e_e2, e_e3, e_e4,
                                  e_s1, e_s2, e_s3, e_s4 );
  
  if( sc.isFailure() ) verbose() << " didn't manage to set cand positions and energy..." << endmsg;
  
  if(m_debugInfo) {
    verbose() << endmsg;
    verbose() << format(" electron s1 = %8.1f  s2 = %8.1f  s3 = %8.1f  s4 = %8.1f ",e_s1,e_s2,
                    e_s3,e_s4 )  << endmsg;
    verbose() << format(" electron e1 = %8.1f  e2 = %8.1f  e3 = %8.1f  e4 = %8.1f ",e_e1,e_e2,
                        e_e3,e_e4 )  << endmsg;
    verbose() << format(" electron x1 = %8.1f  x2 = %8.1f  x3 = %8.1f  x4 = %8.1f ",e_x1,e_x2,
                    e_x3,e_x4 )  << endmsg;
    verbose() << format(" electron y1 = %8.1f  y2 = %8.1f  y3 = %8.1f  y4 = %8.1f ",e_y1,e_y2,
                        e_y3,e_y4 )  << endmsg;
    verbose() << endmsg;
  }
  

//========================================================================================================
  // now correct barycenter position 
  double candx = 0.;
  if( e_s1!=0 && e_s2!=0 )      candx=(e_x1+e_x2)/2;
  else if( e_s1!=0 && e_s4!=0 ) candx=(e_x1+e_x4)/2;
  else if( e_s3!=0 && e_s2!=0 ) candx=(e_x3+e_x2)/2;
  else if( e_s3!=0 && e_s4!=0 ) candx=(e_x3+e_x4)/2;

  double candy = 0.;
  if( e_s1!=0 && e_s3!=0 )      candy=(e_y1+e_y3)/2;
  else if( e_s1!=0 && e_s4!=0 ) candy=(e_y1+e_y4)/2;
  else if( e_s2!=0 && e_s3!=0 ) candy=(e_y2+e_y3)/2;
  else if( e_s2!=0 && e_s4!=0 ) candy=(e_y2+e_y4)/2;
  
  
  double barx = (e_x1*e_e1+e_x2*e_e2+e_x3*e_e3+e_x4*e_e4)/(e_e1+e_e2+e_e3+e_e4);
  double bary = (e_y1*e_e1+e_y2*e_e2+e_y3*e_e3+e_y4*e_e4)/(e_e1+e_e2+e_e3+e_e4);

  double barx_cor = 0.;
  double bary_cor = 0.;
  
 //  X : IP A = 14.0865  B = 5.7185 
//  X : MP A = 20.9507  B = 7.1310 
//  X : OP A = 42.9022  B = 12.0481 

//  Y : IP A = 14.2897  B = 6.0943 
//  Y : MP A = 20.8994  B = 6.8889 
//  Y : OP A = 42.0349  B = 10.6772
  
  double par0_IP = 14.00;double par1_IP =  5.75;
  double par0_MP = 20.90;double par1_MP =  7.00;
  double par0_OP = 42.40;double par1_OP = 11.30;


 
  // cell sizes
  double csIP = 40.5567;
  double csMP = 60.85;
  double csOP = 121.7;
  
  double candEcalPart = 0.;

  // apply corrections
  if( fabs(csIP-e_s1) < 1 ){
    candEcalPart=1.;
    if     ( barx-candx >  csIP/2 ) barx_cor = candx + par1_IP*tan(  csIP/2 / par0_IP );     
    else if( barx-candx < -csIP/2 ) barx_cor = candx + par1_IP*tan( -csIP/2 / par0_IP );     
    else                            barx_cor = candx + par1_IP*tan( (barx-candx) / par0_IP );     
    if     ( bary-candy >  csIP/2 ) bary_cor = candy + par1_IP*tan(  csIP/2 / par0_IP );     
    else if( bary-candy < -csIP/2 ) bary_cor = candy + par1_IP*tan( -csIP/2 / par0_IP );     
    else                            bary_cor = candy + par1_IP*tan( (bary-candy) / par0_IP );     
  }
  
  if( fabs(csMP-e_s1) < 1 ){
    candEcalPart=2.;
    if     ( barx-candx >  csMP/2 ) barx_cor = candx + par1_MP*tan(  csMP/2 / par0_MP );     
    else if( barx-candx < -csMP/2 ) barx_cor = candx + par1_MP*tan( -csMP/2 / par0_MP );     
    else                            barx_cor = candx + par1_MP*tan( (barx-candx) / par0_MP );     
    if     ( bary-candy >  csMP/2 ) bary_cor = candy + par1_MP*tan(  csMP/2 / par0_MP );     
    else if( bary-candy < -csMP/2 ) bary_cor = candy + par1_MP*tan( -csMP/2 / par0_MP );     
    else                            bary_cor = candy + par1_MP*tan( (bary-candy) / par0_MP ); 
  }
  
  if( fabs(csOP-e_s1) < 1 ){
    candEcalPart=3.;
    if     ( barx-candx >  csOP/2 ) barx_cor = candx + par1_OP*tan(  csOP/2 / par0_OP );     
    else if( barx-candx < -csOP/2 ) barx_cor = candx + par1_OP*tan( -csOP/2 / par0_OP );     
    else                            barx_cor = candx + par1_OP*tan( (barx-candx) / par0_OP );     
    if     ( bary-candy >  csOP/2 ) bary_cor = candy + par1_OP*tan(  csOP/2 / par0_OP );     
    else if( bary-candy < -csOP/2 ) bary_cor = candy + par1_OP*tan( -csOP/2 / par0_OP );     
    else                            bary_cor = candy + par1_OP*tan( (bary-candy) / par0_OP ); 
  }
  if(m_debugInfo){
    verbose() << endmsg;
    verbose() << format(" cluster part  = %1i",candEcalPart) << endmsg;
    verbose() << format(" electron cand x    = %4.2f         cand y = %4.2f",candx,candy) << endmsg;
    verbose() << format(" electron bar  x    = %4.2f         bar  y = %4.2f",barx,bary) << endmsg;
    verbose() << format("    barx-candx      = %4.2f     bary-candy = %4.2f",barx-candx,bary-candy) << endmsg;
    verbose() << endmsg;
    verbose() << format(" electron bar COR x = %4.2f      bar y COR = %4.2f",barx_cor,bary_cor) << endmsg;
    verbose() << endmsg;
  }
  
    
  //========================================================================================================


  double zT3 = 9315.;
  double alpha = 1536443.236032;
    
  double ecluster = e_e1+e_e2+e_e3+e_e4;
  double zcluster = 12700.;
    
  double elec_state_p_x  = barx_cor * zT3 / zcluster - alpha/ecluster;
  double elec_state_p_y  = bary_cor * zT3 / zcluster;
  double elec_state_p_tx = (barx_cor-elec_state_p_x)/(zcluster-zT3);
  double elec_state_p_ty = (bary_cor-elec_state_p_y)/(zcluster-zT3);
  double elec_state_p_qp = 1/ecluster;
  
  double elec_state_m_x  = barx_cor * zT3 / zcluster + alpha/ecluster;
  double elec_state_m_tx = (barx_cor-elec_state_m_x)/(zcluster-zT3);

  if(m_debugInfo){
    verbose() << endmsg;
    verbose() << format(" ecluster = %4.1f GeV",ecluster)  << endmsg;
    verbose() << format(" State p : X = %4.1f  Y = %4.1f  TX = %4.3f  TY = %4.3f  Q/P = %4.3f",
                    elec_state_p_x,elec_state_p_y,elec_state_p_tx,elec_state_p_ty,elec_state_p_qp )
              << endmsg;
    verbose() << format(" State m : X = %4.1f  Y = %4.1f  TX = %4.3f  TY = %4.3f  Q/P = %4.3f",
                        elec_state_m_x,elec_state_p_y,elec_state_m_tx,elec_state_p_ty,-elec_state_p_qp )
              << endmsg; 
    verbose() << endmsg;
  }
  
  // set the states
  seedStatePos.setState(elec_state_p_x,elec_state_p_y,zT3,elec_state_p_tx,elec_state_p_ty, elec_state_p_qp);
  seedStateNeg.setState(elec_state_m_x,elec_state_p_y,zT3,elec_state_m_tx,elec_state_p_ty,-elec_state_p_qp);
  
  // set the covariance matrix
  Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
  if( fabs(csIP-e_s1) < 1 ){
    stateCov(0,0) = 15.21; //3.9*3.9    // old value == 12 // sqrt = 3.46
    stateCov(1,1) =  5.29; //2.3*2.3    // 5.5             // sqrt = 2.34
    stateCov(2,2) = 2.8e-3;  
    stateCov(3,3) = 1.5e-3;
    stateCov(4,4) = 8.41e-6;
  }
  
  if( fabs(csMP-e_s1) < 1 ){
    stateCov(0,0) = 28.1;   //5.3*5.3     // 31    // sqrt = 5.57
    stateCov(1,1) = 12.25;  //3.5*3.5     // 12.7  // sqrt = 3.56
    stateCov(2,2) = 2.8e-3;
    stateCov(3,3) = 1.5e-3;
    stateCov(4,4) = 8.41e-6;
  }
  
  if( fabs(csOP-e_s1) < 1 ){
    stateCov(0,0) = 201.6; // 14.2*14.2   // 161  // sqrt = 12.69
    stateCov(1,1) =  86.5; // 9.3*9.3     // 71.5 // sqrt =  8.46
    stateCov(2,2) = 2.8e-3;
    stateCov(3,3) = 1.5e-3;
    stateCov(4,4) = 8.41e-6;
  }
  
  // set state covariance matrix
  seedStatePos.setCovariance(stateCov);
  seedStateNeg.setCovariance(stateCov);
  



  return StatusCode::SUCCESS;
  
}


//=============================================================================================
//  This method returns the L0CaloCand cells pos and energy given the cand and the calodigits
//=============================================================================================
StatusCode PrepareElectronSeed::SetCandPosAndE(const LHCb::L0CaloCandidate& cand,
                                               const LHCb::CaloDigits* calodig,
                                               double& x1, double& x2, double& x3, double& x4,
                                               double& y1, double& y2, double& y3, double& y4,
                                               double& e1, double& e2, double& e3, double& e4,
                                               double& s1, double& s2, double& s3, double& s4 ){
  if(m_debugInfo)
    verbose() << " ElectronCollectTsaCluster::SetCandPosAndE is running" << endmsg;
  
  
  // get the candcell ID
  const LHCb::CaloCellID& candcellid = cand.id();
  
  // first test that the candidate cell id is valid...
  if( m_ecal->valid(candcellid) ){
    
    // loop on calodigits
    LHCb::CaloDigits::const_iterator id;
    for( id=calodig->begin(); calodig->end()!=id; ++id ){
      
      // get cell ID for this digit
      const LHCb::CaloCellID& cellid = (*id)->cellID();
      
      // test cellID is valid and set s,x,y and e for the candidate
      if( m_ecal->valid(cellid) ){
        
        if( candcellid.area() == cellid.area() && 
            candcellid.col()  == cellid.col() && 
            candcellid.row()  == cellid.row() ){
          //  verbose() << " cell1 = " << cellid << endmsg;
          x1 = m_ecal->cellX(candcellid);
          y1 = m_ecal->cellY(candcellid);
          e1 = (*id)->e();
          s1 = m_ecal->cellSize(candcellid);
        }
        
        
        // test if this digit is a neighbour of the cand one with calo tool
        if( m_neighbour(candcellid,cellid) ){
          
          if( candcellid.area()  == cellid.area() && 
              candcellid.col()+1 == cellid.col()  && 
              candcellid.row()   == cellid.row() ){
            //            verbose() << " cell2 = " << cellid << endmsg;
            x2 = m_ecal->cellX(cellid);
            y2 = m_ecal->cellY(cellid);
            e2 = (*id)->e();
            s2 = m_ecal->cellSize(cellid);
          }
          
          if( candcellid.area()  == cellid.area() && 
              candcellid.col()   == cellid.col()  && 
              candcellid.row()+1 == cellid.row() ){
            //  verbose() << " cell3 = " << cellid << endmsg;
            x3 = m_ecal->cellX(cellid);
            y3 = m_ecal->cellY(cellid);
            e3 = (*id)->e();
            s3 = m_ecal->cellSize(cellid);
          }
          
          if( candcellid.area()  == cellid.area() && 
              candcellid.col()+1 == cellid.col()  && 
              candcellid.row()+1 == cellid.row() ){
            //  verbose() << " cell4 = " << cellid << endmsg;
            x4 = m_ecal->cellX(cellid);
            y4 = m_ecal->cellY(cellid);
            e4 = (*id)->e();
            s4 = m_ecal->cellSize(cellid);
          }
          
        }// if cells are neighbors    
      }// valid cell id
    }// loop over digits
  }// cand cell id is valid

      
  return StatusCode::SUCCESS;
  
}
