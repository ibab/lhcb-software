// GaudiKernel
#include "GaudiKernel/MsgStream.h"
// DetDesc 
#include "DetDesc/DetectorElementException.h"
#include "DetDesc/DetectorElement.h"

//////////////////////////////////////////////////////////////////////////////
DetectorElementException::DetectorElementException
( const std::string     & name  , 
  const DetectorElement * DE    , 
  const StatusCode      & sc    ) 
  : GaudiException( name , "*DetElemException*" , sc )
  , m_dee_DetectorElement( DE ) 
{}
//////////////////////////////////////////////////////////////////////////////
DetectorElementException::DetectorElementException
( const std::string     & name      , 
  const GaudiException  & Exception , 
  const DetectorElement * DE        , 
  const StatusCode      & sc        ) 
  : GaudiException( name , "*DetElemException*" , sc , Exception )
  , m_dee_DetectorElement   ( DE ) 
{}
//////////////////////////////////////////////////////////////////////////////
DetectorElementException::~DetectorElementException() throw() {}
//////////////////////////////////////////////////////////////////////////////
GaudiException* 
DetectorElementException::clone() const 
{ return  new DetectorElementException(*this); }
//////////////////////////////////////////////////////////////////////////////
std::ostream& DetectorElementException::printOut( std::ostream& os ) const 
{
  os << " \t" << tag() << " \t " << message() ; 
  switch( code() )
  {
  case StatusCode::SUCCESS : os << "\t StatusCode=SUCCESS"    ;  break ; 
  case StatusCode::FAILURE : os << "\t StatusCode=FAILURE"    ;  break ; 
  default                  : os << "\t StatusCode=" << code() ;  break ; 
  }
  if ( 0 != m_dee_DetectorElement ) 
  { 
    os << "\t for DetectorElement=" ;
    m_dee_DetectorElement->printOut( os )  ; 
  }
  else 
  { os << "\t for UKNNOWN DetectorElement"           ; }
  ///
  return  previous() ? ( previous()->printOut(os) ) : ( os << std::endl ) ;
}
//////////////////////////////////////////////////////////////////////////////
MsgStream&    DetectorElementException::printOut( MsgStream& os  ) const 
{
  os << " \t" << tag() << " \t " << message() ; 
  switch( code() )
  {
  case StatusCode::SUCCESS : os << "\t StatusCode=SUCCESS"    ;  break ; 
  case StatusCode::FAILURE : os << "\t StatusCode=FAILURE"    ;  break ; 
  default                  : os << "\t StatusCode=" << code() ;  break ; 
  }
  if ( 0 != m_dee_DetectorElement ) 
  { 
    os << "\t for DetectorElement=" ;
    m_dee_DetectorElement->printOut( os )  ; 
  }
  else
  { os << "\t for UKNNOWN DetectorElement"           ; }
  ///
  return  previous() ? ( previous()->printOut(os) ) : ( os << endmsg ) ;
} 
//////////////////////////////////////////////////////////////////////////////
