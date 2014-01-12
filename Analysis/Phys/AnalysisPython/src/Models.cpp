// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & ST:
// ============================================================================
#include <limits>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/GaudiException.h"
// ============================================================================
// Local
// ============================================================================
#include "Analysis/Models.h"
// ============================================================================
// ROOT 
// ============================================================================
#include "RooArgSet.h"
#include "RooRealVar.h"
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
// constructor from all parameters 
// ============================================================================
Analysis::Models::BreitWigner::BreitWigner 
( const char*                     name  , 
  const char*                     title ,
  RooAbsReal&                     x     ,
  RooAbsReal&                     mass  ,
  RooAbsReal&                     width ,
  const Gaudi::Math::BreitWigner& bw    ) 
  : RooAbsPdf  ( name , title ) 
//
  , m_x     ( "x"  , "Observable" , this , x     ) 
  , m_mass  ( "m0" , "Peak"       , this , mass  ) 
  , m_width ( "g0" , "Width"      , this , width )
//
  , m_bw    ( bw ) 
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
// get the amplitude 
// ============================================================================
std::complex<double> Analysis::Models::BreitWigner::amplitude () const
{
  //
  m_bw.setM0   ( m_mass  ) ;
  m_bw.setGamma ( m_width ) ;
  //
  return m_bw.amplitude ( m_x ) ;
}

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
Analysis::Models::Kstar::Kstar 
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          mass      ,
  RooAbsReal&          width     ,
  const double         k_mass    ,
  const double         pi_mass   ) 
  : Analysis::Models::BreitWigner ( name    , 
                                    title   , 
                                    x       , 
                                    mass    ,
                                    width   ,
                                    k_mass  , 
                                    pi_mass , 
                                    1       , 
                                    Gaudi::Math::BreitWigner::Jackson_A2 )
{}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Kstar::Kstar 
( const Analysis::Models::Kstar& right , 
  const char*                    name  ) 
  : Analysis::Models::BreitWigner ( right , name ) 
{}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::Kstar::~Kstar(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Kstar*
Analysis::Models::Kstar::clone( const char* name ) const 
{ return new Analysis::Models::Kstar(*this,name) ; }

// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::BW23L::BW23L
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          mass      ,
  RooAbsReal&          width     ,
  const double         m1        , 
  const double         m2        ,
  const unsigned short l         , 
  //
  const double         m3        , 
  const double         m         , 
  const double         L         ) 
  : RooAbsPdf ( name , title ) 
  , m_x     ( "x"     , "Observable" , this , x     ) 
  , m_mass  ( "mass"  , "BW/Peak"    , this , mass  ) 
  , m_width ( "wigth" , "BW/Width"   , this , width )
//
  , m_bw      ( 10 , 1 , m1 , m2 , m3 , m , l , L ) 
{
  m_bw.setM0    ( m_mass  ) ;
  m_bw.setGamma ( m_width ) ;
}
// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::BW23L::BW23L
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          mass      ,
  RooAbsReal&          width     ,
  const double         m1        , 
  const double         m2        ,
  const unsigned short l                         ,
  const Gaudi::Math::BreitWigner::JacksonRho rho , 
  const double         m3        , 
  const double         m         , 
  const double         L         ) 
  : RooAbsPdf ( name , title ) 
  , m_x     ( "x"     , "Observable" , this , x     ) 
  , m_mass  ( "mass"  , "BW/Peak"    , this , mass  ) 
  , m_width ( "wigth" , "BW/Width"   , this , width )
//
  , m_bw      ( 10 , 1 , m1 , m2 , m3 , m , l , L , rho ) 
{
  m_bw.setM0    ( m_mass  ) ;
  m_bw.setGamma ( m_width ) ;
}
// ============================================================================
// constructor from main parameters and "shape"
// ============================================================================
Analysis::Models::BW23L::BW23L
( const char*          name      , 
  const char*          title     , 
  RooAbsReal&          x         ,
  RooAbsReal&          mass      ,
  RooAbsReal&          width     ,
  const Gaudi::Math::BW23L& bw   ) // shape 
  : RooAbsPdf ( name , title ) 
  , m_x     ( "x"     , "Observable" , this , x     ) 
  , m_mass  ( "mass"  , "BW/Peak"    , this , mass  ) 
  , m_width ( "wigth" , "BW/Width"   , this , width )
