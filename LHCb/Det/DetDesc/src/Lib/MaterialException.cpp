///
#include <iostream>
/// GaudiKernbel
#include "GaudiKernel/MsgStream.h"
/// DetDesc 
#include "DetDesc/Material.h"
#include "DetDesc/MaterialException.h"


////////////////////////////////////////////////////////////////////////////////////////
MaterialException::MaterialException( const std::string& message      , 
				      const Material*    mat    =  0  ) 
  : GaudiException( message , "*MaterialException*" , StatusCode::FAILURE ) 
  , m_me_mat      ( mat      ) 
{ };
////////////////////////////////////////////////////////////////////////////////////////
MaterialException::MaterialException( const std::string   &  message      ,
				      const GaudiException&  Exception    ,  
				      const Material*        mat    =  0  ) 
  : GaudiException( message , "*MaterialException*" , StatusCode::FAILURE , Exception ) 
  , m_me_mat      ( mat     ) 
{};
////////////////////////////////////////////////////////////////////////////////////////
MaterialException::MaterialException( const MaterialException& right )
  : GaudiException( right          )
  , m_me_mat      ( right.m_me_mat )
{};
////////////////////////////////////////////////////////////////////////////////////////
MaterialException::~MaterialException(){ m_me_mat = 0 ; };
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
  return previous() ? ( os << previous()->printOut( os ) ) :  os  ; 
  ///
};
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
  os << m_me_mat << endreq ; 
  ///
  return previous() ? ( previous()->printOut( os ) ) : os   ; 
  ///
};
/////////////////////////////////////////////////////////////////////////////////////////






