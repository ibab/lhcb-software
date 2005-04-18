// $Id: OnlineAddress.cpp,v 1.1.1.1 2005-04-18 15:31:41 frankb Exp $
//====================================================================
//	OnlineEvtSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : GaudiOnline
//
//	Author     : M.Frank
//====================================================================

// Include files
#include "GaudiOnline/OnlineAddress.h"
#include "GaudiOnline/StreamDescriptor.h"

// Standard Constructor
GaudiOnline::Address::Address(long type, const CLID& clid, const StreamDescriptor* comm_data)
: GenericAddress(type, clid), m_com(comm_data)
{
  if ( m_com )  {
    return;
  }
  throw std::runtime_error("Invalid context token (NULL) passed to GaudiOnline::Address");
}

// Standard Destructor
GaudiOnline::Address::~Address()
{
}
