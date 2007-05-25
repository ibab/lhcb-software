// $id:  $

// // from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from Event
#include "Event/L0CaloCandidate.h"
#include "Event/CaloDigit.h"
#include "Event/State.h"

// from Det
#include "CaloDet/DeCalorimeter.h"

// from CaloUtils
#include "CaloUtils/CellNeighbour.h"


//local
#include "PrepareHadronSeed.h"

// Declaration of the Tool Factory
// static const  ToolFactory<PrepareHadronSeed>          s_factory ;
// const        IToolFactory& PrepareHadronSeedFactory = s_factory ;

DECLARE_TOOL_FACTORY( PrepareHadronSeed );

// Standard Constructor
PrepareHadronSeed::PrepareHadronSeed(const std::string& type, 
                                         const std::string& name, 
                                         const IInterface* parent) 
  : GaudiTool( type, name, parent )
    
{
  // Declare the algorithm's properties which can be set at run time
    declareInterface<IPrepareCaloSeed>(this);

  
}

// Standard Destructor -------------------------------------
PrepareHadronSeed::~PrepareHadronSeed() {}

//-------------------------------------
StatusCode PrepareHadronSeed::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return sc;
  }
  //always() << " Initialize PrepareHadronSeed" << endmsg;

  // Retrieve the ECAL detector element
  m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  // set det for calo tool
  m_neighbour_ecal.setDet ( m_ecal );
  
  // Retrieve the HCAL detector element
  m_hcal = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
  // set det for calo tool
  m_neighbour_hcal.setDet ( m_hcal );
  
  m_debugInfo = false;//@ja make this to job option

  /*
   *  Resolution of the L0Cand at T3, 
   *  Preliminary numbers! (RMS)
   * if enough energy in ECal use this
   *  ECal 3 regions:  sigmaX:  32 / 23 / 41 mm
   *                   sigmaY:  27 / 17 / 22 mm
   *                   sigmaTx: 22 / 27 / 50 mrad 
   *                   sigmaTy:  8 / 17 / 48 mrad
   */
  sigmaX2[0]=1024.; sigmaX2[1]=529.; sigmaX2[2]=1681.; 
  sigmaY2[0]=729.; sigmaY2[1]=289.; sigmaY2[2]=484.; 
  sigmaTx2[0]=484.e-6; sigmaTx2[1]=729.e-6; sigmaTx2[2]=2500.e-6;  
  sigmaTy2[0]=64.e-6; sigmaTy2[1]=289.e-6; sigmaTy2[2]=2304.e-6; 

  /*
   *  if not enough energy in ECal only HCal 
   *  2 regions: sigmaX: 32 / 86 mm
   *             sigmaY: 25 / 47 mm
   *             sigmaTx: 29 / 70 mrad
   *             sigmaTy: 11 / 69 mrad
   */
  sigmaX2[3]=1024.; sigmaX2[4]=7396.; 
  sigmaY2[3]=625.;  sigmaY2[4]=2209.; 
  sigmaTx2[3]=841.e-6; sigmaTx2[4]=4900.e-6; 
  sigmaTy2[3]=121.e-6; sigmaTy2[4]=4761.e-6; 

  
  return StatusCode::SUCCESS;
}

