// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Time.h"

#include <fstream>

// local
#include "CondDBLogger.h"

// Factory implementation
DECLARE_SERVICE_FACTORY(CondDBLogger)

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBLogger
//
// 2008-01-24 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBLogger::CondDBLogger( const std::string& name, ISvcLocator* svcloc ):
  base_class(name,svcloc), m_loggedReader(0), m_logFile(0) {

  declareProperty("LoggedReader",  m_loggedReaderName = "",
                  "Fully qualified name of the ICondDBReader to which the calls"
                  " have to be forwarded.");
  declareProperty("LogFile",       m_logFileName      = "",
                  "Path to the log file (it is overwritten if it exists). "
                  "If not specified or set to empty, the file name is set from "
                  "the name of the instance plus '.log'." );

}
//=============================================================================
// Destructor
//=============================================================================
CondDBLogger::~CondDBLogger() {}

//=============================================================================
// initialize
//=============================================================================
StatusCode CondDBLogger::initialize(){
  StatusCode sc = base_class::initialize();
  if (sc.isFailure()) return sc;

  MsgStream log(msgSvc(), name() );
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Initialize" << endmsg;

  if ( m_loggedReaderName.empty() ){
    log << MSG::ERROR << "Property LoggedReader is not set." << endmsg;
    return StatusCode::FAILURE;
  }

  // locate the CondDBReader
  sc = service(m_loggedReaderName,m_loggedReader,true);
  if (  !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not locate " << m_loggedReaderName << endmsg;
    return sc;
  }
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Retrieved '" << m_loggedReaderName  << "'" << endmsg;

  // Set the default value of the file name if not specified.
  if ( m_logFileName.empty() ){
    m_logFileName = name() + ".log";
    log << MSG::INFO << "Property LogFile not specified, using '"
                     << m_logFileName << "'" << endmsg;
  }

  // Open the output file and start writing.
  m_logFile =  std::auto_ptr<std::ostream>(new std::ofstream(m_logFileName.c_str()));
  if ( ! m_logFile->good() ) {
    log << MSG::ERROR << "Problems opening " << m_logFileName << endmsg;
    return StatusCode::FAILURE;
  }
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "File '" << m_logFileName << "' opened for writing." << endmsg;

  (*m_logFile) << "INI: " << Gaudi::Time::current().ns() << " " << name() << " logging " << m_loggedReaderName << std::endl;

  return sc;
}

//=============================================================================
// finalize
//=============================================================================
StatusCode CondDBLogger::finalize(){
  MsgStream log(msgSvc(), name() );
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Finalize" << endmsg;

  if ( m_loggedReader ) {
    m_loggedReader->release();
    m_loggedReader = 0;
  }

  if (m_logFile.get()) {
    (*m_logFile) << "FIN: " << Gaudi::Time::current().ns() << " " << name() << std::endl;
    m_logFile.reset(0);
  }

  return base_class::finalize();
}

//=========================================================================
//  retrieve an object
//=========================================================================
StatusCode CondDBLogger::getObject (const std::string &path, const Gaudi::Time &when,
                                    DataPtr &data,
                                    std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel)
{
  if ( m_loggedReader ) {
    (*m_logFile) << "GET: " << Gaudi::Time::current().ns() << " "
                            << path << " " << channel << " "
                            << when.ns() << " " << std::flush;
    StatusCode sc = m_loggedReader->getObject(path,when,data,descr,since,until,channel);
    (*m_logFile) << sc << std::endl;
    return sc;
  }
  return StatusCode::FAILURE;
}
StatusCode CondDBLogger::getObject (const std::string &path, const Gaudi::Time &when,
                                    DataPtr &data,
                                    std::string &descr, Gaudi::Time &since, Gaudi::Time &until, const std::string &channel)
{
  if ( m_loggedReader ) {
    (*m_logFile) << "GCN: " << Gaudi::Time::current().ns() << " "
                            << path << " " << channel << " "
                            << when.ns() << " " << std::flush;
    StatusCode sc = m_loggedReader->getObject(path,when,data,descr,since,until,channel);
    (*m_logFile) << sc << std::endl;
    return sc;
  }
  return StatusCode::FAILURE;
}

