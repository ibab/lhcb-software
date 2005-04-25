//$Id: DAQCondDBAlgorithm.cpp,v 1.6 2005-04-25 11:54:09 marcocle Exp $

#include "DAQCondDBAlgorithm.h"

#include "DetCond/ICondDBAccessSvc.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Timing.h"

#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/types.h"
#include "CoolKernel/ValidityKey.h"

// from POOL
#include "AttributeList/AttributeListSpecification.h"
#include "AttributeList/AttributeList.h"

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
    m_nsInitialized(0),
    m_nsExec(0),
    m_nsDBIO(0),
    m_dbAccSvc(0),
    m_payloadSpec(0)
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

  // Locate the CondDBAccessSvc
  StatusCode sc = serviceLocator()->getService("CondDBAccessSvc",
                                    ICondDBAccessSvc::interfaceID(),(IInterface*&)m_dbAccSvc);
  if (  !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not locate CondDBAccessSvc" << endreq;
    return sc;
  }
  log << MSG::DEBUG << "Retrieved a handle to the CondDBAccessSvc" << endreq;

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

  // Get a pointer to the DB to speed up a bit
  cool::IDatabasePtr &db = m_dbAccSvc->database();

  // Check if root folderSet exists
  log << MSG::DEBUG 
      << "Checking if CondDB root folderSet `"
      << rootName << "' already exists" << endreq;
  bool rootFolderSetExists;
  rootFolderSetExists = db->existsFolderSet( rootName );

  // If root folderSet exists already then do nothing
  // If root folderSet does not exist then create it
  if ( rootFolderSetExists ) {
    log << MSG::DEBUG
	<< "Root folderSet already exists: no need to create it" << endreq;
  } else {      
    log << MSG::INFO 
	<< "Root folderSet does not exist: create it" << endreq;
    try {
      db->createFolderSet(rootName,"Root folderSet for the ConditionsDB");
    } catch (cool::Exception &e) {
      log << MSG::ERROR
          << e << endreq;
      return StatusCode::FAILURE;
    }
    log << MSG::DEBUG
	<< "Root folderSet did not exist and was succesfully created" 
	<< endreq;
  }
 
  log << MSG::DEBUG << "Prepare the AttributeListSpecification" << endmsg;
  m_payloadSpec = new pool::AttributeListSpecification();
  m_payloadSpec->push_back("data","string");

  // Check if DAQ CondDB folder exists
  std::string daqFolderFullName = rootName + "/" + m_daqFolderName;
  log << MSG::DEBUG 
      << "Checking if DAQ CondDB folder `"
      << daqFolderFullName << "' already exists" << endreq;
  log << MSG::DEBUG
      << "" << endreq;
  bool daqFolderExists = db->existsFolder( daqFolderFullName );

  // If DAQ CondDB folder exists already then do nothing
  // If DAQ CondDB folder does not exist then create it
  if ( daqFolderExists ) {
    log << MSG::DEBUG
	<< "DAQ CondDB folder already exists: no need to create it" << endreq;
  } else {      
    log << MSG::INFO 
	<< "DAQ CondDB folder does not exist: create it" << endreq;
    try {
      db->createFolder( daqFolderFullName, *m_payloadSpec,
                        "DAQ folder for the ConditionsDB",
                        cool::FolderVersioning::SINGLE_VERSION, true );
    } catch (cool::Exception &e) {
      log << MSG::ERROR
          << e << endreq;
      return StatusCode::FAILURE;
    }
    
    log << MSG::DEBUG
	<< "DAQ CondDB folder did not exist and was succesfully created" 
	<< endreq;
  }
  
  m_nsInitialized = System::currentTime(System::nanoSec);

  // Initialization completed  
  log << MSG::INFO << "Initialization completed" << endreq;
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode DAQCondDBAlgorithm::execute( ) {

  static cool::ValidityKey val_since_fake = 0;

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Execute()" << endreq;
  m_daqEventNumber++;

  // Start the nsExec timer 
  m_nsExec -= System::currentTime(System::nanoSec);

  // At every event store a new condition
  pool::AttributeList payload(*m_payloadSpec);
  payload["data"].setValue<std::string>(m_daqRecord);

  cool::IDatabasePtr &db = m_dbAccSvc->database();

  std::string daqFolderFullName = rootName + "/" + m_daqFolderName;

  try {
    longlong startIO = System::currentTime(System::nanoSec);
    //    db->getFolder(daqFolderFullName)
    //      ->storeObject(System::currentTime(System::nanoSec)-m_nsInitialized,
    //                    cool::ValidityKeyMax, payload);
    db->getFolder(daqFolderFullName)
      ->storeObject(val_since_fake++,
                    cool::ValidityKeyMax, payload);
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
  } catch (cool::Exception &e) {
    log << MSG::ERROR
        << "Error in storing condition object in the CondDB" << endreq;
    log << MSG::ERROR
        << "*** cool::Exception caught:" << endreq;
    log << MSG::ERROR
        << "*** " << e << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG
	  << "Condition object written to the database " 
	  << endreq;

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

