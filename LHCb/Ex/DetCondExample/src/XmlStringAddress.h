//$Id: XmlStringAddress.h,v 1.2 2001-11-23 18:12:23 andreav Exp $
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
		    const std::string& xmlString ); // the Xml string data

 public:

  // XmlString-specific methods

  /// Get the XML string data
  const std::string xmlString ( );

};

#endif // DETCONDEXAMPLE_XMLSTRINGADDRESS_H

