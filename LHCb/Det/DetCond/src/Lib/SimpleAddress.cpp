//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetCond/src/Lib/SimpleAddress.cpp,v 1.1.1.1 2001-09-14 15:07:21 andreav Exp $

#include "DetCond/SimpleAddress.h"
#include "GaudiKernel/IDataDirectory.h"

//---------------------------------------------------------------------------

/// Constructor 
SimpleAddress::SimpleAddress ( unsigned char svcType,
			       const CLID& clid,			       
			       IDataDirectory* entry = 0 )
  : m_clID          ( clid    )
  , m_objectName    ( ""      )
  , m_refCount      ( 0       )
  , m_directory     ( 0       )
  , m_svcType       ( svcType )
{
  setDirectory( entry );
  addRef();
}

//---------------------------------------------------------------------------

/// Destructor
SimpleAddress::~SimpleAddress()
{
}

//---------------------------------------------------------------------------

/// Add reference to object
unsigned long SimpleAddress::addRef () {
  return ++m_refCount;
}
  
//---------------------------------------------------------------------------

/// Release reference to object
unsigned long SimpleAddress::release () {
  int cnt = --m_refCount;
  if ( 0 == cnt )   {
    delete this;
  }
  return cnt;
}

//---------------------------------------------------------------------------

/// Retrieve class id
const CLID& SimpleAddress::clID() const {
  return m_clID;
}

//---------------------------------------------------------------------------

/// Retrieve storage type
unsigned char SimpleAddress::svcType() const {
  return m_svcType;
}

//---------------------------------------------------------------------------

/// DUMMY
const std::string& SimpleAddress::dbName () const { 
  static std::string dbName = "DUMMY";
  return dbName; 
};

//---------------------------------------------------------------------------

/// DUMMY
const std::string& SimpleAddress::containerName () const { 
  static std::string containerName = "DUMMY";
  return containerName; 
};

//---------------------------------------------------------------------------

/// Full path in the transient data store
const std::string& SimpleAddress::objectName () const { 
  return m_objectName;
};

//---------------------------------------------------------------------------

/// Pointer to directory
IDataDirectory* SimpleAddress::directory() {
  return m_directory;
}

//---------------------------------------------------------------------------

/// DUMMY
GenericLink* SimpleAddress::genericLink () { 
  return 0;   
};

//---------------------------------------------------------------------------

/// Set pointer to directory
void SimpleAddress::setDirectory(IDataDirectory* pDirectory) {
  m_directory = pDirectory;
  if ( 0 == pDirectory ) {
    m_objectName = "" ;
  } else {
    m_objectName = pDirectory->name( );
  }
}

//---------------------------------------------------------------------------
