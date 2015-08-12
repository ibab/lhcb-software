// Include files 
 
// CondDB
#include "GaudiKernel/IDetDataSvc.h"

// local
#include "LumiCheckCondDB.h"
#include "GetLumiParameters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiCheckCondDB
// 
// 2009-02-27 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiCheckCondDB )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiCheckCondDB::LumiCheckCondDB( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_databaseTool(0),
    m_incSvc(0)
{
  // expect the data to be written at LHCb::LumiFSRLocation::Default
  declareProperty( "StartTime"          , m_startTime         = 1269817249  );
  declareProperty( "NumberSteps"        , m_numberSteps       = 80  );
  declareProperty( "StepHours"          , m_stepHours         = 72  );

}
//=============================================================================
// Destructor
//=============================================================================
LumiCheckCondDB::~LumiCheckCondDB() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiCheckCondDB::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  // prepare database tool
  m_databaseTool = tool<IGetLumiParameters>( "GetLumiParameters" , "lumiDatabaseTool" ); // public

  // get the detectorDataSvc
  m_dds = detSvc();
  if (m_dds == NULL) {
    return StatusCode::SUCCESS;
  }

  // incident service
  m_incSvc = svc<IIncidentSvc> ( "IncidentSvc" , true );
  
  //check extended file incidents are defined
#ifdef GAUDI_FILE_INCIDENTS
  m_incSvc->addListener( this, IncidentType::BeginInputFile);
  m_incSvc->addListener( this, IncidentType::EndInputFile);
  if ( msgLevel(MSG::DEBUG) ) debug() << "registered with incSvc" << endmsg;
  //if not then the counting is not reliable
#else
  warn() << "cannot register with incSvc" << endmsg;
#endif //GAUDI_FILE_INCIDENTS

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiCheckCondDB::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // check the DB in execution mode
  checkDB("execute").ignore();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Stop
//=============================================================================
StatusCode LumiCheckCondDB::stop() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Stop" << endmsg;

  // check the DB in stop mode
  checkDB("stop").ignore();

  return GaudiAlgorithm::stop();  // must be called after all other actions
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LumiCheckCondDB::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  // check the DB in finalize mode
  // StatusCode rc = checkDB("finalize");

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

// ==========================================================================
// IIncindentListener interface
// ==========================================================================
void LumiCheckCondDB::handle( const Incident& incident )
{
  //check extended file incidents are defined
#ifdef GAUDI_FILE_INCIDENTS
  if(incident.type()==IncidentType::BeginInputFile)
  {
    if ( msgLevel(MSG::DEBUG) ) debug() << "==> Incident: BeginInputFile" << endmsg;
    // check the DB in file-open mode
    checkDB("BeginInputFile").ignore();
  }
#endif

}

//=============================================================================
// check DB in event loop
//=============================================================================
StatusCode LumiCheckCondDB::checkDB(std::string state) {
  info() << "========== Probe DB: START ========== " << state << endmsg;

  unsigned long long xt0 = m_startTime;
  for ( unsigned long long ixt = 0; ixt < m_numberSteps; ixt++ ) {
    unsigned long long xt = ixt*3600*m_stepHours + xt0;
    info() << "Generate event at unix time: " << xt << endmsg;
    
    // set event time and trigger event
    Gaudi::Time gxt = xt * 1000*1000*1000;
    m_dds->setEventTime( gxt );
    if ( updMgrSvc()->newEvent(gxt).isFailure() ) error() << "ERROR newEvent " << endmsg;
    if ( m_dds->eventTime() != gxt ) error() << "time read back: " << m_dds->eventTime() << endmsg;
    Gaudi::Time xtfound = m_dds->eventTime();
    info() << "Event time: " << xtfound.format(true, "%Y-%m-%d %H:%M") << endmsg;
  
    // look at the new DB parameters
    unsigned int toolTCK =  m_databaseTool->getTCK();
    info() << " TCK:           " << toolTCK ;
    double toolOdinFraction = m_databaseTool->OdinFraction();
    info() << " OdinFraction:  " << toolOdinFraction ;
    double toolRandomRate = m_databaseTool->HLTRandomRate();
    info() << " Randomrate:    " << toolRandomRate ;
    double toolFrequency = m_databaseTool->LHCFrequency();
    info() << " LHCfrequency:  " << toolFrequency ;
    double toolRateBB = m_databaseTool->RandomRateBB();
    info() << " BB RandomRate: " << toolRateBB ;
    long toolBunches = m_databaseTool->CollidingBunches();
    info() << " bunches:       " << toolBunches ;
    double calibScale = m_databaseTool->CalibScale();
    info() << " absolutescale: " << calibScale << endmsg;
  }
  info() << "========== Probe DB: END   ==========" << endmsg;

  return StatusCode::SUCCESS;
}
