// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ThreadGaudi.h"

#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/exception.hpp"

// local
#include "CondDBSQLiteCopyAccSvc.h"

// Factory implementation
DECLARE_SERVICE_FACTORY(CondDBSQLiteCopyAccSvc)

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBSQLiteCopyAccSvc
//
// 2007-03-22 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBSQLiteCopyAccSvc::CondDBSQLiteCopyAccSvc( const std::string& name, ISvcLocator* svcloc ):
  CondDBAccessSvc(name,svcloc)
{
  declareProperty("OriginalFile",    m_source_path        = "" );
  declareProperty("DestinationFile", m_dest_path          = "" );
  declareProperty("DBName",          m_dbname             = "" );
  declareProperty("ForceCopy",       m_force_copy         = false );
  declareProperty("IgnoreCopyError", m_ignore_copy_error  = false );
}

//=============================================================================
// initialize
//=============================================================================
StatusCode CondDBSQLiteCopyAccSvc::initialize(){
  //before initializing the parent, I have to copy the file
  StatusCode sc = setProperties();
  if ( ! sc.isSuccess() ) {
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Failed to set properties" << endmsg;
    return sc;
  }

  // this should be done after getting the properties
  MsgStream log(msgSvc(), name() );

  // preliminary checks on the options
  if ( m_source_path.empty() ) {
    log << MSG::ERROR << "You must provide the source file path via the option '"
        << name() << ".OriginalFile'" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( m_dest_path.empty() ) {
    log << MSG::ERROR << "You must provide the destination file path via the option '"
        << name() << ".DestinationFile'" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( m_dbname.empty() ) {
    log << MSG::ERROR << "You must provide the database name via the option '"
        << name() << ".DBName'" << endmsg;
    return StatusCode::FAILURE;
  }

  try {

    // if "force" mode is selected: remove the destination file if it exists
    if ( m_force_copy ) {
      bool file_existed = boost::filesystem::remove( m_dest_path );
      if ( file_existed ) {
        log << MSG::WARNING << "Removed file '" << m_dest_path << "' to replace it" << endmsg;
      }
    }

    // copy the source file
    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "Copying "
          << m_source_path << " -> "
          << m_dest_path << endmsg;
    boost::filesystem::copy_file(m_source_path,m_dest_path);

  }
  catch (boost::filesystem::filesystem_error &e){

    MSG::Level lvl = MSG::ERROR;
    if ( m_ignore_copy_error ) lvl = MSG::WARNING;

    log << lvl << "Problems occurred copying the file" << endmsg;
    log << lvl << e.what() << endmsg;
    if ( ! m_ignore_copy_error )
      return StatusCode::FAILURE;
  }

  /*
  // I need to override the connection string property
  IJobOptionsSvc* jos;
  const bool CREATEIF(true);
  sc = serviceLocator()->service( "JobOptionsSvc", jos, CREATEIF );
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Service JobOptionsSvc not found" << endmsg;
  }

  sc = jos->addPropertyToCatalogue( getGaudiThreadGenericName(name()),
                                    StringProperty( "ConnectionString",
                                                    "sqlite_file:" + m_dest_path + "/" + m_dbname) );
  jos->release();
  if ( ! sc.isSuccess() ) {
    log << MSG::ERROR << "Failed to override the property '" << name() << ".ConnectionString'"<< endmsg;
    return sc;
  }
  */

  // Set the connection string to be used (the one from the base class will be ignored).
  m_sqlite_connstring = "sqlite_file:" + m_dest_path + "/" + m_dbname;

  // Initialize the base class.
  return CondDBAccessSvc::initialize();
}

//=============================================================================
// Destructor
//=============================================================================
CondDBSQLiteCopyAccSvc::~CondDBSQLiteCopyAccSvc() {}

//=============================================================================
// Return the connection string used to connect to the database.
//=============================================================================
const std::string &CondDBSQLiteCopyAccSvc::connectionString() const {
  return m_sqlite_connstring;
}

//=============================================================================
