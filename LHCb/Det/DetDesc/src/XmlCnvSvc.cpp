///	$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/XmlCnvSvc.cpp,v 1.1.1.1 2000-11-23 13:44:47 ranjard Exp $
#define  DETDESC_CDFCNVSVC_CPP 1

#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "GaudiKernel/MsgStream.h"

#include "DetDesc/XmlCnvSvc.h"

#include "DetDesc/XmlExprParser.h"

/// RCS Id for identification of object version
///static const char* rcsid = "$Id: XmlCnvSvc.cpp,v 1.1.1.1 2000-11-23 13:44:47 ranjard Exp $";

/// Forward and external declarations

extern unsigned char XML_StorageType;
extern const IAddrFactory& XmlAddressFactory;


/// Instantiation of a static factory class used by clients to create
/// instances of this service
static SvcFactory<XmlCnvSvc>          xmlcnvsvc_factory;
const ISvcFactory& XmlCnvSvcFactory = xmlcnvsvc_factory;

/// Standard Constructor
XmlCnvSvc::XmlCnvSvc(const std::string& name, ISvcLocator* svc)
: ConversionSvc(name, svc, XML_StorageType),
  m_xp( 0 )
{
  setAddressFactory(&XmlAddressFactory);
  declareProperty( "AllowGenericConversion", m_genericConversion = false );
}

/// Standard Destructor
XmlCnvSvc::~XmlCnvSvc()
{
}

/// Initialize the service.
StatusCode XmlCnvSvc::initialize()
{

  // Before anything we have to initialize grand mother
  StatusCode status = ConversionSvc::initialize();

  MsgStream log(msgSvc(), "XmlCnvSvc" );
  
  if ( status.isSuccess() ) {
    
    // Set detector data store
    IDataProviderSvc *pDDPS = 0;
    status = serviceLocator()->getService("DetectorDataSvc",
      IID_IDataProviderSvc, (IInterface*&)pDDPS);
    
    if ( status.isSuccess() )
    {
      status = setStore ( pDDPS );
    }
    else
    {
      return status;
    }
    
    // Get my properties from the JobOptionsSvc
    status = setProperties();
    if( !status.isSuccess() )
    {
      return StatusCode::FAILURE;
    }
  }

  // Initialize the XML4C2 system
  try  {
    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException& toCatch)  {
    log << MSG::FATAL << "Error during initialization! :\n"
      << XMLString::transcode( toCatch.getMessage() ) << endreq;

    status = StatusCode::FAILURE;
  } 

  setProperties();

  // Initialize numerical expressions parser
  m_xp = new XmlExprParser( msgSvc() );

  return status;
}

/// Stop the service.
StatusCode XmlCnvSvc::finalize()
{
  if( m_xp != 0 )  {
    delete m_xp;
  }

  return ConversionSvc::finalize();
}

/// Query interface
StatusCode XmlCnvSvc::queryInterface(const IID& riid, void** ppvInterface)
{
  if ( IID_IXmlSvc == riid )  {
    *ppvInterface = (IXmlSvc*)this;
  }
  else  {
    // Interface is not directly availible: try out a base class
    return ConversionSvc::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

/// Evaluate a numerical expresion
double XmlCnvSvc::eval( char* expr, bool check )
{
  return m_xp->eval( expr, check );
}

/// Evaluate a numerical expresion
double XmlCnvSvc::eval( const char* expr, bool check )
{
  return m_xp->eval( expr, check );
}

/// Evaluate a numerical expresion
double XmlCnvSvc::eval( std::string& expr, bool check )
{
  return m_xp->eval( expr.c_str(), check );
}

/// Evaluate a numerical expresion
double XmlCnvSvc::eval( const std::string& expr, bool check )
{
  return m_xp->eval( expr.c_str(), check );
}



