//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetCond/src/Lib/SimpleStringAddress.cpp,v 1.1.1.1 2001-09-14 15:07:21 andreav Exp $

#include "DetCond/SimpleStringAddress.h"
#include "GaudiKernel/GenericLink.h"

//--------------------------------------------------------------------------

/// Preferred constructor (USE THIS)
SimpleStringAddress::SimpleStringAddress ( const unsigned char svcType,
					   const CLID&         clid,
					   const std::string&  dbName,
					   const std::string&  containerName )
  : SimpleAddress   ( svcType,
		      clid,
  		      0             )
  , m_dbName        ( dbName        )
  , m_containerName ( containerName )
{
}

//---------------------------------------------------------------------------

/// Constructor (DO NOT USE THIS)
SimpleStringAddress::SimpleStringAddress ( const GenericLinkBase& link )
  : SimpleAddress   ( link.svcType(),
		      link.clID(),
  		      0                      )
{
}

//---------------------------------------------------------------------------

/// Destructor
SimpleStringAddress::~SimpleStringAddress()
{
}

//---------------------------------------------------------------------------

/// Get first string
const std::string& SimpleStringAddress::dbName ( ) const
{
  return m_dbName;
}

//---------------------------------------------------------------------------

/// Get second string
const std::string& SimpleStringAddress::containerName ( ) const
{
  return m_containerName;
}

//---------------------------------------------------------------------------

/// Update first string (required by AddressFactory and IAddressCreator)
void SimpleStringAddress::setDbName ( const std::string& dbName )
{
  m_dbName = dbName;
}

//---------------------------------------------------------------------------

/// Update second string (required by AddressFactory and IAddressCreator)
void SimpleStringAddress::setContainerName ( const std::string& containerName )
{
  m_containerName = containerName;
}

//---------------------------------------------------------------------------

/// DUMMY (required by AddressFactory and IAddressCreator)
void SimpleStringAddress::setObjectName ( const std::string& )
{
}

//---------------------------------------------------------------------------
