//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetCond/DetCond/SimpleAddress.h,v 1.1.1.1 2001-09-14 15:07:22 andreav Exp $
#ifndef DETCOND_SIMPLEADDRESS_H
#define DETCOND_SIMPLEADDRESS_H 1

#include <string>

// Base class
#include "GaudiKernel/IOpaqueAddress.h"

///---------------------------------------------------------------------------
/** @class SimpleAddress SimpleAddress.h Det/DetCond/SimpleAddress.h

    A simple base class for a generic address implementing IOpaqueAddress.
    This class can not be instantiated as its constructor is protected.
    
    <P>

    This class has been designed to be simpler than GenericAddress.

    <UL>

    <LI>
    The IOpaqueAddress interface is fully implemented,
    however only a few methods are meaningful, 
    while most return dummy answers:
    <UL>
    <LI> clID()          -> class ID
    <LI> svcType()       -> storage type (set to 0 for this base class)
    <LI> dbName()        -> DUMMY
    <LI> containerName() -> DUMMY
    <LI> objectName()    -> full path in the transient data store
    <LI> directory()     -> pointer to IDataDirectory in the transient store
    <LI> genericLink()   -> DUMMY
    </UL>

    <LI>
    Only a simple (protected) constructor is provided for SimpleAddress.
    <UL>
    <LI> 
    The two constructors and the three setter methods needed by AddrFactory 
    are not introduced: no address factory of type AddrFactory can be 
    instantiated unless they are implemented in the derived class.
    <LI> 
    Since an AddrFactory is used in the implementation of the IAddrCreator 
    interface by ConversionSvc (in turn called by that of PersistencySvc),
    The IAddrCreator interfaces of ConversionSvc and PersistencySvc cannot
    be used either unless the five methods above are implemented in the
    derived class, since an AddrFactory is used in the implementation of  
    IAddrCreator by ConversionSvc (in turn called by that of PersistencySvc).
    </UL>    

    </UL>
    
    @author Andrea Valassi 
    @date August 2001
*///---------------------------------------------------------------------------

class SimpleAddress : virtual public IOpaqueAddress {
    
 protected:
    
  /// Constructor 
  SimpleAddress( unsigned char svcType,         // storage type
		 const CLID& clid,              // classID
		 IDataDirectory* entry = 0 );   // transient data store entry

  /// Destructor
  virtual ~SimpleAddress();

 private:
  
  // IOpaqueAddress-related data memebers

  /// Class ID
  CLID            m_clID;

  /// Full path in the transient data store
  std::string     m_objectName;

  /// Reference count
  unsigned long   m_refCount;
  
  /// Pointer to the corresponding directory in the transient data store
  IDataDirectory* m_directory;

  /// Service type
  unsigned char   m_svcType;

 public:

  // Implementation of the IOpaqueAddress interface

  /// Add reference to object
  virtual unsigned long addRef              ();

  /// Release reference to object
  virtual unsigned long release             ();

  /// Retrieve class id
  virtual const CLID& clID                  () const;

  /// Retrieve storage type
  virtual unsigned char svcType             () const;

  /// DUMMY
  virtual const std::string& dbName         () const;

  /// DUMMY
  virtual const std::string& containerName  () const;

  /// Retrieve full path in the transient data store
  virtual const std::string& objectName     () const;

  /// Retrieve directory pointer
  virtual IDataDirectory* directory         ();

  /// DUMMY
  virtual GenericLink* genericLink          ();

  /// Update pointer to directory in the transient data store
  virtual void setDirectory ( IDataDirectory* dir );

};

#endif // DETCOND_SIMPLEADDRESS_H

