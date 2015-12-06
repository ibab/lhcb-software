// $id:  $

// // from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// from Event
#include "Event/CaloDigit.h"
#include "Event/Track.h"
#include "Event/State.h"

// from Det
#include "CaloDet/DeCalorimeter.h"

#include "CaloDAQ/ICaloDataProvider.h"

//local
#include "ElectronSeedTool.h"

using namespace LHCb;


DECLARE_TOOL_FACTORY( ElectronSeedTool )


// Standard Constructor
ElectronSeedTool::ElectronSeedTool(const std::string& type, 
                                   const std::string& name, 
                                   const IInterface* parent) 
  : GaudiTool( type, name, parent )
  ,  m_fieldOff(false)
  , m_ecal(0)
  , m_magFieldSvc(0)
  , m_caloDaq(0)
  , m_DataStore(0)
{
  // Declare the algorithm's properties which can be set at run time
  declareInterface<ICaloSeedTool>(this);
  declareProperty("debugMode",m_debugMode = false);
  
  // Resolution of the L0Cand at T3, tune JA 2007-03-22
  declareProperty("sigmaX2",  m_sigmaX2  = {{9.,30.25,225.}} );
  declareProperty("sigmaY2",  m_sigmaY2  = {{4.,12.25,64.}} );
  declareProperty("sigmaTx2", m_sigmaTx2 = {{9.e-6,9.e-6,36.e-6}} );
  declareProperty("sigmaTy2", m_sigmaTy2 = {{25.e-6,49.e-6,64.e-6}} );
  clearElectronVariables();

}

// Standard Destructor -------------------------------------
ElectronSeedTool::~ElectronSeedTool() {}

//-------------------------------------
StatusCode ElectronSeedTool::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return sc;
  }
  debug() << " Initialize ElectronSeedTool" << endmsg;

  // Retrieve the ECAL detector element
  m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );

  //Retrieve tool for Ecal decoding on demand
  m_caloDaq=tool<ICaloDataProvider>("CaloDataProvider","EcalDataProvider",this);
  
  if(m_debugMode){
    //tool for debug information
    m_DataStore = tool<L0ConfDataStore>("L0ConfDataStore");
  }
  
  // cell sizes for ECal IP / MP / OP
  cellSize[0]=40.5567; cellSize[1]=60.85; cellSize[2]=121.7; 
  
  //set parameters for S-shape correction
  parIP[0]=14.;  parIP[1]=5.75;
  parMP[0]=20.9; parMP[1]=7.;
  parOP[0]=42.4; parOP[1]=11.3;
  
  //double sParIP[2], sParMP[2], sParOP[2];
  //calculate these once ..
  sParIP[0] = tan(  cellSize[0]*0.5 / parIP[0] );
  sParIP[1] = tan( -cellSize[0]*0.5 / parIP[0] );
  sParMP[0] = tan(  cellSize[1]*0.5 / parMP[0] );
  sParMP[1] = tan( -cellSize[1]*0.5 / parMP[0] );
  sParOP[0] = tan(  cellSize[2]*0.5 / parOP[0] );
  sParOP[1] = tan( -cellSize[2]*0.5 / parOP[0] );

//check vectors set by job options
  if( 3!=m_sigmaX2.size() || 3!=m_sigmaY2.size() 
      || 3!=m_sigmaTx2.size() || 3!=m_sigmaTy2.size() ){
    error()<<"Size of search winow vectors is not valid, check you options!"<<endmsg;
    return StatusCode::FAILURE;
  }

  // subscribe to the updatemanagersvc with a dependency on the magnetic field svc
  IUpdateManagerSvc* m_updMgrSvc = svc<IUpdateManagerSvc>("UpdateManagerSvc", true);
  m_magFieldSvc = svc<ILHCbMagnetSvc>("MagneticFieldSvc", true);
  m_updMgrSvc->registerCondition( this,m_magFieldSvc,&ElectronSeedTool::updateField) ;
  
  // initialize with the current conditions
  return m_updMgrSvc->update(this) ;
}

