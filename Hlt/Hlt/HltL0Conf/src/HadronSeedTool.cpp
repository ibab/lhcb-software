// $Id: HadronSeedTool.cpp,v 1.2 2007-08-16 17:39:44 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from Event
#include "Event/L0CaloCandidate.h"
#include "Event/CaloDigit.h"
#include "Event/Track.h"
#include "Event/State.h"

// from Det
#include "CaloDet/DeCalorimeter.h"

#include "CaloDAQ/ICaloDataProvider.h"

//boost
#include <boost/assign/list_of.hpp>

// local
#include "HadronSeedTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HadronSeedTool
//
// 2007-07-17 : Johannes Albrecht
//-----------------------------------------------------------------------------

using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HadronSeedTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HadronSeedTool::HadronSeedTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  //this is preliminary!!!
  //declare search windows, tuned JA: 31-05-2007 
  declareProperty("sigmaX2", m_sigmaX2 = boost::assign::list_of(441.)(441.)(1681.)(961.)(6889.) );
  declareProperty("sigmaY2", m_sigmaY2 = boost::assign::list_of(196.)(169.)(625.)(625.)(2704.) );
  declareProperty("sigmaTx2", m_sigmaTx2 = boost::assign::list_of(2.5e-5)(3.6e-5)(1.44e-4)(4.9e-5)(2.56e-4) );
  declareProperty("sigmaTy2", m_sigmaTy2 = boost::assign::list_of(4.9e-5)(1.e-4)(1.e-4)(6.4e-5)(1.44e-4) );
  declareProperty("debugMode", m_debugMode = false );

  declareInterface<ICaloSeedTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
HadronSeedTool::~HadronSeedTool() {} 

//=============================================================================

//-------------------------------------
StatusCode HadronSeedTool::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return sc;
  }
  debug() << " Initialize HadronSeedTool" << endmsg;

  
  // Retrieve the ECAL detector element
  m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
 
  // Retrieve the HCAL detector element
  m_hcal = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
 
  m_ecalDaq=tool<ICaloDataProvider>("CaloDataProvider","EcalDataProvider",this);
  m_hcalDaq=tool<ICaloDataProvider>("CaloDataProvider","HcalDataProvider",this);
  
  //tool for debug information
  m_DataStore = tool<L0ConfDataStore>("L0ConfDataStore");
  
  cellSizeECal[0]=40.56; cellSizeECal[1]=60.85; cellSizeECal[2]=121.7;
  zECal = 12830.;
  zHCal = 13690.;
  zT3 = 9315.;  
  return StatusCode::SUCCESS;
}


