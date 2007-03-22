// $id:  $

// // from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from Event
#include "Event/CaloDigit.h"

// from Det
#include "CaloDet/DeCalorimeter.h"

// from CaloUtils
#include "CaloUtils/CellNeighbour.h"

//local
#include "PrepareElectronSeed.h"

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
  
 /*
   *  Resolution of the L0Cand at T3, tune JA 2007-03-22
   *  3 regions:  sigmaX:  3 / 5.5 / 15 mm
   *              sigmaY:  2 / 3.5 /  8 mm
   *              sigmaTx: 1.5 / 2 / 3 mrad
   *              sigmaTy: 1.5 / 2.5 / 3 mrad
   */
  sigmaX2[0]=9.; sigmaX2[1]=30.25; sigmaX2[2]=225.; 
  sigmaY2[0]=4.; sigmaY2[1]=12.25; sigmaY2[2]=64.; 
  sigmaTx2[0]=2.25e-6; sigmaTx2[1]=4.e-6; sigmaTx2[2]=9.e-6;  
  sigmaTy2[0]=2.25e-6; sigmaTy2[1]=6.25e-6; sigmaTy2[2]=9.e-6; 
  
  // cell sizes for ECal IP / MP / OP
  cellSize[0]=40.5567; cellSize[1]=60.85; cellSize[2]=121.7; 
  
  //set parameters for S-shape correction
  parIP[0]=14.;  parIP[1]=5.75;
  parMP[0]=20.9; parMP[1]=7.;
  parOP[0]=42.4; parOP[1]=11.3;
  
  //double sParIP[2], sParMP[2], sParOP[2];
  //calculate these once ..
  sParIP[0] = tan(  cellSize[0]/2. / parIP[0] );
  sParIP[1] = tan( -cellSize[0]/2. / parIP[0] );
  sParMP[0] = tan(  cellSize[1]/2. / parMP[0] );
  sParMP[1] = tan( -cellSize[1]/2. / parMP[0] );
  sParOP[0] = tan(  cellSize[2]/2. / parOP[0] );
  sParOP[1] = tan( -cellSize[2]/2. / parOP[0] );



  return StatusCode::SUCCESS;
}

