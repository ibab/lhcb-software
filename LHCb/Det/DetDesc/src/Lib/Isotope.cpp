// $Id: Isotope.cpp,v 1.8 2008-05-20 08:15:27 smenzeme Exp $
/// DetDesc 
#include "DetDesc/Isotope.h"
///

////////////////////////////////////////////////////////////////////////////////////
Isotope::Isotope( const std::string name    , 
                  const double      A       , 
                  const double      z       , 
		  const double      i       ,
                  const double      density ,
                  const double      rl      ,  
                  const double      al      ,
                  const double      temp    , 
                  const double      press   , 
                  const eState      s       )
  : Material( name , density , rl , al , temp , press , s )
    , m_A( A ) 
    , m_Z( z )
    , m_I( i )
    , m_C( 0 )
    , m_a( 0.)
    , m_m( 0.) 
    , m_X0(0.)
    , m_X1(0.)
{}
////////////////////////////////////////////////////////////////////////////////////
Isotope::~Isotope(){}
