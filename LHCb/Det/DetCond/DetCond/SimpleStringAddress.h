//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetCond/DetCond/SimpleStringAddress.h,v 1.1.1.1 2001-09-14 15:07:22 andreav Exp $
#ifndef DETCOND_SIMPLESTRINGADDRESS_H
#define DETCOND_SIMPLESTRINGADDRESS_H 1

// Base classes
#include "GaudiKernel/IOpaqueAddress.h"
#include "DetCond/SimpleAddress.h"

// Forward declarations
class GenericLinkBase;

///---------------------------------------------------------------------------
/** @class SimpleStringAddress SimpleStringAddress.h Det/DetCond/SimpleStringAddress.h

    A simple base class for an IOpaqueAddress that can be characterised 
    using just one or two strings. 

    <P>

    This class can not be instantiated as its constructors are protected.
    All setter methods needed by a templated AddrFactory, and constructors 
    similar to those needed, are provided: this makes it easy to create 
    instances of addresses derived from this class by using the IAddrCreator 
    interface of PersistencySvc and ConversionSvc. The main string can be 
    specified and retrieved as the "dbName" of the address. A second optional 
    string can be specified as the "containername".

    <P>

    This class is derived from SimpleAddress. 
    As in SimpleAddress, the genericLink() method remains dummy.
    As in SimpleAddress, objectname() returns the path in the data store:
    to avoid undesired overwriting, the setObject() method is dummy.

    @author Andrea Valassi 
    @date August 2001
*///---------------------------------------------------------------------------

class SimpleStringAddress : virtual public IOpaqueAddress, 
			    public SimpleAddress {
  
 protected:
    
  /// Destructor
  ~SimpleStringAddress();
  
 protected:
  
  // These protected methods are required by AddrFactory and IAddressCreator.
  // AddrFactory must be declared as a friend of the derived class.

  /// Constructor (USE THIS)
  SimpleStringAddress( const unsigned char svcType,       // svcType
		       const CLID& clid,                  // classID
		       const std::string& firstString,    // string data
		       const std::string& secondString ); // string data

  /// Constructor (DO NOT USE THIS)
  SimpleStringAddress( const GenericLinkBase& link );  
  
  /// Update first string
  virtual void setDbName( const std::string& dbName );
  
  /// Update second string
  virtual void setContainerName( const std::string& containerName );

  /// DUMMY
  virtual void setObjectName( const std::string& );
  
 public:

  // Reimplemented from SimpleAddress

  /// Get first string
  virtual const std::string& dbName         () const;

  /// Get second string
  virtual const std::string& containerName  () const;
			    
 private:
  
  // Specific data memebers
  
  /// First string
  std::string     m_dbName;

  /// Second string
  std::string     m_containerName;

};

#endif // DETCOND_SIMPLESTRINGADDRESS_H

