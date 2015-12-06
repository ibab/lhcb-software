// $Id: HadronSeedTool.cpp,v 1.14 2010-04-30 12:00:14 albrecht Exp $
// Include files 

#include <cmath>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IUpdateManagerSvc.h"

// from Event
#include "Event/L0CaloCandidate.h"
#include "Event/CaloDigit.h"
#include "Event/Track.h"
#include "Event/State.h"

// from Det
#include "CaloDet/DeCalorimeter.h"
#include "CaloDAQ/ICaloDataProvider.h"

// local
#include "HadronSeedTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HadronSeedTool
//
// 2007-07-17 : Johannes Albrecht
//-----------------------------------------------------------------------------

using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HadronSeedTool )

// static constants
const double HadronSeedTool::zECal = 12830.;
const double HadronSeedTool::zHCal = 13690.;
const double HadronSeedTool::zT3 = 9315.;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HadronSeedTool::HadronSeedTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_ecal(0) 
  , m_hcal(0)
  , m_ecalDaq(0)
  , m_hcalDaq(0)
  , m_l0ConfExtrapolator(0)
  , m_magFieldSvc(0)
  , m_DataStore(0)
  , m_fieldOff(false)
{
  //resolutions w/o calo decoding --> only region 3,4 (HCal) sensible
  declareProperty("l0SigmaX2",  m_l0SigmaX2  = {0.,0.,0.,1849.,8281. } );
  declareProperty("l0SigmaY2",  m_l0SigmaY2  = {0.,0.,0.,1521.,6400. } );
  declareProperty("l0SigmaTx2", m_l0SigmaTx2 = {0.,0.,0.,8.1e-5,1.69e-4 } );
  declareProperty("l0SigmaTy2", m_l0SigmaTy2 = {0.,0.,0.,3.6e-5,1.e-4 } );

  //improved resolutions with decoded calo cells
  declareProperty("sigmaX2",  m_sigmaX2  = { 441.,441.,1600.,729.,2601.} );
  declareProperty("sigmaY2",  m_sigmaY2  = { 196.,169.,625.,625.,2704.} );
  declareProperty("sigmaTx2", m_sigmaTx2 = { 2.5e-5,3.6e-5,1.44e-4,4.9e-5,2.56e-4} );
  declareProperty("sigmaTy2", m_sigmaTy2 = { 4.9e-5,1.e-4,1.e-4,6.4e-5,1.44e-4} );

  declareProperty("debugMode", m_debugMode = false );
  declareProperty("decodeCalos",m_decodeCalos = false );
  
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

  m_l0ConfExtrapolator = tool<IL0ConfExtrapolator>("L0ConfExtrapolator");
  
  //check vectors set by job options
  if( 5!=m_sigmaX2.size() || 5!=m_sigmaY2.size() 
      || 5!=m_sigmaTx2.size() || 5!=m_sigmaTy2.size() 
      || 5!=m_l0SigmaX2.size() || 5!=m_l0SigmaY2.size() 
      || 5!=m_l0SigmaTx2.size() || 5!=m_l0SigmaTy2.size() ){
    error()<<"Size of search winow vectors is not valid, check you options!"<<endmsg;
    return StatusCode::FAILURE;
  }

 // subscribe to the updatemanagersvc with a dependency on the magnetic field svc
  IUpdateManagerSvc* m_updMgrSvc = svc<IUpdateManagerSvc>("UpdateManagerSvc", true);
  m_magFieldSvc = svc<ILHCbMagnetSvc>("MagneticFieldSvc", true);
  m_updMgrSvc->registerCondition( this,m_magFieldSvc,&HadronSeedTool::updateField) ;
  
  // initialize with the current conditions
  return m_updMgrSvc->update(this) ;
}


StatusCode HadronSeedTool::updateField()
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

