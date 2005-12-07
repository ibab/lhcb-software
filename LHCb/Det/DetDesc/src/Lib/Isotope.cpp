// $Id: Isotope.cpp,v 1.6 2005-12-07 13:19:07 cattanem Exp $
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
