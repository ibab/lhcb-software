// $Id: IXmlEntityResolver.h,v 1.1 2008-06-12 18:43:56 marcocle Exp $
#ifndef XMLTOOLS_IXMLENTITYRESOLVER_H 
#define XMLTOOLS_IXMLENTITYRESOLVER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"

#include "xercesc/sax/EntityResolver.hpp"


/** @class IXmlEntityResolver IXmlEntityResolver.h XmlTools/IXmlEntityResolver.h
 *  
 *  Interface to allow a service to provide a specialized xerces::EntityResolver.
 *  The XmlParserSvc.EntityResolverSvc must be set to the name of the service implementing this interface
 *  in order to use the specialized xerces::EntityResolver to handle special stemIDs
 *  (like "conddb:/...").
 *
 *  @author Marco Clemencic
 *  @date   2005-10-12
 */
class IXmlEntityResolver : public extend_interfaces<IInterface> {
public: 
  DeclareInterfaceID(IXmlEntityResolver, 2, 0 );

  /// Return a pointer to the actual implementation of a xercesc::EntityResolver.
  virtual xercesc::EntityResolver *resolver() = 0;

};
#endif // XMLTOOLS_IXMLENTITYRESOLVERSVC_H
