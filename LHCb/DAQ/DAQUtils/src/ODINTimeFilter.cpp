// Include files 

// local
#include "ODINTimeFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ODINTimeFilter
//
// 2009-10-07 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ODINTimeFilter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ODINTimeFilter::ODINTimeFilter( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  // time intervall selector
  declareProperty ( "timeSelector" , m_time = true);
  declareProperty ( "yRange" , m_yRange   );
  declareProperty ( "mRange" , m_mRange   );
  declareProperty ( "dRange" , m_dRange   );
  declareProperty ( "hRange" , m_hRange   );
  declareProperty ( "mnRange" , m_mnRange );
  declareProperty ( "sRange" , m_sRange   );
  declareProperty ( "nsRange", m_nsRange   );
  //event ID intervall selector
  declareProperty ( "eventSelector" , m_evt = false);
  declareProperty ( "eRange" , m_eRange   );
  // BCID interval Range
  declareProperty ( "BCIDSelector" , m_bx = false);
  declareProperty ( "bRange" , m_bRange   );
  //
  declareProperty ( "localTime" , m_loc = true);
  declareProperty ( "eventTime" , m_print = false);

  m_yRange=std::make_pair(-1,-1);
  m_mRange=std::make_pair(-1,-1);
  m_dRange=std::make_pair(-1,-1);
  m_hRange=std::make_pair(-1,-1);
  m_mnRange=std::make_pair(-1,-1);
  m_sRange=std::make_pair(-1,-1);
  m_nsRange=std::make_pair(-1,-1);
  m_eRange=std::make_pair(-1,-1);
  m_bRange=std::make_pair(-1,-1);
  

 }
//=============================================================================
// Destructor
//=============================================================================
ODINTimeFilter::~ODINTimeFilter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ODINTimeFilter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  criteriaPrintOut();  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ODINTimeFilter::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;


  // get ODIN
  LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  if( odin == NULL ) return Error("ODIN cannot be loaded");

  // get time & eventID 
  Gaudi::Time time = odin->eventTime();
  ulonglong event  = odin->eventNumber();
  int       run  = odin->runNumber();
  int       bx  = odin->bunchId();
  

  setFilterPassed( true );

  // timing check
  // WARNING : month from 1->12 (Gaudi::Time uses 0->11)

  Gaudi::Time minTime = Gaudi::Time(val(m_yRange.first   , time.year(m_loc)   ),
                                    val(m_mRange.first   , time.month(m_loc) , -1 ),
                                    val(m_dRange.first   , time.day(m_loc)    ),
                                    val(m_hRange.first   , time.hour(m_loc)    ),
                                    val(m_mnRange.first  , time.minute(m_loc) ),
                                    val(m_sRange.first   , time.second(m_loc) ),
                                    val(m_nsRange.first   , time.nsecond() ),
                                    m_loc);
  
  Gaudi::Time maxTime = Gaudi::Time(val(m_yRange.second   , time.year(m_loc)   ),
                                    val(m_mRange.second   , time.month(m_loc)  , -1),
                                    val(m_dRange.second   , time.day(m_loc)    ),
                                    val(m_hRange.second   , time.hour(m_loc)   ),
                                    val(m_mnRange.second  , time.minute(m_loc) ),
                                    val(m_sRange.second   , time.second(m_loc) ),
                                    val(m_nsRange.second  , time.nsecond() ),
                                    m_loc);
  

  // timing selection
  counter("Event") += 1 ;  
  if( m_time ){
    if (def(m_yRange) || def(m_mRange) || def(m_dRange) || def(m_hRange) ||def(m_mnRange) ||def(m_sRange) || def(m_sRange))
      if( minTime > time || maxTime < time )setFilterPassed(false);
  }
  if( m_evt ){
    if( !check( event            , m_eRange  ))setFilterPassed(false);
  }
  if( m_bx ){
    if( !check( bx               , m_bRange  ))setFilterPassed(false);
  }

  if(filterPassed())counter("Filtered Events") += 1 ;


  if(m_print)info() << "[Run : " << run  
                    << ", EventId  : " << event 
                    << ", BCID : "<< bx << "]"
                    << " @ EventTime : "  
                    << time.year(m_loc) << "/"  
                    << time.month(m_loc) +1 << "/"  
                    << time.day(m_loc) << " "  
                    << time.hour(m_loc) << ":"  
                    << time.minute(m_loc) << ":"  
                    << time.second(m_loc) << "::"
                    << time.nsecond()
                    << " (accepted :  " << filterPassed()  << ")" << endmsg;
  return StatusCode::SUCCESS;
}


int ODINTimeFilter::val(int v1,int v2,int v3){
  if( v1 < 0)return  v2;
  return  (v1+v3) ;
}

bool ODINTimeFilter::def(std::pair<double,double> range){
  return (range.first>=0 && range.second>=0);
}
bool ODINTimeFilter::def(std::pair<int,int> range){
  return (range.first>=0 && range.second>=0);
}

void ODINTimeFilter::criteriaPrintOut(){
  bool ok1 = true;
  bool ok2 = true;
  bool ok3 = true;
  if (!def(m_yRange) &&
      !def(m_mRange) && 
      !def(m_dRange) && 
      !def(m_hRange) && 
      !def(m_mnRange) && 
      !def(m_sRange) &&
      !def(m_nsRange) )ok1 = false;

  if (!def(m_eRange) )ok2 = false; 
  if (!def(m_bRange) )ok3 = false; 

  if( !ok1 && !ok2 && !ok3)info() << "Empty filtering criteria - all events passing " << endmsg;
  else{
    if( !ok1 )  info() << "No event intervall defined" << endmsg;
    if( !ok2 )  info() << "No time intervall defined" << endmsg;
    if( !ok3 )  info() << "No BCID intervall defined" << endmsg;
    info() << "Filter event in : " << endmsg;
    if(def(m_yRange))info()  << "year    Range ["   << m_yRange.first  << ","<<m_yRange.second <<"]"<< endmsg;
    if(def(m_mRange))info()  << "month   Range ["   << m_mRange.first  << ","<<m_mRange.second <<"]"<< endmsg;
    if(def(m_dRange))info()  << "day     Range ["   << m_dRange.first  << ","<<m_dRange.second <<"]"<< endmsg;
    if(def(m_hRange))info()  << "hour    Range ["   << m_hRange.first  << ","<<m_hRange.second <<"]"<< endmsg;
    if(def(m_mnRange))info()  << "minute  Range ["   << m_mnRange.first << ","<<m_mnRange.second<<"]"<< endmsg;
    if(def(m_sRange))info()  << "second  Range ["   << m_sRange.first  << ","<<m_sRange.second <<"]"<< endmsg;
    if(def(m_sRange))info()  << "nsecond  Range ["   << m_nsRange.first  << ","<<m_nsRange.second <<"]"<< endmsg;
    if(def(m_eRange))info()  << "eventID    Range [" << m_eRange.first  << ","<<m_eRange.second <<"]"<< endmsg;
    if(def(m_bRange))info()  << "BCID    Range [" << m_bRange.first  << ","<<m_bRange.second <<"]"<< endmsg;
  }
}

bool ODINTimeFilter::check(ulonglong val, std::pair<double,double> range){
  if( !def(range) )return true;
  if( val >= (ulonglong) range.first && val <= (ulonglong) range.second )return true;
  return false;
}
    
bool ODINTimeFilter::check(int val, std::pair<int,int> range){
  if( !def(range) )return true;
  if( val >= range.first && val <=  range.second )return true;
  return false;
}
    

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ODINTimeFilter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  criteriaPrintOut();
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
  
  //=============================================================================
