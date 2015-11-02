// Include files

// from Gaudi
// needed to sleep between two operations
#include "GaudiKernel/Sleep.h"

#include "DetCond/ICondDBReader.h"
#include <fstream>

// local
#include "CondDBReplayAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBReplayAlg
//
// Jan 25, 2008 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CondDBReplayAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBReplayAlg::CondDBReplayAlg( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_reader(NULL)
{
  declareProperty("Reader",  m_readerName = "CondDBCnvSvc",
                  "Name of the reader to use to replay the requests.");
  declareProperty("LogFile", m_logFileName = "",
                  "Path to the log file to re-play. "
                  "If not specified or set to empty, the file name is set from "
                  "the name of the instance plus '.log'." );
}
//=============================================================================
// Destructor
//=============================================================================
CondDBReplayAlg::~CondDBReplayAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode CondDBReplayAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "==> Initialize" << endmsg;

  const bool create = true;
  m_reader = svc<ICondDBReader>(m_readerName,create);

  // Open the input file.
  std::unique_ptr<std::istream> logFile(new std::ifstream(m_logFileName.c_str()));
  if ( ! logFile->good() ) {
    error() << "Problems opening " << m_logFileName << endmsg;
    return StatusCode::FAILURE;
  }
  info() << "File '" << m_logFileName << "' opened for reading." << endmsg;

  // Parse the input file
  std::string opcode, tmp;
  operation_t operation;
  Gaudi::Time last_time;
  Gaudi::Time::ValueType tmptime;
  while ( ! logFile->eof() ) {

    (*logFile) >> opcode;
    if ("GET:" == opcode || "GCN:" == opcode) { // we use this operation...

      operation.use_numeric_channel = ("GET:" == opcode);

      (*logFile) >> tmptime; operation.time = Gaudi::Time(tmptime);
      (*logFile) >> operation.node;
      (*logFile) >> tmptime; operation.evttime = Gaudi::Time(tmptime);

      if (operation.use_numeric_channel) {
        (*logFile) >> operation.channel;
      }
      else {
        (*logFile) >> operation.chn_name;
      }

      if ( last_time > operation.time ) {
        error() << "Error in the log file: the operation time is not strictly increasing";
        return StatusCode::FAILURE;
      }

      //info() << operation.time.ns() << " " << operation.node << " "  << operation.evttime.ns() << " "  << operation.channel << endmsg;
      last_time = operation.time;
      m_operations.push_back(operation);
    }
    // skip the rest of the line
    std::getline(*logFile,tmp);
  }
  info() << "Found " << m_operations.size() << " operations to replay." << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CondDBReplayAlg::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "==> Execute" << endmsg;

  info() << "Replaying database operations ..." << endmsg;
  ICondDBReader::DataPtr data;
  std::string descr;
  Gaudi::Time since, until;
  // replay the operations
  Gaudi::Time last_optime, last_time;
  bool first = true;
  for(list_t::iterator op = m_operations.begin(); op != m_operations.end(); ++op) {

    if ( first ) {
      // we do not have to wait for the first operation
      first = false;
    } else {
      // calculate how much we have to sleep
      Gaudi::Time::ValueType ns_to_sleep = (op->time.ns() - last_optime.ns()) // time between operations
        - (Gaudi::Time::current().ns() - last_time.ns()); // time wasted

      if ( ns_to_sleep > 0 ) Gaudi::NanoSleep(ns_to_sleep);
    }

    last_optime = op->time;

    // I have to store the current time before the operation otherwise
    // we to not count the time that the operation takes as already elapsed.
    last_time = Gaudi::Time::current();

    // Get the object
    if (op->use_numeric_channel) {
      m_reader->getObject(op->node,op->evttime,data,descr,since,until,op->channel).ignore();
    }
    else {
      m_reader->getObject(op->node,op->evttime,data,descr,since,until,op->chn_name).ignore();
    }

  }
  info() << "Replay completed." << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CondDBReplayAlg::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