ICondDBReader::IOVList CondDBLogger::getIOVs(const std::string & path, const IOV &iov, cool::ChannelId channel)
{
  if ( m_loggedReader ) {
    (*m_logFile) << "IOV: " << Gaudi::Time::current().ns() << " "
                            << path << " " << channel << " "
                            << iov.since.ns() << " " << iov.until.ns() << std::endl;
    return m_loggedReader->getIOVs(path, iov, channel);
  }
  return ICondDBReader::IOVList();
}

ICondDBReader::IOVList CondDBLogger::getIOVs(const std::string & path, const IOV &iov, const std::string & channel)
{
  if ( m_loggedReader ) {
    (*m_logFile) << "ICN: " << Gaudi::Time::current().ns() << " "
                            << path << " " << channel << " "
                            << iov.since.ns() << " " << iov.until.ns() << std::endl;
    return m_loggedReader->getIOVs(path, iov, channel);
  }
  return ICondDBReader::IOVList();
}

//=========================================================================
//  get the list of child nodes of a folderset
//=========================================================================
StatusCode CondDBLogger::getChildNodes (const std::string &path, std::vector<std::string> &node_names)
{
  if ( m_loggedReader ) {
    (*m_logFile) << "GCH: " << Gaudi::Time::current().ns() << " " << path <<  " " << std::flush;
    StatusCode sc = m_loggedReader->getChildNodes(path,node_names);
    (*m_logFile) << sc << std::endl;
    return sc;
  }
  return StatusCode::FAILURE;
}

//=========================================================================
//  get the list of child nodes of a folderset
//=========================================================================
StatusCode CondDBLogger::getChildNodes (const std::string &path,
                                        std::vector<std::string> &folders,
                                        std::vector<std::string> &foldersets)
{
  if ( m_loggedReader ) {
    (*m_logFile) << "GCH: " << Gaudi::Time::current().ns() << " " << path <<  " " << std::flush;
    StatusCode sc = m_loggedReader->getChildNodes(path,folders,foldersets);
    (*m_logFile) << sc << std::endl;
    return sc;
  }
  return StatusCode::FAILURE;
}

//=========================================================================
// Tells if the path is available in the database.
//=========================================================================
bool CondDBLogger::exists(const std::string &path) {
  if ( m_loggedReader ) {
    (*m_logFile) << "XST: " << Gaudi::Time::current().ns() << " " << path <<  " " << std::flush;
    bool out = m_loggedReader->exists(path);
    (*m_logFile) << out << std::endl;
    return out;
  }
  return false;
}

//=========================================================================
// Tells if the path (if it exists) is a folder.
//=========================================================================
bool CondDBLogger::isFolder(const std::string &path) {
  if ( m_loggedReader ) {
    (*m_logFile) << "IFL: " << Gaudi::Time::current().ns() << " " << path <<  " " << std::flush;
    bool out = m_loggedReader->isFolder(path);
    (*m_logFile) << out << std::endl;
    return out;
  }
  return false;
}

//=========================================================================
// Tells if the path (if it exists) is a folderset.
//=========================================================================
bool CondDBLogger::isFolderSet(const std::string &path) {
  if ( m_loggedReader ) {
    (*m_logFile) << "IFS: " << Gaudi::Time::current().ns() << " " << path <<  " " << std::flush;
    bool out = m_loggedReader->isFolderSet(path);
    (*m_logFile) << out << std::endl;
    return out;
  }
  return false;
}

//=========================================================================
// Force disconnection from database.
//=========================================================================
void CondDBLogger::disconnect() {
  if ( m_loggedReader ) {
    (*m_logFile) << "DIS: " << Gaudi::Time::current().ns() << std::endl;
    m_loggedReader->disconnect();
  }
}

//=========================================================================
// Collect the list of used tags and databases
//=========================================================================
void CondDBLogger::defaultTags ( std::vector<LHCb::CondDBNameTagPair>& tags ) const {
  if ( m_loggedReader ) {
    (*m_logFile) << "TAG: " << Gaudi::Time::current().ns() << std::endl;
    m_loggedReader->defaultTags(tags);
  }
}

//=============================================================================
