//$Id: DAQCondDBAlgorithm.cpp,v 1.3 2004-12-08 18:50:51 marcocle Exp $

#include "DAQCondDBAlgorithm.h"

#include "DetCond/IConditionsDBCnvSvc.h"
#include "DetCond/IConditionsDBGate.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Timing.h"

#ifdef __CondDBOracle__
#include "ConditionsDB/CondDBOracleObjectFactory.h"
#endif

#ifdef __CondDBMySQL__
#include "ConditionsDB/CondDBMySQLObjectFactory.h"
#endif

#include "ConditionsDB/ICondDBMgr.h"
#include "ConditionsDB/ICondDBBasicFolderMgr.h"
#include "ConditionsDB/ICondDBBasicDataAccess.h"

/// Instantiation of a static factory to create instances of this algorithm
static const AlgFactory<DAQCondDBAlgorithm> Factory;
const IAlgFactory& DAQCondDBAlgorithmFactory = Factory;

// Define the name of the root folderset
// This used to be "/" in Objectivity.
// In the Oracle implementation it is required to have a name "/XXX"
const std::string rootName = "/CONDDB";  

// Define the attributes for all folders created in this example
const std::string folderAttributes = "";

//----------------------------------------------------------------------------

/// Constructor
DAQCondDBAlgorithm::DAQCondDBAlgorithm( const std::string& name, 
                                        ISvcLocator* pSvcLocator )
  : Algorithm(name, pSvcLocator),
    m_daqRecord(""),
    m_daqEventNumber(0),
    m_nsExec(0),
    m_nsDBIO(0),
    m_condDBMgr(0)
{
  declareProperty( "daqFolderName",    m_daqFolderName   = "DAQ" );
  declareProperty( "daqRecordSize",    m_daqRecordSize   = 10    );
  declareProperty( "daqShowProgress",  m_daqShowProgress = 0     );
}

//----------------------------------------------------------------------------

/// Initialization of the algorithm. 
/// Get CondDB folder name and string size.
/// Create the CondDB folders where DAQ data will be stored.

