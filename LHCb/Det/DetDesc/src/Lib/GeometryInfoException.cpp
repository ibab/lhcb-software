/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
/// local 
#include "GeometryInfo.h" 
#include "GeometryInfoException.h" 
///


/////////////////////////////////////////////////////////////////////////////////////////////////
GeometryInfoException::GeometryInfoException( const std::string  & name , 
					      const GeometryInfo * gi   ,
					      const StatusCode   & sc   )
  : GaudiException( name , "*GeometryInfoException*" , sc )
  , m_gie_geometryInfo  ( gi   ) 	
{};
///////////////////////////////////////////////////////////////////////////////////////////////////
GeometryInfoException::GeometryInfoException( const std::string    & name , 
					      const GaudiException & ge   , 
					      const GeometryInfo   * gi   ,
					      const StatusCode     & sc   )
  : GaudiException( name , "*GeometryInfoException*" , sc , ge )
  , m_gie_geometryInfo  ( gi   ) 	
{};	
////////////////////////////////////////////////////////////////////////////////////////////////////
GeometryInfoException::~GeometryInfoException(){};
////////////////////////////////////////////////////////////////////////////////////////////////////
GaudiException* GeometryInfoException::clone() const  { return  new GeometryInfoException(*this); };  
////////////////////////////////////////////////////////////////////////////////////////////////////
std::ostream& GeometryInfoException::printOut( std::ostream& os ) const 
{
  ///
  os << "\t" << tag() << " \t" << message() ;
  /// 
  switch( code() )
    {
    case StatusCode::SUCCESS : os << "\t StatusCode=SUCCESS"                    ;  break ; 
    case StatusCode::FAILURE : os << "\t StatusCode=FAILURE"                    ;  break ; 
    default                  : os << "\t StatusCode=" << std::setw(7) << code() ;  break ; 
    }
  ///  
  os << m_gie_geometryInfo << std::endl ; 
  ///
  return previous() ? ( os << previous()->printOut( os ) ) :  os  ; 
  ///
};
/////////////////////////////////////////////////////////////////////////////////////////////////////
MsgStream&    GeometryInfoException::printOut( MsgStream&  os     ) const 
{
  ///
  os << "\t" << tag() << " \t" << message() ;
  /// 
  switch( code() )
    {
    case StatusCode::SUCCESS : os << "\t StatusCode=SUCCESS"                    ;  break ; 
    case StatusCode::FAILURE : os << "\t StatusCode=FAILURE"                    ;  break ; 
    default                  : os << "\t StatusCode=" << std::setw(7) << code() ;  break ; 
    }
  ///  
  os << m_gie_geometryInfo << endreq ; 
  ///
  return previous() ? ( previous()->printOut( os ) ) :  os  ; 
  ///
};
/////////////////////////////////////////////////////////////////////////////////////////////////////










