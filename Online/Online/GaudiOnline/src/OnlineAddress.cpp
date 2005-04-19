// $Id: OnlineAddress.cpp,v 1.2 2005-04-19 16:59:59 frankb Exp $
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
#include <stdexcept>

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
