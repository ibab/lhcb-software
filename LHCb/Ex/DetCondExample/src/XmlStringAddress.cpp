//$Id: XmlStringAddress.cpp,v 1.3 2001-12-10 22:17:59 andreav Exp $

#include "XmlStringAddress.h"

//--------------------------------------------------------------------------

/// Constructor required by AddressFactory and IAddressCreator
XmlStringAddress::XmlStringAddress ( const CLID& clid,
				     const std::string& xmlString,
				     const std::string& xmlName )
  : GenericAddress   ( XML_StorageType,
		       clid,
		       xmlString,
		       xmlName,
		       1 )
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

/// Get the name of the element that will be looked for in the XML string
const std::string XmlStringAddress::xmlName ( ) {
  return par()[1]; 
}

//---------------------------------------------------------------------------
