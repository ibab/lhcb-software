// $Id$
// ============================================================================
// Include files 
// ============================================================================
#include <numeric>
// ============================================================================
// Event 
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/RecVertices.h"
// ============================================================================
// KalmanFilter 
// ============================================================================
#include "KalmanFilter/FastVertex.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltUtils.h"
// ============================================================================
/** @file  
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08-01
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::RecVertices::MaxDOCA::~MaxDOCA () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::RecVertices::MaxDOCA*
LoKi::RecVertices::MaxDOCA::clone() const 
{ return new LoKi::RecVertices::MaxDOCA { *this } ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::RecVertices::MaxDOCA::result_type 
LoKi::RecVertices::MaxDOCA::operator() 
  ( LoKi::RecVertices::MaxDOCA::argument v ) const 
{
  if ( !v ) {
    Error ("LHCb::VertexBase* points to NULL, return PositiveInfinity");
    return LoKi::Constants::PositiveInfinity ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( !rv ) {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return PositiveInfinity");
    return LoKi::Constants::PositiveInfinity ;
  }
  //
  const auto& tracks = rv->tracks() ;
  //
  double docamax  = LoKi::Constants::NegativeInfinity ;
  for ( auto i = tracks.begin() ; tracks.end() != i ; ++i ) 
  {
    auto oper = [&](double x, const SmartRef<LHCb::Track>& j) 
                { return std::max( x, doca(*i,j) ); } ;
    docamax = std::accumulate( i+1, std::end(tracks), docamax, oper );
  }
  // 
  return docamax ;
}
// ============================================================================
// evaluate DOCA for two tracks
// ============================================================================
double LoKi::RecVertices::MaxDOCA::doca
( const LHCb::Track* t1 , 
  const LHCb::Track* t2 ) const 
{
  //
  if ( !t1 || !t2 ) { return LoKi::Constants::PositiveInfinity ; }
  if ( t1 == t2   ) { return 0 ; }
  //
  double _doca  = LoKi::Constants::NegativeInfinity ;
  //
  StatusCode sc = LoKi::FastVertex::distance ( t1 , t2 , _doca , true ) ;
  if ( sc.isFailure() ) {  return LoKi::Constants::PositiveInfinity ;  }
  //
  return _doca ; 
}
// ============================================================================
// evaluate chi2(DOCA) for two tracks
// ============================================================================
double LoKi::RecVertices::MaxDOCA::docachi2 
( const LHCb::Track* t1 , 
  const LHCb::Track* t2 ) const 
{
  //
  if ( !t1 || !t2 ) { return LoKi::Constants::PositiveInfinity ; }
  if ( t1 == t2   ) { return 0 ; }
  //
  double _doca = LoKi::Constants::NegativeInfinity ;
  double _chi2 = LoKi::Constants::NegativeInfinity ;
  //
  StatusCode sc = 
    LoKi::FastVertex::distance ( t1 , t2 , _doca , _chi2 , true ) ;
  if ( sc.isFailure() ) {  return LoKi::Constants::PositiveInfinity ;  }
  //
  //
  return _chi2 ; 
}
// ============================================================================
// evaluate DOCA for two tracks
// ============================================================================
StatusCode LoKi::RecVertices::MaxDOCA::distance 
( const LHCb::Track* t1   , 
  const LHCb::Track* t2   , 
  double&            doca , 
  double&            chi2 ) const
{
  //
  if ( !t1 || !t2 ) { return StatusCode::FAILURE ;  }
  //
  if ( t1 == t2 ) 
  { 
    doca = 0 ;
    chi2 = 0 ;
    return StatusCode::SUCCESS ;
  }
  // 
  return LoKi::FastVertex::distance ( t1 , t2 , doca , chi2 , true ) ;
  //
}
// ======================================================================
// check DOCA & chi2(DOCA) for two tracks
bool LoKi::RecVertices::MaxDOCA::check
( const LHCb::Track* t1      , 
  const LHCb::Track* t2      , 
  double             docamax , 
  double             chi2max ) const 
{
  //
  if ( !t1 || !t2 ) { return false ; }
  if ( t1 == t2   ) { return true  ; }
  //
  return 
    LoKi::FastVertex::checkDistance ( t1 , t2 , docamax , chi2max ) ;
}


// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream&
LoKi::RecVertices::MaxDOCA::fillStream ( std::ostream& s ) const 
{ return s << "RV_MAXDOCA" ; }

// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::RecVertices::MaxDOCAChi2::~MaxDOCAChi2 () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::RecVertices::MaxDOCAChi2*
LoKi::RecVertices::MaxDOCAChi2::clone() const 
{ return new LoKi::RecVertices::MaxDOCAChi2 { *this } ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::RecVertices::MaxDOCAChi2::result_type 
LoKi::RecVertices::MaxDOCAChi2::operator() 
  ( LoKi::RecVertices::MaxDOCAChi2::argument v ) const 
{
  if ( !v ) 
  {
    Error ("LHCb::VertexBase* points to NULL, return PositiveInfinity");
    return LoKi::Constants::PositiveInfinity ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( !rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return PositiveInfinity");
    return LoKi::Constants::PositiveInfinity ;
  }
  //
  const auto& tracks = rv->tracks() ;
  //
  double chi2max  = LoKi::Constants::NegativeInfinity ;
  for ( auto  i = tracks.begin() ; tracks.end() != i ; ++i ) 
  {
    auto oper = [&](double x, const SmartRef<LHCb::Track>& j) 
                { return std::max( x, docachi2(*i,j) ); } ;
    chi2max  = std::accumulate( i+1, std::end(tracks), chi2max, oper );
  }
  // 
  return chi2max ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream&
LoKi::RecVertices::MaxDOCAChi2::fillStream ( std::ostream& s ) const 
{ return s << "RV_MAXDOCACHI2" ; }


// ============================================================================
/*  constructor with cuts on max(DOCA) and max(chi2(DOCA))
 *  @param docamax  max(DOCA)       (negative: no cut)
 *  @param docamax  max(chi2(DOCA)) (negative: no cut)
 */
// ============================================================================
LoKi::RecVertices::DOCACut::DOCACut
( double docamax ,   // max(DOCA)       (negative: no cut)
  double chi2max )  // max(chi2(DOCA)) (negative: no cut)
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate () 
  , m_docamax ( docamax )
  , m_chi2max ( chi2max )
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::RecVertices::DOCACut::~DOCACut () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::RecVertices::DOCACut*
LoKi::RecVertices::DOCACut::clone() const 
{ return new LoKi::RecVertices::DOCACut { *this } ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::RecVertices::DOCACut::result_type 
LoKi::RecVertices::DOCACut::operator() 
  ( LoKi::RecVertices::DOCACut::argument v ) const 
{
  if ( !v ) 
  {
    Error ("LHCb::VertexBase* points to NULL, return false");
    return false ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( !rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return false");
    return false ;
  }
  //
  //
  const auto& tracks = rv->tracks() ;
  if ( tracks.empty() )        { return false ; }          // RETURN
  //
  for ( auto i = tracks.begin() ; tracks.end() != i ; ++i ) 
  {
    for ( auto j = i + 1 ; tracks.end() != j ; ++j ) 
    { if ( !check ( *i , *j ) ) { return false ; } }      // RETURN 
  }
  // 
  return true ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream&
LoKi::RecVertices::DOCACut::fillStream ( std::ostream& s ) const 
{ return s << "RV_DOCACUT(" << m_docamax << "," << m_chi2max << ")" ; }


// ============================================================================
// The END 
// ============================================================================