StatusCode HadronSeedTool::makeTrack(const LHCb::L0CaloCandidate& hadL0Cand,
                                     LHCb::Track& seedTrack )
{
  if(msgLevel(MSG::DEBUG)) debug()<<"HadronSeedTool::execute!"<<endmsg;
 
  //initialize caloDAQ for this event
  m_ecalDaq->setBank();
  m_hcalDaq->setBank();

  /* 
   *   calculate barycenter of HCal cluster around seed
   */
  double XHCalBarycenter = 0; 
  double YHCalBarycenter = 0;
  double energyHCal = 0;
  int region = 4; //0-2 for ECal, 3-4 for HCal
  
  StatusCode sc = getHCalBarycenter( hadL0Cand, energyHCal , XHCalBarycenter , YHCalBarycenter , region );
  if( !sc ){ 
    warning()<<"Failed to calculate 2x2 HCal cluster"<<endmsg;
    return StatusCode::SUCCESS;
  }
  
  //set the calo id to LHCbids of the track
  seedTrack.addToLhcbIDs( LHCbID( hadL0Cand.id() ) );
  
  /*
   *  if enought energy deposited in ECal, use this to get positions
   */
  double nECalCells = 0;    
  double energyECal = 0.;
  double XECalBarycenter = 0.;
  double YECalBarycenter = 0.;
  int tmpReg = 0;
  
  sc = getECalBarycenter( XHCalBarycenter , YHCalBarycenter , 
                          nECalCells , energyECal  , XECalBarycenter , YECalBarycenter , tmpReg );
  if( !sc ){ 
    warning()<<"Failed to calculate 2x2 ECal cluster"<<endmsg;
    return StatusCode::SUCCESS;
  }
  

  //====================================================================================================
  double xPos, yPos, zPos, oneOverP;
  LHCb::State seedState;

  if( 2 < nECalCells ){
    
    region = tmpReg;
    xPos = XECalBarycenter;
    yPos = YECalBarycenter;
    zPos = zECal;
    oneOverP = 1./(energyECal+energyHCal);
  }
  else{ // use only HCAL
    xPos = XHCalBarycenter;
    yPos = YHCalBarycenter;
    zPos = zHCal;
    oneOverP = 1/(energyHCal);
  }
  
  // set the covariance matrix
  Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
  stateCov(0,0) = m_sigmaX2[region];
  stateCov(1,1) = m_sigmaY2[region];
  stateCov(2,2) = m_sigmaTx2[region];
  stateCov(3,3) = m_sigmaTy2[region];
  stateCov(4,4) = 8.41e-6;
      

  // set states and covariance matrix
  seedState.setState(xPos,yPos,zPos,0,0, oneOverP);
  seedState.setCovariance(stateCov);
  seedState.setLocation(State::MidHCal);

  if(m_debugMode) m_DataStore->region = region;
  
  seedTrack.addToStates(seedState);

  return sc;
  
}


StatusCode HadronSeedTool::getHCalBarycenter( const LHCb::L0CaloCandidate& cand, double& clusterEnergy , 
                                              double& xBarycenter , double& yBarycenter , int& reg )
{
  // get the cand cell ID
  const LHCb::CaloCellID& candcellid = cand.id();
  
  LHCb::CaloCellID caloId2(3,candcellid.area(),candcellid.row(),candcellid.col()+1);
  LHCb::CaloCellID caloId3(3,candcellid.area(),candcellid.row()+1,candcellid.col());
  LHCb::CaloCellID caloId4(3,candcellid.area(),candcellid.row()+1,candcellid.col()+1);

  // first test that the candidate cell id is valid...
  if( m_hcal->valid(candcellid) ){
  
    double energy = m_hcalDaq->digit( candcellid );
    clusterEnergy += energy;
    xBarycenter += energy*m_hcal->cellX(candcellid);
    yBarycenter += energy*m_hcal->cellY(candcellid);

    double cellSize = m_hcal->cellSize(candcellid);
    if( cellSize > 100 && cellSize < 200 )  reg = 3;
    else if( cellSize > 200 && cellSize < 300 ) reg = 4;
    
  }
  else if( m_hcal->valid(caloId4) ){
    if(msgLevel(MSG::DEBUG)) debug()<<"cell id of first hcal cand is not valid!!!!"<<endmsg;
    double cellSize = m_hcal->cellSize(caloId2);
    if( cellSize > 100 && cellSize < 200 )  reg = 3;
    else if( cellSize > 200 && cellSize < 300 ) reg = 4;
  }
  else{
    warning()<<"cand cell and +1/+1 cell not valid! return!!!!"<<endmsg;
    return StatusCode::FAILURE;
  }
  
  double energy = m_hcalDaq->digit( caloId2 );
  clusterEnergy += energy;
  xBarycenter += energy*m_hcal->cellX(caloId2);
  yBarycenter += energy*m_hcal->cellY(caloId2);
  
  energy = m_hcalDaq->digit( caloId3 );
  clusterEnergy += energy;
  xBarycenter += energy*m_hcal->cellX(caloId3);
  yBarycenter += energy*m_hcal->cellY(caloId3);
  
  energy = m_hcalDaq->digit( caloId4 );
  clusterEnergy += energy;
  xBarycenter += energy*m_hcal->cellX(caloId4);
  yBarycenter += energy*m_hcal->cellY(caloId4);
    
  xBarycenter = xBarycenter / clusterEnergy;
  yBarycenter = yBarycenter / clusterEnergy;
  
  if(msgLevel(MSG::DEBUG)){
    debug()<<"reconstruction in HCAl: \n ( E, x , y ) = ( "
           <<clusterEnergy<<" , "<<xBarycenter <<" , "<<yBarycenter <<endmsg;
  }
  

  return StatusCode::SUCCESS;
  
}


