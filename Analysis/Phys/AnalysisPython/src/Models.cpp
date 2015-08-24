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
  setPars () ;
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
  const Gaudi::Math::FormFactors::JacksonRho rho ) 
  : RooAbsPdf  ( name , title ) 
//
  , m_x     ( "x"  , "Observable" , this , x     ) 
  , m_mass  ( "m0" , "Peak"       , this , mass  ) 
  , m_width ( "g0" , "Width"      , this , width )
//
  , m_bw    ( 0 , 1 , m1 , m2 , L , rho ) 
{
  setPars() ;
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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::BreitWigner::setPars () const 
{
  //
  m_bw.setM0     ( m_mass  ) ;
  m_bw.setGamma0 ( m_width ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::BreitWigner::evaluate() const 
{
  //
  setPars() ;
  //
  return m_bw ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::BreitWigner::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::BreitWigner::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars() ;
  
  //
  return m_bw.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  : Analysis::Models::BreitWigner 
    ( name    , 
      title   , 
      x       , 
      mass    ,
      width   ,
      pi_mass , 
      pi_mass , 
      1       , 
      Gaudi::Math::FormFactors::Jackson_A5 )
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
  : Analysis::Models::BreitWigner 
    ( name    , 
      title   , 
      x       , 
      mass    ,
      width   ,
      k_mass  , 
      pi_mass , 
      1       , 
      Gaudi::Math::FormFactors::Jackson_A2 )
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
Analysis::Models::Phi::Phi 
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          mass      ,
  RooAbsReal&          width     ,
  const double         k_mass    )
  : Analysis::Models::BreitWigner
    ( name    , 
      title   , 
      x       , 
      mass    ,
      width   ,
      k_mass  , 
      k_mass  , 
      1       , 
      Gaudi::Math::FormFactors::Jackson_A2 )
{}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Phi::Phi 
( const Analysis::Models::Phi& right , 
  const char*                    name  ) 
  : Analysis::Models::BreitWigner ( right , name ) 
{}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::Phi::~Phi(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Phi*
Analysis::Models::Phi::clone( const char* name ) const 
{ return new Analysis::Models::Phi(*this,name) ; }


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
  setPars ();
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
  const Gaudi::Math::FormFactors::JacksonRho rho , 
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
  setPars ();
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
  setPars () ;
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
{
  setPars () ;
}
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
  setPars () ;
  return m_bw.amplitude ( m_x ) ;
}
// ============================================================================
void Analysis::Models::BW23L::setPars () const 
{
  //
  m_bw.setM0     ( m_mass  ) ;
  m_bw.setGamma0 ( m_width ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::BW23L::evaluate() const 
{
  //
  setPars () ;
  //
  return m_bw ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::BW23L::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::BW23L::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_bw.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================

  
// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::Flatte::Flatte 
( const char*                name      , 
  const char*                title     ,
  RooAbsReal&                x         ,
  RooAbsReal&                m0        ,
  RooAbsReal&                m0g1      ,
  RooAbsReal&                g2og1     ,
  const Gaudi::Math::Flatte& flatte    ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x      ( "x"     , "Observable" , this , x     ) 
  , m_m0     ( "m0"    , "Peak"       , this , m0    ) 
  , m_m0g1   ( "m0g1"  , "M0*G1"      , this , m0g1  )
  , m_g2og1  ( "g2og1" , "G2/G1"      , this , g2og1 )
    //
  , m_flatte ( flatte ) 
{
  setPars() ;
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
{
  setPars() ;
}
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
void Analysis::Models::Flatte::setPars () const 
{
  //
  m_flatte.setM0     ( m_m0    ) ;
  m_flatte.setM0G1   ( m_m0g1  ) ;
  m_flatte.setG2oG1  ( m_g2og1 ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Flatte::evaluate() const 
{
  //
  setPars () ;
  //
  return m_flatte ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::Flatte::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Flatte::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_flatte.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ===========================================================================
// get the amplitude 
// ===========================================================================
std::complex<double> Analysis::Models::Flatte::amplitude () const  
{
  //
  setPars () ;
  //
  return m_flatte.amplitude ( m_x ) ;
}


// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::Flatte2::Flatte2 
( const char*                name      , 
  const char*                title     ,
  RooAbsReal&                x         ,
  RooAbsReal&                m0        ,
  RooAbsReal&                m0g1      ,
  RooAbsReal&                g2og1     ,
  const Gaudi::Math::Flatte& flatte    ) 
  : RooAbsPdf ( name , title ) 
//
  , m_x       ( "x"     , "Observable" , this , x     ) 
  , m_m0      ( "m0"    , "Peak"       , this , m0    ) 
  , m_m0g1    ( "m0g1"  , "M0*G1"      , this , m0g1  )
  , m_g2og1   ( "g2og1" , "G2/G1"      , this , g2og1 )
    //
  , m_flatte2 ( flatte ) 
{
  setPars() ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Flatte2::Flatte2 
( const Analysis::Models::Flatte2& right , 
  const char*                      name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x       ( "x"     , this , right.m_x     ) 
  , m_m0      ( "m0"    , this , right.m_m0    ) 
  , m_m0g1    ( "m0g1"  , this , right.m_m0g1  )
  , m_g2og1   ( "g2og1" , this , right.m_g2og1 )
//
  , m_flatte2 ( right.m_flatte2 ) 
{
  setPars() ;
}
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
void Analysis::Models::Flatte2::setPars () const 
{
  //
  m_flatte2.setM0     ( m_m0    ) ;
  m_flatte2.setM0G1   ( m_m0g1  ) ;
  m_flatte2.setG2oG1  ( m_g2og1 ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Flatte2::evaluate() const 
{
  //
  setPars () ;
  //
  return m_flatte2 ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::Flatte2::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Flatte2::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_flatte2.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ===========================================================================
// get the amplitude 
// ===========================================================================
std::complex<double> Analysis::Models::Flatte2::amplitude () const  
{
  //
  setPars () ;
  //
  return m_flatte2.amplitude ( m_x ) ;
}





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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::LASS::setPars () const 
{
  //
  m_lass.setM0 ( m_m0 ) ;
  m_lass.setG0 ( m_g0 ) ;
  m_lass.setA  ( m_a  ) ;
  m_lass.setR  ( m_r  ) ;
  m_lass.setE  ( m_e  ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::LASS::evaluate() const 
{
  //
  setPars () ;
  //
  return m_lass ( m_x  ) ;
}
// ============================================================================
Int_t Analysis::Models::LASS::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::LASS::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars() ;
  //
  return m_lass.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ===========================================================================
// get the complex amplitude 
// ===========================================================================
std::complex<double> Analysis::Models::LASS::amplitude() const 
{
  //
  setPars() ;
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
  setPars () ;
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
{
  setPars   () ;
}
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
void Analysis::Models::LASS23L::setPars () const 
{
  //
  m_lass.setM0 ( m_m0 ) ;
  m_lass.setG0 ( m_g0 ) ;
  m_lass.setA  ( m_a  ) ;
  m_lass.setR  ( m_r  ) ;
  m_lass.setE  ( m_e  ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::LASS23L::evaluate() const 
{
  //
  setPars () ;
  //
  return m_lass ( m_x  ) ;
}
// ============================================================================
Int_t Analysis::Models::LASS23L::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::LASS23L::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_lass.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ===========================================================================
// get the complex amplitude 
// ===========================================================================
std::complex<double> Analysis::Models::LASS23L::amplitude() const 
{
  //
  setPars () ;
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
  setPars () ;
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
  setPars () ;
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
void Analysis::Models::Bugg::setPars () const 
{
  //
  m_bugg.setM  ( m_M  ) ;
  m_bugg.setG2 ( m_g2 ) ;
  m_bugg.setB1 ( m_b1 ) ;
  m_bugg.setB2 ( m_b2 ) ;
  m_bugg.setA  ( m_a  ) ;
  m_bugg.setS1 ( m_s1 ) ;
  m_bugg.setS2 ( m_s2 ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Bugg::evaluate() const 
{
  //
  setPars () ;
  //
  return m_bugg ( m_x  ) ;
}
// ============================================================================
Int_t Analysis::Models::Bugg::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Bugg::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars() ;
  //
  return m_bugg.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ===========================================================================
// get the complex amplitude 
// ===========================================================================
std::complex<double> Analysis::Models::Bugg::amplitude() const 
{
  //
  setPars() ;
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
  setPars() ;
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
  setPars() ;
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
void Analysis::Models::Bugg23L::setPars () const 
{
  //
  m_bugg.setM  ( m_M  ) ;
  m_bugg.setG2 ( m_g2 ) ;
  m_bugg.setB1 ( m_b1 ) ;
  m_bugg.setB2 ( m_b2 ) ;
  m_bugg.setA  ( m_a  ) ;
  m_bugg.setS1 ( m_s1 ) ;
  m_bugg.setS2 ( m_s2 ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Bugg23L::evaluate() const 
{
  //
  setPars () ;
  //
  return m_bugg ( m_x  ) ;
}
// ============================================================================
Int_t Analysis::Models::Bugg23L::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Bugg23L::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_bugg.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ===========================================================================
// get the complex amplitude 
// ===========================================================================
std::complex<double> 
Analysis::Models::Bugg23L::amplitude() const 
{
  //
  setPars() ;
  //
  return m_bugg.amplitude ( m_x  ) ;
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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::Voigt::setPars () const 
{
  //
  m_voigt.setM0     ( m_m0    ) ;
  m_voigt.setSigma  ( m_sigma  ) ;
  m_voigt.setGamma  ( m_gamma  ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Voigt::evaluate() const 
{
  //
  setPars() ;
  //
  return m_voigt    ( m_x     ) ;
}
// ============================================================================
Int_t Analysis::Models::Voigt::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Voigt::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars() ;
  //
  return m_voigt.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars () ;
}
// ============================================================================
Analysis::Models::CrystalBall::~CrystalBall(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::CrystalBall*
Analysis::Models::CrystalBall::clone ( const char* name ) const 
{ return new Analysis::Models::CrystalBall (*this , name ) ; }
// ============================================================================
void Analysis::Models::CrystalBall::setPars () const 
{
  //
  m_cb.setM0      ( m_m0     ) ;
  m_cb.setSigma   ( m_sigma  ) ;
  m_cb.setAlpha   ( m_alpha  ) ;
  m_cb.setN       ( m_n      ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::CrystalBall::evaluate() const
{
  //
  setPars () ;
  //
  return m_cb ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::CrystalBall::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::CrystalBall::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars ();
  //
  return m_cb.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================





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
  setPars () ;
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
{
  setPars () ;
}
// ============================================================================
Analysis::Models::CrystalBallRS::~CrystalBallRS(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::CrystalBallRS*
Analysis::Models::CrystalBallRS::clone ( const char* name ) const 
{ return new Analysis::Models::CrystalBallRS (*this , name ) ; }
// ============================================================================
void Analysis::Models::CrystalBallRS::setPars () const 
{
  //
  m_cb.setM0      ( m_m0     ) ;
  m_cb.setSigma   ( m_sigma  ) ;
  m_cb.setAlpha   ( m_alpha  ) ;
  m_cb.setN       ( m_n      ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::CrystalBallRS::evaluate() const
{
  //
  setPars() ;
  //
  return m_cb ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::CrystalBallRS::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::CrystalBallRS::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_cb.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::CrystalBallDS::setPars () const 
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
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::CrystalBallDS::evaluate() const 
{
  //
  setPars () ;
  //
  return m_cb2     ( m_x      ) ;
}
// ============================================================================
Int_t Analysis::Models::CrystalBallDS::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::CrystalBallDS::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars() ;
  //
  return m_cb2.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  , m_needham ( 100 , 1 , 1.9 , 0 , 0 ) 
{
  //
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::Needham::setPars () const 
{
  //
  m_needham . setM0    ( m_m0     ) ;
  m_needham . setSigma ( m_sigma  ) ;
  //
  m_needham . setA0    ( m_a0     ) ;
  m_needham . setA1    ( m_a1     ) ;
  m_needham . setA2    ( m_a2     ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Needham::evaluate() const 
{
  //
  setPars () ;
  //
  return m_needham     ( m_x      ) ;
}
// ============================================================================
Int_t Analysis::Models::Needham::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Needham::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_needham.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::Apolonios::setPars () const 
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
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Apolonios::evaluate() const 
{
  //
  setPars () ;
  //
  return m_apo ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::Apolonios::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Apolonios::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_apo.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================



// ============================================================================
// Apolonios2
// ============================================================================
Analysis::Models::Apolonios2::Apolonios2
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          m0        ,
  RooAbsReal&          sigmaL    ,    
  RooAbsReal&          sigmaR    ,    
  RooAbsReal&          beta      ) 
  : RooAbsPdf ( name , title )
//
  , m_x       ( "x"       , "Observable"                 , this , x      ) 
  , m_m0      ( "m0"      , "mass"                       , this , m0     ) 
  , m_sigmaL  ( "sigmaL"  , "sigmaL"                     , this , sigmaL )
  , m_sigmaR  ( "sigmaR"  , "sigmaR"                     , this , sigmaR )
  , m_beta    ( "beta"    , "beta-parameter"             , this , beta   )
//
  , m_apo2    ( 1 , 1 , 1 , 1 ) 
{
  //
  setPars () ;
  //
}
// ============================================================================
// copy constructor 
// ============================================================================
Analysis::Models::Apolonios2::Apolonios2
( const Analysis::Models::Apolonios2& right , 
  const char*                        name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x       ( "x"       , this , right.m_x      ) 
  , m_m0      ( "m0"      , this , right.m_m0     ) 
  , m_sigmaL  ( "sigmaL"  , this , right.m_sigmaL )
  , m_sigmaR  ( "sigmaR"  , this , right.m_sigmaR )
  , m_beta    ( "beta"    , this , right.m_beta   )
//
  , m_apo2     ( right.m_apo2 ) 
{
  setPars () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::Apolonios2::~Apolonios2 (){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Apolonios2*
Analysis::Models::Apolonios2::clone ( const char* name ) const 
{ return new Analysis::Models::Apolonios2 ( *this , name ) ; }
// ============================================================================
void Analysis::Models::Apolonios2::setPars () const 
{
  //
  m_apo2.setM0      ( m_m0     ) ;
  m_apo2.setSigmaL  ( m_sigmaL ) ;
  m_apo2.setSigmaR  ( m_sigmaR ) ;
  m_apo2.setBeta    ( m_beta   ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Apolonios2::evaluate() const 
{
  //
  setPars () ;
  //
  return m_apo2 ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::Apolonios2::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Apolonios2::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_apo2.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars() ;
}
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
void Analysis::Models::BifurcatedGauss::setPars () const 
{
  //
  m_bg . setPeak   ( m_peak   ) ;
  m_bg . setSigmaL ( m_sigmaL ) ;
  m_bg . setSigmaR ( m_sigmaR ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::BifurcatedGauss::evaluate() const 
{
  //
  setPars () ;
  //
  return m_bg    ( m_x      ) ;
}
// ============================================================================
Int_t Analysis::Models::BifurcatedGauss::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::BifurcatedGauss::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_bg.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::GenGaussV1::setPars () const 
{
  //
  m_ggv1.setMu     ( m_mu    ) ;
  m_ggv1.setAlpha  ( m_alpha ) ;
  m_ggv1.setBeta   ( m_beta  ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::GenGaussV1::evaluate() const 
{
  //
  setPars () ;
  //
  return m_ggv1    ( m_x      ) ;
}
// ============================================================================
Int_t Analysis::Models::GenGaussV1::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::GenGaussV1::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars() ;
  //
  return m_ggv1.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::GenGaussV2::setPars () const 
{
  //
  m_ggv2.setXi     ( m_xi    ) ;
  m_ggv2.setAlpha  ( m_alpha ) ;
  m_ggv2.setKappa  ( m_kappa ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::GenGaussV2::evaluate() const 
{
  //
  setPars () ;
  //
  return m_ggv2    ( m_x      ) ;
}
// ============================================================================
Int_t Analysis::Models::GenGaussV2::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::GenGaussV2::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_ggv2.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::SkewGauss::setPars () const 
{
  //
  m_sg . setXi     ( m_xi    ) ;
  m_sg . setOmega  ( m_omega ) ;
  m_sg . setAlpha  ( m_alpha ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::SkewGauss::evaluate() const 
{
  //
  setPars () ;
  //
  return m_sg      ( m_x      ) ;
}
// ============================================================================
Int_t Analysis::Models::SkewGauss::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::SkewGauss::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_sg.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars() ;
}
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
void Analysis::Models::Bukin::setPars () const 
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
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Bukin::evaluate() const 
{
  //
  setPars () ;
  //
  return m_bukin    ( m_x     ) ;
}
// ============================================================================
Int_t Analysis::Models::Bukin::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Bukin::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars() ;
  //
  return m_bukin.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::StudentT::setPars () const 
{
  //
  m_stt.setM     ( m_mu    ) ;
  m_stt.setSigma ( m_sigma ) ;
  m_stt.setN     ( m_n     ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::StudentT::evaluate() const 
{
  //
  setPars () ;
  //
  return m_stt   ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::StudentT::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::StudentT::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_stt.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================


// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::BifurcatedStudentT::BifurcatedStudentT 
( const char*          name   , 
  const char*          title  ,
  RooAbsReal&          x      ,
  RooAbsReal&          mu     ,
  RooAbsReal&          sigmaL ,
  RooAbsReal&          sigmaR ,
  RooAbsReal&          nL     ,
  RooAbsReal&          nR     )
  : RooAbsPdf  (name ,title ) 
//
  , m_x      ( "x"     , "Observable" , this , x      ) 
  , m_mu     ( "mu"    , "Peak"       , this , mu     ) 
  , m_sigmaL ( "sigmaL" , "Width(L)"  , this , sigmaL )
  , m_sigmaR ( "sigmaR" , "Width(R)"  , this , sigmaR )
  , m_nL     ( "nL"     , "N(L)"      , this , nL     )
  , m_nR     ( "nR"     , "N(R)"      , this , nR     )
//
  , m_stt   ( 0 , 1 , 1 , 2 , 2 ) 
{
  setPars () ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::BifurcatedStudentT::BifurcatedStudentT 
( const Analysis::Models::BifurcatedStudentT& right , 
  const char*                       name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x      ( "x"      , this , right.m_x      ) 
  , m_mu     ( "mu"     , this , right.m_mu     ) 
  , m_sigmaL ( "sigmaL" , this , right.m_sigmaL )
  , m_sigmaR ( "sigmaR" , this , right.m_sigmaR )
  , m_nL     ( "nL"     , this , right.m_nL     )
  , m_nR     ( "nR"     , this , right.m_nR     )
    //
  , m_stt   (               right.m_stt    ) 
{
  setPars () ;
}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::BifurcatedStudentT::~BifurcatedStudentT (){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::BifurcatedStudentT*
Analysis::Models::BifurcatedStudentT::clone( const char* name ) const 
{ return new Analysis::Models::BifurcatedStudentT(*this,name) ; }
// ============================================================================
void Analysis::Models::BifurcatedStudentT::setPars () const 
{
  //
  m_stt.setM      ( m_mu     ) ;
  m_stt.setSigmaL ( m_sigmaL ) ;
  m_stt.setSigmaR ( m_sigmaR ) ;
  m_stt.setNL     ( m_nL     ) ;
  m_stt.setNR     ( m_nR     ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::BifurcatedStudentT::evaluate() const 
{
  //
  setPars () ;
  //
  return m_stt   ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::BifurcatedStudentT::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::BifurcatedStudentT::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_stt.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars() ;
}
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
void Analysis::Models::GramCharlierA::setPars () const 
{
  //
  m_gca.setM0     ( m_m0     ) ;
  m_gca.setSigma  ( m_sigma  ) ;
  m_gca.setKappa3 ( m_kappa3 ) ;
  m_gca.setKappa4 ( m_kappa4 ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::GramCharlierA::evaluate() const 
{
  //
  setPars() ;
  //
  return m_gca ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::GramCharlierA::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::GramCharlierA::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars() ;
  //
  return m_gca.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
Int_t Analysis::Models::PhaseSpace2::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::PhaseSpace2::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  return m_ps2.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;  
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
{
  setPars () ;  
}
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
void Analysis::Models::PhaseSpaceLeft::setPars () const 
{
  //
  m_left.setThreshold ( m_threshold ) ;  
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PhaseSpaceLeft::evaluate() const 
{
  //
  setPars () ;
  //
  return m_left ( m_x ) ; 
}
// ============================================================================
Int_t Analysis::Models::PhaseSpaceLeft::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::PhaseSpaceLeft::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_left.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::PhaseSpaceRight::setPars () const 
{
  //
  m_right.setThreshold ( m_threshold ) ;  
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PhaseSpaceRight::evaluate() const 
{
  //
  setPars () ;
  //
  return m_right ( m_x ) ; 
}
// ============================================================================
Int_t Analysis::Models::PhaseSpaceRight::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::PhaseSpaceRight::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_right.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::PhaseSpaceNL::setPars () const 
{
  //
  m_ps.setThresholds ( m_low , m_high ) ;  
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PhaseSpaceNL::evaluate() const 
{
  //
  setPars () ;
  //
  return m_ps ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::PhaseSpaceNL::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::PhaseSpaceNL::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_ps.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
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
Int_t Analysis::Models::PhaseSpace23L::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::PhaseSpace23L::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  return m_ps23L.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  , m_ps       ( low , high , L , N , 1 ) 
{
  m_phis.add ( phi1 ) ;
  m_iterator = m_phis.createIterator() ;
  //
  const RooRealVar* v = dynamic_cast<RooRealVar*>(&x) ;
  if ( 0 != v ) 
  {
    const double xmin = std::max ( std::min ( low , high ) , v -> getMin () ) ;
    const double xmax = std::min ( std::max ( low , high ) , v -> getMax () ) ;
    m_ps = Gaudi::Math::PhaseSpacePol ( m_ps.phasespace() , 1 , xmin , xmax ) ;  
  }
  //
  setPars() ;
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
  , m_ps       (     low , high , L , N , 2 ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_iterator = m_phis.createIterator() ;
  //
  const RooRealVar* v = dynamic_cast<RooRealVar*>(&x) ;
  if ( 0 != v ) 
  {
    const double xmin = std::max ( std::min ( low , high ) , v -> getMin () ) ;
    const double xmax = std::min ( std::max ( low , high ) , v -> getMax () ) ;
    m_ps = Gaudi::Math::PhaseSpacePol ( m_ps.phasespace() , 2 , xmin , xmax ) ;  
  }
  //
  setPars() ;
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
  , m_ps       (     low , high , L , N , 3 ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_phis.add ( phi3 ) ;
  m_iterator = m_phis.createIterator() ;
  //
  const RooRealVar* v = dynamic_cast<RooRealVar*>(&x) ;
  if ( 0 != v ) 
  {
    const double xmin = std::max ( std::min ( low , high ) , v -> getMin () ) ;
    const double xmax = std::min ( std::max ( low , high ) , v -> getMax () ) ;
    m_ps = Gaudi::Math::PhaseSpacePol ( m_ps.phasespace() , 3 , xmin , xmax ) ;  
  }
  //
  setPars() ;
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
  , m_ps       (                  low , high , L , N , phis.getSize() ) 
{
  //
  TIterator*   tmp  = phis.createIterator() ;
  RooAbsArg*   coef = 0 ;
  unsigned     num  = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_ps.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
  }
  delete tmp ;
  //
  const RooRealVar* v = dynamic_cast<RooRealVar*>(&x) ;
  if ( 0 != v ) 
  {
    const double xmin = std::max ( std::min ( low , high ) , v -> getMin () ) ;
    const double xmax = std::min ( std::max ( low , high ) , v -> getMax () ) ;
    m_ps = Gaudi::Math::PhaseSpacePol ( m_ps.phasespace() , phis.getSize() , xmin , xmax ) ;  
  }
  //
  m_iterator = m_phis.createIterator() ;
  setPars() ;
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
  , m_ps       ( ps , 1 ) 
{
  m_phis.add ( phi1 ) ;
  m_iterator = m_phis.createIterator() ;
  //
  const RooRealVar* v = dynamic_cast<RooRealVar*>(&x) ;
  if ( 0 != v ) 
  {
    const double xmin = std::max ( ps .  lowEdge () , v -> getMin () ) ;
    const double xmax = std::min ( ps . highEdge () , v -> getMax () ) ;
    m_ps = Gaudi::Math::PhaseSpacePol ( m_ps.phasespace() , 1 , xmin , xmax ) ;  
  }
  //
  setPars() ;
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
  , m_ps       ( ps , 2 ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_iterator = m_phis.createIterator() ;
  //
  const RooRealVar* v = dynamic_cast<RooRealVar*>(&x) ;
  if ( 0 != v ) 
  {
    const double xmin = std::max ( ps .  lowEdge () , v -> getMin () ) ;
    const double xmax = std::min ( ps . highEdge () , v -> getMax () ) ;
    m_ps = Gaudi::Math::PhaseSpacePol ( m_ps.phasespace() , 2 , xmin , xmax ) ;  
  }
  //
  setPars() ;
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
  , m_ps       ( ps , 3 ) 
{
  m_phis.add ( phi1 ) ;
  m_phis.add ( phi2 ) ;
  m_phis.add ( phi3 ) ;
  m_iterator = m_phis.createIterator() ;
  //
  const RooRealVar* v = dynamic_cast<RooRealVar*>(&x) ;
  if ( 0 != v ) 
  {
    const double xmin = std::max ( ps .  lowEdge () , v -> getMin () ) ;
    const double xmax = std::min ( ps . highEdge () , v -> getMax () ) ;
    m_ps = Gaudi::Math::PhaseSpacePol ( m_ps.phasespace() , 3 , xmin , xmax ) ;  
  }
  //
  setPars() ;
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
  , m_ps       ( ps , phis.getSize() ) 
{
  //
  TIterator*   tmp  = phis.createIterator() ;
  RooAbsArg*   coef = 0 ;
  unsigned     num  = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_ps.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
  }
  delete tmp ;
  //
  const RooRealVar* v = dynamic_cast<RooRealVar*>(&x) ;
  if ( 0 != v ) 
  {
    const double xmin = std::max ( ps .  lowEdge () , v -> getMin () ) ;
    const double xmax = std::min ( ps . highEdge () , v -> getMax () ) ;
    m_ps = Gaudi::Math::PhaseSpacePol ( m_ps.phasespace() , phis.getSize()  , xmin , xmax ) ;  
  }
  //
  m_iterator = m_phis.createIterator() ;
  //
  setPars() ;
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
{
  m_iterator = m_phis.createIterator () ;
  setPars() ;
}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PhaseSpacePol* 
Analysis::Models::PhaseSpacePol::clone ( const char* name ) const 
{ return new Analysis::Models::PhaseSpacePol ( *this , name ) ; }
// ============================================================================
void Analysis::Models::PhaseSpacePol::setPars () const 
{
  //
  m_iterator->Reset () ;
  //
  RooAbsArg*       phi   = 0 ;
  const RooArgSet* nset  = m_phis.nset() ;
  //
  unsigned short k = 0 ;
  while ( ( phi = (RooAbsArg*) m_iterator->Next() ) )
  {
    const RooAbsReal* r = dynamic_cast<RooAbsReal*> ( phi ) ;
    if ( 0 == r ) { continue ; }
    //
    const double phi   = r->getVal ( nset ) ;
    //
    m_ps.setPar ( k  , phi ) ;
    //
    ++k ;
  }
  //
}
// ============================================================================
// evaluate the function
// ============================================================================
Double_t Analysis::Models::PhaseSpacePol::evaluate () const 
{
  //
  setPars () ;
  //
  return m_ps ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::PhaseSpacePol::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::PhaseSpacePol::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_ps.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================



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
  setPars () ;
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
  setPars () ;
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
void Analysis::Models::PolyPositive::setPars () const 
{
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
}
//
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PolyPositive::evaluate() const 
{
  //
  setPars () ;
  //
  return m_positive ( m_x ) ; 
}
// ============================================================================
Int_t Analysis::Models::PolyPositive::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::PolyPositive::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_positive.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================


// ============================================================================
// monothonic polinomial
// ============================================================================
Analysis::Models::PolyMonothonic::PolyMonothonic
( const char*          name       , 
  const char*          title      ,
  RooAbsReal&          x          ,
  const RooArgList&    phis       , 
  const double         xmin       , 
  const double         xmax       , 
  const bool           increasing ) 
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable"   , this , x ) 
  , m_phis     ( "phi"     , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
//
  , m_monothonic ( phis.getSize() , xmin , xmax , increasing ) 
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
  setPars () ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::PolyMonothonic::PolyMonothonic
( const Analysis::Models::PolyMonothonic&  right ,      
  const char*                              name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x          ( "x"      , this , right.m_x     ) 
  , m_phis       ( "phis"   , this , right.m_phis  ) 
    //
  , m_iterator   ( 0 ) 
    //
  , m_monothonic ( right.m_monothonic ) 
{
  m_iterator = m_phis.createIterator () ;
  setPars () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PolyMonothonic::~PolyMonothonic() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PolyMonothonic*
Analysis::Models::PolyMonothonic::clone( const char* name ) const 
{ return new Analysis::Models::PolyMonothonic(*this,name) ; }
// ============================================================================
void Analysis::Models::PolyMonothonic::setPars () const 
{
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
    m_monothonic.setPar ( k  , phi ) ;
    //
    ++k ;
  }
}
//
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PolyMonothonic::evaluate() const 
{
  //
  setPars () ;
  //
  return m_monothonic ( m_x ) ; 
}
// ============================================================================
Int_t Analysis::Models::PolyMonothonic::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::PolyMonothonic::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_monothonic.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================




// ============================================================================
// convex polinomial
// ============================================================================
Analysis::Models::PolyConvex::PolyConvex
( const char*          name       , 
  const char*          title      ,
  RooAbsReal&          x          ,
  const RooArgList&    phis       , 
  const double         xmin       , 
  const double         xmax       , 
  const bool           increasing ,
  const bool           convex     ) 
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable"   , this , x ) 
  , m_phis     ( "phi"     , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
//
  , m_convex ( phis.getSize() , xmin , xmax , increasing , convex ) 
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
  setPars () ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::PolyConvex::PolyConvex
( const Analysis::Models::PolyConvex&  right ,      
  const char*                          name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x          ( "x"      , this , right.m_x     ) 
  , m_phis       ( "phis"   , this , right.m_phis  ) 
    //
  , m_iterator   ( 0 ) 
    //
  , m_convex ( right.m_convex ) 
{
  m_iterator = m_phis.createIterator () ;
  setPars () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PolyConvex::~PolyConvex () { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PolyConvex*
Analysis::Models::PolyConvex::clone( const char* name ) const 
{ return new Analysis::Models::PolyConvex(*this,name) ; }
// ============================================================================
void Analysis::Models::PolyConvex::setPars () const 
{
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
    m_convex.setPar ( k  , phi ) ;
    //
    ++k ;
  }
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PolyConvex::evaluate() const 
{
  //
  setPars () ;
  //
  return m_convex ( m_x ) ; 
}
// ============================================================================
Int_t Analysis::Models::PolyConvex::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::PolyConvex::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_convex.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================


// ============================================================================
// convex polinomial
// ============================================================================
Analysis::Models::PolySigmoid::PolySigmoid
( const char*          name       , 
  const char*          title      ,
  RooAbsReal&          x          ,
  const RooArgList&    phis       , 
  const double         xmin       , 
  const double         xmax       , 
  RooAbsReal&          alpha      ,
  RooAbsReal&          x0         )
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable"   , this , x      ) 
  , m_phis     ( "phi"     , "Coefficients" , this          )
  , m_alpha    ( "alpha"   , "Alpha"        , this , alpha  )
  , m_x0       ( "x0"      , "X0"           , this , x0     )
    //
  , m_iterator ( 0 ) 
    //
  , m_sigmoid  ( phis.getSize() , xmin , xmax , m_alpha , m_x0 ) 
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
  setPars () ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::PolySigmoid::PolySigmoid
( const Analysis::Models::PolySigmoid&  right ,      
  const char*                           name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x          ( "x"      , this , right.m_x     ) 
  , m_phis       ( "phis"   , this , right.m_phis  ) 
  , m_alpha      ( "alpha"  , this , right.m_alpha ) 
  , m_x0         ( "x0"     , this , right.m_x0    ) 
    //
  , m_iterator   ( 0 ) 
    //
  , m_sigmoid ( right.m_sigmoid ) 
{
  m_iterator = m_phis.createIterator () ;
  setPars () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PolySigmoid::~PolySigmoid(){ delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PolySigmoid*
Analysis::Models::PolySigmoid::clone( const char* name ) const 
{ return new Analysis::Models::PolySigmoid(*this,name) ; }
// ============================================================================
void Analysis::Models::PolySigmoid::setPars () const 
{
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
    m_sigmoid.setPar ( k  , phi ) ;
    //
    ++k ;
  }
  //
  m_sigmoid.setAlpha ( m_alpha ) ;
  m_sigmoid.setX0    ( m_x0    ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PolySigmoid::evaluate() const 
{
  //
  setPars () ;
  //
  return m_sigmoid ( m_x ) ; 
}
// ============================================================================
Int_t Analysis::Models::PolySigmoid::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::PolySigmoid::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_sigmoid.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================


// ============================================================================
// positive spline 
// ============================================================================
/*  constructor with the spline 
 *  @param name  the name 
 *  @param title the  title
 *  @param x     the  variable 
 *  @param spine the spline  
 *  @param phis  vector of parameters 
 */
// ============================================================================
Analysis::Models::PositiveSpline::PositiveSpline 
( const char*                        name, 
  const char*                        title     ,
  RooAbsReal&                        x         ,
  const Gaudi::Math::PositiveSpline& spline    ,   // the spline 
  RooArgList&                        phis      )   // parameters
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable"   , this , x ) 
  , m_phis     ( "phi"     , "Coefficients" , this     )
    //
  , m_iterator ( 0 ) 
    //
  , m_spline   ( spline ) 
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
  setPars () ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::PositiveSpline::PositiveSpline
( const Analysis::Models::PositiveSpline&  right ,      
  const char*                            name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_phis     ( "phis"   , this , right.m_phis  ) 
    //
  , m_iterator ( 0 ) 
    //
  , m_spline ( right.m_spline ) 
{
  m_iterator = m_phis.createIterator () ;
  setPars () ;
}

// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PositiveSpline::~PositiveSpline() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PositiveSpline*
Analysis::Models::PositiveSpline::clone( const char* name ) const 
{ return new Analysis::Models::PositiveSpline(*this,name) ; }
// ============================================================================
void Analysis::Models::PositiveSpline::setPars () const 
{
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
    m_spline.setPar ( k  , phi ) ;
    //
    ++k ;
  }
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PositiveSpline::evaluate() const 
{
  //
  setPars () ;
  //
  return m_spline ( m_x ) ; 
}
// ============================================================================
Int_t Analysis::Models::PositiveSpline::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::PositiveSpline::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_spline.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================





// ============================================================================
// monothonic spline 
// ============================================================================
/** constructor with the spline 
 *  @param name  the name 
 *  @param title the  title
 *  @param x     the  variable 
 *  @param spine the spline  
 *  @param phis  vector of parameters 
 */
// ============================================================================
Analysis::Models::MonothonicSpline::MonothonicSpline 
( const char*                          name, 
  const char*                          title     ,
  RooAbsReal&                          x         ,
  const Gaudi::Math::MonothonicSpline& spline    ,   // the spline 
  RooArgList&                          phis      )   // parameters
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable"   , this , x ) 
  , m_phis     ( "phi"     , "Coefficients" , this     )
    //
  , m_iterator ( 0 ) 
    //
  , m_spline   ( spline ) 
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
  setPars () ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::MonothonicSpline::MonothonicSpline 
( const Analysis::Models::MonothonicSpline&  right ,      
  const char*                                name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_phis     ( "phis"   , this , right.m_phis  ) 
    //
  , m_iterator ( 0 ) 
    //
  , m_spline ( right.m_spline ) 
{
  m_iterator = m_phis.createIterator () ;
  setPars () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::MonothonicSpline::~MonothonicSpline() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::MonothonicSpline*
Analysis::Models::MonothonicSpline::clone( const char* name ) const 
{ return new Analysis::Models::MonothonicSpline(*this,name) ; }
// ============================================================================
void Analysis::Models::MonothonicSpline::setPars () const 
{
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
    m_spline.setPar ( k  , phi ) ;
    //
    ++k ;
  }
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::MonothonicSpline::evaluate() const 
{
  //
  setPars () ;
  //
  return m_spline ( m_x ) ; 
}
// ============================================================================
Int_t Analysis::Models::MonothonicSpline::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::MonothonicSpline::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_spline.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================

// ============================================================================
// monothonic spline 
// ============================================================================
/** constructor with the spline 
 *  @param name  the name 
 *  @param title the  title
 *  @param x     the  variable 
 *  @param spine the spline  
 *  @param phis  vector of parameters 
 */
// ============================================================================
Analysis::Models::ConvexSpline::ConvexSpline 
( const char*                          name, 
  const char*                          title     ,
  RooAbsReal&                          x         ,
  const Gaudi::Math::ConvexSpline& spline    ,   // the spline 
  RooArgList&                          phis      )   // parameters
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable"   , this , x ) 
  , m_phis     ( "phi"     , "Coefficients" , this     )
    //
  , m_iterator ( 0 ) 
    //
  , m_spline   ( spline ) 
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
  setPars () ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::ConvexSpline::ConvexSpline 
( const Analysis::Models::ConvexSpline&  right ,      
  const char*                                name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_phis     ( "phis"   , this , right.m_phis  ) 
    //
  , m_iterator ( 0 ) 
    //
  , m_spline ( right.m_spline ) 
{
  m_iterator = m_phis.createIterator () ;
  setPars () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::ConvexSpline::~ConvexSpline() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::ConvexSpline*
Analysis::Models::ConvexSpline::clone( const char* name ) const 
{ return new Analysis::Models::ConvexSpline(*this,name) ; }
// ============================================================================
void Analysis::Models::ConvexSpline::setPars () const 
{
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
    m_spline.setPar ( k  , phi ) ;
    //
    ++k ;
  }
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::ConvexSpline::evaluate() const 
{
  //
  setPars () ;
  //
  return m_spline ( m_x ) ; 
}
// ============================================================================
Int_t Analysis::Models::ConvexSpline::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::ConvexSpline::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_spline.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================


// ============================================================================
// generic polinomial times exponent 
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
  , m_positive ( phis.getSize() , 0 , xmin , xmax ) 
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
  m_positive.setTau ( m_tau ) ;
  setPars () ;
  //
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
  setPars () ;
  //
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
void Analysis::Models::ExpoPositive::setPars () const 
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
  m_positive.setTau ( m_tau ) ;
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::ExpoPositive::evaluate() const 
{
  //
  setPars () ;
  //
  return m_positive ( m_x   ) ;
}
// ============================================================================
Int_t Analysis::Models::ExpoPositive::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::ExpoPositive::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_positive.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================


// ============================================================================
// generic polinomial times exponent 
// ============================================================================
Analysis::Models::TwoExpoPositive::TwoExpoPositive
( const char*          name      , 
  const char*          title     ,
  RooAbsReal&          x         ,
  RooAbsReal&          alpha     , 
  RooAbsReal&          delta     , 
  RooAbsReal&          x0        , 
  const RooArgList&    phis      , 
  const double         xmin      , 
  const double         xmax      ) 
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable"   , this , x     ) 
  , m_alpha    ( "alpha"   , "slope 1"      , this , alpha )
  , m_delta    ( "delta"   , "delta slope"  , this , delta )
  , m_x0       ( "x0"      , "threshold"    , this , x0    )
  , m_phis     ( "phi"     , "Coefficients" , this )
//
  , m_iterator ( 0 ) 
    //
  , m_2expopos ( phis.getSize() , 1 , 2 , 1 , xmin , xmax ) 
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
  setPars () ;
  //
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::TwoExpoPositive::TwoExpoPositive
( const Analysis::Models::TwoExpoPositive&  right ,      
  const char*                               name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_alpha    ( "alpha"  , this , right.m_alpha )
  , m_delta    ( "delta"  , this , right.m_delta )
  , m_x0       ( "x0"     , this , right.m_x0    )
  , m_phis     ( "phis"   , this , right.m_phis  ) 
//
  , m_iterator ( 0 ) 
//
  , m_2expopos ( right.m_2expopos ) 
{
  m_iterator = m_phis.createIterator () ;
  setPars () ;
  //
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::TwoExpoPositive::~TwoExpoPositive() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::TwoExpoPositive*
Analysis::Models::TwoExpoPositive::clone( const char* name ) const 
{ return new Analysis::Models::TwoExpoPositive(*this,name) ; }
// ============================================================================
void Analysis::Models::TwoExpoPositive::setPars () const 
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
    m_2expopos.setPar ( k  , phi ) ;
    //
    ++k ;
  }
  //
  m_2expopos.setAlpha ( m_alpha ) ;
  m_2expopos.setDelta ( m_delta ) ;
  m_2expopos.setX0    ( m_x0    ) ;
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::TwoExpoPositive::evaluate() const 
{
  //
  setPars () ;
  //
  return m_2expopos( m_x   ) ;
}
// ============================================================================
Int_t Analysis::Models::TwoExpoPositive::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::TwoExpoPositive::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_2expopos.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::GammaDist::setPars () const 
{
  //
  m_gamma.setK     ( m_k     ) ;
  m_gamma.setTheta ( m_theta ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::GammaDist::evaluate() const 
{
  //
  setPars () ;
  //
  return m_gamma   ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::GammaDist::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::GammaDist::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  // 
  return m_gamma.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::GenGammaDist::setPars () const 
{
  //
  m_ggamma.setK     ( m_k     ) ;
  m_ggamma.setTheta ( m_theta ) ;
  m_ggamma.setP     ( m_p     ) ;
  m_ggamma.setLow   ( m_low   ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::GenGammaDist::evaluate() const 
{
  //
  setPars () ;
  //
  return m_ggamma   ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::GenGammaDist::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::GenGammaDist::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_ggamma.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars ();
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
{
  setPars () ;
}
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
void Analysis::Models::Amoroso::setPars () const 
{
  //
  m_amoroso.setTheta     ( m_theta ) ;
  m_amoroso.setAlpha     ( m_alpha ) ;
  m_amoroso.setBeta      ( m_beta  ) ;
  m_amoroso.setA         ( m_a     ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Amoroso::evaluate() const 
{
  //
  setPars () ;
  //
  return m_amoroso   ( m_x     ) ;
}
// ============================================================================
Int_t Analysis::Models::Amoroso::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Amoroso::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_amoroso.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::LogGammaDist::setPars () const 
{
  //
  m_gamma.setK     ( m_k     ) ;
  m_gamma.setTheta ( m_theta ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::LogGammaDist::evaluate() const 
{
  //
  setPars () ;
  //
  return m_gamma   ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::LogGammaDist::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::LogGammaDist::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_gamma.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::Log10GammaDist::setPars () const 
{
  //
  m_gamma.setK     ( m_k     ) ;
  m_gamma.setTheta ( m_theta ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Log10GammaDist::evaluate() const 
{
  //
  setPars () ;
  //
  return m_gamma   ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::Log10GammaDist::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Log10GammaDist::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_gamma.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  setPars () ;
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
{
  setPars () ;
}
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
void Analysis::Models::LogGamma::setPars () const 
{
  //
  m_lgamma.setNu     ( m_nu     ) ;
  m_lgamma.setLambda ( m_lambda ) ;
  m_lgamma.setAlpha  ( m_alpha  ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::LogGamma::evaluate() const 
{
  //
  setPars () ;
  //
  return m_lgamma    ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::LogGamma::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::LogGamma::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars() ;
  //
  return m_lgamma.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================


// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::BetaPrime::BetaPrime
( const char*          name   , 
  const char*          title  ,
  RooAbsReal&          x      ,
  RooAbsReal&          alpha  ,
  RooAbsReal&          beta   ,
  RooAbsReal&          scale  ,
  RooAbsReal&          shift  )
  : RooAbsPdf ( name , title ) 
//
  , m_x       ( "x"      , "Observable" , this , x      ) 
  , m_alpha   ( "alpha"  , "alpha"      , this , alpha  ) 
  , m_beta    ( "beta"   , "beta"       , this , beta   ) 
  , m_scale   ( "scale"  , "scale"      , this , scale  ) 
  , m_shift   ( "shift"  , "shift"      , this , shift  ) 
    //
  , m_betap   ( 3 , 3 , 1 , 0 ) 
{
  setPars() ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::BetaPrime::BetaPrime
( const Analysis::Models::BetaPrime& right ,
  const char*                       name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x      ( "x"      , this , right.m_x      ) 
  , m_alpha  ( "alpha"  , this , right.m_alpha  )
  , m_beta   ( "beta"   , this , right.m_beta   )
  , m_scale  ( "scale"  , this , right.m_scale  )
  , m_shift  ( "shift"  , this , right.m_shift  )
//
  , m_betap  (                   right.m_betap  ) 
{
  setPars () ;
}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::BetaPrime::~BetaPrime () {}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::BetaPrime*
Analysis::Models::BetaPrime::clone( const char* name ) const 
{ return new Analysis::Models::BetaPrime ( *this , name) ; }
// ============================================================================
void Analysis::Models::BetaPrime::setPars () const 
{
  //
  m_betap.setAlpha  ( m_alpha  ) ;
  m_betap.setBeta   ( m_beta   ) ;
  m_betap.setScale  ( m_scale  ) ;
  m_betap.setShift  ( m_shift  ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::BetaPrime::evaluate() const 
{
  //
  setPars () ;
  //
  return m_betap    ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::BetaPrime::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::BetaPrime::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_betap.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================


// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::SinhAsinh::SinhAsinh
( const char*          name    , 
  const char*          title   ,
  RooAbsReal&          x       ,
  RooAbsReal&          mu      ,
  RooAbsReal&          sigma   ,
  RooAbsReal&          epsilon ,
  RooAbsReal&          delta   )
  : RooAbsPdf ( name , title ) 
    //
  , m_x       ( "x"       , "Observable"    , this , x       ) 
  , m_mu      ( "mu"      , "mu/location"   , this , mu      ) 
  , m_sigma   ( "sigma"   , "sigma/scale"   , this , sigma   ) 
  , m_epsilon ( "epsilon" , "epsilon/skew"  , this , epsilon ) 
  , m_delta   ( "delta"   , "delta/tail"    , this , delta   ) 
    //
  , m_sinhasinh  ( 1 , 1 , 0 , 1 )  
{
  setPars() ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::SinhAsinh::SinhAsinh
( const Analysis::Models::SinhAsinh& right , 
  const char*                         name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x       ( "x"       , this , right.m_x       ) 
  , m_mu      ( "mu"      , this , right.m_mu      )
  , m_sigma   ( "sigma"   , this , right.m_sigma   )
  , m_epsilon ( "epsilon" , this , right.m_epsilon )
  , m_delta   ( "delta"   , this , right.m_delta   )
    //
  , m_sinhasinh ( right.m_sinhasinh ) 
{
  setPars () ;
}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::SinhAsinh::~SinhAsinh(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::SinhAsinh*
Analysis::Models::SinhAsinh::clone( const char* name ) const 
{ return new Analysis::Models::SinhAsinh ( *this , name) ; }
// ============================================================================
void Analysis::Models::SinhAsinh::setPars () const 
{
  //
  m_sinhasinh.setMu       ( m_mu      ) ;
  m_sinhasinh.setSigma    ( m_sigma   ) ;
  m_sinhasinh.setEpsilon  ( m_epsilon ) ;
  m_sinhasinh.setDelta    ( m_delta   ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::SinhAsinh::evaluate() const 
{
  //
  setPars () ;
  //
  return m_sinhasinh ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::SinhAsinh::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::SinhAsinh::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_sinhasinh.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================


// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::JohnsonSU::JohnsonSU
( const char*          name    , 
  const char*          title   ,
  RooAbsReal&          x       ,
  RooAbsReal&          xi      ,
  RooAbsReal&          lam     ,
  RooAbsReal&          delta   ,
  RooAbsReal&          gamma   )
  : RooAbsPdf ( name , title ) 
    //
  , m_x       ( "x"       , "Observable"    , this , x       ) 
  , m_xi      ( "xi"      , "mu/location"   , this , xi      ) 
  , m_lambda  ( "lambda"  , "lambda/scale"  , this , lam     ) 
  , m_delta   ( "delta"   , "delta/shape"   , this , delta   ) 
  , m_gamma   ( "gamma"   , "gamma/shape"   , this , gamma   ) 
    //
  , m_johnsonSU  ( 0 , 1 , 1 , 0 )  
{
  setPars() ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::JohnsonSU::JohnsonSU
( const Analysis::Models::JohnsonSU&  right , 
  const char*                         name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x       ( "x"       , this , right.m_x       ) 
  , m_xi      ( "xi"      , this , right.m_xi      )
  , m_lambda  ( "sigma"   , this , right.m_lambda  )
  , m_delta   ( "delta"   , this , right.m_delta   )
  , m_gamma   ( "gamma"   , this , right.m_gamma   )
    //
  , m_johnsonSU ( right.m_johnsonSU ) 
{
  setPars () ;
}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::JohnsonSU::~JohnsonSU(){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::JohnsonSU*
Analysis::Models::JohnsonSU::clone( const char* name ) const 
{ return new Analysis::Models::JohnsonSU ( *this , name) ; }
// ============================================================================
void Analysis::Models::JohnsonSU::setPars () const 
{
  //
  m_johnsonSU.setXi       ( m_xi      ) ;
  m_johnsonSU.setLambda   ( m_lambda  ) ;
  m_johnsonSU.setDelta    ( m_delta   ) ;
  m_johnsonSU.setGamma    ( m_gamma   ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::JohnsonSU::evaluate() const 
{
  //
  setPars () ;
  //
  return m_johnsonSU ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::JohnsonSU::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::JohnsonSU::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_johnsonSU.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================


// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::Landau::Landau
( const char*          name   , 
  const char*          title  ,
  RooAbsReal&          x      ,
  RooAbsReal&          scale  ,
  RooAbsReal&          shift  )
  : RooAbsPdf ( name , title ) 
//
  , m_x       ( "x"      , "Observable" , this , x      ) 
  , m_scale   ( "scale"  , "scale"      , this , scale  ) 
  , m_shift   ( "shift"  , "shift"      , this , shift  ) 
    //
  , m_landau  ( 1 , 0 ) 
{
  setPars() ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Landau::Landau
( const Analysis::Models::Landau& right ,
  const char*                     name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x      ( "x"      , this , right.m_x      ) 
  , m_scale  ( "scale"  , this , right.m_scale  )
  , m_shift  ( "shift"  , this , right.m_shift  )
//
  , m_landau (                   right.m_landau ) 
{
  setPars () ;
}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::Landau::~Landau () {}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Landau*
Analysis::Models::Landau::clone( const char* name ) const 
{ return new Analysis::Models::Landau ( *this , name) ; }
// ============================================================================
void Analysis::Models::Landau::setPars () const 
{
  //
  m_landau.setScale  ( m_scale  ) ;
  m_landau.setShift  ( m_shift  ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Landau::evaluate() const 
{
  //
  setPars () ;
  //
  return m_landau ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::Landau::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Landau::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_landau.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================


// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::Atlas::Atlas
( const char*          name   , 
  const char*          title  ,
  RooAbsReal&          x      ,
  RooAbsReal&          mu     ,
  RooAbsReal&          sigma  )
  : RooAbsPdf ( name , title ) 
    //
  , m_x      ( "x"      , "Observable" , this , x      ) 
  , m_mu     ( "mu"     , "location"   , this , mu     ) 
  , m_sigma  ( "sigma"  , "sigma"      , this , sigma  ) 
    //
  , m_atlas  ( 0 , 1 ) 
{
  setPars() ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Atlas::Atlas
( const Analysis::Models::Atlas&  right ,
  const char*                     name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x      ( "x"      , this , right.m_x      ) 
  , m_mu     ( "mu"     , this , right.m_mu     )
  , m_sigma  ( "sigma"  , this , right.m_sigma  )
//
  , m_atlas  (                   right.m_atlas  ) 
{
  setPars () ;
}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::Atlas::~Atlas () {}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Atlas*
Analysis::Models::Atlas::clone( const char* name ) const 
{ return new Analysis::Models::Atlas ( *this , name) ; }
// ============================================================================
void Analysis::Models::Atlas::setPars () const 
{
  //
  m_atlas.setMean  ( m_mu    ) ;
  m_atlas.setSigma ( m_sigma ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Atlas::evaluate() const 
{
  //
  setPars () ;
  //
  return m_atlas ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::Atlas::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Atlas::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_atlas.integral ( m_x.min( rangeName ) , m_x.max( rangeName ) ) ;
}
// ============================================================================

// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::Argus::Argus
( const char*          name   , 
  const char*          title  ,
  RooAbsReal&          x      ,
  RooAbsReal&          shape  ,
  RooAbsReal&          high   ,
  RooAbsReal&          low    )
  : RooAbsPdf ( name , title ) 
//
  , m_x       ( "x"      , "Observable" , this , x      ) 
  , m_shape   ( "shape"  , "shape"      , this , shape  ) 
  , m_high    ( "high"   , "high"       , this , high   ) 
  , m_low     ( "low"    , "low"        , this , low    ) 
    //
  , m_argus  ( 1 , 1 , 0 ) 
{
  setPars() ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Argus::Argus
( const Analysis::Models::Argus&  right ,
  const char*                     name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x      ( "x"      , this , right.m_x      ) 
  , m_shape  ( "shape"  , this , right.m_shape  )
  , m_high   ( "high"   , this , right.m_high   )
  , m_low    ( "low"    , this , right.m_low    )
//
  , m_argus  (                   right.m_argus  ) 
{
  setPars () ;
}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::Argus::~Argus () {}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Argus*
Analysis::Models::Argus::clone( const char* name ) const 
{ return new Analysis::Models::Argus ( *this , name) ; }
// ============================================================================
void Analysis::Models::Argus::setPars () const 
{
  //
  m_argus.setShape  ( m_shape  ) ;
  m_argus.setLow    ( m_low    ) ;
  m_argus.setHigh   ( m_high   ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Argus::evaluate() const 
{
  //
  setPars () ;
  //
  return m_argus ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::Argus::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Argus::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_argus.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================



// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::Tsallis::Tsallis
( const char*           name      , 
  const char*           title     ,
  RooAbsReal&           x         , 
  RooAbsReal&           n         ,   // parameter N 
  RooAbsReal&           T         ,   // parameter T
  RooAbsReal&           mass      )   // particle mass (fixed)
  : RooAbsPdf ( name , title ) 
    //
  , m_x       ( "x"      , "Observable"  , this , x      ) 
  , m_n       ( "n"      , "shape"       , this , n      ) 
  , m_T       ( "T"      , "temperature" , this , T      ) 
  , m_mass    ( "m"      , "mass"        , this , mass   ) 
    //
  , m_tsallis  ( 0 , 10 , 1 ) 
{
  setPars() ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::Tsallis::Tsallis
( const Analysis::Models::Tsallis& right ,
  const char*                      name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x       ( "x"  , this , right.m_x       ) 
  , m_n       ( "n"  , this , right.m_n       )
  , m_T       ( "T"  , this , right.m_T       )
  , m_mass    ( "m"  , this , right.m_mass    )
    //
  , m_tsallis (               right.m_tsallis ) 
{
  setPars () ;
}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::Tsallis::~Tsallis () {}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Tsallis*
Analysis::Models::Tsallis::clone( const char* name ) const 
{ return new Analysis::Models::Tsallis ( *this , name) ; }
// ============================================================================
void Analysis::Models::Tsallis::setPars () const 
{
  //
  m_tsallis.setMass  ( m_mass ) ;
  m_tsallis.setN     ( m_n    ) ;
  m_tsallis.setT     ( m_T    ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Tsallis::evaluate() const 
{
  //
  setPars () ;
  //
  return m_tsallis ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::Tsallis::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Tsallis::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_tsallis.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================



// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::QGSM::QGSM
( const char*           name      , 
  const char*           title     ,
  RooAbsReal&           x         , 
  RooAbsReal&           b         ,   // parameter b 
  RooAbsReal&           mass      )   // particle mass (fixed)
  : RooAbsPdf ( name , title ) 
    //
  , m_x       ( "x"      , "Observable"  , this , x      ) 
  , m_b       ( "b"      , "slope"       , this , b      ) 
  , m_mass    ( "m"      , "mass"        , this , mass   ) 
    //
  , m_qgsm  ( 0 , 1 ) 
{
  setPars() ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::QGSM::QGSM
( const Analysis::Models::QGSM&    right ,
  const char*                      name  ) 
  : RooAbsPdf ( right , name ) 
//
  , m_x    ( "x"  , this , right.m_x    ) 
  , m_b    ( "b"  , this , right.m_b    )
  , m_mass ( "m"  , this , right.m_mass )
    //
  , m_qgsm (               right.m_qgsm ) 
{
  setPars () ;
}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::QGSM::~QGSM () {}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::QGSM*
Analysis::Models::QGSM::clone( const char* name ) const 
{ return new Analysis::Models::QGSM ( *this , name) ; }
// ============================================================================
void Analysis::Models::QGSM::setPars () const 
{
  //
  m_qgsm.setMass  ( m_mass ) ;
  m_qgsm.setB     ( m_b    ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::QGSM::evaluate() const 
{
  //
  setPars () ;
  //
  return m_qgsm ( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::QGSM::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::QGSM::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_qgsm.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
}
// ============================================================================




// ============================================================================
// constructor from all parameters 
// ============================================================================
Analysis::Models::TwoExpos::TwoExpos 
( const char*          name   , 
  const char*          title  ,
  RooAbsReal&          x      ,
  RooAbsReal&          alpha  ,
  RooAbsReal&          delta  ,
  RooAbsReal&          x0     )
  : RooAbsPdf ( name , title ) 
//
  , m_x       ( "x"      , "Observable" , this , x     ) 
  , m_alpha   ( "alpha"  , "alpha"      , this , alpha ) 
  , m_delta   ( "delta"  , "delta"      , this , delta ) 
  , m_x0      ( "x0"     , "x0"         , this , x0    ) 
    //
  , m_2expos  ( 1 , 1 , 0 )
{
  setPars() ;
}
// ============================================================================
// "copy" constructor 
// ============================================================================
Analysis::Models::TwoExpos::TwoExpos 
( const Analysis::Models::TwoExpos&  right ,
  const char*                     name  ) 
  : RooAbsPdf ( right , name ) 
    //
  , m_x      ( "x"      , this , right.m_x     ) 
  , m_alpha  ( "alpha"  , this , right.m_alpha ) 
  , m_delta  ( "delta"  , this , right.m_delta ) 
  , m_x0     ( "x0"      , this , right.m_x0    ) 
    //
  , m_2expos (                   right.m_2expos ) 
{
  setPars () ;
}
// ============================================================================
// destructor
// ============================================================================
Analysis::Models::TwoExpos::~TwoExpos (){}
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::TwoExpos*
Analysis::Models::TwoExpos::clone( const char* name ) const 
{ return new Analysis::Models::TwoExpos ( *this , name) ; }
// ============================================================================
void Analysis::Models::TwoExpos::setPars () const 
{
  //
  m_2expos.setAlpha  ( m_alpha  ) ;
  m_2expos.setDelta  ( m_delta  ) ;
  m_2expos.setX0     ( m_x0     ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::TwoExpos::evaluate() const 
{
  //
  setPars () ;
  //
  return m_2expos( m_x ) ;
}
// ============================================================================
Int_t Analysis::Models::TwoExpos::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if ( matchArgs ( allVars , analVars , m_x ) ) { return 1 ; }
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::TwoExpos::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( code == 1 ) ;
  if ( 1 != code ) {}
  //
  setPars () ;
  //
  return m_2expos.integral ( m_x.min(rangeName) , m_x.max(rangeName) ) ;
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
  : RooAbsPdf  ( name , title ) 
  , m_x        ( "x"       , "Observable-X" , this , x ) 
  , m_y        ( "y"       , "Observable-Y" , this , y ) 
  , m_phis     ( "phis"    , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
//
  , m_positive ( nX , nY , x.getMin() , x.getMax() , y.getMin() , y.getMax() ) 
{
  //
  TIterator*   tmp  = phis.createIterator() ;
  RooAbsArg*   coef = 0 ;
  unsigned int num  = 0 ;
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
  setPars () ;
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
  setPars () ;
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
void Analysis::Models::Poly2DPositive::setPars () const 
{
  //
  m_iterator->Reset () ;
  //
  RooAbsArg*       phi   = 0 ;
  const RooArgSet* nset  = m_phis.nset() ;
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
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Poly2DPositive::evaluate() const 
{
  //
  setPars () ;
  //
  return m_positive ( m_x , m_y ) ; 
}
// ============================================================================
Int_t Analysis::Models::Poly2DPositive::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if      ( matchArgs ( allVars , analVars , m_x , m_y ) ) { return 1 ; }
  else if ( matchArgs ( allVars , analVars , m_x       ) ) { return 2 ; }
  else if ( matchArgs ( allVars , analVars       , m_y ) ) { return 3 ; }
  //
  return 0 ;
}
//_____________________________________________________________________________
Double_t Analysis::Models::Poly2DPositive::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( 1 == code || 2 ==code || 3 == code ) ;
  //
  setPars () ;
  //
  return 
    1 == code ? m_positive.integral   (        m_x.min(rangeName) , m_x.max(rangeName) ,
                                               m_y.min(rangeName) , m_y.max(rangeName) ) : 
    2 == code ? m_positive.integrateX ( m_y  , m_x.min(rangeName) , m_x.max(rangeName) ) : 
    3 == code ? m_positive.integrateY ( m_x  , m_y.min(rangeName) , m_y.max(rangeName) ) : 0.0 ;  
}
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
  setPars () ;
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
  setPars () ;
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
void Analysis::Models::Poly2DSymPositive::setPars () const 
{
  //
  m_iterator->Reset () ;
  //
  RooAbsArg*       phi   = 0 ;
  const RooArgSet* nset  = m_phis.nset() ;
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
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Poly2DSymPositive::evaluate() const 
{
  //
  m_iterator->Reset () ;
  //
  setPars () ;
  //
  return m_positive ( m_x , m_y ) ; 
}
// ============================================================================
Int_t Analysis::Models::Poly2DSymPositive::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if      ( matchArgs ( allVars , analVars , m_x , m_y ) ) { return 1 ; }
  else if ( matchArgs ( allVars , analVars , m_x       ) ) { return 2 ; }
  else if ( matchArgs ( allVars , analVars       , m_y ) ) { return 3 ; }
  //
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Poly2DSymPositive::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( 1 == code || 2 == code || 3 == code ) ;
  //
  setPars () ;
  //
  return 
    1 == code ? m_positive.integral   (        m_x.min(rangeName) , m_x.max(rangeName) , 
                                               m_y.min(rangeName) , m_y.max(rangeName) ) : 
    2 == code ? m_positive.integrateX ( m_y  , m_x.min(rangeName) , m_x.max(rangeName) ) : 
    3 == code ? m_positive.integrateY ( m_x  , m_y.min(rangeName) , m_y.max(rangeName) ) : 0.0 ;  
}

// ============================================================================
//  PS(x)*PS(y)*Polynom 
// ============================================================================
Analysis::Models::PS2DPol::PS2DPol
( const char*          name      , 
  const char*          title     ,
  RooRealVar&          x         ,
  RooRealVar&          y         ,
  const Gaudi::Math::PhaseSpaceNL& psx , 
  const Gaudi::Math::PhaseSpaceNL& psy ,
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
  , m_function ( psx, psy , nX , nY , 
                 x.getMin() , x.getMax() ,
                 y.getMin() , y.getMax() )
{
  //
  
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  unsigned num = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_function.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_function.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::PS22DPol"               , 
                           StatusCode::FAILURE                ) ; }
  
  //
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
// generic polinomial
// ============================================================================
Analysis::Models::PS2DPol::PS2DPol
( const char*          name      , 
  const char*          title     ,
  RooRealVar&          x         ,
  RooRealVar&          y         ,
  const Gaudi::Math::PS2DPol& ps , 
  RooArgList&          phis      ) 
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable-X" , this , x ) 
  , m_y        ( "y"       , "Observable-Y" , this , y ) 
  , m_phis     ( "phis"    , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
    //
  , m_function ( ps ) 
{
  //
  
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  unsigned num = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_function.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_function.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::PS22DPol"               , 
                           StatusCode::FAILURE                ) ; }
  
  //
  m_iterator = m_phis.createIterator() ;
  setPars () ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::PS2DPol::PS2DPol
( const Analysis::Models::PS2DPol&  right ,      
  const char*                       name  ) 
  : RooAbsPdf  ( right , name ) 
    //
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_y        ( "y"      , this , right.m_y     ) 
  , m_phis     ( "phis"   , this , right.m_phis  ) 
    //
  , m_iterator ( 0 ) 
    //
  , m_function ( right.m_function )
{
  m_iterator = m_phis.createIterator () ;
  setPars () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PS2DPol::~PS2DPol() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PS2DPol*
Analysis::Models::PS2DPol::clone( const char* name ) const 
{ return new Analysis::Models::PS2DPol ( *this , name ) ; }
// ============================================================================
void Analysis::Models::PS2DPol::setPars () const 
{
  //
  m_iterator->Reset () ;
  //
  RooAbsArg*       phi   = 0 ;
  const RooArgSet* nset  = m_phis.nset() ;
  //
  unsigned short k = 0 ;
  while ( ( phi = (RooAbsArg*) m_iterator->Next() ) )
  {
    const RooAbsReal* r = dynamic_cast<RooAbsReal*> ( phi ) ;
    if ( 0 == r ) { continue ; }
    //
    const double phiv   = r->getVal ( nset ) ;
    //
    m_function.setPar ( k  , phiv ) ;
    //
    ++k ;
  }
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PS2DPol::evaluate() const 
{
  //
  setPars () ;
  //
  return m_function ( m_x , m_y ) ;
}
// ============================================================================
Int_t Analysis::Models::PS2DPol::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if      ( matchArgs ( allVars , analVars , m_x , m_y ) ) { return 1 ; }
  else if ( matchArgs ( allVars , analVars , m_x       ) ) { return 2 ; }
  else if ( matchArgs ( allVars , analVars       , m_y ) ) { return 3 ; }
  //
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::PS2DPol::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( 1 == code || 2 == code || 3 == code ) ;
  //
  setPars () ;
  //
  return 
    1 == code ? m_function.integral   (        m_x.min(rangeName) , m_x.max(rangeName) , 
                                               m_y.min(rangeName) , m_y.max(rangeName) ) : 
    2 == code ? m_function.integrateX ( m_y  , m_x.min(rangeName) , m_x.max(rangeName) ) : 
    3 == code ? m_function.integrateY ( m_x  , m_y.min(rangeName) , m_y.max(rangeName) ) : 0.0 ;  
}
// ============================================================================
//  PS(x)*PS(y)*SymPolynom 
// ============================================================================
Analysis::Models::PS2DPolSym::PS2DPolSym
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
  , m_phis     ( "phis"    , "Coefficients" , this     )
    //
  , m_iterator ( 0 ) 
    //
  , m_function ( ps , n , x.getMin() , x.getMax() ) 
{
  //
  
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  unsigned num = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_function.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_function.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::PS22DPol"               , 
                           StatusCode::FAILURE                ) ; }
  
  //
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
// generic polinomial
// ============================================================================
Analysis::Models::PS2DPolSym::PS2DPolSym
( const char*          name      , 
  const char*          title     ,
  RooRealVar&          x         ,
  RooRealVar&          y         ,
  const Gaudi::Math::PS2DPolSym& ps , 
  RooArgList&          phis      ) 
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable-X" , this , x ) 
  , m_y        ( "y"       , "Observable-Y" , this , y ) 
  , m_phis     ( "phis"    , "Coefficients" , this     )
    //
  , m_iterator ( 0  ) 
    //
  , m_function ( ps ) 
{
  //
  
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  unsigned num = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_function.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_function.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::PS22DPol"               , 
                           StatusCode::FAILURE                ) ; }
  
  //
  m_iterator = m_phis.createIterator() ;
  setPars () ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::PS2DPolSym::PS2DPolSym
( const Analysis::Models::PS2DPolSym& right ,      
  const char*                         name  ) 
  : RooAbsPdf  ( right , name ) 
    //
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_y        ( "y"      , this , right.m_y     ) 
  , m_phis     ( "phis"   , this , right.m_phis  ) 
    //
  , m_iterator ( 0 ) 
    //
  , m_function ( right.m_function )
{
  m_iterator = m_phis.createIterator () ;
  setPars () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::PS2DPolSym::~PS2DPolSym() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::PS2DPolSym*
Analysis::Models::PS2DPolSym::clone( const char* name ) const 
{ return new Analysis::Models::PS2DPolSym ( *this , name ) ; }
// ============================================================================
void Analysis::Models::PS2DPolSym::setPars () const 
{
  m_iterator->Reset () ;
  //
  RooAbsArg*       phi   = 0 ;
  const RooArgSet* nset  = m_phis.nset() ;
  //
  unsigned short k = 0 ;
  while ( ( phi = (RooAbsArg*) m_iterator->Next() ) )
  {
    const RooAbsReal* r = dynamic_cast<RooAbsReal*> ( phi ) ;
    if ( 0 == r ) { continue ; }
    //
    const double phiv   = r->getVal ( nset ) ;
    //
    m_function.setPar ( k  , phiv ) ;
    //
    ++k ;
  }
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::PS2DPolSym::evaluate() const 
{
  //
  setPars () ;
  //
  return m_function ( m_x , m_y ) ;
}
// ============================================================================
Int_t Analysis::Models::PS2DPolSym::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if      ( matchArgs ( allVars , analVars , m_x , m_y ) ) { return 1 ; }
  else if ( matchArgs ( allVars , analVars , m_x       ) ) { return 2 ; }
  else if ( matchArgs ( allVars , analVars       , m_y ) ) { return 3 ; }
  //
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::PS2DPolSym::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( 1 == code || 2 == code || 3 == code ) ;
  //
  setPars () ;
  //
  return 
    1 == code ? m_function.integral   (        m_x.min(rangeName) , m_x.max(rangeName) , 
                                               m_y.min(rangeName) , m_y.max(rangeName) ) : 
    2 == code ? m_function.integrateX ( m_y  , m_x.min(rangeName) , m_x.max(rangeName) ) : 
    3 == code ? m_function.integrateY ( m_x  , m_y.min(rangeName) , m_y.max(rangeName) ) : 0.0 ;  
}


// ============================================================================
//  exp(x)*PS(y)*Polynom 
// ============================================================================
Analysis::Models::ExpoPS2DPol::ExpoPS2DPol
( const char*          name      , 
  const char*          title     ,
  RooRealVar&          x         ,
  RooRealVar&          y         ,
  RooAbsReal&          tau       ,
  const Gaudi::Math::PhaseSpaceNL& psy ,
  const unsigned short nX        ,
  const unsigned short nY        ,
  RooArgList&          phis      ) 
  : RooAbsPdf  ( name , title ) 
  , m_x        ( "x"       , "Observable-X"      , this , x   ) 
  , m_y        ( "y"       , "Observable-Y"      , this , y   ) 
  , m_tau      ( "tau"     , "Exponential slope" , this , tau ) 
  , m_phis     ( "phis"    , "Coefficients"      , this       )
    //
  , m_iterator ( 0 ) 
    //
  , m_function ( psy        ,
                 x.getMin() , x.getMax() ,
                 nX         , nY         , 
                 y.getMin() , y.getMax() )
{
  //
  
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  unsigned num = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_function.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_function.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::ExpoPS22DPol"           , 
                           StatusCode::FAILURE                ) ; }
  
  //
  m_iterator = m_phis.createIterator() ;
}
// ============================================================================
// generic polinomial
// ============================================================================
Analysis::Models::ExpoPS2DPol::ExpoPS2DPol
( const char*          name      , 
  const char*          title     ,
  RooRealVar&          x         ,
  RooRealVar&          y         ,
  RooAbsReal&          tau       ,
  const Gaudi::Math::ExpoPS2DPol& ps , 
  RooArgList&          phis      ) 
  : RooAbsPdf ( name , title ) 
  , m_x        ( "x"       , "Observable-X"      , this , x   ) 
  , m_y        ( "y"       , "Observable-Y"      , this , y   ) 
  , m_tau      ( "tau"     , "Exponential slope" , this , tau ) 
  , m_phis     ( "phis"    , "Coefficients"      , this       )
    //
  , m_iterator ( 0 ) 
    //
  , m_function ( ps ) 
{
  //
  
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  unsigned num = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_function.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_function.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::ExpoPS22DPol"           , 
                           StatusCode::FAILURE                ) ; }
  
  //
  m_iterator = m_phis.createIterator() ;
  setPars () ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::ExpoPS2DPol::ExpoPS2DPol
( const Analysis::Models::ExpoPS2DPol&  right ,      
  const char*                       name  ) 
  : RooAbsPdf  ( right , name ) 
    //
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_y        ( "y"      , this , right.m_y     ) 
  , m_tau      ( "tau"    , this , right.m_tau   ) 
  , m_phis     ( "phis"   , this , right.m_phis  ) 
    //
  , m_iterator ( 0 ) 
    //
  , m_function ( right.m_function )
{
  m_iterator = m_phis.createIterator () ;
  setPars () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::ExpoPS2DPol::~ExpoPS2DPol() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::ExpoPS2DPol*
Analysis::Models::ExpoPS2DPol::clone( const char* name ) const 
{ return new Analysis::Models::ExpoPS2DPol ( *this , name ) ; }
// ============================================================================
void Analysis::Models::ExpoPS2DPol::setPars () const 
{
  //
  m_iterator->Reset () ;
  //
  RooAbsArg*       phi   = 0 ;
  const RooArgSet* nset  = m_phis.nset() ;
  //
  unsigned short k = 0 ;
  while ( ( phi = (RooAbsArg*) m_iterator->Next() ) )
  {
    const RooAbsReal* r = dynamic_cast<RooAbsReal*> ( phi ) ;
    if ( 0 == r ) { continue ; }
    //
    const double phiv   = r->getVal ( nset ) ;
    //
    m_function.setPar ( k  , phiv ) ;
    //
    ++k ;
  }
  //
  m_function.setTau ( m_tau ) ;
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::ExpoPS2DPol::evaluate() const 
{
  //
  setPars () ;
  //
  return m_function ( m_x , m_y ) ;
}
// ============================================================================
Int_t Analysis::Models::ExpoPS2DPol::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if      ( matchArgs ( allVars , analVars , m_x , m_y ) ) { return 1 ; }
  else if ( matchArgs ( allVars , analVars , m_x       ) ) { return 2 ; }
  else if ( matchArgs ( allVars , analVars       , m_y ) ) { return 3 ; }
  //
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::ExpoPS2DPol::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( 1 == code || 2 == code || 3 == code ) ;
  //
  setPars () ;
  //
  return 
    1 == code ? m_function.integral   (        m_x.min(rangeName) , m_x.max(rangeName) , 
                                               m_y.min(rangeName) , m_y.max(rangeName) ) : 
    2 == code ? m_function.integrateX ( m_y  , m_x.min(rangeName) , m_x.max(rangeName) ) : 
    3 == code ? m_function.integrateY ( m_x  , m_y.min(rangeName) , m_y.max(rangeName) ) : 0.0 ;  
}



// ============================================================================
//  exp(x)*exp(y)*Polynom 
// ============================================================================
Analysis::Models::Expo2DPol::Expo2DPol
( const char*          name      , 
  const char*          title     ,
  RooRealVar&          x         ,
  RooRealVar&          y         ,
  RooAbsReal&          taux      ,
  RooAbsReal&          tauy      ,
  const unsigned short nX        ,
  const unsigned short nY        ,
  RooArgList&          phis      ) 
  : RooAbsPdf  ( name , title ) 
  , m_x        ( "x"       , "Observable-X"      , this , x    ) 
  , m_y        ( "y"       , "Observable-Y"      , this , y    ) 
  , m_taux     ( "taux"    , "Exponential slope" , this , taux ) 
  , m_tauy     ( "tauy"    , "Exponential slope" , this , tauy ) 
  , m_phis     ( "phis"    , "Coefficients"      , this        )
    //
  , m_iterator ( 0 ) 
    //
  , m_function ( x.getMin() , x.getMax() ,
                 y.getMin() , y.getMax() ,
                 nX         , nY         )
{
  //
  
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  unsigned num = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_function.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_function.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::ExpoPS22DPol"           , 
                           StatusCode::FAILURE                ) ; }
  
  //
  m_function.setTauX ( m_taux ) ;
  m_function.setTauY ( m_tauy ) ;
  //
  m_iterator = m_phis.createIterator() ;
  setPars () ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::Expo2DPol::Expo2DPol
( const Analysis::Models::Expo2DPol&  right ,      
  const char*                       name  ) 
  : RooAbsPdf  ( right , name ) 
    //
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_y        ( "y"      , this , right.m_y     ) 
  , m_taux     ( "taux"   , this , right.m_taux  ) 
  , m_tauy     ( "tauy"   , this , right.m_tauy  ) 
  , m_phis     ( "phis"   , this , right.m_phis  ) 
    //
  , m_iterator ( 0 ) 
    //
  , m_function ( right.m_function )
{
  m_iterator = m_phis.createIterator () ;
  setPars () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::Expo2DPol::~Expo2DPol() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Expo2DPol*
Analysis::Models::Expo2DPol::clone( const char* name ) const 
{ return new Analysis::Models::Expo2DPol ( *this , name ) ; }
// ============================================================================
void Analysis::Models::Expo2DPol::setPars () const 
{
  //
  m_iterator->Reset () ;
  //
  RooAbsArg*       phi   = 0 ;
  const RooArgSet* nset  = m_phis.nset() ;
  //
  unsigned short k = 0 ;
  while ( ( phi = (RooAbsArg*) m_iterator->Next() ) )
  {
    const RooAbsReal* r = dynamic_cast<RooAbsReal*> ( phi ) ;
    if ( 0 == r ) { continue ; }
    //
    const double phiv   = r->getVal ( nset ) ;
    //
    m_function.setPar ( k  , phiv ) ;
    //
    ++k ;
  }
  //
  m_function.setTauX ( m_taux ) ;
  m_function.setTauY ( m_tauy ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Expo2DPol::evaluate() const 
{
  //
  setPars () ;
  //
  return m_function ( m_x , m_y ) ;
}
// ============================================================================
Int_t Analysis::Models::Expo2DPol::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if      ( matchArgs ( allVars , analVars , m_x , m_y ) ) { return 1 ; }
  else if ( matchArgs ( allVars , analVars , m_x       ) ) { return 2 ; }
  else if ( matchArgs ( allVars , analVars       , m_y ) ) { return 3 ; }
  //
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Expo2DPol::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( 1 == code || 2 == code || 3 == code ) ;
  //
  setPars () ;
  //
  return 
    1 == code ? m_function.integral   (        m_x.min(rangeName) , m_x.max(rangeName) , 
                                               m_y.min(rangeName) , m_y.max(rangeName) ) : 
    2 == code ? m_function.integrateX ( m_y  , m_x.min(rangeName) , m_x.max(rangeName) ) : 
    3 == code ? m_function.integrateY ( m_x  , m_y.min(rangeName) , m_y.max(rangeName) ) : 0.0 ;  
}






// ============================================================================
//  exp(x)*exp(y)*SymPolynom 
// ============================================================================
Analysis::Models::Expo2DPolSym::Expo2DPolSym
( const char*          name      , 
  const char*          title     ,
  RooRealVar&          x         ,
  RooRealVar&          y         ,
  RooAbsReal&          tau       ,
  const unsigned short n         ,
  RooArgList&          phis      ) 
  : RooAbsPdf  ( name , title ) 
  , m_x        ( "x"       , "Observable-X"      , this , x    ) 
  , m_y        ( "y"       , "Observable-Y"      , this , y    ) 
  , m_tau      ( "tau"     , "Exponential slope" , this , tau  ) 
  , m_phis     ( "phis"    , "Coefficients"      , this        )
    //
  , m_iterator ( 0 ) 
    //
  , m_function ( x.getMin() , x.getMax() , n )
{
  //
  
  TIterator* tmp  = phis.createIterator() ;
  RooAbsArg* coef = 0 ;
  unsigned num = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_function.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_function.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::ExpoPS22DPol"           , 
                           StatusCode::FAILURE                ) ; }
  
  //
  m_iterator = m_phis.createIterator() ;
  setPars () ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::Expo2DPolSym::Expo2DPolSym
( const Analysis::Models::Expo2DPolSym& right ,      
  const char*                           name  ) 
  : RooAbsPdf  ( right , name ) 
    //
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_y        ( "y"      , this , right.m_y     ) 
  , m_tau      ( "tau"    , this , right.m_tau   ) 
  , m_phis     ( "phis"   , this , right.m_phis  ) 
    //
  , m_iterator ( 0 ) 
    //
  , m_function ( right.m_function )
{
  m_iterator = m_phis.createIterator () ;
  setPars () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::Expo2DPolSym::~Expo2DPolSym() { delete m_iterator ; }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Expo2DPolSym*
Analysis::Models::Expo2DPolSym::clone( const char* name ) const 
{ return new Analysis::Models::Expo2DPolSym ( *this , name ) ; }
// ============================================================================
void Analysis::Models::Expo2DPolSym::setPars () const 
{
  m_iterator->Reset () ;
  //
  RooAbsArg*       phi   = 0 ;
  const RooArgSet* nset  = m_phis.nset() ;
  //
  unsigned short k = 0 ;
  while ( ( phi = (RooAbsArg*) m_iterator->Next() ) )
  {
    const RooAbsReal* r = dynamic_cast<RooAbsReal*> ( phi ) ;
    if ( 0 == r ) { continue ; }
    //
    const double phiv   = r->getVal ( nset ) ;
    //
    m_function.setPar ( k  , phiv ) ;
    //
    ++k ;
  }
  //
  m_function.setTau ( m_tau ) ;
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Expo2DPolSym::evaluate() const 
{
  //
  setPars () ;
  //
  return m_function ( m_x , m_y ) ;
}
// ============================================================================
Int_t Analysis::Models::Expo2DPolSym::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if      ( matchArgs ( allVars , analVars , m_x , m_y ) ) { return 1 ; }
  else if ( matchArgs ( allVars , analVars , m_x       ) ) { return 2 ; }
  else if ( matchArgs ( allVars , analVars       , m_y ) ) { return 3 ; }
  //
  return 0 ;
}
// ============================================================================
Double_t Analysis::Models::Expo2DPolSym::analyticalIntegral 
( Int_t       code      , 
  const char* rangeName ) const 
{
  assert ( 1 == code || 2 == code || 3 == code ) ;
  //
  setPars () ;
  //
  return 
    1 == code ? m_function.integral   (        m_x.min(rangeName) , m_x.max(rangeName) , 
                                               m_y.min(rangeName) , m_y.max(rangeName) ) : 
    2 == code ? m_function.integrateX ( m_y  , m_x.min(rangeName) , m_x.max(rangeName) ) : 
    3 == code ? m_function.integrateY ( m_x  , m_y.min(rangeName) , m_y.max(rangeName) ) : 0.0 ;  
}
// ============================================================================







// ============================================================================
// generic 2D-spline
// ============================================================================
Analysis::Models::Spline2D::Spline2D
( const char*          name      , 
  const char*          title     ,
  RooRealVar&          x         ,
  RooRealVar&          y         ,
  const Gaudi::Math::Spline2D& spline, 
  RooArgList&          phis      ) 
  : RooAbsPdf  ( name , title ) 
  , m_x        ( "x"       , "Observable-X" , this , x ) 
  , m_y        ( "y"       , "Observable-Y" , this , y ) 
  , m_phis     ( "phis"    , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
    //
  , m_spline   ( spline ) 
{
  //
  TIterator*   tmp  = phis.createIterator() ;
  RooAbsArg*   coef = 0 ;
  unsigned int num  = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_spline.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_spline.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::Spline2D"         , 
                           StatusCode::FAILURE          ) ; }
  
  //
  m_iterator = m_phis.createIterator() ;
  setPars () ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::Spline2D::Spline2D
( const Analysis::Models::Spline2D&  right ,      
  const char*                              name  ) 
  : RooAbsPdf  ( right , name ) 
//
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_y        ( "y"      , this , right.m_y     ) 
  , m_phis     ( "phis"   , this , right.m_phis  ) 
//
  , m_iterator ( 0 ) 
//
  , m_spline   ( right.m_spline ) 
{
  m_iterator = m_phis.createIterator () ;
  setPars () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::Spline2D::~Spline2D() 
{ if ( 0 != m_iterator ) { delete m_iterator ; } }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Spline2D*
Analysis::Models::Spline2D::clone( const char* name ) const 
{ return new Analysis::Models::Spline2D(*this,name) ; }
// ============================================================================
void Analysis::Models::Spline2D::setPars () const 
{
  //
  m_iterator->Reset () ;
  //
  RooAbsArg*       phi   = 0 ;
  const RooArgSet* nset  = m_phis.nset() ;
  //
  unsigned short k = 0 ;
  while ( ( phi = (RooAbsArg*) m_iterator->Next() ) )
  {
    const RooAbsReal* r = dynamic_cast<RooAbsReal*> ( phi ) ;
    if ( 0 == r ) { continue ; }
    //
    const double phiv   = r->getVal ( nset ) ;
    //
    m_spline.setPar ( k  , phiv ) ;
    //
    ++k ;
  }
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Spline2D::evaluate() const 
{
  //
  setPars () ;
  //
  return m_spline ( m_x , m_y ) ; 
}
// ============================================================================
Int_t Analysis::Models::Spline2D::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if      ( matchArgs ( allVars , analVars , m_x , m_y ) ) { return 1 ; }
  else if ( matchArgs ( allVars , analVars , m_x       ) ) { return 2 ; }
  else if ( matchArgs ( allVars , analVars       , m_y ) ) { return 3 ; }
  //
  return 0 ;
}
//_____________________________________________________________________________
Double_t Analysis::Models::Spline2D::analyticalIntegral 
( Int_t       code       , 
  const char* rangeName  ) const 
{
  //
  assert ( 1 == code || 2 == code || 3 == code ) ;
  //
  setPars () ;
  //
  return 
    1 == code ? m_spline.integral   (        m_x.min(rangeName) , m_x.max(rangeName) , 
                                             m_y.min(rangeName) , m_y.max(rangeName) ) : 
    2 == code ? m_spline.integrateX ( m_y  , m_x.min(rangeName) , m_x.max(rangeName) ) : 
    3 == code ? m_spline.integrateY ( m_x  , m_y.min(rangeName) , m_y.max(rangeName) ) : 0.0 ;  
}

// ============================================================================
// generic 2D symmetric spline
// ============================================================================
Analysis::Models::Spline2DSym::Spline2DSym
( const char*          name      , 
  const char*          title     ,
  RooRealVar&          x         ,
  RooRealVar&          y         ,
  const Gaudi::Math::Spline2DSym& spline, 
  RooArgList&          phis      ) 
  : RooAbsPdf  ( name , title ) 
  , m_x        ( "x"       , "Observable-X" , this , x ) 
  , m_y        ( "y"       , "Observable-Y" , this , y ) 
  , m_phis     ( "phis"    , "Coefficients" , this     )
//
  , m_iterator ( 0 ) 
    //
  , m_spline   ( spline ) 
{
  //
  TIterator*   tmp  = phis.createIterator() ;
  RooAbsArg*   coef = 0 ;
  unsigned int num  = 0 ;
  while ( ( coef = (RooAbsArg*) tmp->Next() ) && num < m_spline.npars() )
  {
    RooAbsReal* r = dynamic_cast<RooAbsReal*> ( coef ) ;
    if ( 0 == r ) { continue ; }
    m_phis.add ( *coef ) ;
    ++num ;  
  }
  delete tmp ;
  //
  if ( num != m_spline.npars() ) 
  { throw GaudiException ( "Invalid size of parameters vector", 
                           "Analysis::Spline2DSym"      , 
                           StatusCode::FAILURE          ) ; }
  
  //
  m_iterator = m_phis.createIterator() ;
  setPars () ;
}
// ============================================================================
// copy constructor
// ============================================================================
Analysis::Models::Spline2DSym::Spline2DSym
( const Analysis::Models::Spline2DSym&  right ,      
  const char*                              name  ) 
  : RooAbsPdf  ( right , name ) 
//
  , m_x        ( "x"      , this , right.m_x     ) 
  , m_y        ( "y"      , this , right.m_y     ) 
  , m_phis     ( "phis"   , this , right.m_phis  ) 
//
  , m_iterator ( 0 ) 
//
  , m_spline   ( right.m_spline ) 
{
  m_iterator = m_phis.createIterator () ;
  setPars () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Models::Spline2DSym::~Spline2DSym() 
{ if ( 0 != m_iterator ) { delete m_iterator ; } }
// ============================================================================
// clone 
// ============================================================================
Analysis::Models::Spline2DSym*
Analysis::Models::Spline2DSym::clone( const char* name ) const 
{ return new Analysis::Models::Spline2DSym(*this,name) ; }
// ============================================================================
void Analysis::Models::Spline2DSym::setPars () const 
{
  //
  m_iterator->Reset () ;
  //
  RooAbsArg*       phi   = 0 ;
  const RooArgSet* nset  = m_phis.nset() ;
  //
  unsigned short k = 0 ;
  while ( ( phi = (RooAbsArg*) m_iterator->Next() ) )
  {
    const RooAbsReal* r = dynamic_cast<RooAbsReal*> ( phi ) ;
    if ( 0 == r ) { continue ; }
    //
    const double phiv   = r->getVal ( nset ) ;
    //
    m_spline.setPar ( k  , phiv ) ;
    //
    ++k ;
  }
  //
}
// ============================================================================
// the actual evaluation of function 
// ============================================================================
Double_t Analysis::Models::Spline2DSym::evaluate() const 
{
  //
  setPars () ;
  //
  return m_spline ( m_x , m_y ) ; 
}
// ============================================================================
Int_t Analysis::Models::Spline2DSym::getAnalyticalIntegral
( RooArgSet&     allVars      , 
  RooArgSet&     analVars     ,
  const char* /* rangename */ ) const 
{
  if      ( matchArgs ( allVars , analVars , m_x , m_y ) ) { return 1 ; }
  else if ( matchArgs ( allVars , analVars , m_x       ) ) { return 2 ; }
  else if ( matchArgs ( allVars , analVars       , m_y ) ) { return 3 ; }
  //
  return 0 ;
}
//_____________________________________________________________________________
Double_t Analysis::Models::Spline2DSym::analyticalIntegral 
( Int_t       code       , 
  const char* rangeName  ) const 
{
  //
  assert ( 1 == code || 2 == code || 3 == code ) ;
  //
  setPars () ;
  //
  return 
    1 == code ? m_spline.integral   (        m_x.min(rangeName) , m_x.max(rangeName) , 
                                             m_y.min(rangeName) , m_y.max(rangeName) ) : 
    2 == code ? m_spline.integrateX ( m_y  , m_x.min(rangeName) , m_x.max(rangeName) ) : 
    3 == code ? m_spline.integrateY ( m_x  , m_y.min(rangeName) , m_y.max(rangeName) ) : 0.0 ;  
}

// ============================================================================
// The END 
// ============================================================================
