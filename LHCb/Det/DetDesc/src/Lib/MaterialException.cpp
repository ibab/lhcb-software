// $Id: MaterialException.cpp,v 1.8 2009-04-17 08:54:24 cattanem Exp $
///
/// GaudiKernbel
#include "GaudiKernel/MsgStream.h"
/// DetDesc
#include "DetDesc/Material.h"
#include "DetDesc/MaterialException.h"
#include <iostream>


////////////////////////////////////////////////////////////////////////////////////////
MaterialException::MaterialException( const std::string& message      ,
                                      const Material*    mat          )
  : GaudiException( message , "*MaterialException*" , StatusCode::FAILURE )
  , m_me_mat      ( mat      )
{ }
////////////////////////////////////////////////////////////////////////////////////////
MaterialException::MaterialException( const std::string   &  message      ,
                                      const GaudiException&  Exception    ,
                                      const Material*        mat          )
  : GaudiException( message , "*MaterialException*" , StatusCode::FAILURE , Exception )
  , m_me_mat      ( mat     )
{}
////////////////////////////////////////////////////////////////////////////////////////
MaterialException::MaterialException( const MaterialException& right )
  : std::exception(), GaudiException( right          )
  , m_me_mat      ( right.m_me_mat )
{}
////////////////////////////////////////////////////////////////////////////////////////
MaterialException::~MaterialException() throw() { m_me_mat = 0 ; }
////////////////////////////////////////////////////////////////////////////////////////
std::ostream& MaterialException::printOut( std::ostream& os ) const
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
  os << m_me_mat << std::endl ;
  ///
  return previous() ? ( previous()->printOut( os ) ) :  os  ;
  ///
}
/////////////////////////////////////////////////////////////////////////////////////////
MsgStream&    MaterialException::printOut( MsgStream&    os ) const
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
  os << m_me_mat << endmsg ;
  ///
  return previous() ? ( previous()->printOut( os ) ) : os   ;
  ///
}
/////////////////////////////////////////////////////////////////////////////////////////






