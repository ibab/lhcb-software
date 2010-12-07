// $Id$
// ============================================================================
// Include files 
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
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
{ return new LoKi::RecVertices::MaxDOCA ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::RecVertices::MaxDOCA::result_type 
LoKi::RecVertices::MaxDOCA::operator() 
  ( LoKi::RecVertices::MaxDOCA::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ("LHCb::VertexBase* points to NULL, return PositiveInfinity");
    return LoKi::Constants::PositiveInfinity ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( 0 == rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return PositiveInfinity");
    return LoKi::Constants::PositiveInfinity ;
  }
  //
  typedef SmartRefVector<LHCb::Track> TRACKS   ;
  typedef TRACKS::const_iterator      ITERATOR ;
  //
  const TRACKS& tracks = rv->tracks() ;
  //
  double docamax  = LoKi::Constants::NegativeInfinity ;
  for ( ITERATOR i = tracks.begin() ; tracks.end() != i ; ++i ) 
  {
    for ( ITERATOR j = i + 1 ; tracks.end() != j ; ++j ) 
    { docamax = std::max ( docamax , doca ( *i , *j ) ) ; }
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
  if ( 0  == t1 || 0 == t2 ) { return LoKi::Constants::PositiveInfinity ; }
  if ( t1 == t2            ) { return 0 ; }
  //
  double _doca  = LoKi::Constants::NegativeInfinity ;
  //
  StatusCode sc = 
    LoKi::FastVertex::distance ( t1 , t2 , _doca , true ) ;
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
  if ( 0  == t1 || 0 == t2 ) { return LoKi::Constants::PositiveInfinity ; }
  if ( t1 == t2            ) { return 0 ; }
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
  if ( 0  == t1 || 0 == t2 ) { return StatusCode::FAILURE ;  }
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
  const double       docamax , 
  const double       chi2max ) const 
{
  //
  if ( 0  == t1 || 0 == t2 ) { return false ; }
  if ( t1 == t2 )            { return true  ; }
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
{ return new LoKi::RecVertices::MaxDOCAChi2 ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::RecVertices::MaxDOCAChi2::result_type 
LoKi::RecVertices::MaxDOCAChi2::operator() 
  ( LoKi::RecVertices::MaxDOCAChi2::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ("LHCb::VertexBase* points to NULL, return PositiveInfinity");
    return LoKi::Constants::PositiveInfinity ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( 0 == rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return PositiveInfinity");
    return LoKi::Constants::PositiveInfinity ;
  }
  //
  typedef SmartRefVector<LHCb::Track> TRACKS   ;
  typedef TRACKS::const_iterator      ITERATOR ;
  //
  const TRACKS& tracks = rv->tracks() ;
  //
  double chi2max  = LoKi::Constants::NegativeInfinity ;
  for ( ITERATOR i = tracks.begin() ; tracks.end() != i ; ++i ) 
  {
    for ( ITERATOR j = i + 1 ; tracks.end() != j ; ++j ) 
    { chi2max = std::max ( chi2max , docachi2 ( *i , *j ) ) ; }
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
( const double docamax ,   // max(DOCA)       (negative: no cut)
  const double chi2max )  // max(chi2(DOCA)) (negative: no cut)
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
{ return new LoKi::RecVertices::DOCACut ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::RecVertices::DOCACut::result_type 
LoKi::RecVertices::DOCACut::operator() 
  ( LoKi::RecVertices::DOCACut::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ("LHCb::VertexBase* points to NULL, return false");
    return false ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( 0 == rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return false");
    return false ;
  }
  //
  typedef SmartRefVector<LHCb::Track> TRACKS   ;
  typedef TRACKS::const_iterator      ITERATOR ;
  //
  const TRACKS& tracks = rv->tracks() ;
  if ( tracks.empty() )        { return false ; }          // RETURN
  //
  for ( ITERATOR i = tracks.begin() ; tracks.end() != i ; ++i ) 
  {
    for ( ITERATOR j = i + 1 ; tracks.end() != j ; ++j ) 
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