StatusCode PrepareHadronSeed::prepareSeed(const LHCb::L0CaloCandidate& hadL0Cand,
                                          LHCb::State& seedStatePos,
                                          LHCb::State& seedStateNeg )
{

  if(m_debugInfo) debug()<<"PrepareHadronSeed::execute!"<<endmsg;

  //===================================================================================================
  // electrons items
  had_et=0.;
  had_x1=0.;had_x2=0.;had_x3=0.;had_x4=0.;
  had_y1=0.;had_y2=0.;had_y3=0.;had_y4=0.;
  had_e1=0.;had_e2=0.;had_e3=0.;had_e4=0.;
  had_s1=0.;had_s2=0.;had_s3=0.;had_s4=0.;
  //===================================================================================================
//===================================================================================================
  // get the ECAL and HCAL digits 
  const LHCb::CaloDigits* ecaldigits =get<LHCb::CaloDigits>(LHCb::CaloDigitLocation::Ecal);
  const LHCb::CaloDigits* hcaldigits =get<LHCb::CaloDigits>(LHCb::CaloDigitLocation::Hcal);

  if(m_debugInfo){
    verbose() << endmsg;
    verbose() << " n digits (ECAL) = " << ecaldigits->size() << endmsg;
    verbose() << " n digits (HCAL) = " << hcaldigits->size() << endmsg;
    verbose() << endmsg;
  }
  
  // get candidates cells x, y, energy and size for electron candidate
  StatusCode sc = SetCandPosAndE( hadL0Cand , hcaldigits ,
                                  had_x1, had_x2, had_x3, had_x4,
                                  had_y1, had_y2, had_y3, had_y4,
                                  had_e1, had_e2, had_e3, had_e4,
                                  had_s1, had_s2, had_s3, had_s4 );
  if( sc.isFailure() ) verbose() << " didn't manage to set cand positions and energy..." << endmsg;
  
  if(m_debugInfo){
    verbose() << endmsg;
    verbose() << format(" hadron s1 = %8.1f  s2 = %8.1f  s3 = %8.1f  s4 = %8.1f ",had_s1,had_s2,
                        had_s3,had_s4 )  << endmsg;
    verbose() << format(" hadron e1 = %8.1f  e2 = %8.1f  e3 = %8.1f  e4 = %8.1f ",had_e1,had_e2,
                        had_e3,had_e4 )  << endmsg;
    verbose() << format(" hadron x1 = %8.1f  x2 = %8.1f  x3 = %8.1f  x4 = %8.1f ",had_x1,had_x2,
                        had_x3,had_x4 )  << endmsg;
    verbose() << format(" hadron y1 = %8.1f  y2 = %8.1f  y3 = %8.1f  y4 = %8.1f ",had_y1,had_y2,
                        had_y3,had_y4 )  << endmsg;
    verbose() << endmsg;
  }
  
    //===================================================================================================

  
  //========================================================================================================
  // now compute barycenter position and correct it
  float csIPH = 131.3;
  float csOPH = 262.6;
  float csIPE = 40.5567;
  float csMPE = 60.85;
  float csOPE = 121.7;
  
  int hcalpart=0;
  // determine in which part of HCAL the cand comes from
  if     ( 0!=had_s1 && fabs(had_s1-csIPH)<1 ) hcalpart=1;
  else if( 0!=had_s2 && fabs(had_s2-csIPH)<1 ) hcalpart=1;
  else if( 0!=had_s3 && fabs(had_s3-csIPH)<1 ) hcalpart=1;
  else if( 0!=had_s4 && fabs(had_s4-csIPH)<1 ) hcalpart=1;
  
  if     ( 0!=had_s1 && fabs(had_s1-csOPH)<1 ) hcalpart=2;
  else if( 0!=had_s2 && fabs(had_s2-csOPH)<1 ) hcalpart=2;
  else if( 0!=had_s3 && fabs(had_s3-csOPH)<1 ) hcalpart=2;
  else if( 0!=had_s4 && fabs(had_s4-csOPH)<1 ) hcalpart=2;
    
  float EHCALCLU = had_e1+had_e2+had_e3+had_e4;
  float XHCALBAR = (had_x1*had_e1+had_x2*had_e2+had_x3*had_e3+had_x4*had_e4)/EHCALCLU;
  float YHCALBAR = (had_y1*had_e1+had_y2*had_e2+had_y3*had_e3+had_y4*had_e4)/EHCALCLU;
  float XHCALCLU = 0.;
  float YHCALCLU = 0.;
  if     ( 0!=had_s1 ){ XHCALCLU = had_x1+had_s1/2.; YHCALCLU = had_y1+had_s1/2.; }
  else if( 0!=had_s2 ){ XHCALCLU = had_x2-had_s2/2.; YHCALCLU = had_y2+had_s2/2.; }
  else if( 0!=had_s3 ){ XHCALCLU = had_x3+had_s3/2.; YHCALCLU = had_y3-had_s3/2.; }
  else if( 0!=had_s4 ){ XHCALCLU = had_x4-had_s4/2.; YHCALCLU = had_y4-had_s4/2.; }
  
  double Hpar0_IP = 66.56;double Hpar1_IP = 51.01;
  double Hpar0_OP = 141.6;double Hpar1_OP = 96.76;
  
  float XHCALCOR = 0.;
  float YHCALCOR = 0.;
  if( 1==hcalpart ){
    XHCALCOR = XHCALCLU  + Hpar1_IP*tan( (XHCALBAR-XHCALCLU)/Hpar0_IP );
    YHCALCOR = YHCALCLU  + Hpar1_IP*tan( (YHCALBAR-YHCALCLU)/Hpar0_IP );
  }
  if( 2==hcalpart ){
    XHCALCOR = XHCALCLU  + Hpar1_OP*tan( (XHCALBAR-XHCALCLU)/Hpar0_OP );
    YHCALCOR = YHCALCLU  + Hpar1_OP*tan( (YHCALBAR-YHCALCLU)/Hpar0_OP );
  }

  // now try to get ECAL cells
  int necalcellused = 0;
  
  // now we have the position in HCAL extrapolate it to ECAL
  float XECAL = XHCALCOR*12830/13690;
  float YECAL = YHCALCOR*12830/13690;
  int ECALPART = ECALpart(XECAL,YECAL);
  
  float EECALCLU = 0.;
  float XECALBAR = 0.;
  float YECALBAR = 0.;
  float XECALMAX = -100000.;float XECALMIN = 100000.;
  float YECALMAX = -100000.;float YECALMIN = 100000.;
    
  // now get the ECAL cells around this point
  LHCb::CaloDigits::const_iterator id;
  for( id=ecaldigits->begin(); ecaldigits->end()!=id; ++id ){
    
    const LHCb::CaloCellID& cellid = (*id)->cellID();
    if( m_ecal->valid(cellid) ){
      if( 0<(*id)->e() ){
        
        if( 1==ECALPART && fabs(m_ecal->cellX(cellid)-XECAL)<csIPE && 
            fabs(m_ecal->cellY(cellid)-YECAL)<csIPE &&
            fabs(m_ecal->cellSize(cellid)-csIPE)<1 ){
          necalcellused++;
          EECALCLU+=(*id)->e();
          XECALBAR+=m_ecal->cellX(cellid)*(*id)->e();
          YECALBAR+=m_ecal->cellY(cellid)*(*id)->e();
          if( m_ecal->cellX(cellid) > XECALMAX ) XECALMAX = m_ecal->cellX(cellid);
          if( m_ecal->cellX(cellid) < XECALMIN ) XECALMIN = m_ecal->cellX(cellid);
          if( m_ecal->cellY(cellid) > YECALMAX ) YECALMAX = m_ecal->cellY(cellid);
          if( m_ecal->cellY(cellid) < YECALMIN ) YECALMIN = m_ecal->cellY(cellid);
        }
        
        if( 2==ECALPART && fabs(m_ecal->cellX(cellid)-XECAL)<csMPE && 
            fabs(m_ecal->cellY(cellid)-YECAL)<csMPE &&
            fabs(m_ecal->cellSize(cellid)-csMPE)<1 ){
          necalcellused++;
          EECALCLU+=(*id)->e();
          XECALBAR+=m_ecal->cellX(cellid)*(*id)->e();
          YECALBAR+=m_ecal->cellY(cellid)*(*id)->e();
          if( m_ecal->cellX(cellid) > XECALMAX ) XECALMAX = m_ecal->cellX(cellid);
          if( m_ecal->cellX(cellid) < XECALMIN ) XECALMIN = m_ecal->cellX(cellid);
          if( m_ecal->cellY(cellid) > YECALMAX ) YECALMAX = m_ecal->cellY(cellid);
          if( m_ecal->cellY(cellid) < YECALMIN ) YECALMIN = m_ecal->cellY(cellid);
        }
        
        if( 3==ECALPART && fabs(m_ecal->cellX(cellid)-XECAL)<csOPE && 
            fabs(m_ecal->cellY(cellid)-YECAL)<csOPE &&
            fabs(m_ecal->cellSize(cellid)-csOPE)<1 ){
          necalcellused++;
          EECALCLU+=(*id)->e();
          XECALBAR+=m_ecal->cellX(cellid)*(*id)->e();
          YECALBAR+=m_ecal->cellY(cellid)*(*id)->e();
          if( m_ecal->cellX(cellid) > XECALMAX ) XECALMAX = m_ecal->cellX(cellid);
          if( m_ecal->cellX(cellid) < XECALMIN ) XECALMIN = m_ecal->cellX(cellid);
          if( m_ecal->cellY(cellid) > YECALMAX ) YECALMAX = m_ecal->cellY(cellid);
          if( m_ecal->cellY(cellid) < YECALMIN ) YECALMIN = m_ecal->cellY(cellid);
        }  
      }// e>0
    }// valid cell
  }// loop over ecal digits

  float XECALCOR = 0.;
  float YECALCOR = 0.;
  float XECALCLU = 0.;
  float YECALCLU = 0.;
  int ecalpart = 0;
  if( 2<necalcellused ){
    
    XECALBAR=XECALBAR/EECALCLU;
    YECALBAR=YECALBAR/EECALCLU;
    
    ecalpart = ECALpart( XECALBAR , YECALBAR );
      
    if( XECALMAX!=XECALMIN ){
      XECALCLU=(XECALMAX+XECALMIN)/2.;
    }
    else{
      if     ( 1==ecalpart ){
        if     ( XECALMIN>XECAL ) XECALCLU=XECALMIN-csIPE/2.;
        else if( XECALMIN<XECAL ) XECALCLU=XECALMIN+csIPE/2.;
      }
      else if( 2==ecalpart ){
        if     ( XECALMIN>XECAL ) XECALCLU=XECALMIN-csMPE/2.;
        else if( XECALMIN<XECAL ) XECALCLU=XECALMIN+csMPE/2.;
      }
      else if( 3==ecalpart ){
        if     ( XECALMIN>XECAL ) XECALCLU=XECALMIN-csOPE/2.;
        else if( XECALMIN<XECAL ) XECALCLU=XECALMIN+csOPE/2.;
      }
    }// else

    if( YECALMAX!=YECALMIN ) YECALCLU=(YECALMAX+YECALMIN)/2.;
    else{
      if     ( 1==ecalpart ){
        if     ( YECALMIN>YECAL ) YECALCLU=YECALMIN-csIPE/2.;
        else if( YECALMIN<YECAL ) YECALCLU=YECALMIN+csIPE/2.;
      }
      else if( 2==ecalpart ){
        if     ( YECALMIN>YECAL ) YECALCLU=YECALMIN-csMPE/2.;
        else if( YECALMIN<YECAL ) YECALCLU=YECALMIN+csMPE/2.;
      }
      else if( 3==ecalpart ){
        if     ( YECALMIN>YECAL ) YECALCLU=YECALMIN-csOPE/2.;
        else if( YECALMIN<YECAL ) YECALCLU=YECALMIN+csOPE/2.;
      }
    }
    
    // correct for position
    double Epar0_IP = 20.65;double Epar1_IP = 15.84;
    double Epar0_MP = 30.15;double Epar1_MP = 20.34;
    double Epar0_OP = 56.82;double Epar1_OP = 35.68;
    
    if( 1==ecalpart ){
      XECALCOR = XECALCLU  + Epar1_IP*tan( (XECALBAR-XECALCLU)/Epar0_IP );
      YECALCOR = YECALCLU  + Epar1_IP*tan( (YECALBAR-YECALCLU)/Epar0_IP );
    }
    if( 2==ecalpart ){
      XECALCOR = XECALCLU  + Epar1_MP*tan( (XECALBAR-XECALCLU)/Epar0_MP );
      YECALCOR = YECALCLU  + Epar1_MP*tan( (YECALBAR-YECALCLU)/Epar0_MP );
    }
    if( 3==ecalpart ){
      XECALCOR = XECALCLU  + Epar1_OP*tan( (XECALBAR-XECALCLU)/Epar0_OP );
      YECALCOR = YECALCLU  + Epar1_OP*tan( (YECALBAR-YECALCLU)/Epar0_OP );
    }
    
  }// necalcellused > 0
  //========================================================================================================
  

  
  double zT3 = 9390.0;
    
  if( 2 < necalcellused ){
    
    double had_state_p_x  = XECALCOR*zT3/12830 - ( 11.5+1.585e6/(EHCALCLU+EECALCLU) );
    double had_state_p_y  = YECALCOR*zT3/12830;
    double had_state_p_tx = (XECALCOR-had_state_p_x)/(12830-zT3);
    double had_state_p_ty = (YECALCOR-had_state_p_y)/(12830-zT3);
    double had_state_p_qp = 1/(EHCALCLU+EECALCLU);
  
    double had_state_m_x  = XECALCOR*zT3/12830 + ( 11.5+1.585e6/(EHCALCLU+EECALCLU) );
    double had_state_m_tx = (XECALCOR-had_state_m_x)/(12830-zT3);
    
    // set the states
    seedStatePos.setState(had_state_p_x,had_state_p_y,zT3,had_state_p_tx,had_state_p_ty, had_state_p_qp);
    seedStateNeg.setState(had_state_m_x,had_state_p_y,zT3,had_state_m_tx,had_state_p_ty,-had_state_p_qp);
  }
  else{ // use only HCAL
    
    double had_state_p_x  = XHCALCOR*zT3/13690 - ( 17 + 1.712e6/EHCALCLU );
    double had_state_p_y  = YHCALCOR*zT3/13690;
    double had_state_p_tx = (XHCALCOR-had_state_p_x)/(13690-zT3);
    double had_state_p_ty = (YHCALCOR-had_state_p_y)/(13690-zT3);
    double had_state_p_qp = 1/(EHCALCLU);
  
    double had_state_m_x  = XHCALCOR*zT3/13690 + ( 17 + 1.712e6/EHCALCLU );
    double had_state_m_tx = (XHCALCOR-had_state_m_x)/(13690-zT3);
    
    // set the states
    seedStatePos.setState(had_state_p_x,had_state_p_y,zT3,had_state_p_tx,had_state_p_ty, had_state_p_qp);
    seedStateNeg.setState(had_state_m_x,had_state_p_y,zT3,had_state_m_tx,had_state_p_ty,-had_state_p_qp);
  }
  
  int region = 0;
  if( 2 < necalcellused ){
    if( 1==ecalpart ) region = 0;
    if( 2==ecalpart ) region = 1;
    if( 3==ecalpart ) region = 2;
  }
  else{
    if( 1==hcalpart ) region = 3;
    if( 2==hcalpart ) region = 4;
  }
  
  // set the covariance matrix
  Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
  stateCov(0,0) = sigmaX2[region];
  stateCov(1,1) = sigmaY2[region];
  stateCov(2,2) = sigmaTx2[region];
  stateCov(3,3) = sigmaTy2[region];
  stateCov(4,4) = 8.41e-6;
      

  // set state covariance matrix
  seedStatePos.setCovariance(stateCov);
  seedStateNeg.setCovariance(stateCov);
 
  verbose() << "Set stateCov \n" << stateCov << endmsg ;
  
  return StatusCode::SUCCESS;
  
}


