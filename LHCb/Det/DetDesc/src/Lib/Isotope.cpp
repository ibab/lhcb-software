// $Id: Isotope.cpp,v 1.5 2003-04-25 08:52:24 sponce Exp $
/// DetDesc 
#include "DetDesc/Isotope.h"
///

////////////////////////////////////////////////////////////////////////////////////
Isotope::Isotope( const std::string name    , 
                  const double      a       , 
                  const double      z       , 
                  const double      n       , 
                  const double      density ,
                  const double      rl      ,  
                  const double      al      ,
                  const double      temp    , 
                  const double      press   , 
                  const eState      s       )
  : Material( name , density , rl , al , temp , press , s )
  , m_A( a ) 
  , m_Z( z ) 
  , m_N( n )
{};
////////////////////////////////////////////////////////////////////////////////////
Isotope::~Isotope(){};
////////////////////////////////////////////////////////////////////////////////////
StreamBuffer& Isotope::serialize( StreamBuffer& os )
{
  Material::serialize( os );
  return os >> m_A >> m_Z >> m_N ;   
};
////////////////////////////////////////////////////////////////////////////////////
StreamBuffer& Isotope::serialize( StreamBuffer& os ) const  
{
  Material::serialize( os );
  return os << m_A << m_Z << m_N ;   
};
////////////////////////////////////////////////////////////////////////////////////
