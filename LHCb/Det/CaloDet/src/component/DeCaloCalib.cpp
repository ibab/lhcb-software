// $Id: DeCaloCalib.cpp,v 1.1 2008-09-26 15:45:39 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
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
DECLARE_ALGORITHM_FACTORY( DeCaloCalib );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeCaloCalib::DeCaloCalib( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_rndmSvc           (  0    )
{

  declareProperty( "DetectorName"   , m_detectorName );
  declareProperty( "Method"         , m_method = "Flat"); // Flat/Gauss/User
  declareProperty( "Params"         , m_params);
  declareProperty( "Key"            , m_key = "CellID" );
  declareProperty( "deltaGain"      , m_deltas);

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

  debug() << "==> Initialize" << endmsg;

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
    error() << "Unknown Calo detector name " << m_detectorName << endreq;
    return StatusCode::FAILURE;
  }

  info() << " ======= SIMULATING THE (MIS)Calibration of "<< m_detectorName << " gains  ======= " << endreq;
  
  
  // Params
  m_rndmSvc = svc< IRndmGenSvc>( "RndmGenSvc" , true );
  Rndm::Numbers shoot;
  
  double a,b;
  if( m_method == "Gauss" ){
    // Gaussian random (mean, rms)
    info() << "---- Method : gaussian random timing values "<< endreq;
    if(m_params.size() != 2)error() << "wrong parameters size" << endreq;
    a= *(m_params.begin());
    b= *(m_params.begin()+1);
    info() << " mean/sigma = " << a << "/" << b << endreq;
    sc = shoot.initialize(rndmSvc() , Rndm::Gauss( a , b ));
    if( !sc.isSuccess() )return sc;
  }
  else if(m_method == "Flat"){
    // Flat random (min, max)
    info() << "---- Method : flat random timing values "<< endreq;
    if(m_params.size() != 2)error() << "wrong parameters size" << endreq;
    a= *(m_params.begin());
    b= *(m_params.begin()+1);
    info() << " min/max = " << a << "/" << b << endreq;
    sc=shoot.initialize(rndmSvc() , Rndm::Flat( a , b ));
    if( !sc.isSuccess() )return sc;
  }  
  else if(m_method == "User"){
    info() << "---- Method : user-defined timing values "<< endreq;
    info() << "Timing value have been defined for " << m_deltas.size() << " cells " << endreq;
    info() << "Default value [" << m_deltas["Default"] <<  "] will be applied to " 
           << m_calo->numberOfCells()- m_deltas.size() << " other cells." << endreq;
    if( m_key == "CellID" ){
      info() << "The timing values are mapped with KEY = CellID " << endreq;
    }
    else if( m_key == "Index" ){
      info() << "The timing values are are mapped with KEY = Index" << endreq;
    }
    else{
      error() << "undefined deltaKey : must be either 'CellID' or 'Index' " << endreq;
    return StatusCode::FAILURE;
    }
  }
  else{
    error() << "Method " << m_method << " unknown - should be 'Flat', 'Gauss' or 'User'"<< endreq;
    return StatusCode::FAILURE;
  }


  // update cellParams

  CaloVector<CellParam>& cells = m_calo->cellParams();
  std::vector<int> cellids,cellind;
  std::vector<double> gains,dgains;
  for(CaloVector<CellParam>::iterator icell = cells.begin() ; icell != cells.end() ; icell++){
    LHCb::CaloCellID id = (*icell).cellID() ;
    if( !m_calo->valid  ( id )  )continue;
    if( m_calo->isPinId( id )   )continue;
    long num = m_calo->cellIndex( id );
    double dt;
    if( m_method == "User" ){
      long index = id.index();
      if( m_key == "Index" )index = num;
      dt = delta( index );
    }
    else{
      dt = shoot();
    }
    debug() << num << " Calibration constant for cellID " << id << " : " << dt << endreq;
    (*icell).setCalibration ( dt ) ; // 

    cellids.push_back( id.index()      );
    cellind.push_back( num             );
    gains.push_back  ( (*icell).gain() );
    dgains.push_back ( (*icell).calibration());
  }
  
  // Ntupling
  Tuple ntp = nTuple(500,m_detectorName + "DeCalib" ,CLID_ColumnWiseTuple);
  int max = m_calo->numberOfCells();
  sc=ntp->farray("cellID"   , cellids  ,"Nchannels",max);
  sc=ntp->farray("index"    , cellind  ,"Nchannels",max);
  sc=ntp->farray("gain"     , gains    ,"Nchannels",max);
  sc=ntp->farray("calib"    , dgains   ,"Nchannels",max);
  sc=ntp->write();
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DeCaloCalib::execute() {

  debug() << "==> Execute" << endmsg;



  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DeCaloCalib::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