//=============================================================================================
//  This method returns the L0CaloCand cells pos and energy given the cand and the calodigits
//=============================================================================================
StatusCode PrepareHadronSeed::SetCandPosAndE( const LHCb::L0CaloCandidate& cand,
                                                    const LHCb::CaloDigits* hcaldig,
                                                    double& x1, double& x2, double& x3, double& x4,
                                                    double& y1, double& y2, double& y3, double& y4,
                                                    double& e1, double& e2, double& e3, double& e4,
                                                    double& s1, double& s2, double& s3, double& s4 ){

  if(m_debugInfo) debug() << " HadronSShapeCorrections::SetCandPosAndE begins" << endmsg;
  
  
  int ncellretrieved = 0;

  // get the cand cell ID
  const LHCb::CaloCellID& candcellid = cand.id();
  if(m_debugInfo) debug() << " candcellid calo = " << candcellid.calo() << endmsg;
  
  // first test that the candidate cell id is valid...
  if( m_hcal->valid(candcellid) ){
    
    // loop on calodigits to get neighbor cells
    LHCb::CaloDigits::const_iterator id;
    for( id=hcaldig->begin(); hcaldig->end()!=id; ++id ){
      
      // get cell ID for this digit
      const LHCb::CaloCellID& cellid = (*id)->cellID();
      
      // test cellID is valid and set s,x,y and e for the candidate
      if( m_hcal->valid(cellid) ){

        // get the same cell that the cand
        if( candcellid.area() == cellid.area() && 
            candcellid.col()  == cellid.col()  && 
            candcellid.row()  == cellid.row() ){
          ncellretrieved++;
          if(m_debugInfo) debug() << " cell1 = " << cellid << endmsg;
          x1 = m_hcal->cellX(cellid);
          y1 = m_hcal->cellY(cellid);
          e1 = (*id)->e();
          s1 = m_hcal->cellSize(cellid);
        }
        
        if( candcellid.area()  == cellid.area() && 
            candcellid.col()+1 == cellid.col()  && 
            candcellid.row()   == cellid.row() ){
          ncellretrieved++;
          if(m_debugInfo) debug() << " cell2 = " << cellid << endmsg;
          x2 = m_hcal->cellX(cellid);
          y2 = m_hcal->cellY(cellid);
          e2 = (*id)->e();
          s2 = m_hcal->cellSize(cellid);
        } 
        if( candcellid.area()  == cellid.area() && 
            candcellid.col()   == cellid.col()  && 
            candcellid.row()+1 == cellid.row() ){
          ncellretrieved++;
          if(m_debugInfo) debug() << " cell3 = " << cellid << endmsg;
          x3 = m_hcal->cellX(cellid);
          y3 = m_hcal->cellY(cellid);
          e3 = (*id)->e();
          s3 = m_hcal->cellSize(cellid);
          }
        if( candcellid.area()  == cellid.area() && 
            candcellid.col()+1 == cellid.col()  && 
            candcellid.row()+1 == cellid.row() ){
          if(m_debugInfo) debug() << " cell4 = " << cellid << endmsg;
          ncellretrieved++;
          x4 = m_hcal->cellX(cellid);
          y4 = m_hcal->cellY(cellid);
          e4 = (*id)->e();
          s4 = m_hcal->cellSize(cellid);
        }
        
      }// valid cell id
    }// loop over digits
  }// cand cell id is valid
  else{
    
    if(m_debugInfo) debug() << " cellid is not valid for HCAL !" << endmsg;
    // the candidate cell id is not valid...
    // try with +1 +1   for HCAL, calo==3
    LHCb::CaloCellID tmpid4(3,candcellid.area(),candcellid.row()+1,candcellid.col()+1);
    
    if( m_hcal->valid(tmpid4) ){
      // retrieve digit to get the deposited energy
      const LHCb::CaloDigit* digit = hcaldig->object(tmpid4);
      x4 = m_hcal->cellX(tmpid4);
      y4 = m_hcal->cellY(tmpid4);
      if( digit ) e4 = digit->e();
      s4 = m_hcal->cellSize(tmpid4);  
    }
    
    LHCb::CaloCellID tmpid3(3,candcellid.area(),candcellid.row()+1,candcellid.col());
    if( m_hcal->valid(tmpid3) ){
      // retrieve digit to e
      const LHCb::CaloDigit* digit = hcaldig->object(tmpid3);
      x3 = m_hcal->cellX(tmpid3);
      y3 = m_hcal->cellY(tmpid3);
      if( digit ) e3 = digit->e();
      s3 = m_hcal->cellSize(tmpid3);
    }
    
    LHCb::CaloCellID tmpid2(3,candcellid.area(),candcellid.row(),candcellid.col()+1);
    if( m_hcal->valid(tmpid2) ){
      // retrieve digit to e
      const LHCb::CaloDigit* digit = hcaldig->object(tmpid2);
      x2 = m_hcal->cellX(tmpid2);
      y2 = m_hcal->cellY(tmpid2);
      if( digit ) e2 = digit->e();
      s2 = m_hcal->cellSize(tmpid2);
    }  
  }// else cand id is not valid
  
  if(m_debugInfo)  debug() << " n cells retrieved = " << ncellretrieved << endmsg;
    
  return StatusCode::SUCCESS;
  
}

//=============================================================================================
int PrepareHadronSeed::ECALpart( float x , float y ){
  
  if     ( x==0. && y==0. )                       return 0;
  else if( fabs(x) <  969.6 && fabs(y) <  727.2 ) return 1;
  else if( fabs(x) < 1939.2 && fabs(y) < 1212.0 ) return 2;
  else                                            return 3;

}
