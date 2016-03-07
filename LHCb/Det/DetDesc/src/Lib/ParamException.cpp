//$Id: ParamException.cpp,v 1.2 2006-01-19 08:51:54 marcocle Exp $

// Include files
#include "GaudiKernel/StatusCode.h"

#include "DetDesc/ParamException.h"

//----------------------------------------------------------------------------

/// Constructor
ParamException::ParamException( const std::string& name, ExceptionType t ):
  GaudiException( "", "*ParamException*", StatusCode::FAILURE ),
  m_type(t)
{
  std::ostringstream msg;
  
  switch (m_type) {
  case BAD_KEY:
    msg << "Param `" << name << "' not found";
    break;
  case BAD_TYPE:
    msg << "Param `" << name << "' has a different type from the one requested";
    break;
  }

  m_message = msg.str();
}

//----------------------------------------------------------------------------
/// Constructor
ParamException::ParamException( const std::string& name,
                                const std::type_info &req, const std::type_info &actual ):
  GaudiException( "", "*ParamException*", StatusCode::FAILURE ),
  m_type(BAD_TYPE)
{
  std::ostringstream msg;
  msg << "Param `" << name << "' has a different type from the one requested (requested = "
      << req.name() << ", actual = " << actual.name() << ")";
  m_message = msg.str();
}

//----------------------------------------------------------------------------

/// Destructor
ParamException::~ParamException() throw() {}

//----------------------------------------------------------------------------