StatusCode HadronSeedTool::getECalBarycenter( double xHCal , double yHCal , double& nCells , 
                                              double&  clusterEnergy, double& xECal, double& yECal , int& region  )
{
  // extrapolate HCal position to ECal 
  double xPredicted = xHCal*zECal/zHCal;
  double yPredicted = yHCal*zECal/zHCal;
  region = ECALpart(xPredicted,yPredicted);
  if( 0 > region ) {
    warning()<<"Problem with ECALPART, region = "<<region<<" , exit!"<<endmsg;
    return StatusCode::FAILURE;
  }

  // now get the ECAL cells around this point  
  const Gaudi::XYZPoint centerpoint = Gaudi::XYZPoint(xPredicted,yPredicted,zECal);
  LHCb::CaloCellID centerCell = m_ecal->Cell( centerpoint );

  std::vector<LHCb::CaloCellID> ecalCells;
  //ecalCells.push_back(centerCell);
  
  int cellArea = centerCell.area();
  int cellRow = centerCell.row();
  int cellCol = centerCell.col();
  
  for( int iRow = 0 ; iRow < 3 ; iRow++ ){
    for( int iCol = 0 ; iCol < 3 ; iCol++ ){
      LHCb::CaloCellID tmpid( 2 , cellArea , cellRow-1+iRow  , cellCol-1+iCol );      
      if( m_ecal->valid( tmpid ) ){
        ecalCells.push_back( tmpid );
      }
    }
  }
  
  if(msgLevel(MSG::DEBUG))  debug()<<"number of ecal cells: "<<ecalCells.size() <<endmsg;
  

  std::vector<LHCb::CaloCellID>::const_iterator id;
  for( id=ecalCells.begin(); ecalCells.end()!=id; ++id ){
    
    const LHCb::CaloCellID& cellid = (*id);
    
    if( m_ecal->valid(cellid) ){
   
      double cellE = m_ecalDaq->digit( cellid );
      
      if( 0 < cellE ){

        double cellX = m_ecal->cellX(cellid);
        double cellY = m_ecal->cellY(cellid);
        double cellSize = m_ecal->cellSize(cellid);
        
        //require cell to lie within 1 cell size to prediction
        if( fabs(cellX-xPredicted)                       < cellSizeECal[region]
            && fabs(cellY-yPredicted)                    < cellSizeECal[region]
            && fabs(cellSize - cellSizeECal[region])     < 1 ) {
          
          nCells++;
          clusterEnergy+=cellE;
          xECal+=cellX*cellE;
          yECal+=cellY*cellE;
    
        }
      }//cellE > 0
    }//valid cell id
  }//loop ecal digis
  
  xECal=xECal/clusterEnergy;
  yECal=yECal/clusterEnergy;
  
  if(msgLevel(MSG::DEBUG)){
    debug()<<"reconstruction in ECAl: \n ( E, x , y ) = ( "
           <<clusterEnergy<<" , "<<xECal <<" , "<<yECal <<endmsg;
  }  
  
  return StatusCode::SUCCESS;
}

int HadronSeedTool::ECALpart( double x , double y ){
  
  if     ( x==0. && y==0. )                       return -1;
  else if( fabs(x) <  969.6 && fabs(y) <  727.2 ) return 0;
  else if( fabs(x) < 1939.2 && fabs(y) < 1212.0 ) return 1;
  else                                            return 2;

}
