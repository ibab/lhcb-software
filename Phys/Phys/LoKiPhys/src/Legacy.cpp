// $Id$
// ============================================================================
// Include files 
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Legacy.h"
#include "LoKi/Constants.h"
#include "LoKi/Child.h"
#include "LoKi/Constants.h"
#include "LoKi/Child.h"
#include "LoKi/Particles34.h"
#include "LoKi/GetTools.h"
// ============================================================================
/** @file 
 *  
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design. 
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-04-30
 */
// ============================================================================
// Default Constructor
// ============================================================================
LoKi::Legacy::DHCoplanarity::DHCoplanarity()
  : LoKi::AuxDesktopBase () 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
{ }
// ============================================================================
// MANDATORY : virtual destrcutor 
// ============================================================================
LoKi::Legacy::DHCoplanarity::~DHCoplanarity(){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::Legacy::DHCoplanarity*
LoKi::Legacy::DHCoplanarity::clone() const 
{ return new DHCoplanarity ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Legacy::DHCoplanarity::result_type 
LoKi::Legacy::DHCoplanarity::operator() 
  ( LoKi::Legacy::DHCoplanarity::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle points to NULL, return InvalidDistance") ;
    return LoKi::Constants::InvalidDistance ;
  }
  //
  // get the best primary vertex
  const LHCb::VertexBase* pv = bestVertex ( p ) ;
  if ( 0 == pv ) 
  {
    Error ("Best primary Vertex points to NULL, return InvalidDistance") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // 
  // get the first daughter 
  const LHCb::Particle* first = LoKi::Child::child ( p , 1 ) ;
  if ( 0 == first ) 
  {
    Error ("The first daughter points to NULL, return InvalidDistance") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // get the vertex for the first daughter  
  const LHCb::VertexBase* vx = first->endVertex() ;
  if ( 0 == vx ) 
  {
    Error ("The vertex for the first daughter points to NULL, return InvalidDistance") ;
    return LoKi::Constants::InvalidDistance ;
  }
  /// get the second daughter:
  const LHCb::Particle* second = LoKi::Child::child ( p , 2 ) ;
  if ( 0 == second ) 
  {
    Error ("The second daughter points to NULL, return InvalidDistance") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // get the lines
  typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> Line_ ; 
  
  // the line from PV to D0-vertex 
  const Line_ line1 ( pv -> position () , 
                      vx -> position () ) ;
  
  // the line along the second daughter particle 
  const Line_ line2 ( second -> referencePoint()   , 
                      second -> momentum().Vect()  ) ;
  
  // finally evaluate the distance between the lines:
  return Gaudi::Math::distance ( line1 , line2 ) ;
}
// ============================================================================
// OPTIONAL: nice printot
// ============================================================================
std::ostream& 
LoKi::Legacy::DHCoplanarity::fillStream ( std::ostream& s ) const 
{ return s << "BPVDVDOCA" ; }

// ============================================================================
// ============================================================================

// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Legacy::DaughterParticleCTau::DaughterParticleCTau 
( const unsigned short   index  , 
  const ILifetimeFitter* fitter ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_index  ( index  ) 
  , m_fitter ( fitter ) 
{
  if ( !m_fitter ) 
  { m_fitter = LoKi::GetTools::lifetimeFitter ( *this ) ; }
  Assert ( !(!m_fitter) , "Unable to get ILifetimeFitter" ) ;
}
// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Legacy::DaughterParticleCTau::DaughterParticleCTau 
( const unsigned short                     index  , 
  const LoKi::Interface<ILifetimeFitter>&  fitter ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_index  ( index  ) 
  , m_fitter ( fitter ) 
{
  if ( !m_fitter ) 
  { m_fitter = LoKi::GetTools::lifetimeFitter ( *this ) ; }
  Assert ( !(!m_fitter), "Unable to get ILifetimeFitter" ) ;
}
// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Legacy::DaughterParticleCTau::DaughterParticleCTau 
( const unsigned short  index  , 
  const std::string&    fitter ) 
  : LoKi::AuxFunBase( std::tie ( index , fitter ) )
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_index  ( index   ) 
  , m_fitter ( nullptr ) 
{
  if ( !m_fitter ) 
  { m_fitter = LoKi::GetTools::lifetimeFitter ( *this , fitter ) ; }
  Assert ( !(!m_fitter) , 
           "Unable to get ILifetimeFitter '" + fitter + "'") ;
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Legacy::DaughterParticleCTau::~DaughterParticleCTau()
{
  if ( m_fitter && !gaudi() ) 
  {
    // Warning("ILifetimeFitter: manual reset");
    m_fitter.reset() ;
  }      
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Legacy::DaughterParticleCTau*
LoKi::Legacy::DaughterParticleCTau::clone() const 
{ return new DaughterParticleCTau ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Legacy::DaughterParticleCTau::result_type
LoKi::Legacy::DaughterParticleCTau::operator()
  ( LoKi::Legacy::DaughterParticleCTau::argument p ) const 
{ return ctau ( p ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Legacy::DaughterParticleCTau::fillStream 
( std::ostream& s ) const
{ return s << " DPCTAU( " << index() << " ) " ; }
// ============================================================================
// get results from the fit 
// ============================================================================
StatusCode LoKi::Legacy::DaughterParticleCTau::fit 
( const LHCb::Particle* p           , 
  double&               ctau        , 
  double&               ctauerr     , 
  double&               ctaufitchi2 ) const 
{
  if ( 0 == p ) 
  { return Error ("LHCb::Particle* points to NULL") ; }
  // get the vertex 
  const LHCb::VertexBase* v = p->endVertex() ;
  if ( 0 == v ) 
  { return Error ("LHCb::VertexBase* points to NULL") ; }
  // get the daughter
  const LHCb::Particle* c = LoKi::Child::child ( p , m_index ) ;
  if ( 0 == c ) 
  { return Error ("Child points to NULL, return InvalidDistance") ; }
  // check  the tool:
  if ( !m_fitter ) 
  { m_fitter = LoKi::GetTools::lifetimeFitter ( *this ) ; }
  Assert ( !(!m_fitter) , "Invalid ILifetimeFitter") ;
  // use the tool 
  //
  double ltime        = LoKi::Constants::InvalidTime ;
  double ltimeerr     = LoKi::Constants::InvalidTime ;
  double ltimefitchi2 = LoKi::Constants::InvalidChi2 ;
  //
  StatusCode sc = m_fitter -> fit ( *v , *c , ltime , ltimeerr , ltimefitchi2 ) ;
  if ( sc.isFailure() ) 
  { return Warning ( "Error from ILifetimeFitter::fit", sc, 3 ) ; }
  //
  ctau         = ltime    * Gaudi::Units::c_light ;
  ctauerr      = ltimeerr * Gaudi::Units::c_light ;
  ctaufitchi2  = ltimefitchi2 ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// get ctau
// ============================================================================
double LoKi::Legacy::DaughterParticleCTau::ctau  
( const LHCb::Particle* p ) const 
{
  double _ctau        = LoKi::Constants::InvalidDistance ;
  double _ctauerr     = LoKi::Constants::InvalidDistance ;
  double _ctaufitchi2 = LoKi::Constants::InvalidChi2 ;
  //
  StatusCode sc = fit ( p , _ctau , _ctauerr , _ctaufitchi2 ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from fit, return InvalidDistance", sc, 3 ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  return  _ctau ;
}
// ============================================================================
// get ctau error 
// ============================================================================
double LoKi::Legacy::DaughterParticleCTau::ctauErr  
( const LHCb::Particle* p ) const 
{
  double _ctau        = LoKi::Constants::InvalidDistance ;
  double _ctauerr     = LoKi::Constants::InvalidDistance ;
  double _ctaufitchi2 = LoKi::Constants::InvalidChi2 ;
  //
  StatusCode sc = fit ( p , _ctau , _ctauerr , _ctaufitchi2 ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from fit, return InvalidDistance", sc, 3 ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  return  _ctauerr ;
}
// ============================================================================
// get ctau fit chi2 
// ============================================================================
double LoKi::Legacy::DaughterParticleCTau::ctauFitChi2 
( const LHCb::Particle* p ) const 
{
  double _ctau        = LoKi::Constants::InvalidDistance ;
  double _ctauerr     = LoKi::Constants::InvalidDistance ;
  double _ctaufitchi2 = LoKi::Constants::InvalidChi2 ;
  //
  StatusCode sc = fit ( p , _ctau , _ctauerr , _ctaufitchi2 ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from fit, return InvalidChi2", sc, 3 ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  return  _ctaufitchi2  ;
}
// ============================================================================
// get ctau significance
// ============================================================================
double LoKi::Legacy::DaughterParticleCTau::ctauSignificance
( const LHCb::Particle* p ) const 
{
  double _ctau        = LoKi::Constants::InvalidDistance ;
  double _ctauerr     = LoKi::Constants::InvalidDistance ;
  double _ctaufitchi2 = LoKi::Constants::InvalidChi2 ;
  //
  StatusCode sc = fit ( p , _ctau , _ctauerr , _ctaufitchi2 ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from fit, return NegativeInfinity", sc, 3 ) ;
    return LoKi::Constants::NegativeInfinity ;
  }
  return  _ctau / _ctauerr ;
}

// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Legacy::DaughterParticleCTauErr::DaughterParticleCTauErr
( const unsigned short   index  , 
  const ILifetimeFitter* fitter ) 
  : LoKi::Legacy::DaughterParticleCTau ( index , fitter ) 
{
}// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Legacy::DaughterParticleCTauErr::DaughterParticleCTauErr
( const unsigned short   index  , 
  const LoKi::Interface<ILifetimeFitter>&  fitter ) 
  : LoKi::Legacy::DaughterParticleCTau ( index , fitter ) 
{}
// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Legacy::DaughterParticleCTauErr::DaughterParticleCTauErr
( const unsigned short   index  , 
  const std::string&     fitter ) 
  : LoKi::AuxFunBase( std::tie ( index , fitter ) )
  , LoKi::Legacy::DaughterParticleCTau ( index , fitter ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Legacy::DaughterParticleCTauErr::~DaughterParticleCTauErr () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Legacy::DaughterParticleCTauErr*
LoKi::Legacy::DaughterParticleCTauErr::clone() const 
{ return new DaughterParticleCTauErr( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Legacy::DaughterParticleCTauErr::result_type
LoKi::Legacy::DaughterParticleCTauErr::operator()
  ( LoKi::Legacy::DaughterParticleCTauErr::argument p ) const 
{ return ctauErr( p ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Legacy::DaughterParticleCTauErr::fillStream 
( std::ostream& s ) const
{ return s << " DPCTAUERR( " << index() << " ) " ; }


// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Legacy::DaughterParticleCTauFitChi2::DaughterParticleCTauFitChi2
( const unsigned short   index  , 
  const ILifetimeFitter* fitter ) 
  : LoKi::Legacy::DaughterParticleCTau ( index , fitter ) 
{
}// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Legacy::DaughterParticleCTauFitChi2::DaughterParticleCTauFitChi2
( const unsigned short   index  , 
  const LoKi::Interface<ILifetimeFitter>&  fitter ) 
  : LoKi::Legacy::DaughterParticleCTau ( index , fitter ) 
{}
// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Legacy::DaughterParticleCTauFitChi2::DaughterParticleCTauFitChi2
( const unsigned short   index  , 
  const std::string&     fitter ) 
  : LoKi::AuxFunBase( std::tie ( index , fitter ) )
  , LoKi::Legacy::DaughterParticleCTau ( index , fitter ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Legacy::DaughterParticleCTauFitChi2::~DaughterParticleCTauFitChi2 () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Legacy::DaughterParticleCTauFitChi2*
LoKi::Legacy::DaughterParticleCTauFitChi2::clone() const 
{ return new DaughterParticleCTauFitChi2( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Legacy::DaughterParticleCTauFitChi2::result_type
LoKi::Legacy::DaughterParticleCTauFitChi2::operator()
  ( LoKi::Legacy::DaughterParticleCTauFitChi2::argument p ) const 
{ return ctauFitChi2( p ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Legacy::DaughterParticleCTauFitChi2::fillStream 
( std::ostream& s ) const
{ return s << " DPCTAUFITCHI2( " << index() << " ) " ; }


// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Legacy::DaughterParticleCTauSignificance::DaughterParticleCTauSignificance
( const unsigned short   index  , 
  const ILifetimeFitter* fitter ) 
  : LoKi::Legacy::DaughterParticleCTau ( index , fitter ) 
{
}// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Legacy::DaughterParticleCTauSignificance::DaughterParticleCTauSignificance
( const unsigned short   index  , 
  const LoKi::Interface<ILifetimeFitter>&  fitter ) 
  : LoKi::Legacy::DaughterParticleCTau ( index , fitter ) 
{}
// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Legacy::DaughterParticleCTauSignificance::DaughterParticleCTauSignificance
( const unsigned short   index  , 
  const std::string&     fitter ) 
  : LoKi::Legacy::DaughterParticleCTau ( index , fitter ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Legacy::DaughterParticleCTauSignificance::~DaughterParticleCTauSignificance () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Legacy::DaughterParticleCTauSignificance*
LoKi::Legacy::DaughterParticleCTauSignificance::clone() const 
{ return new DaughterParticleCTauSignificance( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Legacy::DaughterParticleCTauSignificance::result_type
LoKi::Legacy::DaughterParticleCTauSignificance::operator()
  ( LoKi::Legacy::DaughterParticleCTauSignificance::argument p ) const 
{ return ctauSignificance( p ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Legacy::DaughterParticleCTauSignificance::fillStream 
( std::ostream& s ) const
{ return s << " DPCTAUSIGNIFICANCE( " << index() << " ) " ; }



// ============================================================================
// The END 
// ============================================================================
