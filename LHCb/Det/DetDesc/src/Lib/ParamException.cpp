//$Id: ParamException.cpp,v 1.1 2003-04-23 10:06:49 sponce Exp $

// Include files
#include "DetDesc/ParamException.h"
#include "GaudiKernel/StatusCode.h"

//----------------------------------------------------------------------------

/// Constructor
ParamException::ParamException( const std::string& name )
  : GaudiException( name, "*ParamException*", StatusCode::FAILURE ) {
};

//----------------------------------------------------------------------------

/// Destructor
ParamException::~ParamException() throw() { 
};

//----------------------------------------------------------------------------

