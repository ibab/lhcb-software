//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/src/XmlStringAddress.cpp,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $

#include "XmlStringAddress.h"
#include "GaudiKernel/AddrFactory.h"

/// Instantiation of a static factory to create instances of this address
static const AddrFactory<XmlStringAddress> s_Factory(XMLSTRING_StorageType);
const IAddrFactory& XmlStringAddressFactory = s_Factory;

//--------------------------------------------------------------------------

/// Constructor required by AddressFactory and IAddressCreator
XmlStringAddress::XmlStringAddress ( const CLID& clid,            
				     const std::string& xmlString,
				     int,                          // DUMMY
				     const std::string& )          // DUMMY
  : SimpleStringAddress   ( XMLSTRING_StorageType,
			    clid,
			    xmlString,
			    "DUMMY" )
{
}

//---------------------------------------------------------------------------

/// Constructor required by AddressFactory and IAddressCreator
XmlStringAddress::XmlStringAddress ( const GenericLinkBase& link )
  : SimpleStringAddress( link )
{
}

//---------------------------------------------------------------------------

/// Destructor
XmlStringAddress::~XmlStringAddress()
{
}

//---------------------------------------------------------------------------

/// Get the xml string data
const std::string XmlStringAddress::xmlString ( ) {
  return dbName(); 
}

//---------------------------------------------------------------------------
