// $Id: CondDBEntityResolverSvc.h,v 1.2 2006-07-14 10:00:35 marcocle Exp $
#ifndef CONDDBENTITYRESOLVERSVC_H 
#define CONDDBENTITYRESOLVERSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "XmlTools/IXmlEntityResolverSvc.h"

class ICondDBReader;

/** @class CondDBEntityResolverSvc CondDBEntityResolverSvc.h
 *  
 *  Class implementing IXmlEntityResolverSvc interface and xercesc::EntityResolver.
 *  An instance of this service can be used to allow inclusion of XML DTDs and documents
 *  from CondDB. It works implementing the EntityResolver interface defined in Xerces-C++,
 *  which allow to intercept the requests of external XML data.\n
 *  Examples of CondDB external references:
 *  \li \<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure"\>
 *  \li \<!ENTITY Parameters SYSTEM "conddb:/Parameters/SpecialParamters"\>
 *
 *  @author Marco Clemencic
 *  @date   2005-10-18
 */
class CondDBEntityResolverSvc: public Service,
                               virtual public IXmlEntityResolverSvc,
                               virtual public xercesc::EntityResolver {

  // Only factories can access protected constructors
  friend class SvcFactory<CondDBEntityResolverSvc>;

protected:

  /// Standard constructor
  CondDBEntityResolverSvc( const std::string& name, ISvcLocator* svc ); 

  virtual ~CondDBEntityResolverSvc( ); ///< Destructor

public:

  // ---- Overloaded from Service ----

  /// Initialize the service
  virtual StatusCode initialize();
  
  /// Finalize the service
  virtual StatusCode finalize();


  // ---- Implementation of IInterface ----

  /// Query interfaces of the object..
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);


  // ---- Implementation of IXmlEntityResolverSvc -------------
  
  /// Return a pointer to the actual implementation of a xercesc::EntityResolver.
  xercesc::EntityResolver *resolver();


  // ---- Implementation of xercesc::EntityResolver -------------

  /// Create a Xerces-C input source based on the given systemId (publicId is ignored).
  /// If the systemId does not begin with "conddb:", it returns NULL, so the parser go on with the default action.
  xercesc::InputSource *resolveEntity(const XMLCh *const publicId, const XMLCh *const systemId);

private:

  /// Name of the CondDBCnvSvc instance.
  /// Set via the property "CondDBReader", default to "CondDBCnvSvc".
  std::string m_condDBReaderName;

  /// Pointer to the CondDBCnvSvc instance.
  ICondDBReader *m_condDBReader;

  /// Name of the DetectorDataService instance (for the event time).
  /// Set via the property "DetDataSvc", default to "DetDataSvc/DetectorDataSvc".
  std::string m_detDataSvcName;

  /// Pointer to the DetectorDataService instance (for the event time).
  IDetDataSvc *m_detDataSvc;

};
#endif // CONDDBENTITYRESOLVERSVC_H
