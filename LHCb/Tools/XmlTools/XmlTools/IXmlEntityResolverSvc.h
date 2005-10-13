// $Id: IXmlEntityResolverSvc.h,v 1.1 2005-10-13 16:52:01 marcocle Exp $
#ifndef XMLTOOLS_IXMLENTITYRESOLVERSVC_H 
#define XMLTOOLS_IXMLENTITYRESOLVERSVC_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IInterface.h"

#include "xercesc/sax/EntityResolver.hpp"

static const InterfaceID IID_IXmlEntityResolverSvc ( "IXmlEntityResolverSvc", 1, 0 );

/** @class IXmlEntityResolverSvc IXmlEntityResolverSvc.h XmlTools/IXmlEntityResolverSvc.h
 *  
 *  Interface to allow a service to provide a specialized xerces::EntityResolver.
 *  The XmlParserSvc.EntityResolverSvc must be set to the name of the service implementing this interface
 *  in order to use the specialized xerces::EntityResolver to handle special stemIDs
 *  (like "conddb:/...").
 *
 *  @author Marco Clemencic
 *  @date   2005-10-12
 */
class IXmlEntityResolverSvc : virtual public IInterface {
public: 

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IXmlEntityResolverSvc; }

  /// Return a pointer to the actual implementation of a xercesc::EntityResolver.
  virtual xercesc::EntityResolver *resolver() = 0;

};
#endif // XMLTOOLS_IXMLENTITYRESOLVERSVC_H