//
  , m_bw        ( bw ) 
{
  m_bw.setM0    ( m_mass  ) ;
  m_bw.setGamma ( m_width ) ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::BW23L::BW23L
( const Analysis::Models::BW23L& right , 
  const char*                     name  ) 
  : RooAbsPdf ( right , name )
//
  , m_x      ( "x"     , this , right.m_x     ) 
  , m_mass   ( "mass"  , this , right.m_mass  ) 
  , m_width  ( "width" , this , right.m_width )
  , m_bw     ( right.m_bw )
{}
// ============================================================================
Analysis::Models::BW23L::~BW23L(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::BW23L* 
Analysis::Models::BW23L::clone ( const char* name ) const 
{ return new Analysis::Models::BW23L( *this , name ) ; }
// ============================================================================
// get the amplitude 
// ============================================================================
std::complex<double> Analysis::Models::BW23L::amplitude () const
{
  m_bw.setM0    ( m_mass  ) ;
  m_bw.setGamma ( m_width ) ;
  return m_bw.amplitude ( m_x ) ;
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::BW23L::evaluate() const 
{
  m_bw.setM0     ( m_mass  ) ;
  m_bw.setGamma0 ( m_width ) ;
  //
  return m_bw ( m_x ) ;
}
  
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
// get the amplitude 
// ===========================================================================
std::complex<double> Analysis::Models::Flatte::amplitude () const  
{
  m_flatte.setM0     ( m_m0    ) ;
  m_flatte.setM0G1   ( m_m0g1  ) ;
  m_flatte.setG2oG1  ( m_g2og1 ) ;
  //
  return m_flatte.amplitude ( m_x ) ;
}

  





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
std::complex<double> Analysis::Models::Flatte2::amplitude () const  
{
  m_flatte.setM0     ( m_m0    ) ;
  m_flatte.setM0G1   ( m_m0g1  ) ;
  m_flatte.setG2oG1  ( m_g2og1 ) ;
  //
  return m_flatte.flatte2_amp ( m_x ) ;
}





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
// constructor form all parameters 
// ============================================================================
Analysis::Models::CrystalBall::CrystalBall
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          m0        ,
  RooAbsReal&          sigma     ,    
  RooAbsReal&          alpha     ,
  RooAbsReal&          n         ) 
  : RooAbsPdf ( name , title )
//
  , m_x       ( "x"       , "Observable"   , this , x      ) 
  , m_m0      ( "m0"      , "CB/mass"      , this , m0     ) 
  , m_sigma   ( "sigma"   , "CB/sigma"     , this , sigma  )
//
  , m_alpha   ( "alpha"   , "CB/alpha"     , this , alpha  ) 
  , m_n       ( "n"       , "CB/n"         , this , n      ) 
//
  , m_cb      ( 100 , 1 , 1 , 10 ) 
{
  //
  m_cb.setM0      ( m_m0     ) ;
  m_cb.setSigma   ( m_sigma  ) ;
  m_cb.setAlpha   ( m_alpha  ) ;
  m_cb.setN       ( m_n      ) ;
  //
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::CrystalBall::CrystalBall
( const Analysis::Models::CrystalBall& right , 
  const char*                          name  ) 
  : RooAbsPdf ( right , name )
//
  , m_x       ( "x"       , this , right.m_x      ) 
  , m_m0      ( "m0"      , this , right.m_m0     ) 
  , m_sigma   ( "sigma"   , this , right.m_sigma  )
//
  , m_alpha   ( "alpha"   , this , right.m_alpha  ) 
  , m_n       ( "n"       , this , right.m_n      ) 
//
  , m_cb      ( 100 , 1 , 1 , 10 ) 
{}
// ============================================================================
Analysis::Models::CrystalBall::~CrystalBall(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::CrystalBall*
Analysis::Models::CrystalBall::clone ( const char* name ) const 
{ return new Analysis::Models::CrystalBall (*this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::CrystalBall::evaluate() const
{
  //
  m_cb.setM0      ( m_m0     ) ;
  m_cb.setSigma   ( m_sigma  ) ;
  m_cb.setAlpha   ( m_alpha  ) ;
  m_cb.setN       ( m_n      ) ;
  //
  return m_cb ( m_x ) ;
}

// ============================================================================
// constructor form all parameters 
// ============================================================================
Analysis::Models::CrystalBallRS::CrystalBallRS
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          m0        ,
  RooAbsReal&          sigma     ,    
  RooAbsReal&          alpha     ,
  RooAbsReal&          n         ) 
  : RooAbsPdf ( name , title )
//
  , m_x       ( "x"       , "Observable"   , this , x      ) 
  , m_m0      ( "m0"      , "CB/mass"      , this , m0     ) 
  , m_sigma   ( "sigma"   , "CB/sigma"     , this , sigma  )
//
  , m_alpha   ( "alpha"   , "CB/alpha"     , this , alpha  ) 
  , m_n       ( "n"       , "CB/n"         , this , n      ) 
//
  , m_cb      ( 100 , 1 , 1 , 10 ) 
{
  //
  m_cb.setM0      ( m_m0     ) ;
  m_cb.setSigma   ( m_sigma  ) ;
  m_cb.setAlpha   ( m_alpha  ) ;
  m_cb.setN       ( m_n      ) ;
  //
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::CrystalBallRS::CrystalBallRS
( const Analysis::Models::CrystalBallRS& right , 
  const char*                          name  ) 
  : RooAbsPdf ( right , name )
//
  , m_x       ( "x"       , this , right.m_x      ) 
  , m_m0      ( "m0"      , this , right.m_m0     ) 
  , m_sigma   ( "sigma"   , this , right.m_sigma  )
//
  , m_alpha   ( "alpha"   , this , right.m_alpha  ) 
  , m_n       ( "n"       , this , right.m_n      ) 
//
  , m_cb      ( 100 , 1 , 1 , 10 ) 
{}
// ============================================================================
Analysis::Models::CrystalBallRS::~CrystalBallRS(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::CrystalBallRS*
Analysis::Models::CrystalBallRS::clone ( const char* name ) const 
{ return new Analysis::Models::CrystalBallRS (*this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::CrystalBallRS::evaluate() const
{
  //
  m_cb.setM0      ( m_m0     ) ;
  m_cb.setSigma   ( m_sigma  ) ;
  m_cb.setAlpha   ( m_alpha  ) ;
  m_cb.setN       ( m_n      ) ;
  //
  return m_cb ( m_x ) ;
}


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
  m_needham . setA0    ( m_a0     ) ;
  m_needham . setA1    ( m_a1     ) ;
  m_needham . setA2    ( m_a2     ) ;
  //
  return m_needham     ( m_x      ) ;
}
// ============================================================================
// get current alpha 
// ============================================================================
double Analysis::Models::Needham::alpha   () const 
{
  const double s  =         m_sigma  ;
  double       a  =         m_a0     ;
  a              += s *     m_a1     ;
  a              += s * s * m_a2     ;
  //
  return a ;
}

// ============================================================================
// Apolonios
// ============================================================================
Analysis::Models::Apolonios::Apolonios
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          m0        ,
  RooAbsReal&          sigma     ,    
  RooAbsReal&          alpha     ,    
  RooAbsReal&          n         ,    
  RooAbsReal&          b         ) 
  : RooAbsPdf ( name , title )
//
  , m_x       ( "x"       , "Observable"                 , this , x      ) 
  , m_m0      ( "m0"      , "mass"                       , this , m0     ) 
  , m_sigma   ( "sigma"   , "sigma"                      , this , sigma  )
  , m_alpha   ( "alpha"   , "alpha"                      , this , alpha  )
  , m_n       ( "n"       , "n-parameter"                , this , n      )
  , m_b       ( "b"       , "b-parameter"                , this , b      )
//
  , m_apo ( 1 , 1 , 1 , 1 , 1 ) 
{
  //
  m_apo.setM0      ( m_m0     ) ;
  m_apo.setSigma   ( m_sigma  ) ;
  m_apo.setAlpha   ( m_alpha  ) ;
  m_apo.setN       ( m_n      ) ;
  m_apo.setB       ( m_b      ) ;
  //
}
// ============================================================================
// copy constructor 
// ============================================================================
Analysis::Models::Apolonios::Apolonios
( const Analysis::Models::Apolonios& right , 
  const char*                        name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x       ( "x"       , this , right.m_x      ) 
  , m_m0      ( "m0"      , this , right.m_m0     ) 
  , m_sigma   ( "sigma"   , this , right.m_sigma  )
  , m_alpha   ( "alpha"   , this , right.m_alpha  )
  , m_n       ( "n"       , this , right.m_n      )
  , m_b       ( "b"       , this , right.m_b      )
//
  , m_apo     ( right.m_apo ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::Apolonios::~Apolonios (){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Apolonios*
Analysis::Models::Apolonios::clone ( const char* name ) const 
{ return new Analysis::Models::Apolonios ( *this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Apolonios::evaluate() const 
{
  //
  m_apo.setM0      ( m_m0     ) ;
  m_apo.setSigma   ( m_sigma  ) ;
  m_apo.setAlpha   ( m_alpha  ) ;
  m_apo.setN       ( m_n      ) ;
  m_apo.setB       ( m_b      ) ;
  //
  return m_apo ( m_x ) ;
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
Analysis::Models::LASS::LASS
( const char*          name  , 
  const char*          title ,
  RooAbsReal&          x     ,
  RooAbsReal&          m1430 ,
  RooAbsReal&          g1430 ,
  RooAbsReal&          a     , 
  RooAbsReal&          r     , 
  RooAbsReal&          e     , 
  const double         m1    , 
  const double         m2    )
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
  , m_lass  ( m1      , m2      , 
              1430    , 300     , 
              1.94e-3 , 1.76e-1 , 1.0 ) 
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
Analysis::Models::LASS::LASS 
( const Analysis::Models::LASS& right , 
  const char*                   name  ) 
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
Analysis::Models::LASS::~LASS (){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::LASS*
Analysis::Models::LASS::clone( const char* name ) const 
{ return new Analysis::Models::LASS ( *this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::LASS::evaluate() const 
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
std::complex<double> Analysis::Models::LASS::amplitude() const 
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
// constructor from all parameters 
// ============================================================================
Analysis::Models::Bugg::Bugg  
( const char*          name               , 
  const char*          title              ,
  RooAbsReal&          x                  ,
  RooAbsReal&          M                  ,   // sigma M 
  RooAbsReal&          g2                 ,   // sigma G2 
  RooAbsReal&          b1                 ,   // sigma B1 
  RooAbsReal&          b2                 ,   // sigma B2
  RooAbsReal&          a                  ,   // sigma a 
  RooAbsReal&          s1                 ,   // sigma s1 
  RooAbsReal&          s2                 ,   // sigma s2 
  const double         m1                 )   // mass of pi GeV 
  : RooAbsPdf ( name , title ) 
//
  , m_x     ( "x"     , "Observable"      , this , x     ) 
  , m_M     ( "M"     , "Bugg/M"          , this , M     ) 
  , m_g2    ( "g2"    , "Bugg/G2"         , this , g2    ) 
  , m_b1    ( "b1"    , "Bugg/b1"         , this , b1    )
  , m_b2    ( "b2"    , "Bugg/b2"         , this , b2    )
  , m_a     ( "a"     , "Bugg/a"          , this , a     )
  , m_s1    ( "s1"    , "Bugg/s1"         , this , s1    )
  , m_s2    ( "s2"    , "Bugg/s2"         , this , s2    )
//
  , m_bugg  ( 0.92 , 0.0024 , 0.5848 , 1.6663 , 1.082 , 2.8 , 3.5 , m1 )  
{
  m_bugg.setM  ( m_M  ) ;
  m_bugg.setG2 ( m_g2 ) ;
  m_bugg.setB1 ( m_b1 ) ;
  m_bugg.setB2 ( m_b2 ) ;
  m_bugg.setA  ( m_a  ) ;
  m_bugg.setS1 ( m_s1 ) ;
  m_bugg.setS2 ( m_s2 ) ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Bugg::Bugg 
( const Analysis::Models::Bugg& right , 
  const char*                      name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x     ( "x"     , this , right.m_x     ) 
  , m_M     ( "M"     , this , right.m_M     ) 
  , m_g2    ( "g2"    , this , right.m_g2    ) 
  , m_b1    ( "b1"    , this , right.m_b1    )
  , m_b2    ( "b2"    , this , right.m_b2    )
  , m_a     ( "a"     , this , right.m_a     )
  , m_s1    ( "s1"    , this , right.m_s1    )
  , m_s2    ( "s2"    , this , right.m_s2    )
//
  , m_bugg  ( right.m_bugg ) 
{
  m_bugg.setM  ( m_M  ) ;
  m_bugg.setG2 ( m_g2 ) ;
  m_bugg.setB1 ( m_b1 ) ;
  m_bugg.setB2 ( m_b2 ) ;
  m_bugg.setA  ( m_a  ) ;
  m_bugg.setS1 ( m_s1 ) ;
  m_bugg.setS2 ( m_s2 ) ;
}
// ============================================================================
Analysis::Models::Bugg::~Bugg(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Bugg*
Analysis::Models::Bugg::clone( const char* name ) const 
{ return new Analysis::Models::Bugg ( *this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Bugg::evaluate() const 
{
  m_bugg.setM  ( m_M  ) ;
  m_bugg.setG2 ( m_g2 ) ;
  m_bugg.setB1 ( m_b1 ) ;
  m_bugg.setB2 ( m_b2 ) ;
  m_bugg.setA  ( m_a  ) ;
  m_bugg.setS1 ( m_s1 ) ;
  m_bugg.setS2 ( m_s2 ) ;
  //
  return m_bugg ( m_x  ) ;
}
// ===========================================================================
// get the complex amplitude 
// ===========================================================================
std::complex<double> Analysis::Models::Bugg::amplitude() const 
{
  m_bugg.setM  ( m_M  ) ;
  m_bugg.setG2 ( m_g2 ) ;
  m_bugg.setB1 ( m_b1 ) ;
  m_bugg.setB2 ( m_b2 ) ;
  m_bugg.setA  ( m_a  ) ;
  m_bugg.setS1 ( m_s1 ) ;
  m_bugg.setS2 ( m_s2 ) ;
  //
  return m_bugg.amplitude ( m_x  ) ;
}
// ============================================================================




// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::Bugg23L::Bugg23L  
( const char*          name               , 
  const char*          title              ,
  RooAbsReal&          x                  ,
  RooAbsReal&          M                  ,   // sigma M 
  RooAbsReal&          g2                 ,   // sigma G2 
  RooAbsReal&          b1                 ,   // sigma B1 
  RooAbsReal&          b2                 ,   // sigma B2
  RooAbsReal&          a                  ,   // sigma a 
  RooAbsReal&          s1                 ,   // sigma s1 
  RooAbsReal&          s2                 ,   // sigma s2 
  const double         m1                 ,   // mass of pi GeV 
  const double         m3                 ,   // mass of third particle 
  const double         m                  ,   // mass of mother  
  const unsigned short L                  )
  : RooAbsPdf ( name , title ) 
//
  , m_x     ( "x"     , "Observable"      , this , x     ) 
  , m_M     ( "M"     , "Bugg/M"          , this , M     ) 
  , m_g2    ( "g2"    , "Bugg/G2"         , this , g2    ) 
  , m_b1    ( "b1"    , "Bugg/b1"         , this , b1    )
  , m_b2    ( "b2"    , "Bugg/b2"         , this , b2    )
  , m_a     ( "a"     , "Bugg/a"          , this , a     )
  , m_s1    ( "s1"    , "Bugg/s1"         , this , s1    )
  , m_s2    ( "s2"    , "Bugg/s2"         , this , s2    )
//
  , m_bugg  ( 0.92 , 0.0024 , 0.5848 , 1.6663 , 1.082 , 2.8 , 3.5 , m1 , m3 , m , L )  
{
  m_bugg.setM  ( m_M  ) ;
  m_bugg.setG2 ( m_g2 ) ;
  m_bugg.setB1 ( m_b1 ) ;
  m_bugg.setB2 ( m_b2 ) ;
  m_bugg.setA  ( m_a  ) ;
  m_bugg.setS1 ( m_s1 ) ;
  m_bugg.setS2 ( m_s2 ) ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Bugg23L::Bugg23L 
( const Analysis::Models::Bugg23L& right , 
  const char*                      name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x     ( "x"     , this , right.m_x     ) 
  , m_M     ( "M"     , this , right.m_M     ) 
  , m_g2    ( "g2"    , this , right.m_g2    ) 
  , m_b1    ( "b1"    , this , right.m_b1    )
  , m_b2    ( "b2"    , this , right.m_b2    )
  , m_a     ( "a"     , this , right.m_a     )
  , m_s1    ( "s1"    , this , right.m_s1    )
  , m_s2    ( "s2"    , this , right.m_s2    )
//
  , m_bugg  ( right.m_bugg ) 
{
  m_bugg.setM  ( m_M  ) ;
  m_bugg.setG2 ( m_g2 ) ;
  m_bugg.setB1 ( m_b1 ) ;
  m_bugg.setB2 ( m_b2 ) ;
  m_bugg.setA  ( m_a  ) ;
  m_bugg.setS1 ( m_s1 ) ;
  m_bugg.setS2 ( m_s2 ) ;
}
// ============================================================================
Analysis::Models::Bugg23L::~Bugg23L(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Bugg23L*
Analysis::Models::Bugg23L::clone( const char* name ) const 
{ return new Analysis::Models::Bugg23L ( *this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Bugg23L::evaluate() const 
{
  m_bugg.setM  ( m_M  ) ;
  m_bugg.setG2 ( m_g2 ) ;
  m_bugg.setB1 ( m_b1 ) ;
  m_bugg.setB2 ( m_b2 ) ;
  m_bugg.setA  ( m_a  ) ;
  m_bugg.setS1 ( m_s1 ) ;
  m_bugg.setS2 ( m_s2 ) ;
  //
  return m_bugg ( m_x  ) ;
}
// ===========================================================================
// get the complex amplitude 
// ===========================================================================
std::complex<double> 
Analysis::Models::Bugg23L::amplitude() const 
{
  m_bugg.setM  ( m_M  ) ;
  m_bugg.setG2 ( m_g2 ) ;
  m_bugg.setB1 ( m_b1 ) ;
  m_bugg.setB2 ( m_b2 ) ;
  m_bugg.setA  ( m_a  ) ;
  m_bugg.setS1 ( m_s1 ) ;
  m_bugg.setS2 ( m_s2 ) ;
  //
  return m_bugg.amplitude ( m_x  ) ;
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
// Bifurcated Gauss 
// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::BifurcatedGauss::BifurcatedGauss 
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         , 
  RooAbsReal&          peak      , 
  RooAbsReal&          sigmaL    , 
  RooAbsReal&          sigmaR    ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x       ( "x"       , "Observable"   , this , x      ) 
  , m_peak    ( "peak"    , "peak"         , this , peak   ) 
  , m_sigmaL  ( "sigmaL"  , "sigma(left)"  , this , sigmaL )
  , m_sigmaR  ( "sigmaR"  , "sigma(right)" , this , sigmaR )
//
  , m_bg      ( 0 , 1 , 1 ) 
{
  m_bg . setPeak   ( m_peak   ) ;
  m_bg . setSigmaL ( m_sigmaL ) ;
  m_bg . setSigmaR ( m_sigmaR ) ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::BifurcatedGauss::BifurcatedGauss 
( const Analysis::Models::BifurcatedGauss& right , 
  const char*                              name   ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x      ( "x"      , this , right.m_x      ) 
  , m_peak   ( "peak"   , this , right.m_peak   ) 
  , m_sigmaL ( "sigmaL" , this , right.m_sigmaL ) 
  , m_sigmaR ( "sigmaR" , this , right.m_sigmaR ) 
//
  , m_bg     ( right.m_bg ) 
{}
// ============================================================================
// desctructor
// ============================================================================
Analysis::Models::BifurcatedGauss::~BifurcatedGauss(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::BifurcatedGauss*
Analysis::Models::BifurcatedGauss::clone( const char* name ) const 
{ return new Analysis::Models::BifurcatedGauss ( *this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::BifurcatedGauss::evaluate() const 
{
  //
  m_bg.setPeak   ( m_peak   ) ;
  m_bg.setSigmaL ( m_sigmaL ) ;
  m_bg.setSigmaR ( m_sigmaR ) ;
  //
  return m_bg    ( m_x      ) ;
}
// ============================================================================



// ============================================================================
//         GenGaussV1 
// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::GenGaussV1::GenGaussV1
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         , 
  RooAbsReal&          mu        , 
  RooAbsReal&          alpha     , 
  RooAbsReal&          beta      )  
  : RooAbsPdf ( name , title ) 
//
  , m_x       ( "x"       , "Observable" , this , x      ) 
  , m_mu      ( "mu"      , "mu"         , this , mu     ) 
  , m_alpha   ( "alpha"   , "alpha"      , this , alpha  ) 
  , m_beta    ( "beta"    , "beta"       , this , beta   ) 
//
  , m_ggv1    ( 0 , 1 , 2 ) 
{
  //
  m_ggv1.setMu     ( m_mu    ) ;
  m_ggv1.setAlpha  ( m_alpha ) ;
  m_ggv1.setBeta   ( m_beta  ) ;
  //
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::GenGaussV1::GenGaussV1 
( const Analysis::Models::GenGaussV1& right , 
  const char*                         name   ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x      ( "x"      , this , right.m_x      ) 
  , m_mu     ( "mu"     , this , right.m_mu     ) 
  , m_alpha  ( "alpha"  , this , right.m_alpha  ) 
  , m_beta   ( "beta"   , this , right.m_beta   ) 
//
  , m_ggv1   ( right.m_ggv1 ) 
{}
// ============================================================================
// desctructor
// ============================================================================
Analysis::Models::GenGaussV1::~GenGaussV1(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::GenGaussV1*
Analysis::Models::GenGaussV1::clone( const char* name ) const 
{ return new Analysis::Models::GenGaussV1 ( *this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::GenGaussV1::evaluate() const 
{
  //
  m_ggv1.setMu     ( m_mu    ) ;
  m_ggv1.setAlpha  ( m_alpha ) ;
  m_ggv1.setBeta   ( m_beta  ) ;
  //
  return m_ggv1    ( m_x      ) ;
}
// ============================================================================


// ============================================================================
//         GenGaussV2
// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::GenGaussV2::GenGaussV2
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         , 
  RooAbsReal&          xi        , 
  RooAbsReal&          alpha     , 
  RooAbsReal&          kappa     )  
  : RooAbsPdf ( name , title ) 
//
  , m_x       ( "x"       , "Observable" , this , x      ) 
  , m_xi      ( "xi"      , "xi"         , this , xi     ) 
  , m_alpha   ( "alpha"   , "alpha"      , this , alpha  ) 
  , m_kappa   ( "kappa"   , "kappa"      , this , kappa  ) 
//
  , m_ggv2    ( 0 , 1 , 0 ) 
{
  //
  m_ggv2.setXi     ( m_xi    ) ;
  m_ggv2.setAlpha  ( m_alpha ) ;
  m_ggv2.setKappa  ( m_kappa ) ;
  //
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::GenGaussV2::GenGaussV2 
( const Analysis::Models::GenGaussV2& right , 
  const char*                         name   ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x      ( "x"      , this , right.m_x      ) 
  , m_xi     ( "xi"     , this , right.m_xi     ) 
  , m_alpha  ( "alpha"  , this , right.m_alpha  ) 
  , m_kappa  ( "kappa"  , this , right.m_kappa  ) 
//
  , m_ggv2   ( right.m_ggv2 ) 
{}
// ============================================================================
// desctructor
// ============================================================================
Analysis::Models::GenGaussV2::~GenGaussV2(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::GenGaussV2*
Analysis::Models::GenGaussV2::clone( const char* name ) const 
{ return new Analysis::Models::GenGaussV2 ( *this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::GenGaussV2::evaluate() const 
{
  //
  m_ggv2.setXi     ( m_xi    ) ;
  m_ggv2.setAlpha  ( m_alpha ) ;
  m_ggv2.setKappa  ( m_kappa ) ;
  //
  return m_ggv2    ( m_x      ) ;
}
// ============================================================================


// ============================================================================
//         SkewGauss
// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::SkewGauss::SkewGauss
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         , 
  RooAbsReal&          xi        , 
  RooAbsReal&          omega     , 
  RooAbsReal&          alpha     )  
  : RooAbsPdf ( name , title ) 
//
  , m_x       ( "x"       , "Observable" , this , x      ) 
  , m_xi      ( "xi"      , "xi"         , this , xi     ) 
  , m_omega   ( "omega"   , "omega"      , this , omega  ) 
  , m_alpha   ( "alpha"   , "alpha"      , this , alpha  ) 
//
  , m_sg    ( 0 , 1 , 0 ) 
{
  //
  m_sg . setXi     ( m_xi    ) ;
  m_sg . setOmega  ( m_omega ) ;
  m_sg . setAlpha  ( m_alpha ) ;
  //
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::SkewGauss::SkewGauss
( const Analysis::Models::SkewGauss& right , 
  const char*                        name   ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x      ( "x"      , this , right.m_x      ) 
  , m_xi     ( "xi"     , this , right.m_xi     ) 
  , m_omega  ( "omega"  , this , right.m_omega  ) 
  , m_alpha  ( "alpha"  , this , right.m_alpha  ) 
//
  , m_sg     ( right.m_sg ) 
{}
// ============================================================================
// desctructor
// ============================================================================
Analysis::Models::SkewGauss::~SkewGauss (){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::SkewGauss*
Analysis::Models::SkewGauss::clone( const char* name ) const 
{ return new Analysis::Models::SkewGauss ( *this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::SkewGauss::evaluate() const 
{
  //
  m_sg . setXi     ( m_xi    ) ;
  m_sg . setOmega  ( m_omega ) ;
  m_sg . setAlpha  ( m_alpha ) ;
  //
  return m_sg      ( m_x      ) ;
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
Analysis::Models::GenGammaDist::GenGammaDist
( const char*          name  , 
  const char*          title ,
  RooAbsReal&          x     ,
  RooAbsReal&          k     ,
  RooAbsReal&          theta ,
  RooAbsReal&          p     ,
  RooAbsReal&          low   )
  : RooAbsPdf  (name ,title ) 
//
  , m_x       ( "x"     , "Observable" , this , x     ) 
  , m_k       ( "k"     , "Shape"      , this , k     ) 
  , m_theta   ( "theta" , "Scale"      , this , theta )
  , m_p       ( "p"     , "P"          , this , p     )
  , m_low     ( "low"   , "Low"        , this , low   )
//
  , m_ggamma   ( 2 , 1 , 1 , 0 ) 
{
  m_ggamma.setK     ( m_k     ) ;
  m_ggamma.setTheta ( m_theta ) ;
  m_ggamma.setP     ( m_p     ) ;
  m_ggamma.setLow   ( m_low   ) ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::GenGammaDist::GenGammaDist
( const Analysis::Models::GenGammaDist& right ,
  const char*                           name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x      ( "x"     , this , right.m_x      ) 
  , m_k      ( "k"     , this , right.m_k      ) 
  , m_theta  ( "theta" , this , right.m_theta  )
  , m_p      ( "p"     , this , right.m_p      )
  , m_low    ( "low"   , this , right.m_low    )
//
  , m_ggamma (                  right.m_ggamma ) 
{}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::GenGammaDist::~GenGammaDist () {}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::GenGammaDist*
Analysis::Models::GenGammaDist::clone( const char* name ) const 
{ return new Analysis::Models::GenGammaDist ( *this , name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::GenGammaDist::evaluate() const 
{
  m_ggamma.setK     ( m_k     ) ;
  m_ggamma.setTheta ( m_theta ) ;
  m_ggamma.setP     ( m_p     ) ;
  m_ggamma.setLow   ( m_low   ) ;
  //
  return m_ggamma   ( m_x ) ;
}
// ============================================================================


// ============================================================================
//    Amoroso
// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::Amoroso::Amoroso
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         , 
  RooAbsReal&          theta     , 
  RooAbsReal&          alpha     , 
  RooAbsReal&          beta      ,
  RooAbsReal&          a         ) 
  : RooAbsPdf ( name ,title ) 
//
  , m_x       ( "x"     , "Observable" , this , x     ) 
  , m_theta   ( "theta" , "theta"      , this , theta )
  , m_alpha   ( "alpha" , "alpha"      , this , alpha )
  , m_beta    ( "beta"  , "beta"       , this , beta  )
  , m_a       ( "a"     , "a"          , this , a     )
//
  , m_amoroso   ( 1 , 1 , 1 , 0 ) 
{
  m_amoroso.setTheta     ( m_theta ) ;
  m_amoroso.setAlpha     ( m_alpha ) ;
  m_amoroso.setBeta      ( m_beta  ) ;
  m_amoroso.setA         ( m_a     ) ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Amoroso::Amoroso
( const Analysis::Models::Amoroso& right , 
  const char*                      name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x       ( "x"     , this , right.m_x     ) 
  , m_theta   ( "theta" , this , right.m_theta )
  , m_alpha   ( "alpha" , this , right.m_alpha )
  , m_beta    ( "beta"  , this , right.m_beta  )
  , m_a       ( "a"     , this , right.m_a     )
//
  , m_amoroso ( right.m_amoroso ) 
{}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::Amoroso::~Amoroso () {}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Amoroso*
Analysis::Models::Amoroso::clone( const char* name ) const 
{ return new Analysis::Models::Amoroso ( *this , name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Amoroso::evaluate() const 
{
  m_amoroso.setTheta ( m_theta ) ;
  m_amoroso.setAlpha ( m_alpha ) ;
  m_amoroso.setBeta  ( m_beta  ) ;
  m_amoroso.setA     ( m_a     ) ;
  //
  return m_amoroso   ( m_x     ) ;
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
Analysis::Models::LogGamma::LogGamma
( const char*          name   , 
  const char*          title  ,
  RooAbsReal&          x      ,
  RooAbsReal&          nu     ,
  RooAbsReal&          lambda ,
  RooAbsReal&          alpha  )
  : RooAbsPdf ( name , title ) 
//
  , m_x       ( "x"      , "Observable" , this , x      ) 
  , m_nu      ( "nu"     , "nu"         , this , nu     ) 
  , m_lambda  ( "lambda" , "lambda"     , this , lambda ) 
  , m_alpha   ( "alpha"  , "alpha"      , this , alpha  )
//
  , m_lgamma   ( 0 , 1 , 1 ) 
{
  m_lgamma.setNu     ( m_nu     ) ;
  m_lgamma.setLambda ( m_lambda ) ;
  m_lgamma.setAlpha  ( m_alpha  ) ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::LogGamma::LogGamma
( const Analysis::Models::LogGamma& right ,
  const char*                       name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x      ( "x"      , this , right.m_x      ) 
  , m_nu     ( "nu"     , this , right.m_nu     ) 
  , m_lambda ( "lambda" , this , right.m_lambda )
  , m_alpha  ( "alpha"  , this , right.m_alpha  )
//
  , m_lgamma (                   right.m_lgamma ) 
{}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::LogGamma::~LogGamma () {}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::LogGamma*
Analysis::Models::LogGamma::clone( const char* name ) const 
{ return new Analysis::Models::LogGamma ( *this , name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::LogGamma::evaluate() const 
{
  //
  m_lgamma.setNu     ( m_nu     ) ;
  m_lgamma.setLambda ( m_lambda ) ;
  m_lgamma.setAlpha  ( m_alpha  ) ;
  //
  return m_lgamma    ( m_x ) ;
}
// ============================================================================


// ============================================================================
//  PhaseSpace x poly
// ============================================================================
Analysis::Models::PhaseSpacePol::PhaseSpacePol 
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  const double         low       , 
  const double         high      ,
  const unsigned short N         , 
  const unsigned short L         , 
  RooAbsReal&          phi1      ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x   ) 
  , m_phis     ( "phi"     , "Coefficients" , this )
//
  , m_iterator ( 0 ) 
//
  , m_ps       (     low , high , L , N ) 
  , m_positive ( 1 , low , high ) 
{
  m_phis.add ( phi1 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
//  PhaseSpace x poly
// ============================================================================
Analysis::Models::PhaseSpacePol::PhaseSpacePol 
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  const double         low       , 
  const double         high      ,
  const unsigned short N         , 
  const unsigned short L         , 
  RooAbsReal&          phi1      ,
  RooAbsReal&          phi2      ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x   ) 
  , m_phis     ( "phi"     , "Coefficients" , this )
//
  , m_iterator ( 0 ) 
//
  , m_ps       (     low , high , L , N ) 
  , m_positive ( 2 , low , high ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
//  PhaseSpace x poly
// ============================================================================
Analysis::Models::PhaseSpacePol::PhaseSpacePol 
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  const double         low       , 
  const double         high      ,
  const unsigned short N         , 
  const unsigned short L         , 
  RooAbsReal&          phi1      ,
  RooAbsReal&          phi2      , 
  RooAbsReal&          phi3      ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x   ) 
  , m_phis     ( "phi"     , "Coefficients" , this )
//
  , m_iterator ( 0 ) 
//
  , m_ps       (     low , high , L , N ) 
  , m_positive ( 3 , low , high ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_phis.add ( phi3 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
//  PhaseSpace x poly
// ============================================================================
Analysis::Models::PhaseSpacePol::PhaseSpacePol 
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  const double         low       , 
  const double         high      ,
  const unsigned short N         , 
  const unsigned short L         , 
  RooArgList&          phis      )
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x    ) 
  , m_phis     ( "phi"     , "Coefficients" , this )
//
  , m_iterator ( 0 ) 
//
  , m_ps       (                  low , high , L , N ) 
  , m_positive ( phis.getSize() , low , high ) 
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
//  PhaseSpace x poly
// ============================================================================
Analysis::Models::PhaseSpacePol::PhaseSpacePol 
( const char*                      name      , 
  const char*                      title     ,
  RooAbsReal&                      x         ,
  const Gaudi::Math::PhaseSpaceNL& ps        , 
  RooAbsReal&                      phi1      )
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x   ) 
  , m_phis     ( "phi"     , "Coefficients" , this )
//
  , m_iterator ( 0 ) 
//
  , m_ps       ( ps ) 
  , m_positive ( 1 , ps.lowEdge() , ps.highEdge() ) 
{
  m_phis.add ( phi1 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
//  PhaseSpace x poly
// ============================================================================
Analysis::Models::PhaseSpacePol::PhaseSpacePol 
( const char*                      name      , 
  const char*                      title     ,
  RooAbsReal&                      x         ,
  const Gaudi::Math::PhaseSpaceNL& ps        , 
  RooAbsReal&                      phi1      ,
  RooAbsReal&                      phi2      )
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x   ) 
  , m_phis     ( "phi"     , "Coefficients" , this )
//
  , m_iterator ( 0 ) 
//
  , m_ps       ( ps ) 
  , m_positive ( 2 , ps.lowEdge() , ps.highEdge() ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
//  PhaseSpace x poly
// ============================================================================
Analysis::Models::PhaseSpacePol::PhaseSpacePol 
( const char*                      name      , 
  const char*                      title     ,
  RooAbsReal&                      x         ,
  const Gaudi::Math::PhaseSpaceNL& ps        , 
  RooAbsReal&                      phi1      ,
  RooAbsReal&                      phi2      ,
  RooAbsReal&                      phi3      )
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x   ) 
  , m_phis     ( "phi"     , "Coefficients" , this )
//
  , m_iterator ( 0 ) 
//
  , m_ps       ( ps ) 
  , m_positive ( 3 , ps.lowEdge() , ps.highEdge() ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_phis.add ( phi3 ) ;
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
//  PhaseSpace x poly
// ============================================================================
Analysis::Models::PhaseSpacePol::PhaseSpacePol 
( const char*                      name      , 
  const char*                      title     ,
  RooAbsReal&                      x         ,
  const Gaudi::Math::PhaseSpaceNL& ps        , 
  RooArgList&                      phis      )
  : RooAbsPdf ( name , title ) 
//
  , m_x        ( "x"       , "Observable"   , this , x    ) 
  , m_phis     ( "phi"     , "Coefficients" , this )
//
  , m_iterator ( 0 ) 
//
  , m_ps       ( ps ) 
  , m_positive ( phis.getSize() , ps.lowEdge() , ps.highEdge() ) 
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
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PhaseSpacePol::~PhaseSpacePol () { delete m_iterator; }
// ======================================================================
// "copy" constructor 
// ======================================================================
Analysis::Models::PhaseSpacePol::PhaseSpacePol 
( const Analysis::Models::PhaseSpacePol& right , 
  const char*                            name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x        ( "x"      , this , right.m_x    ) 
  , m_phis     ( "phis"   , this , right.m_phis ) 
//
  , m_iterator ( 0 ) 
//
  , m_ps       ( right.m_ps       ) 
  , m_positive ( right.m_positive ) 
{
  m_iterator = m_phis.createIterator () ;
}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PhaseSpacePol* 
Analysis::Models::PhaseSpacePol::clone ( const char* name ) const 
{ return new Analysis::Models::PhaseSpacePol ( *this , name ) ; }
// ============================================================================
// evaluate the function
// ============================================================================
Double_t Analysis::Models::PhaseSpacePol::evaluate () const 
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
  return m_ps ( m_x ) * m_positive ( m_x ) ;
}
// ============================================================================



// ============================================================================
// generic polinomial
// ============================================================================
Analysis::Models::Poly2DPositive::Poly2DPositive 
( const char*          name      , 
  const char*          title     ,
  RooRealVar&          x         ,
  RooRealVar&          y         ,
  const unsigned short nX        , 
  const unsigned short nY        ,
  RooArgList&          phis      ) 
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable-X" , this , x ) 
  , m_y        ( "y"       , "Observable-Y" , this , y ) 
  , m_phis     ( "phis"    , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( nX , nY , x.getMin() , x.getMax() , y.getMin() , y.getMax() ) 
{
  //
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  unsigned int num = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_positive.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_positive.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::Poly2DPositive"         , 
                           StatusCode::FAILURE                ) ; }
  
  //
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::Poly2DPositive::Poly2DPositive
( const Analysis::Models::Poly2DPositive&  right ,      
  const char*                              name  ) 
  : RooAbsPdf  ( right , name ) 
//
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_y        ( "y"      , this , right.m_y     ) 
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
Analysis::Models::Poly2DPositive::~Poly2DPositive() 
{ if ( 0 != m_iterator ) { delete m_iterator ; } }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Poly2DPositive*
Analysis::Models::Poly2DPositive::clone( const char* name ) const 
{ return new Analysis::Models::Poly2DPositive(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Poly2DPositive::evaluate() const 
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
    const double phiv   = r->getVal ( nset ) ;
    //
    m_positive.setPar ( k  , phiv ) ;
    //
    ++k ;
  }
  //
  return m_positive ( m_x , m_y ) ; 
}
// ============================================================================




// ============================================================================
// generic polinomial
// ============================================================================
Analysis::Models::Poly2DSymPositive::Poly2DSymPositive 
( const char*          name      , 
  const char*          title     ,
  RooRealVar&          x         ,
  RooRealVar&          y         ,
  const unsigned short n         ,
  RooArgList&          phis      ) 
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable-X" , this , x ) 
  , m_y        ( "y"       , "Observable-Y" , this , y ) 
  , m_phis     ( "phis"    , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( n , x.getMin() , x.getMax() ) 
{
  //
  if (  x.getMin() != y.getMin() || x.getMax() != y.getMax() )
  { throw GaudiException ( "Non-equal x/y-edges "             , 
                           "Analysis::Poly2DSymPositive"      , 
                           StatusCode::FAILURE                ) ; }
  
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  unsigned num = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_positive.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_positive.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::Poly2DSymPositive"      , 
                           StatusCode::FAILURE                ) ; }
  
  //
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::Poly2DSymPositive::Poly2DSymPositive
( const Analysis::Models::Poly2DSymPositive&  right ,      
  const char*                                 name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_y        ( "y"      , this , right.m_y     ) 
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
Analysis::Models::Poly2DSymPositive::~Poly2DSymPositive() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Poly2DSymPositive*
Analysis::Models::Poly2DSymPositive::clone( const char* name ) const 
{ return new Analysis::Models::Poly2DSymPositive(*this,name) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Poly2DSymPositive::evaluate() const 
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
    const double phiv   = r->getVal ( nset ) ;
    //
    m_positive.setPar ( k  , phiv ) ;
    //
    ++k ;
  }
  //
  return m_positive ( m_x , m_y ) ; 
}
// ============================================================================





// ============================================================================
// generic polinomial
// ============================================================================
Analysis::Models::PS22DPol::PS22DPol
( const char*          name      , 
  const char*          title     ,
  RooRealVar&          x         ,
  RooRealVar&          y         ,
  const Gaudi::Math::PhaseSpace2& ps1       , 
  const Gaudi::Math::PhaseSpace2& ps2       ,
  const unsigned short nX        ,
  const unsigned short nY        ,
  RooArgList&          phis      ) 
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable-X" , this , x ) 
  , m_y        ( "y"       , "Observable-Y" , this , y ) 
  , m_phis     ( "phis"    , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( nX , nY , x.getMin() , x.getMax() , y.getMin() , y.getMax() )
  , m_ps1      ( ps1 ) 
  , m_ps2      ( ps2 )
{
  //
  
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  unsigned num = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_positive.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_positive.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::PS22DPol"               , 
                           StatusCode::FAILURE                ) ; }
  
  //
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::PS22DPol::PS22DPol
( const Analysis::Models::PS22DPol&  right ,      
  const char*                        name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_y        ( "y"      , this , right.m_y     ) 
  , m_phis     ( "phis"   , this , right.m_phis  ) 
//
  , m_iterator ( 0 ) 
//
  , m_positive ( right.m_positive ) 
  , m_ps1      ( right.m_ps1      ) 
  , m_ps2      ( right.m_ps2      ) 
{
  m_iterator = m_phis.createIterator () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PS22DPol::~PS22DPol() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PS22DPol*
Analysis::Models::PS22DPol::clone( const char* name ) const 
{ return new Analysis::Models::PS22DPol ( *this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PS22DPol::evaluate() const 
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
    const double phiv   = r->getVal ( nset ) ;
    //
    m_positive.setPar ( k  , phiv ) ;
    //
    ++k ;
  }
  //
  return m_positive ( m_x , m_y ) * m_ps1 ( m_x ) * m_ps2 ( m_y ) ;
}
// ============================================================================




// ============================================================================
// generic polinomial
// ============================================================================
Analysis::Models::PSnl2DPol::PSnl2DPol
( const char*          name      , 
  const char*          title     ,
  RooRealVar&          x         ,
  RooRealVar&          y         ,
  const Gaudi::Math::PhaseSpaceNL& ps1 , 
  const Gaudi::Math::PhaseSpaceNL& ps2 ,
  const unsigned short nX        ,
  const unsigned short nY        ,
  RooArgList&          phis      ) 
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable-X" , this , x ) 
  , m_y        ( "y"       , "Observable-Y" , this , y ) 
  , m_phis     ( "phis"    , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( nX , nY , x.getMin() , x.getMax() , y.getMin() , y.getMax() )
  , m_ps1      ( ps1 ) 
  , m_ps2      ( ps2 )
{
  //
  
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  unsigned num = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_positive.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_positive.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::PSnl2DPol"              , 
                           StatusCode::FAILURE                ) ; }
  
  //
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::PSnl2DPol::PSnl2DPol
( const Analysis::Models::PSnl2DPol&  right ,      
  const char*                         name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_y        ( "y"      , this , right.m_y     ) 
  , m_phis     ( "phis"   , this , right.m_phis  ) 
//
  , m_iterator ( 0 ) 
//
  , m_positive ( right.m_positive ) 
  , m_ps1      ( right.m_ps1      ) 
  , m_ps2      ( right.m_ps2      ) 
{
  m_iterator = m_phis.createIterator () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PSnl2DPol::~PSnl2DPol() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PSnl2DPol*
Analysis::Models::PSnl2DPol::clone( const char* name ) const 
{ return new Analysis::Models::PSnl2DPol ( *this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PSnl2DPol::evaluate() const 
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
    const double phiv   = r->getVal ( nset ) ;
    //
    m_positive.setPar ( k  , phiv ) ;
    //
    ++k ;
  }
  //
  return m_positive ( m_x , m_y ) * m_ps1 ( m_x ) * m_ps2 ( m_y ) ;
}
// ============================================================================






// ============================================================================
// generic polinomial
// ============================================================================
Analysis::Models::PS2s2DPol::PS2s2DPol
( const char*          name      , 
  const char*          title     ,
  RooRealVar&          x         ,
  RooRealVar&          y         ,
  const Gaudi::Math::PhaseSpace2& ps        ,
  const unsigned short n         ,
  RooArgList&          phis      ) 
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable-X" , this , x ) 
  , m_y        ( "y"       , "Observable-Y" , this , y ) 
  , m_phis     ( "phi"     , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( n , x.getMin() , x.getMax() ) 
  , m_ps       ( ps ) 
{
  //
  if (  x.getMin() != y.getMin() || x.getMax() != y.getMax() )
  { throw GaudiException ( "Non-equal x/y-edges "             , 
                           "Analysis::PS2s2DPol"              , 
                           StatusCode::FAILURE                ) ; }
  
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  unsigned num = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_positive.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_positive.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::PS2s2DPol"              , 
                           StatusCode::FAILURE                ) ; }
  
  //
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::PS2s2DPol::PS2s2DPol
( const Analysis::Models::PS2s2DPol& right ,      
  const char*                                 name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_y        ( "y"      , this , right.m_y     ) 
  , m_phis     ( "phis"   , this , right.m_phis  ) 
//
  , m_iterator ( 0 ) 
//
  , m_positive ( right.m_positive ) 
  , m_ps       ( right.m_ps       ) 
{
  m_iterator = m_phis.createIterator () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PS2s2DPol::~PS2s2DPol() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PS2s2DPol*
Analysis::Models::PS2s2DPol::clone( const char* name ) const 
{ return new Analysis::Models::PS2s2DPol ( *this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PS2s2DPol::evaluate() const 
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
  return m_positive ( m_x , m_y ) * m_ps ( m_x ) * m_ps ( m_y ) ;
}
// ============================================================================



// ============================================================================
// generic polinomial
// ============================================================================
Analysis::Models::PSnls2DPol::PSnls2DPol
( const char*          name      , 
  const char*          title     ,
  RooRealVar&          x         ,
  RooRealVar&          y         ,
  const Gaudi::Math::PhaseSpaceNL& ps ,
  const unsigned short n         ,
  RooArgList&          phis      ) 
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable-X" , this , x ) 
  , m_y        ( "y"       , "Observable-Y" , this , y ) 
  , m_phis     ( "phi"     , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( n , x.getMin() , x.getMax() ) 
  , m_ps       ( ps ) 
{
  //
  if (  x.getMin() != y.getMin() || x.getMax() != y.getMax() )
  { throw GaudiException ( "Non-equal x/y-edges "             , 
                           "Analysis::PSnls2DPol"             , 
                           StatusCode::FAILURE                ) ; }
  
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  unsigned num = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_positive.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_positive.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::PSnls2DPol"             , 
                           StatusCode::FAILURE                ) ; }
  
  //
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::PSnls2DPol::PSnls2DPol
( const Analysis::Models::PSnls2DPol& right ,      
  const char*                                 name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_y        ( "y"      , this , right.m_y     ) 
  , m_phis     ( "phis"   , this , right.m_phis  ) 
//
  , m_iterator ( 0 ) 
//
  , m_positive ( right.m_positive ) 
  , m_ps       ( right.m_ps       ) 
{
  m_iterator = m_phis.createIterator () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PSnls2DPol::~PSnls2DPol() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PSnls2DPol*
Analysis::Models::PSnls2DPol::clone( const char* name ) const 
{ return new Analysis::Models::PSnls2DPol ( *this , name ) ; }
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PSnls2DPol::evaluate() const 
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
  return m_positive ( m_x , m_y ) * m_ps ( m_x ) * m_ps ( m_y ) ;
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
