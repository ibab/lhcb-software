//$Id: CondDBCnvSvc.cpp,v 1.8 2006-02-01 19:42:36 marcocle Exp $
#include <string>

#include "CondDBCnvSvc.h"
#include "DetCond/ICondDBAccessSvc.h"

#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ICnvFactory.h"

/// Instantiation of a static factory to create instances of this service
static SvcFactory<CondDBCnvSvc>          CondDBCnvSvc_factory;
const ISvcFactory& CondDBCnvSvcFactory = CondDBCnvSvc_factory;

//----------------------------------------------------------------------------

/// Constructor
CondDBCnvSvc::CondDBCnvSvc( const std::string& name, ISvcLocator* svc)
  : ConversionSvc ( name, svc, CONDDB_StorageType )
{
  declareProperty("CondDBAccessServices",m_dbAccSvcNames);
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
  log << MSG::INFO << "Specific initialization starting" << endreq;

  // Locate the Database Access Service
  if (m_dbAccSvcNames.empty()) m_dbAccSvcNames.push_back("CondDBAccessSvc");
  std::vector<std::string>::const_iterator svcName;
  for ( svcName = m_dbAccSvcNames.begin(); svcName != m_dbAccSvcNames.end(); ++svcName ){
    ICondDBAccessSvc *svcInt;
    sc = service("CondDBAccessSvc",*svcName,svcInt);
    if (  !sc.isSuccess() ) {
      log << MSG::ERROR << "Could not locate CondDBAccessSvc/" << *svcName << endreq;
      return sc;
    }
    m_dbAccSvcs.push_back(svcInt);
    log << MSG::DEBUG << "Retrieved CondDBAccessSvc/" << *svcName << endreq;
  }
  log << MSG::INFO << "Specific initialization completed" << endreq;
  return sc;
}

//----------------------------------------------------------------------------

/// Finalize the service.
StatusCode CondDBCnvSvc::finalize()
{
  MsgStream log(msgSvc(), name() );
  log << MSG::DEBUG << "Finalizing" << endreq;
  std::vector<ICondDBAccessSvc*>::iterator accSvc;
  for ( accSvc = m_dbAccSvcs.begin(); accSvc != m_dbAccSvcs.end(); ++accSvc ) (*accSvc)->release();
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
extern const ICnvFactory &RelyConverterFactory;

StatusCode CondDBCnvSvc::addConverter(const CLID& clid){
  MsgStream log(msgSvc(), name() );
  StatusCode status = ConversionSvc::addConverter(clid);
  if (status.isSuccess()){
    return status;
  }else{ // not found in the std way, try the delegation one
    long typ = repSvcType();
    log << MSG::DEBUG << "converter not found, instantiating RelyConverter" << endmsg;
    IConverter* pConverter = RelyConverterFactory.instantiate(serviceLocator());
    if ( 0 != pConverter )    {
      StatusCode status = configureConverter( typ, 0, pConverter );
      if ( status.isSuccess() )   {
        status = initializeConverter( typ, 0, pConverter );
        if ( status.isSuccess() )   {
          status = activateConverter( typ, 0, pConverter );
          if ( status.isSuccess() )   {
            return ConversionSvc::addConverter(pConverter);
          }
        }
      }
      pConverter->release();
    }
  }
  return NO_CONVERTER;
}

/// Retrieve converter from list
IConverter* CondDBCnvSvc::converter(const CLID& clid) {
  IConverter* cnv = 0;
  Workers::iterator i = std::find_if(m_workers->begin(),m_workers->end(),CnvTest(clid));
  if ( i != m_workers->end() )      {
    cnv = (*i).converter();
  }
  if ( 0 == cnv )     {
    StatusCode status = addConverter(clid);
    if ( status.isSuccess() )   {
      i = std::find_if(m_workers->begin(),m_workers->end(),CnvTest(clid));
      if ( i != m_workers->end() )      {
        cnv = (*i).converter();
      }
    }
  }
  return cnv;
}

//----------------------------------------------------------------------------
// Implementation of ICondDBCnvSvc interface
std::vector<ICondDBAccessSvc*> &CondDBCnvSvc::accessServices() { return m_dbAccSvcs; }
const std::vector<ICondDBAccessSvc*> &CondDBCnvSvc::accessServices() const { return m_dbAccSvcs; }

//----------------------------------------------------------------------------
// Implementation of IInterface
StatusCode CondDBCnvSvc::queryInterface(const InterfaceID& riid,
                                        void** ppvUnknown){
  if ( IID_ICondDBCnvSvc.versionMatch(riid) ) {
    *ppvUnknown = (ICondDBCnvSvc*)this;
    addRef();
    return SUCCESS;
  }
  return ConversionSvc::queryInterface(riid,ppvUnknown);
}
