// Include files 

// from Gaudi
#include "GaudiKernel/RndmGenerators.h"
// local
#include "DeCaloCalib.h"
//-----------------------------------------------------------------------------
// Implementation file for class : DeCaloCalib
//
// Simple algorithm to overwrite the condDB calibration constant 
// with arbitrary value (user-defined or gaussian or flat randomly distributed)
// at the DeCalorimeter initialisation level.
//
// Useful to produce mis-calibrated energy and check/develop calibration procedure
// or to check calibration constant without feeding the condDB
//
// 2007-08-22 : Olivier DESCHAMPS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DeCaloCalib )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeCaloCalib::DeCaloCalib( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_calo(NULL)
  , m_rndmSvc(NULL)
{

  declareProperty( "DetectorName"   , m_detectorName );
  declareProperty( "Method"         , m_method = "Flat"); // Flat/Gauss/User
  declareProperty( "Params"         , m_params);           // gauss/flat  parameters
  declareProperty( "Key"            , m_key = "CellID" ); // 'CellID'/'Index' : for User-defined parameters
  declareProperty( "deltaGain"      , m_deltas);          // User defined params mapping  <key : value>
  declareProperty( "EventUpdate"    , m_update = false); // default is update in initialize only
  declareProperty( "Ntupling"       , m_ntup   = true ); 
  declareProperty( "DeadChannelList", m_dead);

  m_params.push_back( 1.0 );
  m_params.push_back( 1.0  );
  m_deltas["Default"] = 1.0 ;

// set default detectorName
  int idx = name.find_last_of(".") +1 ; // return 0 if '.' not found --> OK !!
  m_detectorName = name.substr( idx, 4 ); 
  if ( name.substr(idx,3) == "Prs" ) m_detectorName = "Prs";
  if ( name.substr(idx,3) == "Spd" ) m_detectorName = "Spd";


}
//=============================================================================
// Destructor
//=============================================================================
DeCaloCalib::~DeCaloCalib() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DeCaloCalib::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  //get DeCalorimeter
  if ( "Ecal" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  } else if ( "Hcal" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
  } else if ( "Prs" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
  } else if ( "Spd" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd ); 
  } else {
    error() << "Unknown Calo detector name " << m_detectorName << endmsg;
    return StatusCode::FAILURE;
  }

  info() << " ======= SIMULATING THE (MIS)Calibration of "<< m_detectorName << " gains  ======= " << endmsg;
  
  
  // Params
  m_rndmSvc = svc< IRndmGenSvc>( "RndmGenSvc" , true );
  
  double a,b;
  if( m_method == "Gauss" ){
    // Gaussian random (mean, rms)
    info() << "---- Method : gaussian random timing values "<< endmsg;
    if(m_params.size() != 2)error() << "wrong parameters size" << endmsg;
    a= *(m_params.begin());
    b= *(m_params.begin()+1);
    info() << " mean/sigma = " << a << "/" << b << endmsg;
    sc = m_shoot.initialize(rndmSvc() , Rndm::Gauss( a , b ));
    if( !sc.isSuccess() )return sc;
  }
  else if(m_method == "Flat"){
    // Flat random (min, max)
    info() << "---- Method : flat random timing values "<< endmsg;
    if(m_params.size() != 2)error() << "wrong parameters size" << endmsg;
    a= *(m_params.begin());
    b= *(m_params.begin()+1);
    info() << " min/max = " << a << "/" << b << endmsg;
    sc=m_shoot.initialize(rndmSvc() , Rndm::Flat( a , b ));
    if( !sc.isSuccess() )return sc;
  }  
  else if(m_method == "User"){
    info() << "---- Method : user-defined timing values "<< endmsg;
    info() << "Timing value have been defined for " << m_deltas.size() << " cells " << endmsg;
    info() << "Default value [" << m_deltas["Default"] <<  "] will be applied to " 
           << m_calo->numberOfCells()- m_deltas.size() << " other cells." << endmsg;
    if( m_key == "CellID" ){
      info() << "The calib values are mapped with KEY = CellID " << endmsg;
    }
    else if( m_key == "Index" ){
      info() << "The calib values are are mapped with KEY = Index" << endmsg;
    }
    else{
      error() << "undefined deltaKey : must be either 'CellID' or 'Index' " << endmsg;
    return StatusCode::FAILURE;
    }
  }
  else{
    error() << "Method " << m_method << " unknown - should be 'Flat', 'Gauss' or 'User'"<< endmsg;
    return StatusCode::FAILURE;
  }

  // update cellParams
  update(); 
  
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DeCaloCalib::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  // update at each event ?
  if(m_update)update();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DeCaloCalib::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================

void DeCaloCalib::update() {
  // update cellParams
  CaloVector<CellParam>& cells = (CaloVector<CellParam>&) m_calo->cellParams(); // no-const conversion
  std::vector<int> cellids,cellind;
  std::vector<double> gains,dgains;
  for(CaloVector<CellParam>::iterator icell = cells.begin() ; icell != cells.end() ; icell++){
    LHCb::CaloCellID id = (*icell).cellID() ;
    if( !m_calo->valid  ( id )  )continue;
    if( m_calo->isPinId( id )   )continue; 

    long num = m_calo->cellIndex( id );
    double dt;
    long index = id.index();
    if( m_key == "Index" )index = num;

    if( isDead( index ) ){
      dt = 0.;
      (*icell).addQualityFlag(CaloCellQuality::Dead);  
    }
    else if( m_method == "User" )
      dt = delta( index );
    else
      dt = m_shoot();
    
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << num << " Calibration constant for cellID " << id << " : " << dt << endmsg;
    (*icell).setCalibration ( dt ) ; //
    cellids.push_back( id.index()      );
    cellind.push_back( num             );
    gains.push_back  ( (*icell).gain() );
    dgains.push_back ( (*icell).calibration());
  }

  if(!m_ntup)return ;
  // Ntupling
  StatusCode sc;
  Tuple ntp = nTuple( 500 + CaloCellCode::CaloNumFromName( m_detectorName),
                      m_detectorName + "DeCalib" ,CLID_ColumnWiseTuple);
  int max = m_calo->numberOfCells() ;
  sc=ntp->farray("cellID"   , cellids  ,"nchannels",max);
  sc=ntp->farray("index"    , cellind  ,"nchannels",max);
  sc=ntp->farray("gain"     , gains    ,"nchannels",max);
  sc=ntp->farray("calib"    , dgains   ,"nchannels",max);
  sc=ntp->write();
  if(sc.isFailure())Warning("cannot write NTP").ignore();
}


bool DeCaloCalib::isDead(int channel) {
  if(m_dead.empty())return false;
  for(std::vector<int>::iterator i = m_dead.begin();m_dead.end()!=i;i++){
    if( m_key == "Index"  && channel == *i)return true;
    if( m_key == "CellID" && channel == ( *i & 0x3FFF) ) return true;
  }  
  return false;
}