StatusCode ElectronSeedTool::updateField()
{
  if(msgLevel(MSG::DEBUG)) 
    debug()<<"magnetic field is: "<<m_magFieldSvc->signedRelativeCurrent()<<endmsg;
  
  m_fieldOff=false;
  if( fabs(m_magFieldSvc->signedRelativeCurrent()) < 0.1 ) {
    m_fieldOff=true;
    if(msgLevel(MSG::INFO)) {
      info()<<" magnetic field is below 10% of nominal field! \n Use options for no field!"<<endmsg;
      info()<<"Tool configured for no B field!"<<endmsg;
      info()<<"Position and slope is set correctly, covariance and momemtum _not_!"<<endmsg;
    }
  }
  return StatusCode::SUCCESS ;
}



StatusCode ElectronSeedTool::makeTrack( const LHCb::L0CaloCandidate& eL0Cand,
                                        LHCb::Track& seedTrack  )
{

  if(msgLevel(MSG::DEBUG)) debug()<<"execute!"<<endmsg;

  //initialize Calo decoding for this event
  //m_caloDaq->getBanks();
  
  int ecalRegion = 0;
  
  //===================================================================================================
  //clear electron variables
  clearElectronVariables();
  //===================================================================================================

  //set the calo id to LHCbids of the track
  seedTrack.addToLhcbIDs( LHCbID( eL0Cand.id() ) );

  // get candidates cells x, y, energy and size for electron candidate
  StatusCode sc = SetCandPosAndE( eL0Cand , 
                                  e_x1, e_x2, e_x3, e_x4,
                                  e_y1, e_y2, e_y3, e_y4,
                                  e_e1, e_e2, e_e3, e_e4,
                                  e_s1, e_s2, e_s3, e_s4 );
  
  if( sc.isFailure() ) warning()<< " didn't manage to set cand positions and energy..." << endmsg;
  
  if(msgLevel(MSG::VERBOSE)) {
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
  if( e_s1!=0 && e_s2!=0 )      candx=(e_x1+e_x2)*0.5;
  else if( e_s1!=0 && e_s4!=0 ) candx=(e_x1+e_x4)*0.5;
  else if( e_s3!=0 && e_s2!=0 ) candx=(e_x3+e_x2)*0.5;
  else if( e_s3!=0 && e_s4!=0 ) candx=(e_x3+e_x4)*0.5;

  double candy = 0.;
  if( e_s1!=0 && e_s3!=0 )      candy=(e_y1+e_y3)*0.5;
  else if( e_s1!=0 && e_s4!=0 ) candy=(e_y1+e_y4)*0.5;
  else if( e_s2!=0 && e_s3!=0 ) candy=(e_y2+e_y3)*0.5;
  else if( e_s2!=0 && e_s4!=0 ) candy=(e_y2+e_y4)*0.5;
  
  
  double barx = (e_x1*e_e1+e_x2*e_e2+e_x3*e_e3+e_x4*e_e4)/(e_e1+e_e2+e_e3+e_e4);
  double bary = (e_y1*e_e1+e_y2*e_e2+e_y3*e_e3+e_y4*e_e4)/(e_e1+e_e2+e_e3+e_e4);

  double barx_cor = 0.;
  double bary_cor = 0.;
  
  // apply corrections
  if( fabs(cellSize[0]-e_s1) < 1 ){
    ecalRegion=0;
    if     ( barx-candx >  cellSize[0]*0.5 ) barx_cor = candx + parIP[1] * sParIP[0];
    else if( barx-candx < -cellSize[0]*0.5 ) barx_cor = candx + parIP[1] * sParIP[1];
    else                            barx_cor = candx + parIP[1]*tan( (barx-candx) / parIP[0] );     
    
    if     ( bary-candy >  cellSize[0]*0.5 ) bary_cor = candy + parIP[1] * sParIP[0]; 
    else if( bary-candy < -cellSize[0]*0.5 ) bary_cor = candy + parIP[1] * sParIP[1]; 
    else                            bary_cor = candy + parIP[1]*tan( (bary-candy) / parIP[0] );     
  }
  
  if( fabs(cellSize[1]-e_s1) < 1 ){
    ecalRegion=1;
    if     ( barx-candx >  cellSize[1]*0.5 ) barx_cor = candx + parMP[1] * sParMP[0]; 
    else if( barx-candx < -cellSize[1]*0.5 ) barx_cor = candx + parMP[1] * sParMP[1]; 
    else                            barx_cor = candx + parMP[1]*tan( (barx-candx) / parMP[0] );     
  
    if     ( bary-candy >  cellSize[1]*0.5 ) bary_cor = candy + parMP[1] * sParMP[0]; 
    else if( bary-candy < -cellSize[1]*0.5 ) bary_cor = candy + parMP[1] * sParMP[1]; 
    else                            bary_cor = candy + parMP[1]*tan( (bary-candy) / parMP[0] ); 
  }
  
  if( fabs(cellSize[2]-e_s1) < 1 ){
    ecalRegion=2;
    if     ( barx-candx >  cellSize[2]*0.5 ) barx_cor = candx + parOP[1] * sParOP[0]; 
    else if( barx-candx < -cellSize[2]*0.5 ) barx_cor = candx + parOP[1] * sParOP[1]; 
    else                            barx_cor = candx + parOP[1]*tan( (barx-candx) / parOP[0] );     
    if     ( bary-candy >  cellSize[2]*0.5 ) bary_cor = candy + parOP[1] * sParOP[0]; 
    else if( bary-candy < -cellSize[2]*0.5 ) bary_cor = candy + parOP[1] * sParOP[1]; 
    else                            bary_cor = candy + parOP[1]*tan( (bary-candy) / parOP[0] ); 
  }
  
  if(msgLevel(MSG::VERBOSE)){
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
  double zCluster = 12700.;
  double eCluster = e_e1+e_e2+e_e3+e_e4;
  //double statePos_qOverp = 1./ecluster;
  
  // set the states
  LHCb::State seedState;
  double tx = 0.;
  if(m_fieldOff) tx = barx_cor / double(zCluster);
  double ty = bary_cor / double(zCluster);
  
  seedState.setState( barx_cor , bary_cor , zCluster ,
                      tx , ty , 1./double(eCluster) );
    
  // set the covariance matrix
  Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
  stateCov(0,0) = m_sigmaX2[ecalRegion]; 
  stateCov(1,1) = m_sigmaY2[ecalRegion];
  stateCov(2,2) = m_sigmaTx2[ecalRegion];
  stateCov(3,3) = m_sigmaTy2[ecalRegion];
  stateCov(4,4) = 8.41e-6;

  seedState.setCovariance(stateCov);

  if(m_debugMode) {
    m_DataStore->region.push_back( ecalRegion );
    m_DataStore->region.push_back( ecalRegion );
  }
  
  seedTrack.addToStates(seedState);
    
  return StatusCode::SUCCESS;
  
}


//=============================================================================================
//  This method returns the L0CaloCand cells pos and energy given the cand 
//=============================================================================================
StatusCode ElectronSeedTool::SetCandPosAndE(const LHCb::L0CaloCandidate& cand,
                                            double& x1, double& x2, double& x3, double& x4,
                                            double& y1, double& y2, double& y3, double& y4,
                                            double& e1, double& e2, double& e3, double& e4,
                                            double& s1, double& s2, double& s3, double& s4 ){
  if(msgLevel(MSG::VERBOSE))
    verbose() << " SetCandPosAndE is running" << endmsg;
  
  // get the candcell ID
  const LHCb::CaloCellID& candcellid = cand.id();
  
  // first test that the candidate cell id is valid...
  if( m_ecal->valid(candcellid) ){
    
    x1 = m_ecal->cellX(candcellid);
    y1 = m_ecal->cellY(candcellid);
    e1 = m_caloDaq->digit( candcellid );
    s1 = m_ecal->cellSize(candcellid);

    int cellArea = candcellid.area();
    int cellRow = candcellid.row();
    int cellCol = candcellid.col();

    LHCb::CaloCellID tmpId2(2 , cellArea , cellRow  , cellCol+1 ); 
    x2 = m_ecal->cellX(tmpId2);
    y2 = m_ecal->cellY(tmpId2);
    e2 = m_caloDaq->digit( tmpId2 );
    s2 = m_ecal->cellSize(tmpId2);
          
    LHCb::CaloCellID tmpId3(2 , cellArea , cellRow+1  , cellCol ); 
    x3 = m_ecal->cellX(tmpId3);
    y3 = m_ecal->cellY(tmpId3);
    e3 = m_caloDaq->digit( tmpId3 );
    s3 = m_ecal->cellSize(tmpId3);

    LHCb::CaloCellID tmpId4(2 , cellArea , cellRow+1  , cellCol+1 ); 
    x4 = m_ecal->cellX(tmpId4);
    y4 = m_ecal->cellY(tmpId4);
    e4 = m_caloDaq->digit( tmpId4 );
    s4 = m_ecal->cellSize(tmpId4);

  }// cand cell id is valid

  return StatusCode::SUCCESS;
  
}
