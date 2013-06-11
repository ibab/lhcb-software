// $Id$
// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "Analysis/Models.h"
// ============================================================================
// ROOT 
// ============================================================================
#include "RooArgSet.h"
// ============================================================================
/** @file 
 *  Implementation file for namespace Analysis::Models
 *
 *
 *  @author Vanya BELYAEV  Ivan.Belyaev@cern.ch
 *  @date   2011-11-30
 */
// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::BreitWigner::BreitWigner 
( const char*          name  , 
  const char*          title ,
  RooAbsReal&          x     ,
  RooAbsReal&          mass  ,
  RooAbsReal&          width ,
  const double         m1    , 
  const double         m2    ,
  const unsigned short L     )
  : RooAbsPdf  (name ,title ) 
//
  , m_x     ( "x"  , "Observable" , this , x     ) 
  , m_mass  ( "m0" , "Peak"       , this , mass  ) 
  , m_width ( "g0" , "Width"      , this , width )
//
  , m_bw    ( 0 , 1 , m1 , m2 , L ) 
{
  m_bw.setM0     ( m_mass  ) ;
  m_bw.setGamma0 ( m_width ) ;
}
// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::BreitWigner::BreitWigner 
( const char*          name  , 
  const char*          title ,
  RooAbsReal&          x     ,
  RooAbsReal&          mass  ,
  RooAbsReal&          width ,
  const double         m1    , 
  const double         m2    ,
  const unsigned short L     , 
  const Gaudi::Math::BreitWigner::JacksonRho rho ) 
  : RooAbsPdf  ( name , title ) 
//
  , m_x     ( "x"  , "Observable" , this , x     ) 
  , m_mass  ( "m0" , "Peak"       , this , mass  ) 
  , m_width ( "g0" , "Width"      , this , width )
