//$Id: XmlStringAddress.h,v 1.3 2001-12-10 22:18:00 andreav Exp $
#ifndef DETCONDEXAMPLE_XMLSTRINGADDRESS_H
#define DETCONDEXAMPLE_XMLSTRINGADDRESS_H 1

// Base class
#include "GaudiKernel/GenericAddress.h"

// Forward declarations
class XmlStringCnvSvc;

///---------------------------------------------------------------------------
/** @class XmlStringAddress XmlStringAddress.h Det/DetCond/XmlStringAddress.h

    An IOpaqueAddress for XmlString data persistency.
    
    @author Andrea Valassi 
    @date August 2001
*///---------------------------------------------------------------------------

class XmlStringAddress : public GenericAddress {

  friend XmlStringCnvSvc;

 public:
    
  /// Destructor
  ~XmlStringAddress();

 protected:

  // Only XmlStringCnvSvc (IAddressCreator) can use protected constructors

  /// Constructor
  XmlStringAddress( const CLID& clid,               // classID
		    const std::string& xmlString,   // the Xml string data
		    const std::string& xmlName   ); // the Xml element name

 public:

  // XmlString-specific methods

  /// Get the XML string data
  const std::string xmlString ( );

  /// Get the name of the element that will be looked for in the XML string
  const std::string xmlName ( );

};

#endif // DETCONDEXAMPLE_XMLSTRINGADDRESS_H