StatusCode HadronSeedTool::makeTrack(const LHCb::L0CaloCandidate& hadL0Cand,
                                     LHCb::Track& seedTrack )
{
  if(msgLevel(MSG::DEBUG)) {

    debug()<<"HadronSeedTool::execute!"<<endmsg;
    debug()<<"Properties of L0 candidate:  ( Et, x , y ) = ( "
           <<hadL0Cand.et()<<" , "<<hadL0Cand.position().X() <<" , "<<hadL0Cand.position().Y() <<endmsg;
  }

  // generate a state from L0 candidate information alone to make sure
  // we have a fallback solution
  double z = zHCal;
  double x = hadL0Cand.position().X();
  double y = hadL0Cand.position().Y();
  double zL0 = hadL0Cand.position().Z();
  double tx = 0;
  
  if(m_fieldOff) tx = x / double(zL0);
  
  double ty = y / double(zL0);
  double e = std::abs(hadL0Cand.et()) *
	  std::sqrt(x * x + y * y + zL0 * zL0) / std::sqrt(x * x + y * y);

  //region:  0-2 for ECal, 3-4 for HCal
  int region = m_l0ConfExtrapolator->getCaloRegion(x,y,z);
  if( -1 == region ){
    Warning("Calo region not valid",StatusCode::SUCCESS,1);
    //if position of calo candidate no valid, 
    //assume outermost region --> largest search window
    region = 4;
    //return StatusCode::SUCCESS;
  }
  
  // set the covariance matrix
  //set the resolution for L0 information first (no calo decoding)
  Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
  stateCov(0,0) = m_l0SigmaX2[region];
  stateCov(1,1) = m_l0SigmaY2[region];
  stateCov(2,2) = m_l0SigmaTx2[region];
  stateCov(3,3) = m_l0SigmaTy2[region];
  stateCov(4,4) = 8.41e-6;
  
  if( m_decodeCalos ){
    
    // initialize caloDAQ for this event
    //    m_hcalDaq->getBanks();
    
    // calculate barycenter of HCal cluster around seed
    StatusCode sc = getHCalBarycenter( hadL0Cand, x, y, e, region );
    if ( sc.isFailure() ) {
      if ( -1 == region ) { 
        warning() << "Failed to calculate 2x2 HCal cluster or its region!" << endmsg;
        return StatusCode::SUCCESS;
      }
      info() << "Falling back on position information from L0 candidate"<<endmsg;
    } else {
      // L0 candidate position was refined using hcal - let's see if the
      // ecal helps to narrow down things even further
      // want at least 2 ecal cells with valid data
      //      m_ecalDaq->getBanks();
      sc = getECalBarycenter( x, y, z, e, 2, region );
      // we can just ignore the error code, because we are not guaranteed
      // to find something in the ecal
    }
    //we have decoded Calos, set finer resolution
    stateCov(0,0) = m_sigmaX2[region];
    stateCov(1,1) = m_sigmaY2[region];
    stateCov(2,2) = m_sigmaTx2[region];
    stateCov(3,3) = m_sigmaTy2[region];

  }//end if decode calos
  
  // add state to track
  Gaudi::TrackVector stateVec(x, y, tx, ty, 1. / double(e));
  LHCb::State state(stateVec, stateCov, z, State::MidHCal);
  seedTrack.addToStates(state);
  
  //set the calo id to LHCbids of the track
  seedTrack.addToLhcbIDs( LHCbID( hadL0Cand.id() ) );
    
  if (m_debugMode) {
    m_DataStore->region.push_back( region );
    m_DataStore->region.push_back( region );
  }
  
  return StatusCode::SUCCESS; 
}

int HadronSeedTool::HCALpart(const LHCb::CaloCellID& cell) const
{
  if (!m_hcal->valid(cell)) return -1;
  // HCal IP: cell size 131.3mm -> region 3, 
  // HCal OP cell size 262.6mm -> region 4
  int cellSize = int(m_hcal->cellSize(cell) / 100.);
  if (0 >= cellSize || 3 <= cellSize) return -1;
  else return cellSize + 2;
}

