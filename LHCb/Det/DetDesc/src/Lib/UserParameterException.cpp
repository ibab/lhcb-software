//$Id: UserParameterException.cpp,v 1.1 2001-12-11 10:02:28 sponce Exp $

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
UserParameterException::~UserParameterException() 
{ 
};

//----------------------------------------------------------------------------

