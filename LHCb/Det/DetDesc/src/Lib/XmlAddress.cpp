/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/XmlAddress.cpp,v 1.1 2001-05-14 15:13:41 sponce Exp $
/// Include files
#include "GaudiKernel/AddrFactory.h"

#include "DetDesc/XmlAddress.h"

/// RCS Id for identification of object version
///static const char* rcsid = "$Id: XmlAddress.cpp,v 1.1 2001-05-14 15:13:41 sponce Exp $";

/// XmlAddress Factory definition
static const AddrFactory<XmlAddress> s_Factory(XML_StorageType);
const IAddrFactory& XmlAddressFactory = s_Factory;


/// Standard Constructor
XmlAddress::XmlAddress( const CLID& clid,
                        const std::string& fileName,
                        int   /* seqId */ ,
                        const std::string& containerName ) 
: GenericAddress(XML_StorageType, clid)
{
  setDbName( fileName );
  setContainerName( containerName );
  setObjectName( "XmlDataObject" );
}

/// Standard Constructor
XmlAddress::XmlAddress( const CLID& clid,
                        const std::string& fileName,
                        const std::string& containerName ) 
: GenericAddress(XML_StorageType, clid)
{
  setDbName( fileName );
  setContainerName( containerName );
}

/// Standard Constructor
/// GenericLink argument is composed of XML_StorageType + classID
/// e.g. GenericLink(unsigned char type, const CLID& clsid=0);
XmlAddress::XmlAddress(
                        const GenericLink& link
                       ,const std::string& fileName
                       ,const std::string& containerName
                       ,const std::string& objName
                      )
: GenericAddress( link )
{
      setDbName( fileName );
      setContainerName( containerName );
      setObjectName( objName );
}

/// Standard Constructor
XmlAddress::XmlAddress( const CLID& clid ) 
: GenericAddress(XML_StorageType, clid)
{
}

/// Standard Constructor for non-initialized addresses
XmlAddress::XmlAddress( const GenericLink& link )
: GenericAddress( link )
{
}

/// Standard Constructor for non-initialized addresses
XmlAddress::XmlAddress( const GenericLink* link )
: GenericAddress( *link )
{
}

/// Standard destructor
XmlAddress::~XmlAddress()
{
}

