//$Id: XmlStringAddress.cpp,v 1.2 2001-11-23 18:12:22 andreav Exp $

#include "XmlStringAddress.h"

//--------------------------------------------------------------------------

/// Constructor required by AddressFactory and IAddressCreator
XmlStringAddress::XmlStringAddress ( const CLID& clid,            
				     const std::string& xmlString )
  : GenericAddress   ( XMLSTRING_StorageType,
		       clid,
		       xmlString )
{
}

//---------------------------------------------------------------------------

/// Destructor
XmlStringAddress::~XmlStringAddress()
{
}

//---------------------------------------------------------------------------

/// Get the XML string data
const std::string XmlStringAddress::xmlString ( ) {
  return par()[0]; 
}

//---------------------------------------------------------------------------
