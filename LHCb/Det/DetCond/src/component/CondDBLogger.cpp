// $Id: CondDBLogger.cpp,v 1.2 2008-01-26 15:47:46 marcocle Exp $
// Include files 

#include "GaudiKernel/SvcFactory.h"
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
  Service(name,svcloc), m_loggedReader(0), m_logFile(0) {
  
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
// queryInterface
//=============================================================================
StatusCode CondDBLogger::queryInterface(const InterfaceID& riid,
                                               void** ppvUnknown){
  if ( IID_ICondDBReader.versionMatch(riid) ) {
    *ppvUnknown = (ICondDBReader*)this;
    addRef();
    return SUCCESS;
  } else if ( IID_ICondDBInfo.versionMatch(riid) )   {
    *ppvUnknown = (ICondDBInfo*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface(riid,ppvUnknown);
}

//=============================================================================
// initialize
//=============================================================================
StatusCode CondDBLogger::initialize(){
  StatusCode sc = Service::initialize();
  if (sc.isFailure()) return sc;

  MsgStream log(msgSvc(), name() );

  log << MSG::DEBUG << "Initialize" << endmsg;

  if ( m_loggedReaderName.empty() ){
    log << MSG::ERROR << "Property LoggedReader is not set." << endreq;
    return StatusCode::FAILURE;
  }
  
  // locate the CondDBReader
  sc = service(m_loggedReaderName,m_loggedReader,true);
  if (  !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not locate " << m_loggedReaderName << endreq;
    return sc;
  }
  log << MSG::DEBUG << "Retrieved '" << m_loggedReaderName  << "'" << endreq;

  // Set the default value of the file name if not specified.
  if ( m_logFileName.empty() ){
    m_logFileName = name() + ".log";
    log << MSG::INFO << "Property LogFile not specified, using '"
                     << m_logFileName << "'" << endreq; 
  }

  // Open the output file and start writing.
  m_logFile =  std::auto_ptr<std::ostream>(new std::ofstream(m_logFileName.c_str()));
  if ( ! m_logFile->good() ) {
    log << MSG::ERROR << "Problems opening " << m_logFileName << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << "File '" << m_logFileName << "' opened for writing." << endreq;

  (*m_logFile) << "INI: " << Gaudi::Time::current().ns() << " " << name() << " logging " << m_loggedReaderName << std::endl;

  return sc;
}

//=============================================================================
// finalize
//=============================================================================
StatusCode CondDBLogger::finalize(){
  MsgStream log(msgSvc(), name() );
  log << MSG::DEBUG << "Finalize" << endmsg;

  if ( m_loggedReader ) {
    m_loggedReader->release();
    m_loggedReader = 0;
  }

  if (m_logFile.get()) {
    (*m_logFile) << "FIN: " << Gaudi::Time::current().ns() << " " << name() << std::endl;
    m_logFile.reset(0);
  }

  return Service::finalize();
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

//=========================================================================
//  get the list of child nodes of a folderset
//=========================================================================
StatusCode CondDBLogger::getChildNodes (const std::string &path, std::vector<std::string> &node_names)
{
  if ( m_loggedReader ) {
    (*m_logFile) << "GCH: " << Gaudi::Time::current().ns() << " " << path <<  " " << std::flush;
    StatusCode sc = m_loggedReader->getChildNodes(path,node_names);
    (*m_logFile) << sc << path << std::endl;
    return sc;
  }
  return StatusCode::FAILURE;
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
