//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/src/XmlStringAddress.h,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $
#ifndef DETCONDEXAMPLE_XMLSTRINGADDRESS_H
#define DETCONDEXAMPLE_XMLSTRINGADDRESS_H 1

// Base classes
#include "GaudiKernel/IOpaqueAddress.h"
#include "DetCond/SimpleStringAddress.h"

// Friends
#include "GaudiKernel/AddrFactory.h"

///---------------------------------------------------------------------------
/** @class XmlStringAddress XmlStringAddress.h Det/DetCond/XmlStringAddress.h

    An IOpaqueAddress for XmlString data persistency.
    
    @author Andrea Valassi 
    @date August 2001
*///---------------------------------------------------------------------------

class XmlStringAddress : virtual public IOpaqueAddress, 
                         public SimpleStringAddress {

  /// Only factories can access protected constructors
  friend class AddrFactory<XmlStringAddress> ;

 public:
    
  /// Destructor
  ~XmlStringAddress();

 protected:

  // These methods are required by AddressFactory and IAddressCreator

  /// Constructor
  XmlStringAddress( const CLID& clid,             // classID
		    const std::string& xmlString, // the Xml string data
		    int,                          // DUMMY
		    const std::string& );         // DUMMY

  /// Constructor
  XmlStringAddress( const GenericLinkBase& link );  

 public:

  // XmlString-specific methods

  /// Get the Xml string data
  const std::string xmlString ( );

};

#endif // DETCONDEXAMPLE_XMLSTRINGADDRESS_H

