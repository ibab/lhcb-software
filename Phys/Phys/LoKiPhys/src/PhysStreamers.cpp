// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
#include <algorithm>
#include <functional>
// ============================================================================
// Local
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
#include "LoKi/PhysExtract.h"
#include "LoKi/apply.h"
#include "LoKi/Streamers.h"
// ============================================================================
/** @file 
 *
 *  implementation of some streamers 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-05-24 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  template <class PARTICLE>
  inline
  LHCb::Particle::ConstVector
  _get_ ( PARTICLE                 first , 
          PARTICLE                 last  , 
          const LoKi::Types::Cuts& cuts  )
  {
    //
    LHCb::Particle::ConstVector r ; r.reserve ( last - first  ) ;
    // 
    LoKi::apply_filter ( first , last , cuts , std::back_inserter ( r ) ) ;
    //
    return r ;
  }
  // ==========================================================================
  template <class PARTICLES>
  inline
  LHCb::Particle::ConstVector
  _get_ ( const PARTICLES*         parts , 
          const LoKi::Types::Cuts& cuts  )
  {
    //
    LHCb::Particle::ConstVector r ; 
    if ( !parts || parts->empty()  ) { return r ; }  // RETURN 
    //
    r.reserve ( parts->size() ) ;
    // 
    LoKi::apply_filter ( parts -> begin ()  , 
                         parts -> end   ()  , cuts , std::back_inserter ( r ) ) ;
    //
    return r ;
  }
  // ==========================================================================  
  template <class PARTICLE, class TYPE>
  inline
  std::vector<double>
  _get_ ( PARTICLE                          first , 
          PARTICLE                          last  , 
          const LoKi::Functor<TYPE,double>& func  )
  {
    //
    std::vector<double> r ; r.reserve ( last - first  ) ;
    // 
    LoKi::apply ( first , last , func , std::back_inserter ( r ) ) ;
    //
    return r ;
  }
  // ==========================================================================
  template <class PARTICLES, class TYPE>
  inline
  std::vector<double>
  _get_ ( const PARTICLES*                  parts , 
          const LoKi::Functor<TYPE,double>& func  )
  {
    //
    std::vector<double> r ;
    if ( !parts || parts->empty()  ) { return r ; }  // RETURN 
    //
    r.reserve ( parts->size() ) ;
    // 
    LoKi::apply ( parts -> begin ()  , 
                  parts -> end   ()  , func , std::back_inserter ( r ) ) ;
    //
    return r ;
  }
  // ==========================================================================  
  class _VRTX_ : 
    public std::unary_function<const LHCb::Particle*,bool>
  {
  public:
    // ========================================================================
    _VRTX_ ( const LoKi::Types::VCuts& cuts ) : m_cuts ( cuts ) {}
    // ========================================================================
    inline bool operator() ( const LHCb::Particle* p ) const 
    {
      static const LHCb::VertexBase* s_v ;
      return p ? m_cuts( p->endVertex() ) : m_cuts ( s_v ) ;
    }
    // ========================================================================
  private:
    // ========================================================================
    _VRTX_ () ;
    _VRTX_            ( const _VRTX_& ) ;
    _VRTX_& operator= ( const _VRTX_& ) ;
    // ========================================================================
  private:
    // ========================================================================
    const LoKi::Types::VCuts& m_cuts  ;
    // ========================================================================    
  } ;
  // ==========================================================================
  template <class PARTICLE>
  inline
  LHCb::Particle::ConstVector
  _wget_ ( PARTICLE                  first , 
           PARTICLE                  last  , 
           const LoKi::Types::VCuts& cuts  )
  {
    //
    LHCb::Particle::ConstVector r ; r.reserve ( last - first  ) ;
    // 
    _VRTX_ vx{ cuts };
    std::copy_if ( first , last , std::back_inserter ( r ) , std::cref(vx) ) ;
    //
    return r ;
  }
  // ==========================================================================
  template <class PARTICLES>
  inline
  LHCb::Particle::ConstVector
  _wget_ ( const PARTICLES*          parts ,
           const LoKi::Types::VCuts& cuts  )
  {
    LHCb::Particle::ConstVector r ; 
    if ( parts ) {
      r.reserve ( parts -> size()  ) ;
      _VRTX_ vx ( cuts ) ;
      std::copy_if ( parts -> begin () ,
                     parts -> end   () , std::back_inserter ( r ) , std::cref(vx) ) ;
    }
    return r ;
  }
  // ==========================================================================
}
// ============================================================================
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::ConstVector&    input , 
  const LoKi::Types::Cuts&              cuts  ) 
{ return _get_ ( input.begin() , input.end() , cuts ) ; } 
// ============================================================================
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::Range&          input , 
  const LoKi::Types::Cuts&              cuts  ) 
{ return _get_ ( input.begin() , input.end() , cuts ) ; } 
// ============================================================================
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::Container*      input , 
  const LoKi::Types::Cuts&              cuts  ) 
{ return _get_ ( input , cuts ) ; } 
// ============================================================================
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::Selection*      input , 
  const LoKi::Types::Cuts&              cuts  ) 
{ return _get_ ( input , cuts ) ; } 
// ============================================================================
LHCb::Particle::ConstVector
operator>> 
( const SmartRefVector<LHCb::Particle>& input , 
  const LoKi::Types::Cuts&              cuts  ) 
{ return _get_ ( input.begin() , input.end() , cuts ) ; } 
// ============================================================================
// Vertices 
// ============================================================================
LHCb::VertexBase::ConstVector
operator>> 
( const LoKi::Types::VRange&            input , 
  const LoKi::Types::VCuts&             cuts  ) 
{
  LHCb::VertexBase::ConstVector r ; 
  r.reserve ( input.size() ) ;
  LoKi::apply_filter ( input . begin ()  , 
                       input . end   ()  , cuts , std::back_inserter ( r ) ) ;
  return r ;
}
// ============================================================================
LHCb::VertexBase::ConstVector
operator>> 
( const LHCb::VertexBase::ConstVector&  input , 
  const LoKi::Types::VCuts&             cuts  ) 
{
  LHCb::VertexBase::ConstVector r ; 
  r.reserve ( input.size() ) ;
  LoKi::apply_filter ( input . begin ()  , 
                       input . end   ()  , cuts , std::back_inserter ( r ) ) ;
  return r ;
}
// ============================================================================
LHCb::VertexBase::ConstVector
operator>> 
( const LHCb::Vertex::ConstVector&      input , 
  const LoKi::Types::VCuts&             cuts  ) 
{
  LHCb::VertexBase::ConstVector r ; 
  r.reserve ( input.size() ) ;
  LoKi::apply_filter ( input . begin ()  , 
                       input . end   ()  , cuts , std::back_inserter ( r ) ) ;
  return r ;
}
// ============================================================================
LHCb::RecVertex::ConstVector
operator>> 
( const LHCb::RecVertex::ConstVector&   input , 
  const LoKi::Types::VCuts&             cuts  ) 
{
  LHCb::RecVertex::ConstVector r ; 
  r.reserve ( input.size() ) ;
  LoKi::apply_filter ( input . begin ()  , 
                       input . end   ()  , cuts , std::back_inserter ( r ) ) ;
  return r ;
}
// ============================================================================
LHCb::VertexBase::ConstVector
operator>> 
( const LHCb::Vertex::Container*        input , 
  const LoKi::Types::VCuts&             cuts  ) 
{
  LHCb::VertexBase::ConstVector r ; 
  if ( !input || input->empty() ) { return r ; }
  r.reserve ( input->size() ) ;
  LoKi::apply_filter ( input -> begin ()  , 
                       input -> end   ()  , cuts , std::back_inserter ( r ) ) ;
  return r ;
}
// ============================================================================
LHCb::RecVertex::ConstVector
operator>> 
( const LHCb::RecVertex::Container*     input , 
  const LoKi::Types::VCuts&             cuts  ) 
{
  LHCb::RecVertex::ConstVector r ; 
  if ( !input || input->empty() ) { return r ; }
  r.reserve ( input->size() ) ;
  LoKi::apply_filter ( input -> begin ()  , 
                       input -> end   ()  , cuts , std::back_inserter ( r ) ) ;
  return r ;
}
// ============================================================================
LHCb::VertexBase::ConstVector
operator>> 
( const SmartRefVector<LHCb::Vertex>&   input , 
  const LoKi::Types::VCuts&             cuts  ) 
{
  LHCb::VertexBase::ConstVector r ; 
  r.reserve ( input.size() ) ;
  LoKi::apply_filter ( input . begin ()  , 
                       input . end   ()  , cuts , std::back_inserter ( r ) ) ;
  return r ;
}
// ============================================================================
LHCb::RecVertex::ConstVector
operator>> 
( const SmartRefVector<LHCb::RecVertex>& input , 
  const LoKi::Types::VCuts&              cuts  ) 
{
  LHCb::RecVertex::ConstVector r ; 
  r.reserve ( input.size() ) ;
  LoKi::apply_filter ( input . begin ()  , 
                       input . end   ()  , cuts , std::back_inserter ( r ) ) ;
  return r ;
}
// ============================================================================
// Functions 
// ============================================================================
std::vector<double>
operator>> 
( const LHCb::Particle::ConstVector&    input , 
  const LoKi::Types::Func&              cuts  ) 
{ return _get_ ( input.begin () , input.end () , cuts ) ; }
// ============================================================================
std::vector<double>
operator>> 
( const LHCb::Particle::Range&          input , 
  const LoKi::Types::Func&              cuts  ) 
{ return _get_ ( input.begin () , input.end () , cuts ) ; }
// ============================================================================
std::vector<double>
operator>> 
( const LHCb::Particle::Container*      input , 
  const LoKi::Types::Func&              cuts  ) 
{ return _get_ ( input , cuts ) ; }
// ============================================================================
std::vector<double>
operator>> 
( const LHCb::Particle::Selection*      input , 
  const LoKi::Types::Func&              cuts  ) 
{ return _get_ ( input , cuts ) ; }
// ============================================================================
std::vector<double>
operator>> 
( const SmartRefVector<LHCb::Particle>& input , 
  const LoKi::Types::Func&              cuts  ) 
{ return _get_ ( input.begin () , input.end () , cuts ) ; }
// ============================================================================
// Vertex functions  
// ============================================================================
std::vector<double>
operator>> 
( const LHCb::VertexBase::ConstVector& input , 
  const LoKi::Types::VFunc&            cuts  ) 
{ return _get_ ( input.begin () , input.end () , cuts ) ; }
// ============================================================================
std::vector<double>
operator>> 
( const LHCb::Vertex::ConstVector&     input , 
  const LoKi::Types::VFunc&            cuts  ) 
{ return _get_ ( input.begin () , input.end () , cuts ) ; }
// ============================================================================
std::vector<double>
operator>> 
( const LHCb::RecVertex::ConstVector&   input , 
  const LoKi::Types::VFunc&             cuts  ) 
{ return _get_ ( input.begin () , input.end () , cuts ) ; }
// ============================================================================
std::vector<double>
operator>> 
( const LHCb::Vertex::Container*        input , 
  const LoKi::Types::VFunc&             cuts  ) 
{ return _get_ ( input , cuts ) ; }
// ============================================================================
std::vector<double>
operator>> 
( const LHCb::RecVertex::Container*     input , 
  const LoKi::Types::VFunc&             cuts  ) 
{ return _get_ ( input , cuts ) ; }
// ============================================================================
std::vector<double>
operator>> 
( const SmartRefVector<LHCb::Vertex>&   input , 
  const LoKi::Types::VFunc&             cuts  ) 
{ return _get_ ( input.begin () , input.end () , cuts ) ; }
// ============================================================================
std::vector<double>
operator>> 
( const SmartRefVector<LHCb::RecVertex>&input , 
  const LoKi::Types::VFunc&             cuts  ) 
{ return _get_ ( input.begin () , input.end () , cuts ) ; }
// ============================================================================
// Some weird stuff 
// ============================================================================
#include "LoKi/Particles7.h"
// ============================================================================
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::ConstVector&    input , 
  const LoKi::Types::VCuts&             cuts  ) 
{ return _wget_ ( input.begin() , input.end() , cuts ) ; }
// ============================================================================
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::Container*      input , 
  const LoKi::Types::VCuts&             cuts  ) 
{ return _wget_ ( input , cuts ) ; }
// ============================================================================
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::Selection*      input , 
  const LoKi::Types::VCuts&             cuts  ) 
{ return _wget_ ( input , cuts ) ; }
// ============================================================================
LHCb::Particle::ConstVector
operator>> 
( const SmartRefVector<LHCb::Particle>& input , 
  const LoKi::Types::VCuts&             cuts  ) 
{ return _wget_ ( input.begin() , input.end() , cuts ) ; }
// ============================================================================
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::Range&          input , 
  const LoKi::Types::VCuts&             cuts  ) 
{ return _wget_ ( input.begin() , input.end() , cuts ) ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
