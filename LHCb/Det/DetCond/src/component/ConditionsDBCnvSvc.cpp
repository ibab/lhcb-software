//$Id: ConditionsDBCnvSvc.cpp,v 1.16 2005-04-22 14:09:31 marcocle Exp $
#include <string>

#include "ConditionsDBCnvSvc.h"
#include "DetCond/ICondDBAccessSvc.h"

#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ICnvFactory.h"

#include "GaudiKernel/TimePoint.h"

/// Instantiation of a static factory to create instances of this service
static SvcFactory<ConditionsDBCnvSvc>          ConditionsDBCnvSvc_factory;
const ISvcFactory& ConditionsDBCnvSvcFactory = ConditionsDBCnvSvc_factory;

//----------------------------------------------------------------------------

/// Constructor
ConditionsDBCnvSvc::ConditionsDBCnvSvc( const std::string& name, 
				        ISvcLocator* svc)
  : ConversionSvc ( name, svc, CONDDB_StorageType )
{}

//----------------------------------------------------------------------------

/// Destructor
ConditionsDBCnvSvc::~ConditionsDBCnvSvc()
{}

//----------------------------------------------------------------------------

/// Initialize the service.
StatusCode ConditionsDBCnvSvc::initialize()
{

  // Before anything else, we need to initialise the base class
  StatusCode sc = ConversionSvc::initialize();
  if ( !sc.isSuccess() ) return sc;

  // Now we can get a handle to the MessageSvc
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::INFO << "Specific initialization starting" << endreq;

  // Locate the Database Access Service
  sc = serviceLocator()->getService("CondDBAccessSvc",
                                    ICondDBAccessSvc::interfaceID(),(IInterface*&)m_dbAccSvc);
  if (  !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not locate CondDBAccessSvc" << endreq;
    return sc;
  }
  // Locate the Detector Data Service
  IDataProviderSvc* pDDS = 0;
  sc = serviceLocator()->getService 
    ("DetectorDataSvc",  IID_IDataProviderSvc, (IInterface*&)pDDS);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not locate DetectorDataSvc" << endreq;
    return sc;
  }

  // Set the DetectorDataSvc as data provider service
  sc = setDataProvider ( pDDS );
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not set data provider" << endreq;
    return sc;
  }

  // Query the IDetDataSvc interface of the detector data service
  sc = pDDS->queryInterface(IID_IDetDataSvc, (void**) &m_detDataSvc);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Cannot query IDetDataSvc interface of DetectorDataSvc" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Retrieved IDetDataSvc interface of DetectorDataSvc" << endreq;
  }

  // Locate IConversionSvc interface of the DetectorPersistencySvc
  sc = serviceLocator()->service 
    ("DetectorPersistencySvc", m_detPersSvc, true);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Cannot locate IConversionSvc interface of DetectorPersistencySvc" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Retrieved IConversionSvc interface of DetectorPersistencySvc"	<< endreq;
  }
  
  // Query the IAddressCreator interface of the detector persistency service
  IAddressCreator* iAddrCreator;
  sc = m_detPersSvc->queryInterface(IID_IAddressCreator, (void**) &iAddrCreator);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Cannot query IAddressCreator interface of DetectorPersistencySvc" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Retrieved IAddressCreator interface of DetectorPersistencySvc" << endreq;
  }
  log << MSG::DEBUG << "Set it as the address creator of the ConditionsDBCnvSvc" << endreq;
  sc = setAddressCreator( iAddrCreator );
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Cannot set the address creator" << endreq;
    return sc;
  }

  log << MSG::INFO << "Specific initialization completed" << endreq;
  return sc;
}

//----------------------------------------------------------------------------

/// Finalize the service.
StatusCode ConditionsDBCnvSvc::finalize()
{
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::DEBUG << "Finalizing" << endreq;
  m_dbAccSvc->release();
  m_detPersSvc->release();
  m_detDataSvc->release();
  return ConversionSvc::finalize();
}

//----------------------------------------------------------------------------

/// Create an address using explicit arguments to identify a single object.
/// Par[0] is folder name in the ConditionsDB.
/// Par[1] is entry name in the string (which may contain many conditions,
/// for instance in the case of XML files with more than one element).
StatusCode ConditionsDBCnvSvc::createAddress( long svc_type,
					      const CLID& clid,
					      const std::string* par, 
					      const unsigned long* /*ipar*/,
					      IOpaqueAddress*& refpAddress ) {
  
  // First check that requested address is of type CONDDB_StorageType
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::INFO << "entering createAddress" << endmsg;
  if ( svc_type!= CONDDB_StorageType ) {
    log << MSG::ERROR 
	<< "Cannot create addresses of type " << (int)svc_type 
	<< " which is different from " << (int)CONDDB_StorageType 
	<< endreq;
    return StatusCode::FAILURE;
  }
  
  // Par[0] is folder name in the ConditionsDB.
  std::string folderName = par[0];

  // Par[1] is entry name in the string (which may contain many conditions, 
  // for instance in the case of XML files with more than one element).
  std::string entryName = par[1];

  // Now create the address
  refpAddress = new GenericAddress( CONDDB_StorageType, 
				    clid, 
				    folderName,
				    entryName );
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------
extern ICnvFactory &RelyConverterFactory;

StatusCode ConditionsDBCnvSvc::addConverter(const CLID& clid){
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
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
IConverter* ConditionsDBCnvSvc::converter(const CLID& clid) {
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

