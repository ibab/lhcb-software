// Include files

// from Gaudi
#include "GaudiKernel/StdArrayAsProperty.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
// local
#include "DeCaloTiming.h"
//-----------------------------------------------------------------------------
// Implementation file for class : DeCaloTiming
//
// Simple algorithm to add time shift  (user-defined or Gaussian or flat randomly
// distributed) at the DeCalorimeter initialisation level
// Useful to produce time misalignment (in Gauss) and check/develop alignment procedure
//
// 2007-08-22 : Olivier DESCHAMPS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DeCaloTiming )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeCaloTiming::DeCaloTiming( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty( "Method"         , m_method = "Flat"); // Flat/Gauss/User
  declareProperty( "Params"         , m_params = { 0, 0 });
  declareProperty( "Key"            , m_key = "CellID" );
  declareProperty( "deltaTime"      , m_deltas);

  m_deltas["Default"] = 0.0 * Gaudi::Units::ns ;

  // set default detectorName
  auto idx = name.find_last_of(".");
  if (idx == std::string::npos) idx = 0;
  else                          ++idx;
  m_detectorName = name.substr( idx, 4 );
  if ( name.substr(idx,3) == "Prs" ) m_detectorName = "Prs";
  if ( name.substr(idx,3) == "Spd" ) m_detectorName = "Spd";

  declareProperty( "DetectorName"   , m_detectorName );

}
//=============================================================================
// Initialization
//=============================================================================
StatusCode DeCaloTiming::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  //get DeCalorimeter
  if ( "Ecal" == m_detectorName ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  } else if ( "Hcal" == m_detectorName ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
  } else if ( "Prs" == m_detectorName ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
  } else if ( "Spd" == m_detectorName ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd );
  } else {
    error() << "Unknown Calo detector name " << m_detectorName << endmsg;
    return StatusCode::FAILURE;
  }

  info() << " ======= SIMULATING THE MISALIGNMENT of "<< m_detectorName << " TIMING  ======= " << endmsg;


  // Params
  auto rndmSvc = service<IRndmGenSvc>( "RndmGenSvc" , true );
  Rndm::Numbers shoot;

  if( m_method == "Gauss" ){
    // Gaussian random (mean, rms)
    info() << "---- Method : gaussian random timing values "<< endmsg;
    double a = m_params[0];
    double b = m_params[1];
    info() << " mean/sigma = " << a << "/" << b << endmsg;
    sc = shoot.initialize(rndmSvc , Rndm::Gauss( a , b ));
    if( !sc.isSuccess() )return sc;
  } else if(m_method == "Flat"){
    // Flat random (min, max)
    info() << "---- Method : flat random timing values "<< endmsg;
    double a= m_params[0];
    double b= m_params[1];
    info() << " min/max = " << a << "/" << b << endmsg;
    sc=shoot.initialize(rndmSvc , Rndm::Flat( a , b ));
    if( !sc.isSuccess() )return sc;
  }  else if(m_method == "User"){
    info() << "---- Method : user-defined timing values "<< endmsg;
    info() << "Timing value have been defined for " << m_deltas.size() << " cells " << endmsg;
    info() << "Default value [" << m_deltas["Default"] <<  "] will be applied to "
           << m_calo->numberOfCells()- m_deltas.size() << " other cells." << endmsg;
    if( m_key == "CellID" ){
      info() << "The timing values are mapped with KEY = CellID " << endmsg;
    }
    else if( m_key == "Index" ){
      info() << "The timing values are are mapped with KEY = Index" << endmsg;
    }
    else{
      error() << "undefined deltaKey : must be either 'CellID' or 'Index' " << endmsg;
    return StatusCode::FAILURE;
    }
  } else{
    error() << "Method " << m_method << " unknown - should be 'Flat', 'Gauss' or 'User'"<< endmsg;
    return StatusCode::FAILURE;
  }


  // update cellParams

  auto& cells = const_cast<CaloVector<CellParam>&>( m_calo->cellParams() ); // no-const conversion
  std::vector<int> cellids,cellind;
  std::vector<double> times,dtimes;
  for(auto& icell : cells ) {
    LHCb::CaloCellID id = icell.cellID() ;
    if( !m_calo->valid( id ) || m_calo->isPinId( id ) ) continue;
    long num = m_calo->cellIndex( id );
    double dt = ( m_method != "User" ? shoot()
                                     : delta( m_key != "Index" ? id.index() : num ) );
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << num << " Delta time for cellID " << id << " : " << dt << endmsg;
    icell.setDeltaTime ( dt ) ; // add delta time (ns)
    cellids.push_back( id.index()      );
    cellind.push_back( num             );
    times.push_back  ( icell.time() );
    dtimes.push_back ( icell.deltaTime());
  }

  // Ntupling
  Tuple ntp = nTuple(500,m_detectorName + "DeTiming" ,CLID_ColumnWiseTuple);
  auto max = m_calo->numberOfCells();
  ntp->farray("cellID", cellids  ,"Nchannels",max).ignore();
  ntp->farray("index" , cellind  ,"Nchannels",max).ignore();
  ntp->farray("time"  , times    ,"Nchannels",max).ignore();
  ntp->farray("dtime" , dtimes   ,"Nchannels",max).ignore();
  return ntp->write();
}