StatusCode HadronSeedTool::getHCalBarycenter(const LHCb::L0CaloCandidate& cand,
                                             double& x, double& y, double& e,
                                             int& reg ) const
{
  // get the cand cell ID
  const LHCb::CaloCellID& candcellid = cand.id();

  LHCb::CaloCellID caloId2(3,candcellid.area(),candcellid.row(),candcellid.col()+1);
  LHCb::CaloCellID caloId3(3,candcellid.area(),candcellid.row()+1,candcellid.col());
  LHCb::CaloCellID caloId4(3,candcellid.area(),candcellid.row()+1,candcellid.col()+1);

  // figure out calo region - we may have to test two cells 
  if (-1 == (reg = HCALpart(candcellid))) {
    if (-1 == (reg = HCALpart(caloId4))) {
      warning() << "cand cell and +1/+1 cell not valid! return!!!!" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  const LHCb::CaloCellID* cands[4] = { &candcellid, &caloId2, &caloId3, &caloId4 };

  // calculate barycenter
  double xBarycenter = 0., yBarycenter = 0., clusterEnergy = 0.;
  for (int i = 4; i--; ) {
    if (!m_hcal->valid(*cands[i])) continue;
    double energy = m_hcalDaq->digit(*cands[i]);
    if (0. >= energy) continue;
    clusterEnergy += energy;
    xBarycenter += energy * m_hcal->cellX(*cands[i]);
    yBarycenter += energy * m_hcal->cellY(*cands[i]);
  }

  if ( 0. >= clusterEnergy ) {
    info() << "HCAL cluster energy zero or negative!" << endmsg;
    return StatusCode::FAILURE;
  }
  x = xBarycenter / clusterEnergy;
  y = yBarycenter / clusterEnergy;
  e = clusterEnergy;
  
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "x, y, clusterE = "<< xBarycenter << ", " 
            <<yBarycenter << ", " << clusterEnergy << endmsg;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode HadronSeedTool::getECalBarycenter(double& x, double& y, double& z, double& e,
                                             int minCells, int& region ) const
{
  static const double cellSizeECal[3] = { 40.56, 60.85, 121.7 };
  // extrapolate HCal position to ECal 
  double xPred = x * zECal / double(z);
  double yPred = y * zECal / double(z);
  int reg = ECALpart(xPred, yPred);
  // check that the result is valid
  if( 0 > reg ) {
    warning() << "Problem with ECALPART, region = " << reg << " , exit!"
              << endmsg;
    return StatusCode::FAILURE;
  }

  // now get the ECAL cells around this point  
  const Gaudi::XYZPoint centerpoint = Gaudi::XYZPoint(xPred,yPred,zECal);
  LHCb::CaloCellID centerCell = m_ecal->Cell( centerpoint );

  std::vector<LHCb::CaloCellID> ecalCells;
  ecalCells.reserve(9);
  
  int cellArea = centerCell.area();
  int cellRow = centerCell.row();
  int cellCol = centerCell.col();
  
  for ( int iRow = 0 ; iRow < 3 ; iRow++ ) {
    for ( int iCol = 0 ; iCol < 3 ; iCol++ ) {
      LHCb::CaloCellID tmpid( 2 , cellArea , cellRow-1+iRow  , cellCol-1+iCol );
      // check that ecal id is valid and daq recorded positive energy
      if( m_ecal->valid( tmpid ) && 0. < m_ecalDaq->digit(tmpid) ) {
        // require cell to lie within 1 cell size to prediction
        double cellX = m_ecal->cellX(tmpid);
        double cellY = m_ecal->cellY(tmpid);
        double cellSize = m_ecal->cellSize(tmpid);
        if ( std::abs(cellX - xPred) < cellSizeECal[reg] &&
             std::abs(cellY - yPred) < cellSizeECal[reg] &&
             std::abs(cellSize - cellSizeECal[reg]) < 1 )
          ecalCells.push_back( tmpid );
      }
    }
  }
  
  if (ecalCells.empty()) {
    if ( msgLevel(MSG::DEBUG) ) debug() << "No acceptable ECal cells found." << endmsg;
    return StatusCode::FAILURE;
  }
  if (int(ecalCells.size()) < minCells) {
    if ( msgLevel(MSG::DEBUG) ) debug() << "Too few ECal cells: " 
                                        << ecalCells.size() << " found, need "
                                        << minCells << endmsg;
    return StatusCode::FAILURE;
  }

  if ( msgLevel(MSG::DEBUG) )
	  debug() << "number of ecal cells: " << ecalCells.size() << endmsg;
  
  // calulate barycenter
  double xECal = 0., yECal = 0., clusterEnergy = 0.;
  std::vector<LHCb::CaloCellID>::const_iterator id;
  for ( id=ecalCells.begin(); ecalCells.end()!=id; ++id ) {
    const LHCb::CaloCellID& cellid = *id;
    double cellE = m_ecalDaq->digit( cellid );
    clusterEnergy += cellE;
    xECal += m_ecal->cellX(cellid) * cellE;
    yECal += m_ecal->cellY(cellid) * cellE;
  }
  
  x = xECal / clusterEnergy;
  y = yECal / clusterEnergy;
  z = zECal;
  e += clusterEnergy;
  region = reg;
  
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "reconstruction in ECAL: \n ( E, x , y ) = ( "
            << clusterEnergy << " , " << xECal << " , " << yECal << endmsg;
  }
  
  return StatusCode::SUCCESS;
}

int HadronSeedTool::ECALpart( double x , double y ) const
{
  if      ( 0. == x         && 0. == y         ) return -1;
  else if ( fabs(x) <  650. && fabs(y) <  650. ) return 0;
  else if ( fabs(x) < 1940. && fabs(y) < 1450. ) return 1;
  else                                           return 2;
}
