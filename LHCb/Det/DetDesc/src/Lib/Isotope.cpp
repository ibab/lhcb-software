// $Id: Isotope.cpp,v 1.7 2006-02-10 08:02:45 mneedham Exp $
/// DetDesc 
#include "DetDesc/Isotope.h"
///

////////////////////////////////////////////////////////////////////////////////////
Isotope::Isotope( const std::string name    , 
                  const double      a       , 
                  const double      z       , 
                  const double      density ,
                  const double      rl      ,  
                  const double      al      ,
                  const double      temp    , 
                  const double      press   , 
                  const eState      s       )
  : Material( name , density , rl , al , temp , press , s )
  , m_A( a ) 
  , m_Z( z ) 
{};
////////////////////////////////////////////////////////////////////////////////////
Isotope::~Isotope(){};