//
  , m_bw    ( 0 , 1 , m1 , m2 , L , rho ) 
{
  m_bw.setM0     ( m_mass  ) ;
  m_bw.setGamma0 ( m_width ) ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::BreitWigner::BreitWigner 
( const Analysis::Models::BreitWigner& right , 
  const char*                          name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x     ( "x"  , this , right.m_x     ) 
  , m_mass  ( "m0" , this , right.m_mass  ) 
  , m_width ( "g0" , this , right.m_width )
//
  , m_bw    (               right.m_bw    ) 
{}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::BreitWigner::~BreitWigner(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::BreitWigner*
Analysis::Models::BreitWigner::clone( const char* name ) const 
{ return new Analysis::Models::BreitWigner(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::BreitWigner::evaluate() const 
{
  m_bw.setM0     ( m_mass  ) ;
  m_bw.setGamma0 ( m_width ) ;
  //
  return m_bw ( m_x ) ;
}
// ============================================================================



// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::Rho0::Rho0 
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          mass      ,
  RooAbsReal&          width     ,
  const double         pi_mass   )
  : Analysis::Models::BreitWigner ( name    , 
                                    title   , 
                                    x       , 
                                    mass    ,
                                    width   ,
                                    pi_mass , 
                                    pi_mass , 
                                    1       , 
                                    Gaudi::Math::BreitWigner::Jackson_A5 )
{}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Rho0::Rho0 
( const Analysis::Models::Rho0& right , 
  const char*                   name  ) 
  : Analysis::Models::BreitWigner ( right , name ) 
{}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::Rho0::~Rho0(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Rho0*
Analysis::Models::Rho0::clone( const char* name ) const 
{ return new Analysis::Models::Rho0(*this,name) ; }

// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::Flatte::Flatte 
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          m0        ,
  RooAbsReal&          m0g1      ,
  RooAbsReal&          g2og1     ,
  const double         k_mass    , 
  const double         pi_mass   ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x     ( "x"     , "Observable" , this , x     ) 
  , m_m0    ( "m0"    , "Peak"       , this , m0    ) 
  , m_m0g1  ( "m0g1"  , "M0*G1"      , this , m0g1  )
  , m_g2og1 ( "g2og1" , "G2/G1"      , this , g2og1 )
//
  , m_flatte ( 0 , 1 , 0.1 , k_mass , pi_mass ) 
{
  m_flatte.setM0     ( m_m0    ) ;
  m_flatte.setM0G1   ( m_m0g1  ) ;
  m_flatte.setG2oG1  ( m_g2og1 ) ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Flatte::Flatte 
( const Analysis::Models::Flatte& right , 
  const char*                     name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x     ( "x"     , this , right.m_x     ) 
  , m_m0    ( "m0"    , this , right.m_m0    ) 
  , m_m0g1  ( "m0g1"  , this , right.m_m0g1  )
  , m_g2og1 ( "g2og1" , this , right.m_g2og1 )
//
  , m_flatte ( right.m_flatte ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::Flatte::~Flatte (){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Flatte*
Analysis::Models::Flatte::clone( const char* name ) const 
{ return new Analysis::Models::Flatte(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Flatte::evaluate() const 
{
  m_flatte.setM0     ( m_m0    ) ;
  m_flatte.setM0G1   ( m_m0g1  ) ;
  m_flatte.setG2oG1  ( m_g2og1 ) ;
  //
  return m_flatte ( m_x ) ;
}
// ===========================================================================





// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::Flatte2::Flatte2 
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          m0        ,
  RooAbsReal&          m0g1      ,
  RooAbsReal&          g2og1     ,
  const double         k_mass    , 
  const double         pi_mass   ) 
  : Analysis::Models::Flatte ( name    , 
                               title   , 
                               x       , 
                               m0      , 
                               m0g1    , 
                               g2og1   , 
                               k_mass  , 
                               pi_mass ) 
{}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Flatte2::Flatte2 
( const Analysis::Models::Flatte2& right , 
  const char*                     name  ) 
  : Analysis::Models::Flatte ( right , name ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::Flatte2::~Flatte2 (){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Flatte2*
Analysis::Models::Flatte2::clone( const char* name ) const 
{ return new Analysis::Models::Flatte2(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Flatte2::evaluate() const 
{
  m_flatte.setM0          ( m_m0    ) ;
  m_flatte.setM0G1        ( m_m0g1  ) ;
  m_flatte.setG2oG1       ( m_g2og1 ) ;
  //
  return m_flatte.flatte2 ( m_x ) ;
}
// ============================================================================





// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::PhaseSpaceNL::PhaseSpaceNL
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          low       ,
  RooAbsReal&          high      ,    
  const unsigned short N         , 
  const unsigned short L         ) 
  : RooAbsPdf ( name  , title )
//
  , m_x     ( "x"     , "Observable" , this , x     ) 
  , m_low   ( "low"   , "m(low)"     , this , low   ) 
  , m_high  ( "high"  , "m(high)"    , this , high  ) 
//
  , m_ps    ( 1 , 2 , N , L ) 
{
  m_ps.setThresholds ( m_low , m_high ) ;  
}
// ============================================================================
// copy constructor 
// ============================================================================
Analysis::Models::PhaseSpaceNL::PhaseSpaceNL
( const Analysis::Models::PhaseSpaceNL& right , const char* name ) 
  : RooAbsPdf ( right  , name )
//
  , m_x     ( "x"     , this , right.m_x     ) 
  , m_low   ( "low"   , this , right.m_low   ) 
  , m_high  ( "low"   , this , right.m_high  ) 
//
  , m_ps    (                  right.m_ps    ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PhaseSpaceNL::~PhaseSpaceNL(){}
// ===========================================================================
// clone 
// ============================================================================
Analysis::Models::PhaseSpaceNL*
Analysis::Models::PhaseSpaceNL::clone( const char* name ) const 
{ return new Analysis::Models::PhaseSpaceNL(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PhaseSpaceNL::evaluate() const 
{
  m_ps.setThresholds ( m_low , m_high ) ;  
  //
  return m_ps ( m_x ) ;
}
// ============================================================================


// ============================================================================
// Double-sided CrystalBall
// ============================================================================
Analysis::Models::CrystalBallDS::CrystalBallDS
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          m0        ,
  RooAbsReal&          sigma     ,    
  RooAbsReal&          alphaL    ,    
  RooAbsReal&          nL        ,    
  RooAbsReal&          alphaR    ,    
  RooAbsReal&          nR        )
  : RooAbsPdf ( name , title )
//
  , m_x       ( "x"       , "Observable"                 , this , x      ) 
  , m_m0      ( "m0"      , "mass"                       , this , m0     ) 
  , m_sigma   ( "sigma"   , "sigma"                      , this , sigma  )
  , m_alphaL  ( "alphaL"  , "(left) alpha = 1 + |alpha|" , this , alphaL ) 
  , m_nL      ( "nL"      , "(left) n     = 1 + |n|"     , this ,     nL ) 
  , m_alphaR  ( "alphaR"  , "(left) alpha = 1 + |alpha|" , this , alphaR ) 
  , m_nR      ( "nR"      , "(left) n     = 1 + |n|"     , this ,     nR ) 
//  
  , m_cb2 ( 10 , 1 , 1 , 1 , 1  , 1 ) 
{
  //
  m_cb2.setM0      ( m_m0     ) ;
  m_cb2.setSigma   ( m_sigma  ) ;
  m_cb2.setAlpha_L ( m_alphaL ) ;
  m_cb2.setAlpha_R ( m_alphaR ) ;
  m_cb2.setN_L     ( m_nL     ) ;
  m_cb2.setAlpha_R ( m_alphaR ) ;
  m_cb2.setN_R     ( m_nR     ) ;
  //
}
// ============================================================================
// copy constructor 
// ============================================================================
Analysis::Models::CrystalBallDS::CrystalBallDS
( const Analysis::Models::CrystalBallDS& right , 
  const char*                            name ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x       ( "x"       , this , right.m_x      ) 
  , m_m0      ( "m0"      , this , right.m_m0     ) 
  , m_sigma   ( "sigma"   , this , right.m_sigma  )
  , m_alphaL  ( "alphaL"  , this , right.m_alphaL ) 
  , m_nL      ( "nL"      , this , right.m_nL     ) 
  , m_alphaR  ( "alphaR"  , this , right.m_alphaR ) 
  , m_nR      ( "nR"      , this , right.m_nR     ) 
//  
  , m_cb2     ( right.m_cb2 ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::CrystalBallDS::~CrystalBallDS(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::CrystalBallDS*
Analysis::Models::CrystalBallDS::clone( const char* name ) const 
{ return new Analysis::Models::CrystalBallDS(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::CrystalBallDS::evaluate() const 
{
  //
  m_cb2.setM0      ( m_m0     ) ;
  m_cb2.setSigma   ( m_sigma  ) ;
  m_cb2.setAlpha_L ( m_alphaL ) ;
  m_cb2.setAlpha_R ( m_alphaR ) ;
  m_cb2.setN_L     ( m_nL     ) ;
  m_cb2.setAlpha_R ( m_alphaR ) ;
  m_cb2.setN_R     ( m_nR     ) ;
  //
  return m_cb2     ( m_x      ) ;
}
// ============================================================================


// ============================================================================
// Needham
// ============================================================================
Analysis::Models::Needham::Needham
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          m0        ,
  RooAbsReal&          sigma     ,    
  RooAbsReal&          a0        ,    
  RooAbsReal&          a1        ,    
  RooAbsReal&          a2        )
  : RooAbsPdf ( name , title )
//
  , m_x       ( "x"       , "Observable"                 , this , x      ) 
  , m_m0      ( "m0"      , "mass"                       , this , m0     ) 
  , m_sigma   ( "sigma"   , "sigma"                      , this , sigma  )
//
  , m_a0      ( "a0"      , "a0-parameter"               , this , a0     ) 
  , m_a1      ( "a1"      , "a1-parameter"               , this , a1     ) 
  , m_a2      ( "a2"      , "a2-parameter"               , this , a2     ) 
//
  , m_fixed   ( false ) 
//
  , m_needham ( 100 , 1 , 1.9 , 0 , 0 ) 
{
  //
  m_needham.setM0      ( m_m0     ) ;
  m_needham.setSigma   ( m_sigma  ) ;
  //
  m_needham.setA0      ( m_a0     ) ;
  m_needham.setA1      ( m_a1     ) ;
  m_needham.setA2      ( m_a2     ) ;
  //
}
// ============================================================================
Analysis::Models::Needham::Needham
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          m0        ,
  RooAbsReal&          sigma     ,  
  const double         a0        ,  
  const double         a1        ,  
  const double         a2        ) 
  : RooAbsPdf ( name , title )
//
  , m_x       ( "x"       , "Observable"    , this , x      ) 
  , m_m0      ( "m0"      , "mass"          , this , m0     ) 
  , m_sigma   ( "sigma"   , "sigma"         , this , sigma  )
//
  , m_a0      () 
  , m_a1      ()  
  , m_a2      () 
//
  , m_fixed   ( true  ) 
//
  , m_needham ( 100 , 1 , 1.9 , 0 , 0 ) 
{
  //
  m_needham.setM0      ( m_m0     ) ;
  m_needham.setSigma   ( m_sigma  ) ;
  //
  m_needham.setA0      ( a0       ) ;
  m_needham.setA1      ( a1       ) ;
  m_needham.setA2      ( a2       ) ;
  //
}
// ============================================================================
// copy constructor 
// ============================================================================
Analysis::Models::Needham::Needham
( const Analysis::Models::Needham& right , 
  const char*                      name ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x       ( "x"       , this , right.m_x      ) 
  , m_m0      ( "m0"      , this , right.m_m0     ) 
  , m_sigma   ( "sigma"   , this , right.m_sigma  )
//
  , m_a0      ( "a0"      , this , right.m_a0     ) 
  , m_a1      ( "a1"      , this , right.m_a1     ) 
  , m_a2      ( "a2"      , this , right.m_a2     ) 
//
  , m_fixed   ( right.m_fixed   ) 
  , m_needham ( right.m_needham ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::Needham::~Needham(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Needham*
Analysis::Models::Needham::clone( const char* name ) const 
{ return new Analysis::Models::Needham ( *this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Needham::evaluate() const 
{
  //
  m_needham . setM0    ( m_m0     ) ;
  m_needham . setSigma ( m_sigma  ) ;
  //
  if ( !m_fixed ) 
  {
    m_needham . setA0  ( m_a0     ) ;
    m_needham . setA1  ( m_a1     ) ;
    m_needham . setA2  ( m_a2     ) ;
  }
  //
  return m_needham     ( m_x      ) ;
}
// ============================================================================


// ============================================================================
// Two-body phase space 
// ============================================================================
Analysis::Models::PhaseSpace2::PhaseSpace2
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  const double         m1        , 
  const double         m2        ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x   ( "x" , "Observable" , this , x ) 
//  
  , m_ps2 ( m1 , m2 ) 
{}
// ============================================================================
// "copy constructor"
// ============================================================================
Analysis::Models::PhaseSpace2::PhaseSpace2
( const Analysis::Models::PhaseSpace2& right , const char* name )  
  : RooAbsPdf ( right , name )
//
  , m_x       ( "x"       , this , right.m_x      ) 
//
  , m_ps2     ( right.m_ps2 ) 
//
{}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PhaseSpace2::~PhaseSpace2(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PhaseSpace2*
Analysis::Models::PhaseSpace2::clone( const char* name ) const 
{ return new Analysis::Models::PhaseSpace2(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PhaseSpace2::evaluate() const 
{ return m_ps2 ( m_x ) ; }
// ============================================================================

// ============================================================================
// Two-body phase space from 3-body decays 
// ============================================================================
Analysis::Models::PhaseSpace23L::PhaseSpace23L
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  const double         m1        , 
  const double         m2        ,
  const double         m3        ,
  const double         m         ,
  const unsigned short L         , 
  const unsigned short l         ) 
  : RooAbsPdf ( name , title ) 
  , m_x       ( "x" , "Observable" , this , x ) 
  , m_ps23L   ( m1 , m2 , m3 , m , L , l ) 
{}
// ============================================================================
// "copy constructor"
// ============================================================================
Analysis::Models::PhaseSpace23L::PhaseSpace23L
( const Analysis::Models::PhaseSpace23L& right , const char* name )  
  : RooAbsPdf ( right , name )
  , m_x       ( "x"       , this , right.m_x      ) 
  , m_ps23L   ( right.m_ps23L ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PhaseSpace23L::~PhaseSpace23L(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PhaseSpace23L*
Analysis::Models::PhaseSpace23L::clone( const char* name ) const 
{ return new Analysis::Models::PhaseSpace23L(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PhaseSpace23L::evaluate() const 
{ return m_ps23L ( m_x ) ; }
// ============================================================================

// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::LASS23L::LASS23L
( const char*          name  , 
  const char*          title ,
  RooAbsReal&          x     ,
  RooAbsReal&          m1430 ,
  RooAbsReal&          g1430 ,
  RooAbsReal&          a     , 
  RooAbsReal&          r     , 
  RooAbsReal&          e     ,                
  const double         m1    , 
  const double         m2    ,
  const double         m3    , 
  const double         m     ,
  const unsigned short L     ) 
//
  : RooAbsPdf ( name , title ) 
//
  , m_x     ( "x"     , "Observable"      , this , x     ) 
  , m_m0    ( "m0"    , "K*(1430)-mass"   , this , m1430 ) 
  , m_g0    ( "g0"    , "K*(1430)-width"  , this , g1430 ) 
  , m_a     ( "a"     , "LASS-a"          , this , a     )
  , m_r     ( "r"     , "LASS-r"          , this , r     )
  , m_e     ( "e"     , "LASS-elasticity" , this , e     )
//
  , m_lass  ( m1      , m2      , m3   ,  m  , 
              1430    , 300     , 
              L       , 
              1.94e-3 , 1.76e-1 , 1.0    ) 
{
  m_lass.setM0 ( m_m0 ) ;
  m_lass.setG0 ( m_g0 ) ;
  m_lass.setA  ( m_a  ) ;
  m_lass.setR  ( m_r  ) ;
  m_lass.setE  ( m_e  ) ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::LASS23L::LASS23L 
( const Analysis::Models::LASS23L& right , 
  const char*                      name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x     ( "x"  , this , right.m_x  ) 
  , m_m0    ( "m0" , this , right.m_m0 ) 
  , m_g0    ( "g0" , this , right.m_g0 ) 
  , m_a     ( "a"  , this , right.m_a  ) 
  , m_r     ( "r"  , this , right.m_r  ) 
  , m_e     ( "e"  , this , right.m_e  ) 
//
  , m_lass  ( right.m_lass ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::LASS23L::~LASS23L (){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::LASS23L*
Analysis::Models::LASS23L::clone( const char* name ) const 
{ return new Analysis::Models::LASS23L ( *this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::LASS23L::evaluate() const 
{
  m_lass.setM0  ( m_m0 ) ;
  m_lass.setG0  ( m_g0 ) ;
  m_lass.setA   ( m_a  ) ;
  m_lass.setR   ( m_r  ) ;
  m_lass.setE   ( m_e  ) ;
  //
  return m_lass ( m_x  ) ;
}
// ===========================================================================
// get the complex amplitude 
// ===========================================================================
std::complex<double> Analysis::Models::LASS23L::amplitude() const 
{
  m_lass.setM0  ( m_m0 ) ;
  m_lass.setG0  ( m_g0 ) ;
  m_lass.setA   ( m_a  ) ;
  m_lass.setR   ( m_r  ) ;
  m_lass.setE   ( m_e  ) ;
  //
  return m_lass.amplitude ( m_x  ) ;
}
// ============================================================================


// ============================================================================
// Left-edge of N-body phase space 
// ============================================================================
Analysis::Models::PhaseSpaceLeft::PhaseSpaceLeft
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          threshold ,
  const unsigned short N         ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x         ( "x"  , "Observable" , this , x         ) 
  , m_threshold ( "th" , "Threshold"  , this , threshold  ) 
//  
  , m_left ( 10 , N ) 
{
  m_left.setThreshold ( m_threshold ) ;  
}
// ============================================================================
// "copy constructor"
// ============================================================================
Analysis::Models::PhaseSpaceLeft::PhaseSpaceLeft
( const Analysis::Models::PhaseSpaceLeft& right , const char* name )  
  : RooAbsPdf ( right , name )
//
  , m_x         ( "x"  , this , right.m_x         ) 
  , m_threshold ( "tr" , this , right.m_threshold ) 
//
  , m_left     ( right.m_left ) 
//
{}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PhaseSpaceLeft::~PhaseSpaceLeft(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PhaseSpaceLeft*
Analysis::Models::PhaseSpaceLeft::clone( const char* name ) const 
{ return new Analysis::Models::PhaseSpaceLeft(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PhaseSpaceLeft::evaluate() const 
{
  //
  m_left.setThreshold ( m_threshold ) ;
  //
  return m_left ( m_x ) ; 
}
// ============================================================================


// ============================================================================
// Right-edge of L-body phase space in N-body decays  
// ============================================================================
Analysis::Models::PhaseSpaceRight::PhaseSpaceRight
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          threshold ,
  const unsigned short L         ,
  const unsigned short N         ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x         ( "x"  , "Observable" , this , x         ) 
  , m_threshold ( "th" , "Threshold"  , this , threshold  ) 
//  
  , m_right ( 10 , L , N ) 
{
  m_right.setThreshold ( m_threshold ) ;  
}
// ============================================================================
// "copy constructor"
// ============================================================================
Analysis::Models::PhaseSpaceRight::PhaseSpaceRight
( const Analysis::Models::PhaseSpaceRight& right , const char* name )  
  : RooAbsPdf ( right , name )
//
  , m_x         ( "x"  , this , right.m_x         ) 
  , m_threshold ( "tr" , this , right.m_threshold ) 
//
  , m_right     ( right.m_right ) 
//
{}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PhaseSpaceRight::~PhaseSpaceRight(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PhaseSpaceRight*
Analysis::Models::PhaseSpaceRight::clone( const char* name ) const 
{ return new Analysis::Models::PhaseSpaceRight(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PhaseSpaceRight::evaluate() const 
{
  //
  m_right.setThreshold ( m_threshold ) ;
  //
  return m_right ( m_x ) ; 
}
// ============================================================================


// ============================================================================
//  linear polinomial
// ============================================================================
Analysis::Models::PolyPositive::PolyPositive
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          phi1      , 
  const double         xmin      , 
  const double         xmax      ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x ) 
  , m_phis     ( "phi"     , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( 1 , xmin , xmax ) 
{
  m_phis.add ( phi1 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
//  quadric polinomial
// ============================================================================
Analysis::Models::PolyPositive::PolyPositive
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          phi1      , 
  RooAbsReal&          phi2      , 
  const double         xmin      , 
  const double         xmax      ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x ) 
  , m_phis     ( "phi"     , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( 2 , xmin , xmax ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
//  cubic polinomial
// ============================================================================
Analysis::Models::PolyPositive::PolyPositive
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          phi1      , 
  RooAbsReal&          phi2      , 
  RooAbsReal&          phi3      , 
  const double         xmin      , 
  const double         xmax      ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x ) 
  , m_phis     ( "phi"     , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( 3 , xmin , xmax ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_phis.add ( phi3 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
//  quartic polinomial
// ============================================================================
Analysis::Models::PolyPositive::PolyPositive
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          phi1      , 
  RooAbsReal&          phi2      , 
  RooAbsReal&          phi3      , 
  RooAbsReal&          phi4      , 
  const double         xmin      , 
  const double         xmax      ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x ) 
  , m_phis     ( "phi"     , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( 4 , xmin , xmax ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_phis.add ( phi3 ) ;
  m_phis.add ( phi4 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
//  quintic polinomial
// ============================================================================
Analysis::Models::PolyPositive::PolyPositive
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          phi1      , 
  RooAbsReal&          phi2      , 
  RooAbsReal&          phi3      , 
  RooAbsReal&          phi4      , 
  RooAbsReal&          phi5      , 
  const double         xmin      , 
  const double         xmax      ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x ) 
  , m_phis     ( "phi"     , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( 5 , xmin , xmax ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_phis.add ( phi3 ) ;
  m_phis.add ( phi4 ) ;
  m_phis.add ( phi5 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
// generic polinomial
// ============================================================================
Analysis::Models::PolyPositive::PolyPositive 
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  const RooArgList&    phis      , 
  const double         xmin      , 
  const double         xmax      ) 
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable"   , this , x ) 
  , m_phis     ( "phi"     , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( phis.getSize() , xmin , xmax ) 
{
  //
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
  }
  delete tmp ;
  //
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::PolyPositive::PolyPositive
( const Analysis::Models::PolyPositive&  right ,      
  const char*                            name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_phis     ( "phis"   , this , right.m_phis  ) 
//
  , m_iterator ( 0 ) 
//
  , m_positive ( right.m_positive ) 
{
  m_iterator = m_phis.createIterator () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PolyPositive::~PolyPositive() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PolyPositive*
Analysis::Models::PolyPositive::clone( const char* name ) const 
{ return new Analysis::Models::PolyPositive(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PolyPositive::evaluate() const 
{
  //
  m_iterator->Reset () ;
  //
  RooAbsArg*       phi   = 0 ;
  const RooArgSet* nset  = m_phis.nset() ;
  //
  std::vector<double> sin2phi ;
  //
  unsigned short k = 0 ;
  while ( ( phi = (RooAbsArg*) m_iterator->Next() ) )
  {
    const RooAbsReal* r = dynamic_cast<RooAbsReal*> ( phi ) ;
    if ( 0 == r ) { continue ; }
    //
    const double phi   = r->getVal ( nset ) ;
    //
    m_positive.setPar ( k  , phi ) ;
    //
    ++k ;
  }
  //
  return m_positive ( m_x ) ; 
}
// ============================================================================


// ============================================================================
//  linear polinomial
// ============================================================================
Analysis::Models::ExpoPositive::ExpoPositive
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          tau       , 
  RooAbsReal&          phi1      , 
  const double         xmin      , 
  const double         xmax      ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x   ) 
  , m_tau      ( "tau"     , "Exponential"  , this , tau )
  , m_phis     ( "phi"     , "Coefficients" , this )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( 1 , xmin , xmax ) 
{
  m_phis.add ( phi1 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
//  quadric polinomial
// ============================================================================
Analysis::Models::ExpoPositive::ExpoPositive
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          tau       , 
  RooAbsReal&          phi1      , 
  RooAbsReal&          phi2      , 
  const double         xmin      , 
  const double         xmax      ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x   ) 
  , m_tau      ( "tau"     , "Exponential"  , this , tau )
  , m_phis     ( "phi"     , "Coefficients" , this )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( 2 , xmin , xmax ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
//  cubic polinomial
// ============================================================================
Analysis::Models::ExpoPositive::ExpoPositive
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          tau       , 
  RooAbsReal&          phi1      , 
  RooAbsReal&          phi2      , 
  RooAbsReal&          phi3      , 
  const double         xmin      , 
  const double         xmax      ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x   ) 
  , m_tau      ( "tau"     , "Exponential"  , this , tau )
  , m_phis     ( "phi"     , "Coefficients" , this )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( 3 , xmin , xmax ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_phis.add ( phi3 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
//  quartic polinomial
// ============================================================================
Analysis::Models::ExpoPositive::ExpoPositive
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          tau       , 
  RooAbsReal&          phi1      , 
  RooAbsReal&          phi2      , 
  RooAbsReal&          phi3      , 
  RooAbsReal&          phi4      , 
  const double         xmin      , 
  const double         xmax      ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x   ) 
  , m_tau      ( "tau"     , "Exponential"  , this , tau )
  , m_phis     ( "phi"     , "Coefficients" , this )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( 4 , xmin , xmax ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_phis.add ( phi3 ) ;
  m_phis.add ( phi4 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
//  quintic polinomial
// ============================================================================
Analysis::Models::ExpoPositive::ExpoPositive
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          tau       , 
  RooAbsReal&          phi1      , 
  RooAbsReal&          phi2      , 
  RooAbsReal&          phi3      , 
  RooAbsReal&          phi4      , 
  RooAbsReal&          phi5      , 
  const double         xmin      , 
  const double         xmax      ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x   ) 
  , m_tau      ( "tau"     , "Exponential"  , this , tau )
  , m_phis     ( "phi"     , "Coefficients" , this )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( 5 , xmin , xmax ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_phis.add ( phi3 ) ;
  m_phis.add ( phi4 ) ;
  m_phis.add ( phi5 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
// generic polinomial
// ============================================================================
Analysis::Models::ExpoPositive::ExpoPositive
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          tau       , 
  const RooArgList&    phis      , 
  const double         xmin      , 
  const double         xmax      ) 
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable"   , this , x   ) 
  , m_tau      ( "tau"     , "Exponential"  , this , tau )
  , m_phis     ( "phi"     , "Coefficients" , this )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( phis.getSize() , xmin , xmax ) 
{
  //
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
  }
  delete tmp ;
  //
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::ExpoPositive::ExpoPositive
( const Analysis::Models::ExpoPositive&  right ,      
  const char*                            name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x        ( "x"      , this , right.m_x    ) 
  , m_tau      ( "tau"    , this , right.m_tau  )
  , m_phis     ( "phis"   , this , right.m_phis ) 
//
  , m_iterator ( 0 ) 
//
  , m_positive ( right.m_positive ) 
{
  m_iterator = m_phis.createIterator () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::ExpoPositive::~ExpoPositive() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::ExpoPositive*
Analysis::Models::ExpoPositive::clone( const char* name ) const 
{ return new Analysis::Models::ExpoPositive(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::ExpoPositive::evaluate() const 
{
  //
  m_iterator->Reset () ;
  //
  RooAbsArg*       phi   = 0 ;
  const RooArgSet* nset  = m_phis.nset() ;
  //
  std::vector<double> sin2phi ;
  //
  unsigned short k = 0 ;
  while ( ( phi = (RooAbsArg*) m_iterator->Next() ) )
  {
    const RooAbsReal* r = dynamic_cast<RooAbsReal*> ( phi ) ;
    if ( 0 == r ) { continue ; }
    //
    const double phi   = r->getVal ( nset ) ;
    //
    m_positive.setPar ( k  , phi ) ;
    //
    ++k ;
  }
  //
  const double x = m_x  ;
  const double t = m_tau ;
  //
  const double e = std::exp ( t * x ) ;
  //
  const double xmin = m_positive.xmin () ;
  const double xmax = m_positive.xmax () ;
  //
  const double norm = 
    ( 0 == t ) ? 
    std::abs (              xmax       -             xmin             ) :
    std::abs ( ( std::exp ( xmax * t ) - std::exp  ( xmin * t ) ) / t ) ;
  //
  return m_positive ( m_x ) * e / norm ; 
}
// ============================================================================

// ============================================================================
//         Gram-Charlier type A 
// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::GramCharlierA::GramCharlierA 
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         , 
  RooAbsReal&          m0        , 
  RooAbsReal&          sigma     , 
  RooAbsReal&          kappa3    ,
  RooAbsReal&          kappa4    )
  : RooAbsPdf ( name , title ) 
//
  , m_x       ( "x"       , "Observable" , this , x      ) 
  , m_m0      ( "m0"      , "m0"         , this , m0     ) 
  , m_sigma   ( "sigma"   , "sigma"      , this , sigma  )
  , m_kappa3  ( "kappa3"  , "kappa3"     , this , kappa3 )
  , m_kappa4  ( "kappa4"  , "kappa4"     , this , kappa4 )
//
  , m_gca         ( 10 , 1 , 0 , 0 ) 
{
  //
  m_gca.setM0     ( m_m0     ) ;
  m_gca.setSigma  ( m_sigma  ) ;
  m_gca.setKappa3 ( m_kappa3 ) ;
  m_gca.setKappa4 ( m_kappa4 ) ;
  //
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::GramCharlierA::GramCharlierA
( const Analysis::Models::GramCharlierA& right , 
  const char*                            name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x      ( "x"      , this , right.m_x      ) 
  , m_m0     ( "m0"     , this , right.m_m0     ) 
  , m_sigma  ( "sigma"  , this , right.m_sigma  ) 
  , m_kappa3 ( "kappa3" , this , right.m_kappa3 ) 
  , m_kappa4 ( "kappa4" , this , right.m_kappa4 ) 
//
  , m_gca    ( right.m_gca ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::GramCharlierA::~GramCharlierA(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::GramCharlierA*
Analysis::Models::GramCharlierA::clone( const char* name ) const 
{ return new Analysis::Models::GramCharlierA(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::GramCharlierA::evaluate() const 
{
  //
  m_gca.setM0     ( m_m0     ) ;
  m_gca.setSigma  ( m_sigma  ) ;
  m_gca.setKappa3 ( m_kappa3 ) ;
  m_gca.setKappa4 ( m_kappa4 ) ;
  //
  return m_gca ( m_x ) ;
}
// ============================================================================


// ============================================================================
//         Bukin
// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::Bukin::Bukin 
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         , 
  RooAbsReal&          peak      , 
  RooAbsReal&          sigma     , 
  RooAbsReal&          xi        ,
  RooAbsReal&          rhoL      ,
  RooAbsReal&          rhoR      )
  : RooAbsPdf ( name , title ) 
//
  , m_x       ( "x"       , "Observable" , this , x      ) 
  , m_peak    ( "peak"    , "peak"       , this , peak   ) 
  , m_sigma   ( "sigma"   , "sigma"      , this , sigma  )
  , m_xi      ( "xi"      , "xi"         , this , xi     )
  , m_rhoL    ( "rhoL"    , "rhoL"       , this , rhoL   )
  , m_rhoR    ( "rhoR"    , "rhoR"       , this , rhoR   )
//
  , m_bukin   ( 10 , 1 , 0 , 0 , 0 ) 
{
  //
  m_bukin.setPeak   ( m_peak  ) ;
  m_bukin.setSigma  ( m_sigma ) ;
  m_bukin.setXi     ( m_xi    ) ;
  m_bukin.setRho_L  ( m_rhoL  ) ;
  m_bukin.setRho_R  ( m_rhoR  ) ;
  //
} 
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Bukin::Bukin
( const Analysis::Models::Bukin& right  , 
  const char*                    name   ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x      ( "x"      , this , right.m_x      ) 
  , m_peak   ( "peak"   , this , right.m_peak   ) 
  , m_sigma  ( "sigma"  , this , right.m_sigma  ) 
  , m_xi     ( "xi"     , this , right.m_xi     ) 
  , m_rhoL   ( "rhoL"   , this , right.m_rhoL   ) 
  , m_rhoR   ( "rhoR"   , this , right.m_rhoR   ) 
//
  , m_bukin  ( right.m_bukin ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::Bukin::~Bukin(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Bukin*
Analysis::Models::Bukin::clone( const char* name ) const 
{ return new Analysis::Models::Bukin(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Bukin::evaluate() const 
{
  //
  m_bukin.setPeak   ( m_peak  ) ;
  m_bukin.setSigma  ( m_sigma ) ;
  m_bukin.setXi     ( m_xi    ) ;
  m_bukin.setRhoL   ( m_rhoL  ) ;
  m_bukin.setRhoR   ( m_rhoR  ) ;
  //
  return m_bukin    ( m_x     ) ;
}
// ============================================================================

  



// ============================================================================
//         Voigt
// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::Voigt::Voigt 
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         , 
  RooAbsReal&          m0        , 
  RooAbsReal&          gamma     , 
  RooAbsReal&          sigma     )
  : RooAbsPdf ( name , title ) 
//
  , m_x       ( "x"       , "Observable" , this , x      ) 
  , m_m0      ( "m0"      , "m0"         , this , m0     ) 
  , m_gamma   ( "gamma"   , "gamma"      , this , gamma  )
  , m_sigma   ( "sigma"   , "sigma"      , this , sigma  )
//
  , m_voigt   ( 10 , 1 , 1 ) 
{
  //
  m_voigt.setM0     ( m_m0    ) ;
  m_voigt.setSigma  ( m_sigma  ) ;
  m_voigt.setGamma  ( m_gamma  ) ;
  //
} 
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Voigt::Voigt
( const Analysis::Models::Voigt& right  , 
  const char*                    name   ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x      ( "x"      , this , right.m_x      ) 
  , m_m0     ( "m0"     , this , right.m_m0     ) 
  , m_gamma  ( "gamma"  , this , right.m_gamma  ) 
  , m_sigma  ( "sigma"  , this , right.m_sigma  ) 
//
  , m_voigt  ( right.m_voigt ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::Voigt::~Voigt(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Voigt*
Analysis::Models::Voigt::clone( const char* name ) const 
{ return new Analysis::Models::Voigt(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Voigt::evaluate() const 
{
  //
  m_voigt.setM0     ( m_m0     ) ;
  m_voigt.setSigma  ( m_sigma  ) ;
  m_voigt.setGamma  ( m_gamma  ) ;
  //
  return m_voigt    ( m_x     ) ;
}
// ============================================================================

  
// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::StudentT::StudentT 
( const char*          name  , 
  const char*          title ,
  RooAbsReal&          x     ,
  RooAbsReal&          mu    ,
  RooAbsReal&          sigma ,
  RooAbsReal&          n     )
  : RooAbsPdf  (name ,title ) 
//
  , m_x     ( "x"     , "Observable" , this , x     ) 
  , m_mu    ( "mu"    , "Peak"       , this , mu    ) 
  , m_sigma ( "sigma" , "Width"      , this , sigma )
  , m_n     ( "n"     , "N"          , this , n     )
//
  , m_stt   ( 0 , 1 , 1 ) 
{
  m_stt.setM     ( m_mu    ) ;
  m_stt.setSigma ( m_sigma ) ;
  m_stt.setN     ( m_n     ) ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::StudentT::StudentT
( const Analysis::Models::StudentT& right , 
  const char*                       name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x     ( "x"     , this , right.m_x     ) 
  , m_mu    ( "mu"    , this , right.m_mu    ) 
  , m_sigma ( "sigma" , this , right.m_sigma )
  , m_n     ( "n"     , this , right.m_n     )
//
  , m_stt   (               right.m_stt    ) 
{}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::StudentT::~StudentT(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::StudentT*
Analysis::Models::StudentT::clone( const char* name ) const 
{ return new Analysis::Models::StudentT(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::StudentT::evaluate() const 
{
  m_stt.setM     ( m_mu    ) ;
  m_stt.setSigma ( m_sigma ) ;
  m_stt.setN     ( m_n     ) ;
  //
  return m_stt   ( m_x ) ;
}
// ============================================================================


// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::GammaDist::GammaDist
( const char*          name  , 
  const char*          title ,
  RooAbsReal&          x     ,
  RooAbsReal&          k     ,
  RooAbsReal&          theta )
  : RooAbsPdf  (name ,title ) 
//
  , m_x       ( "x"     , "Observable" , this , x     ) 
  , m_k       ( "k"     , "Shape"      , this , k     ) 
  , m_theta   ( "theta" , "Scale"      , this , theta )
//
  , m_gamma   ( 1 , 1 ) 
{
  m_gamma.setK     ( m_k     ) ;
  m_gamma.setTheta ( m_theta ) ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::GammaDist::GammaDist
( const Analysis::Models::GammaDist& right ,
  const char*                        name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x     ( "x"     , this , right.m_x     ) 
  , m_k     ( "k"     , this , right.m_k     ) 
  , m_theta ( "theta" , this , right.m_theta )
//
  , m_gamma (                  right.m_gamma ) 
{}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::GammaDist::~GammaDist () {}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::GammaDist*
Analysis::Models::GammaDist::clone( const char* name ) const 
{ return new Analysis::Models::GammaDist ( *this , name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::GammaDist::evaluate() const 
{
  m_gamma.setK     ( m_k     ) ;
  m_gamma.setTheta ( m_theta ) ;
  //
  return m_gamma   ( m_x ) ;
}
// ============================================================================

// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::LogGammaDist::LogGammaDist
( const char*          name  , 
  const char*          title ,
  RooAbsReal&          x     ,
  RooAbsReal&          k     ,
  RooAbsReal&          theta )
  : RooAbsPdf  (name ,title ) 
//
  , m_x       ( "x"     , "Observable" , this , x     ) 
  , m_k       ( "k"     , "Shape"      , this , k     ) 
  , m_theta   ( "theta" , "Scale"      , this , theta )
//
  , m_gamma   ( 1 , 1 ) 
{
  m_gamma.setK     ( m_k     ) ;
  m_gamma.setTheta ( m_theta ) ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::LogGammaDist::LogGammaDist
( const Analysis::Models::LogGammaDist& right ,
  const char*                           name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x     ( "x"     , this , right.m_x     ) 
  , m_k     ( "k"     , this , right.m_k     ) 
  , m_theta ( "theta" , this , right.m_theta )
//
  , m_gamma (                  right.m_gamma ) 
{}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::LogGammaDist::~LogGammaDist () {}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::LogGammaDist*
Analysis::Models::LogGammaDist::clone( const char* name ) const 
{ return new Analysis::Models::LogGammaDist ( *this , name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::LogGammaDist::evaluate() const 
{
  m_gamma.setK     ( m_k     ) ;
  m_gamma.setTheta ( m_theta ) ;
  //
  return m_gamma   ( m_x ) ;
}
// ============================================================================

// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::Log10GammaDist::Log10GammaDist
( const char*          name  , 
  const char*          title ,
  RooAbsReal&          x     ,
  RooAbsReal&          k     ,
  RooAbsReal&          theta )
  : RooAbsPdf  (name ,title ) 
//
  , m_x       ( "x"     , "Observable" , this , x     ) 
  , m_k       ( "k"     , "Shape"      , this , k     ) 
  , m_theta   ( "theta" , "Scale"      , this , theta )
//
  , m_gamma   ( 1 , 1 ) 
{
  m_gamma.setK     ( m_k     ) ;
  m_gamma.setTheta ( m_theta ) ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Log10GammaDist::Log10GammaDist
( const Analysis::Models::Log10GammaDist& right ,
  const char*                           name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x     ( "x"     , this , right.m_x     ) 
  , m_k     ( "k"     , this , right.m_k     ) 
  , m_theta ( "theta" , this , right.m_theta )
//
  , m_gamma (                  right.m_gamma ) 
{}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::Log10GammaDist::~Log10GammaDist () {}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Log10GammaDist*
Analysis::Models::Log10GammaDist::clone( const char* name ) const 
{ return new Analysis::Models::Log10GammaDist ( *this , name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Log10GammaDist::evaluate() const 
{
  m_gamma.setK     ( m_k     ) ;
  m_gamma.setTheta ( m_theta ) ;
  //
  return m_gamma   ( m_x ) ;
}
// ============================================================================


// ============================================================================
// constructor from all parameters
// ============================================================================
Analysis::Models::Product::Product 
( const char* name  , 
  const char* title ,
  RooAbsPdf&  pdf1  , 
  RooAbsPdf&  pdf2  ) 
  : RooAbsPdf  (name ,title ) 
//
  , m_pdf1    ( "pdf1"  , "PDF1"  , this , pdf1 ) 
  , m_pdf2    ( "pdf2"  , "PDF2"  , this , pdf2 ) 
{}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Product::Product
( const Analysis::Models::Product& right ,
  const char*                      name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_pdf1  ( "pdf1" , this , right.m_pdf1  ) 
  , m_pdf2  ( "pdf2" , this , right.m_pdf2  ) 
{}
// ============================================================================
// destrcutor 
// ============================================================================
Analysis::Models::Product::~Product(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Product*
Analysis::Models::Product::clone( const char* name ) const 
{ return new Analysis::Models::Product ( *this , name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Product::evaluate() const 
{ return m_pdf1 * m_pdf2 ; }





// ============================================================================
// The END 
// ============================================================================