StatusCode PrepareElectronSeed::prepareSeed( const LHCb::L0CaloCandidate& eL0Cand,
                                             LHCb::State& seedStatePos,
                                             LHCb::State& seedStateNeg )
{

  if(m_debugInfo) debug()<<"PrepareElectronSeed::execute!"<<endmsg;

    int ecalRegion = 0;
  
  
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
  if( e_s1!=0 && e_s2!=0 )      candx=(e_x1+e_x2)/2.;
  else if( e_s1!=0 && e_s4!=0 ) candx=(e_x1+e_x4)/2.;
  else if( e_s3!=0 && e_s2!=0 ) candx=(e_x3+e_x2)/2.;
  else if( e_s3!=0 && e_s4!=0 ) candx=(e_x3+e_x4)/2.;

  double candy = 0.;
  if( e_s1!=0 && e_s3!=0 )      candy=(e_y1+e_y3)/2.;
  else if( e_s1!=0 && e_s4!=0 ) candy=(e_y1+e_y4)/2.;
  else if( e_s2!=0 && e_s3!=0 ) candy=(e_y2+e_y3)/2.;
  else if( e_s2!=0 && e_s4!=0 ) candy=(e_y2+e_y4)/2.;
  
  
  double barx = (e_x1*e_e1+e_x2*e_e2+e_x3*e_e3+e_x4*e_e4)/(e_e1+e_e2+e_e3+e_e4);
  double bary = (e_y1*e_e1+e_y2*e_e2+e_y3*e_e3+e_y4*e_e4)/(e_e1+e_e2+e_e3+e_e4);

  double barx_cor = 0.;
  double bary_cor = 0.;
  
  // apply corrections
  if( fabs(cellSize[0]-e_s1) < 1 ){
    ecalRegion=0;
    if     ( barx-candx >  cellSize[0]/2. ) barx_cor = candx + parIP[1] * sParIP[0];
    else if( barx-candx < -cellSize[0]/2. ) barx_cor = candx + parIP[1] * sParIP[1];
    else                            barx_cor = candx + parIP[1]*tan( (barx-candx) / parIP[0] );     
    
    if     ( bary-candy >  cellSize[0]/2. ) bary_cor = candy + parIP[1] * sParIP[0]; 
    else if( bary-candy < -cellSize[0]/2. ) bary_cor = candy + parIP[1] * sParIP[1]; 
    else                            bary_cor = candy + parIP[1]*tan( (bary-candy) / parIP[0] );     
  }
  
  if( fabs(cellSize[1]-e_s1) < 1 ){
    ecalRegion=1;
    if     ( barx-candx >  cellSize[1]/2. ) barx_cor = candx + parMP[1] * sParMP[0]; 
    else if( barx-candx < -cellSize[1]/2. ) barx_cor = candx + parMP[1] * sParMP[1]; 
    else                            barx_cor = candx + parMP[1]*tan( (barx-candx) / parMP[0] );     
  
    if     ( bary-candy >  cellSize[1]/2. ) bary_cor = candy + parMP[1] * sParMP[0]; 
    else if( bary-candy < -cellSize[1]/2. ) bary_cor = candy + parMP[1] * sParMP[1]; 
    else                            bary_cor = candy + parMP[1]*tan( (bary-candy) / parMP[0] ); 
  }
  
  if( fabs(cellSize[2]-e_s1) < 1 ){
    ecalRegion=2;
    if     ( barx-candx >  cellSize[2]/2. ) barx_cor = candx + parOP[1] * sParOP[0]; 
    else if( barx-candx < -cellSize[2]/2. ) barx_cor = candx + parOP[1] * sParOP[1]; 
    else                            barx_cor = candx + parOP[1]*tan( (barx-candx) / parOP[0] );     
    if     ( bary-candy >  cellSize[2]/2. ) bary_cor = candy + parOP[1] * sParOP[0]; 
    else if( bary-candy < -cellSize[2]/2. ) bary_cor = candy + parOP[1] * sParOP[1]; 
    else                            bary_cor = candy + parOP[1]*tan( (bary-candy) / parOP[0] ); 
  }
  
  if(m_debugInfo){
    verbose() << endmsg;
    verbose() << format(" cluster part  = %1i",ecalRegion) << endmsg;
    verbose() << format(" electron cand x    = %4.2f         cand y = %4.2f",candx,candy) << endmsg;
    verbose() << format(" electron bar  x    = %4.2f         bar  y = %4.2f",barx,bary) << endmsg;
    verbose() << format("    barx-candx      = %4.2f     bary-candy = %4.2f",barx-candx,bary-candy) << endmsg;
    verbose() << endmsg;
    verbose() << format(" electron bar COR x = %4.2f      bar y COR = %4.2f",barx_cor,bary_cor) << endmsg;
    verbose() << endmsg;
  }
  
    
  //========================================================================================================


  double zT3 = 9315.;
  double zcluster = 12700.;
  double alpha = 1536443.236032;
    
  double ecluster = e_e1+e_e2+e_e3+e_e4;

    
  double statePos_x  = barx_cor * zT3 / zcluster - alpha/ecluster;
  double statePos_y  = bary_cor * zT3 / zcluster;
  double statePos_tx = (barx_cor-statePos_x)/(zcluster-zT3);
  double statePos_ty = (bary_cor-statePos_y)/(zcluster-zT3);
  double statePos_qOverp = 1./ecluster;
  
  double stateNeg_x  = barx_cor * zT3 / zcluster + alpha/ecluster;
  double stateNeg_tx = (barx_cor-stateNeg_x)/(zcluster-zT3);

  if(m_debugInfo){
    verbose() << endmsg;
    verbose() << format(" ecluster = %4.1f GeV",ecluster)  << endmsg;
    verbose() << format(" State p : X = %4.1f  Y = %4.1f  TX = %4.3f  TY = %4.3f  Q/P = %4.3f",
                    statePos_x,statePos_y,statePos_tx,statePos_ty,statePos_qOverp )
              << endmsg;
    verbose() << format(" State m : X = %4.1f  Y = %4.1f  TX = %4.3f  TY = %4.3f  Q/P = %4.3f",
                        stateNeg_x,statePos_y,stateNeg_tx,statePos_ty,-statePos_qOverp )
              << endmsg; 
    verbose() << endmsg;
  }
  
  // set the states
  seedStatePos.setState( statePos_x , statePos_y , zT3 ,
                         statePos_tx , statePos_ty , statePos_qOverp );
  seedStateNeg.setState( stateNeg_x , statePos_y , zT3 , 
                         stateNeg_tx , statePos_ty , -statePos_qOverp );
  
  // set the covariance matrix
  Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
  stateCov(0,0) = sigmaX2[ecalRegion]; 
  stateCov(1,1) = sigmaY2[ecalRegion];
  stateCov(2,2) = sigmaTx2[ecalRegion];
  stateCov(3,3) = sigmaTy2[ecalRegion];
  stateCov(4,4) = 8.41e-6;


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
