//$Id: DAQCondDBAlgorithm.cpp,v 1.8 2005-07-14 15:10:22 marcocle Exp $

#include "DAQCondDBAlgorithm.h"

#include "DetDesc/ICondDBAccessSvc.h"

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

// Define the attributes for all folders created in this example
const std::string folderAttributes = "";

//----------------------------------------------------------------------------

/// Constructor
DAQCondDBAlgorithm::DAQCondDBAlgorithm( const std::string& name, 
                                        ISvcLocator* pSvcLocator )
  : GaudiAlgorithm(name, pSvcLocator),
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
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  try {
    // Locate the Database Access Service
    m_dbAccSvc = svc<ICondDBAccessSvc>("CondDBAccessSvc",true);


    // Make sure that the record size is not null
    if ( m_daqRecordSize <= 0 ) {
      warning() << "Invalid size of the DAQ record: " << m_daqRecordSize << endmsg;
      m_daqRecordSize = 1;
      warning() << "DAQ record size set to: " << m_daqRecordSize << endmsg;
    }

    // Build the dummy condition record of the requested size
    long iSize;
    m_daqRecord.reserve(m_daqRecordSize);
    for ( iSize=0; iSize<m_daqRecordSize; iSize++ ) { 
      m_daqRecord += "X";
    }

    // Make sure that test folder name does not contain any "/"
    // TODO
  
    // Get a pointer to the DB to speed up a bit
    cool::IDatabasePtr &db = m_dbAccSvc->database(); 

    debug() << "Prepare the AttributeListSpecification" << endmsg;
    m_payloadSpec = new pool::AttributeListSpecification();
    m_payloadSpec->push_back("data","string");

    // Check if DAQ CondDB folder exists
    std::string daqFolderFullName = "/" + m_daqFolderName;
    debug() << "Checking if DAQ CondDB folder `" << daqFolderFullName << "' already exists" << endmsg;
    bool daqFolderExists = db->existsFolder( daqFolderFullName );

    // If DAQ CondDB folder exists already then do nothing
    // If DAQ CondDB folder does not exist then create it
    if ( daqFolderExists ) {
      debug() << "DAQ CondDB folder already exists: no need to create it" << endmsg;
    } else {      
      info() << "DAQ CondDB folder does not exist: create it" << endmsg;
      try {
        db->createFolder( daqFolderFullName, *m_payloadSpec,
                          "DAQ folder for the ConditionsDB",
                          cool::FolderVersioning::SINGLE_VERSION, true );
      } catch (cool::Exception &e) {
        error() << e << endmsg;
        return StatusCode::FAILURE;
      }
    
      debug() << "DAQ CondDB folder did not exist and was succesfully created" << endmsg;
    }
  
    m_nsInitialized = System::currentTime(System::nanoSec);

  } catch ( GaudiException ) {
    return StatusCode::FAILURE;
  }

  // Initialization completed  
  info() << "Initialization completed" << endmsg;
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode DAQCondDBAlgorithm::execute( ) {

  static cool::ValidityKey val_since_fake = 0;

  debug() << "Execute()" << endmsg;
  ++m_daqEventNumber;

  // Start the nsExec timer 
  m_nsExec -= System::currentTime(System::nanoSec);

  // At every event store a new condition
  pool::AttributeList payload(*m_payloadSpec);
  payload["data"].setValue<std::string>(m_daqRecord);

  cool::IDatabasePtr &db = m_dbAccSvc->database();

  std::string daqFolderFullName = "/" + m_daqFolderName;

  try {
    longlong startIO = System::currentTime(System::nanoSec);
    db->getFolder(daqFolderFullName)
      ->storeObject(System::currentTime(System::nanoSec)-m_nsInitialized,
                    cool::ValidityKeyMax, payload);
    /*
      db->getFolder(daqFolderFullName)
      ->storeObject(val_since_fake++,
      cool::ValidityKeyMax, payload);
    */
    longlong endIO = System::currentTime(System::nanoSec);
    if ( m_daqShowProgress > 0 ) {
      if ( m_daqEventNumber == 
           (m_daqEventNumber/m_daqShowProgress)*m_daqShowProgress ) {
        info() << "Evt # " << m_daqEventNumber << endmsg;
        info() << "Start dbIO at: " << startIO << " ns" << endmsg;
        info() << "End   dbIO at: " << endIO   << " ns" << endmsg;
      }
    }
    m_nsDBIO += (endIO - startIO);
  } catch (cool::Exception &e) {
    error() << "Error in storing condition object in the CondDB" << endmsg;
    error() << "*** cool::Exception caught:" << endmsg;
    error() << "*** " << e << endmsg;
    return StatusCode::FAILURE;
  }
  debug() << "Condition object written to the database " << endmsg;

  // Stop the nsExec timer
  m_nsExec += System::currentTime(System::nanoSec);
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode DAQCondDBAlgorithm::finalize( ) {
  MsgStream log(msgSvc(), name());
  info() << "Finalize()" << endmsg;
  info() << "Total number of events processed: " << m_daqEventNumber << endmsg;
  info() << "DAQ record size in bytes: " << m_daqRecordSize << endmsg;
#ifdef WIN32
  info() << "Accuracy of timing results: 100 ns" << endmsg;
#else
  info() << "Accuracy of timing results: 1 ms" << endmsg;
#endif
  info() << "Total exec time (ms): "
      << m_nsExec / 1e6 << endmsg;
  info() << "Total dbIO time (ms): "
      << m_nsDBIO / 1e6 << endmsg;
  info() << "Total exec time (ms) per event: "
      << float(m_nsExec) / 1e6 / m_daqEventNumber << endmsg;
  info() << "Total dbIO time (ms) per event: "
      << float(m_nsDBIO) / 1e6 / m_daqEventNumber << endmsg;
  info() << "Total exec time (ms) per event record byte: "
      << float(m_nsExec) / 1e6 / m_daqEventNumber / m_daqRecordSize << endmsg;
  info() << "Total dbIO time (ms) per event record byte: "
      << float(m_nsDBIO) / 1e6 / m_daqEventNumber / m_daqRecordSize << endmsg;
  if ( m_nsDBIO > 0 ) {
    info() << "Average dbIO rate (MB/s):   "
           << ( 1e9 / m_nsDBIO ) * m_daqEventNumber * ( m_daqRecordSize / 1e6 ) << endmsg;
    info() << "Average dbIO rate (#evt/s): "
           << ( 1e9 / m_nsDBIO ) * m_daqEventNumber << endmsg;
  } else {
    info() << "Average dbIO rate (MB/s):    "
           << "NOT AVAILABLE" << endmsg;
    info() << "Average dbIO rate (#evts/s): "
           << "NOT AVAILABLE" << endmsg;
  }
  info() << "Finalization completed" << endmsg;
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

