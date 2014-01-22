// $Id$
// ============================================================================
// Include files 
// ============================================================================
// local
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/MultiTracks.h"
// ============================================================================
/** @file  
 *  Couple of functions to deal with multi-tracks
 *  @see Hlt::MultiTrack
 *
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-03-27
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
// ============================================================================
// constructor form the functor, index  and bad-value 
// ============================================================================
LoKi::Candidates::MtFun::MtFun 
( const LoKi::BasicFunctors<const LHCb::Track*>::Function& fun   ,
  const unsigned int                                       index , 
  const double                                             bad   ) 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Function () 
  , m_fun   { fun   } 
  , m_index { index } 
  , m_bad   { bad   }
{}
// ============================================================================
// constructor form the functor, index  
// ============================================================================
LoKi::Candidates::MtFun::MtFun 
( const LoKi::BasicFunctors<const LHCb::Track*>::Function& fun   ,
  unsigned int                                             index )
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Function () 
  , m_fun   { fun   }
  , m_index { index }
  , m_bad   { LoKi::Constants::NegativeInfinity }
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Candidates::MtFun::~MtFun () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Candidates::MtFun*
LoKi::Candidates::MtFun::clone() const 
{ return new LoKi::Candidates::MtFun ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================ 
LoKi::Candidates::MtFun::result_type 
LoKi::Candidates::MtFun::operator()
  ( LoKi::Candidates::MtFun::argument t ) const 
{
  // 
  if ( !t ) {
    Error("Hlt::Candidate points to NULL, return 'bad") ;
    return m_bad ;
  }
  //
  auto mt = t->get<Hlt::MultiTrack> () ;
  //
  if ( !mt ) {
    Error ( "Hlt::Candidate is not Hlt::MultiTrack, return 'bad" ) ;
    return m_bad ;
  }
  //
  if ( mt->tracks().size() < m_index + 1 ) {
    Error ( "Hlt::MultiTrack, invalid tarck-index, return 'bad" ) ;
    return m_bad ;
  }  
  //
  return m_fun ( mt->tracks()[m_index] ) ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Candidates::MtFun::fillStream ( std::ostream& s ) const 
{
  s << "TC_MtFUN (" << m_fun << "," << m_index ;
  if ( LoKi::Constants::NegativeInfinity != m_bad ) { s << "," << m_bad ; }
  return s << ")" ;
}
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
