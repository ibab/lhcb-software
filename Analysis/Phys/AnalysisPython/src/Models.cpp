// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "Analysis/Models.h"
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
Analysis::Models::Rho0::~Rho0(){}
// ============================================================================
                                    

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
  : RooAbsPdf ( right     ) 
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
// The END 
// ============================================================================
