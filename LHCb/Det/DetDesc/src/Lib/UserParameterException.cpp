//$Id: UserParameterException.cpp,v 1.2 2003-01-17 14:03:39 sponce Exp $

// Include files
#include "DetDesc/UserParameterException.h"
#include "GaudiKernel/StatusCode.h"

//----------------------------------------------------------------------------

/// Constructor
UserParameterException::UserParameterException( const std::string& name )
  : GaudiException( name, "*UserParameterException*", StatusCode::FAILURE )
{
};

//----------------------------------------------------------------------------

/// Destructor
UserParameterException::~UserParameterException() throw()
{ 
};

//----------------------------------------------------------------------------

