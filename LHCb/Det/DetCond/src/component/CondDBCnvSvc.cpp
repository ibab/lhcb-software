//$Id: CondDBCnvSvc.cpp,v 1.11 2006-12-14 12:56:16 ranjard Exp $
#include <string>

#include "CondDBCnvSvc.h"
#include "DetCond/ICondDBReader.h"

#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"

/// Instantiation of a static factory to create instances of this service
DECLARE_SERVICE_FACTORY(CondDBCnvSvc)

//----------------------------------------------------------------------------

/// Constructor
CondDBCnvSvc::CondDBCnvSvc( const std::string& name, ISvcLocator* svc)
  : ConversionSvc ( name, svc, CONDDB_StorageType ),
    m_dbReader(0)
{
  declareProperty( "CondDBReader", m_dbReaderName = "CondDBAccessSvc" );
}

//----------------------------------------------------------------------------

/// Destructor
CondDBCnvSvc::~CondDBCnvSvc() {}

//----------------------------------------------------------------------------

/// Initialize the service.
StatusCode CondDBCnvSvc::initialize()
{

  // Before anything else, we need to initialise the base class
  StatusCode sc = ConversionSvc::initialize();
  if ( !sc.isSuccess() ) return sc;

  // Now we can get a handle to the MessageSvc
  MsgStream log(msgSvc(), name() );
  log << MSG::DEBUG << "Specific initialization starting" << endreq;

  // Locate the Database Access Service
  sc = service(m_dbReaderName,m_dbReader,true);
  if (  !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not locate " << m_dbReaderName << endreq;
    return sc;
  }
  log << MSG::DEBUG << "Retrieved " << m_dbReaderName << endreq;
  
  log << MSG::DEBUG << "Specific initialization completed" << endreq;
  return sc;
}

//----------------------------------------------------------------------------

/// Finalize the service.
StatusCode CondDBCnvSvc::finalize()
{
  MsgStream log(msgSvc(), name() );
  log << MSG::DEBUG << "Finalizing" << endreq;
  if (m_dbReader) m_dbReader->release();
  return ConversionSvc::finalize();
}

//----------------------------------------------------------------------------

/// Create an address using explicit arguments to identify a single object.
/// Par[0] is folder name in the CondDB.
/// Par[1] is entry name in the string (which may contain many conditions,
/// for instance in the case of XML files with more than one element).
StatusCode CondDBCnvSvc::createAddress( long svc_type,
                                        const CLID& clid,
                                        const std::string* par, 
                                        const unsigned long* ipar,
                                        IOpaqueAddress*& refpAddress ) {
  
  // First check that requested address is of type CONDDB_StorageType
  MsgStream log(msgSvc(), name() );
  log << MSG::DEBUG << "entering createAddress" << endmsg;
  if ( svc_type!= CONDDB_StorageType ) {
    log << MSG::ERROR 
        << "Cannot create addresses of type " << (int)svc_type 
        << " which is different from " << (int)CONDDB_StorageType 
        << endreq;
    return StatusCode::FAILURE;
  }
  
  // Par[0] is folder name in the CondDB.
  std::string folderName = par[0];

  // Par[1] is entry name in the string (which may contain many conditions, 
  // for instance in the case of XML files with more than one element).
  std::string entryName = par[1];

  // iPar[0] is the cool::ChannelId
  unsigned long channelId = ipar[0];
  
  // Now create the address
  refpAddress = new GenericAddress( CONDDB_StorageType, 
                                    clid,
                                    folderName,
                                    entryName,
                                    channelId );
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Retrieve converter from list
IConverter* CondDBCnvSvc::converter(const CLID& clid) {
  IConverter* cnv = 0;
  cnv = ConversionSvc::converter(clid);
  if ( cnv ) {
    return cnv;
  }
  else {
    return ConversionSvc::converter(CLID_Any);
  }  
}

//----------------------------------------------------------------------------
// Implementation of IInterface
StatusCode CondDBCnvSvc::queryInterface(const InterfaceID& riid,
                                        void** ppvUnknown){
  if ( IID_ICondDBReader.versionMatch(riid) ) {
    *ppvUnknown = (ICondDBReader*)this;
    addRef();
    return SUCCESS;
  }
  return ConversionSvc::queryInterface(riid,ppvUnknown);
}

//----------------------------------------------------------------------------
// Implementation of ICondDBReader
StatusCode CondDBCnvSvc::getObject (const std::string &path, const Gaudi::Time &when,
                                           boost::shared_ptr<coral::AttributeList> &data,
                                           std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel)
{
  return m_dbReader->getObject(path,when,data,descr,since,until,channel);
}
StatusCode CondDBCnvSvc::getChildNodes (const std::string &path, std::vector<std::string> &node_names)
{
  return m_dbReader->getChildNodes(path,node_names);
}