StatusCode DAQCondDBAlgorithm::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize()" << endreq;

  // Locate the ConditionsDBCnvSvc
  // Get a handle to the ConditionsDBGate
  // Get a handle to the CondDBMgr
  StatusCode sc = serviceLocator()->service 
    ( "ConditionsDBCnvSvc", m_condDBCnvSvc );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't locate ConditionsDBCnvSvc" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Succesfully located ConditionDBCnvSvc" << endreq;
  }
  m_condDBMgr = m_condDBCnvSvc->conditionsDBGate()->condDBManager();
  log << MSG::DEBUG << "Retrieved a handle to the CondDBMgr" << endreq;

  // Get properties from the JobOptionsSvc
  sc = setProperties();
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not set jobOptions properties" << endreq;
    return sc;
  }
  log << MSG::DEBUG << "Properties were read from jobOptions" << endreq;
  log << MSG::INFO << "Name of the DAQ CondDB folder: "
      << m_daqFolderName << endreq;
  log << MSG::INFO << "Size of the DAQ CondDB record: "
      << m_daqRecordSize << endreq;

  // Make sure that the record size is not null
  if ( m_daqRecordSize <= 0 ) {
	log << MSG::DEBUG << "Invalid size of the DAQ record: "
        << m_daqRecordSize << endreq;
    m_daqRecordSize = 1;
  }

  // Build the dummy condition record of the requested size
  long iSize;
  for ( iSize=0; iSize<m_daqRecordSize; iSize++ ) { 
    m_daqRecord += "X";
  }
  //log << MSG::VERBOSE << "DAQ record: " << m_daqRecord << endreq;

  // Make sure that test folder name does not contain any "/"
  // TODO

  // Check if root folderSet exists
  log << MSG::DEBUG 
      << "Checking if CondDB root folderSet `"
      << rootName << "' already exists" << endreq;
  bool rootFolderSetExists;
  try {
    m_condDBMgr->startRead();
    rootFolderSetExists = m_condDBMgr->getCondDBBasicFolderMgr()->exist( rootName );
    m_condDBMgr->commit();
  } catch (CondDBException &e) {
    log << MSG::ERROR
	<< "Could not check existence of root folderSet" << endreq;
    log << MSG::ERROR
	<< "*** ConditionsDB exception caught:"          << endreq;
    log << MSG::ERROR
	<< "***  error message: " << e.getMessage()      << endreq;
    log << MSG::ERROR
	<< "***  error code:    " << e.getErrorCode()    << endreq;
    return StatusCode::FAILURE;
  } 

  // If root folderSet exists already then do nothing
  // If root folderSet does not exist then create it
  if ( rootFolderSetExists ) {
    log << MSG::DEBUG
	<< "Root folderSet already exists: no need to create it" << endreq;
  } else {      
    log << MSG::INFO 
	<< "Root folderSet does not exist: create it" << endreq;
    try {
      m_condDBMgr->startUpdate();
      m_condDBMgr->getCondDBBasicFolderMgr()->createCondDBFolderSet( 
        rootName,
	    folderAttributes,
	    "Root folderSet for the ConditionsDB",
	    true ); 
      m_condDBMgr->commit();    
    } catch (CondDBException &e) {
      log << MSG::ERROR
	  << "Error in creating root folderSet in the CondDB" << endreq;
      log << MSG::ERROR
	  << "*** ConditionsDB exception caught:"             << endreq;
      log << MSG::ERROR
	  << "***  error message: " << e.getMessage()         << endreq;
      log << MSG::ERROR
	  << "***  error code:    " << e.getErrorCode()       << endreq;
      return StatusCode::FAILURE;
    }
    log << MSG::DEBUG
	<< "Root folderSet did not exist and was succesfully created" 
	<< endreq;
  }
 
  // Check if DAQ CondDB folder exists
  std::string daqFolderFullName = rootName + "/" + m_daqFolderName;
  log << MSG::DEBUG 
      << "Checking if DAQ CondDB folder `"
      << daqFolderFullName << "' already exists" << endreq;
  log << MSG::DEBUG
      << "" << endreq;
  bool daqFolderExists;
  try {
    m_condDBMgr->startRead();
    daqFolderExists = m_condDBMgr->getCondDBBasicFolderMgr()->exist( daqFolderFullName );
    m_condDBMgr->commit();
  } catch (CondDBException &e) {
    log << MSG::ERROR
	<< "Could not check existence of DAQ CondDB folder" << endreq;
    log << MSG::ERROR
	<< "*** ConditionsDB exception caught:"             << endreq;
    log << MSG::ERROR
	<< "***  error message: " << e.getMessage()         << endreq;
    log << MSG::ERROR
	<< "***  error code:    " << e.getErrorCode()       << endreq;
    return StatusCode::FAILURE;
  } 

  // If DAQ CondDB folder exists already then do nothing
  // If DAQ CondDB folder does not exist then create it
  if ( daqFolderExists ) {
    log << MSG::DEBUG
	<< "DAQ CondDB folder already exists: no need to create it" << endreq;
  } else {      
    log << MSG::INFO 
	<< "DAQ CondDB folder does not exist: create it" << endreq;
    try {
      m_condDBMgr->startUpdate();
      m_condDBMgr->getCondDBBasicFolderMgr()->createCondDBFolder( 
        daqFolderFullName,
	    folderAttributes,
	    "DAQ folder for the ConditionsDB",
	    true ); 
      m_condDBMgr->commit();    
    } catch (CondDBException &e) {
      log << MSG::ERROR
	  << "Error in creating DAQ CondDB folder in the CondDB" << endreq;
      log << MSG::ERROR
	  << "*** ConditionsDB exception caught:"                << endreq;
      log << MSG::ERROR
	  << "***  error message: " << e.getMessage()            << endreq;
      log << MSG::ERROR
	  << "***  error code:    " << e.getErrorCode()          << endreq;
      return StatusCode::FAILURE;
    }
    log << MSG::DEBUG
	<< "DAQ CondDB folder did not exist and was succesfully created" 
	<< endreq;
  }
  	
  // Initialization completed  
  log << MSG::INFO << "Initialization completed" << endreq;
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode DAQCondDBAlgorithm::execute( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Execute()" << endreq;
  m_daqEventNumber++;

  // Start the nsExec timer 
  m_nsExec -= System::currentTime(System::nanoSec);

  // At every event store a new condition
  ICondDBObject* condObject;
  std::string daqFolderFullName = rootName + "/" + m_daqFolderName;
  switch (m_condDBCnvSvc->conditionsDBGate()->implementationCode()){
#ifdef __CondDBOracle__
  case ConditionsDBGateImplementation::CONDDBORACLE :
    condObject = CondDBOracleObjectFactory::createCondDBObject
      ( System::currentTime(System::nanoSec), CondDBplusInf, m_daqRecord, "");
      break;
#endif
#ifdef __CondDBMySQL__
  case ConditionsDBGateImplementation::CONDDBMYSQL :
    condObject = CondDBMySQLObjectFactory::createCondDBObject
      ( System::currentTime(System::nanoSec), CondDBplusInf, m_daqRecord, "");
    break;
#endif
  default:
    log << MSG::ERROR << "I cannot use the ConditionsDB implementation '" <<
      m_condDBCnvSvc->conditionsDBGate()->implementation() << "'" << endmsg;
    return StatusCode::FAILURE;
  }
  
  try {
    longlong startIO = System::currentTime(System::nanoSec);
    m_condDBMgr->startUpdate();
    m_condDBMgr->getCondDBBasicDataAccess()->storeCondDBObject
      ( daqFolderFullName, condObject );
    m_condDBMgr->commit();
    longlong endIO = System::currentTime(System::nanoSec);
    if ( m_daqShowProgress > 0 ) {
      if ( m_daqEventNumber == 
	   (m_daqEventNumber/m_daqShowProgress)*m_daqShowProgress ) {
	log << MSG::INFO << "Evt # " << m_daqEventNumber << endreq;
	log << MSG::INFO << "Start dbIO at: " << startIO << " ns" << endreq;
	log << MSG::INFO << "End   dbIO at: " << endIO   << " ns" << endreq;
      }
    }
    m_nsDBIO += (endIO - startIO);
  } catch (CondDBException &e) {
    log << MSG::ERROR
        << "Error in storing condition object in the CondDB" << endreq;
    log << MSG::ERROR
	    << "*** ConditionsDB exception caught:"              << endreq;
    log << MSG::ERROR
        << "***  error message: " << e.getMessage()          << endreq;
    log << MSG::ERROR
	    << "***  error code:    " << e.getErrorCode()        << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG
	  << "Condition object written to the database " 
	  << endreq;
  switch (m_condDBCnvSvc->conditionsDBGate()->implementationCode()){
#ifdef __CondDBOracle__
  case ConditionsDBGateImplementation::CONDDBORACLE :
    CondDBOracleObjectFactory::destroyCondDBObject(condObject);
      break;
#endif
#ifdef __CondDBMySQL__
  case ConditionsDBGateImplementation::CONDDBMYSQL :
    CondDBMySQLObjectFactory::destroyCondDBObject(condObject);
    break;
#endif
  default:
    log << MSG::ERROR << "I cannot use the ConditionsDB implementation '" <<
      m_condDBCnvSvc->conditionsDBGate()->implementation() << "'" << endmsg;
  }
  
  // Stop the nsExec timer
  m_nsExec += System::currentTime(System::nanoSec);
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode DAQCondDBAlgorithm::finalize( ) {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Finalize()" << endreq;
  log << MSG::INFO << "Total number of events processed: "
      << m_daqEventNumber << endreq;
  log << MSG::INFO << "DAQ record size in bytes: "
      << m_daqRecordSize << endreq;
#ifdef WIN32
  log << MSG::INFO << "Accuracy of timing results: 100 ns" << endreq;
#else
  log << MSG::INFO << "Accuracy of timing results: 1 ms" << endreq;
#endif
  log << MSG::INFO << "Total exec time (ms): "
      << m_nsExec / 1e6 << endreq;
  log << MSG::INFO << "Total dbIO time (ms): "
      << m_nsDBIO / 1e6 << endreq;
  log << MSG::INFO << "Total exec time (ms) per event: "
      << float(m_nsExec) / 1e6 / m_daqEventNumber << endreq;
  log << MSG::INFO << "Total dbIO time (ms) per event: "
      << float(m_nsDBIO) / 1e6 / m_daqEventNumber << endreq;
  log << MSG::INFO << "Total exec time (ms) per event record byte: "
      << float(m_nsExec) / 1e6 / m_daqEventNumber / m_daqRecordSize << endreq;
  log << MSG::INFO << "Total dbIO time (ms) per event record byte: "
      << float(m_nsDBIO) / 1e6 / m_daqEventNumber / m_daqRecordSize << endreq;
  if ( m_nsDBIO > 0 ) {
    log << MSG::INFO << "Average dbIO rate (MB/s):   "
	<< ( 1e9 / m_nsDBIO ) * m_daqEventNumber * ( m_daqRecordSize / 1e6 )
	<< endreq;
    log << MSG::INFO << "Average dbIO rate (#evt/s): "
	<< ( 1e9 / m_nsDBIO ) * m_daqEventNumber
	<< endreq;
  } else {
    log << MSG::INFO << "Average dbIO rate (MB/s):    "
	<< "NOT AVAILABLE" << endreq;
    log << MSG::INFO << "Average dbIO rate (#evts/s): "
	<< "NOT AVAILABLE" << endreq;
  }
  log << MSG::INFO << "Finalization completed" << endreq;
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

