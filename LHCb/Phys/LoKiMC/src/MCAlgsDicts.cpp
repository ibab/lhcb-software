// $Id: MCAlgsDicts.cpp,v 1.2 2007-07-23 17:27:31 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/Algs.h"
#include "LoKi/MCAlgs.h"
#include "LoKi/MCAlgsDicts.h"
// ============================================================================
/** @file 
 *  Implementation file for namespace LoKi::Dicst::Algs
 *  @date 2007-06-04 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
// count in the tree 
// ============================================================================
std::size_t LoKi::Dicts::MCAlgs::count_if 
( const LHCb::MCParticle*      particle  , 
  const LoKi::MCTypes::MCCuts& cut       ,
  const bool                   decayOnly ) 
{ return LoKi::MCAlgs::count_if ( particle , cut , decayOnly ) ; }
// ============================================================================
// count in the tree 
// ============================================================================
std::size_t LoKi::Dicts::MCAlgs::count_if 
( const LHCb::MCVertex*        vertex    , 
  const LoKi::MCTypes::MCCuts& cut       ,
  const bool                   decayOnly ) 
{ return LoKi::MCAlgs::count_if ( vertex , cut , decayOnly ) ; }
// ============================================================================
// check in the tree 
// ============================================================================
bool LoKi::Dicts::MCAlgs::found 
( const LHCb::MCParticle*      particle  , 
  const LoKi::MCTypes::MCCuts& cut       ,
  const bool                   decayOnly ) 
{ return LoKi::MCAlgs::found ( particle , cut , decayOnly ) ; }
// ============================================================================
// check in the tree 
// ============================================================================
bool LoKi::Dicts::MCAlgs::found 
( const LHCb::MCVertex*        vertex    , 
  const LoKi::MCTypes::MCCuts& cut       ,
  const bool                   decayOnly ) 
{ return LoKi::MCAlgs::found ( vertex , cut , decayOnly ) ; }
// ============================================================================
// find minimal value in the tree 
// ============================================================================
double LoKi::Dicts::MCAlgs::min_value 
( const LHCb::MCParticle*       particle  , 
  const LoKi::MCTypes::MCFunc&  fun       , 
  const LoKi::MCTypes::MCCuts&  cut       ,
  const  bool                   decayOnly , 
  double                        result    ) 
{ return LoKi::MCAlgs::min_value ( particle , fun , cut , result , decayOnly ) ; }
// ============================================================================
// find maximal value in the tree 
// ============================================================================
double LoKi::Dicts::MCAlgs::max_value 
( const LHCb::MCParticle*       particle  , 
  const LoKi::MCTypes::MCFunc&  fun       , 
  const LoKi::MCTypes::MCCuts&  cut       ,
  const  bool                   decayOnly , 
  double                        result    ) 
{ return LoKi::MCAlgs::max_value ( particle , fun , cut , result , decayOnly ) ; }
// ============================================================================
// find minimal value in the tree 
// ============================================================================
double LoKi::Dicts::MCAlgs::min_value 
( const LHCb::MCVertex*         vertex    , 
  const LoKi::MCTypes::MCFunc&  fun       , 
  const LoKi::MCTypes::MCCuts&  cut       , 
  const  bool                   decayOnly , 
  double                        result    ) 
{ return LoKi::MCAlgs::min_value ( vertex , fun , cut , result , decayOnly ) ; }
// ============================================================================
// find maximal value in the tree 
// ============================================================================
double LoKi::Dicts::MCAlgs::max_value 
( const LHCb::MCVertex*         vertex    , 
  const LoKi::MCTypes::MCFunc&  fun       , 
  const LoKi::MCTypes::MCCuts&  cut       ,
  const  bool                   decayOnly , 
  double                        result    ) 
{ return LoKi::MCAlgs::max_value ( vertex , fun , cut , result , decayOnly ) ; }
// ============================================================================
// accumulate over the tree 
// ============================================================================
double LoKi::Dicts::MCAlgs::accumulate 
( const LHCb::MCParticle*       particle  , 
  const LoKi::MCTypes::MCFunc&  fun       , 
  const LoKi::MCTypes::MCCuts&  cut       ,
  const  bool                   decayOnly , 
  double                        result    ) 
{
  return LoKi::MCAlgs::accumulate 
    ( particle , fun , cut , result , std::plus<double>() , decayOnly ) ;
} 
// ============================================================================
// accumulate over the tree 
// ============================================================================
double LoKi::Dicts::MCAlgs::accumulate 
( const LHCb::MCVertex*         vertex    , 
  const LoKi::MCTypes::MCFunc&  fun       , 
  const LoKi::MCTypes::MCCuts&  cut       ,
  const  bool                   decayOnly , 
  double                        result    ) 
{
  return LoKi::MCAlgs::accumulate 
    ( vertex , fun , cut , result , std::plus<double>() , decayOnly ) ;
} 
// ============================================================================
// find a minimal element 
// ============================================================================
const LHCb::MCParticle* 
LoKi::Dicts::MCAlgs::min_element 
( const LHCb::MCParticle*       particle  , 
  const LoKi::MCTypes::MCFunc&  fun       , 
  const LoKi::MCTypes::MCCuts&  cut       ,
  const  bool                   decayOnly ) 
{ return LoKi::MCAlgs::min_element ( particle , fun , cut , decayOnly ) ; }
// ============================================================================
// find a minimal element 
// ============================================================================
const LHCb::MCParticle* 
LoKi::Dicts::MCAlgs::min_element 
( const LHCb::MCVertex*         vertex    , 
  const LoKi::MCTypes::MCFunc&  fun       , 
  const LoKi::MCTypes::MCCuts&  cut       ,
  const  bool                   decayOnly ) 
{ return LoKi::MCAlgs::min_element ( vertex , fun , cut , decayOnly ) ; }
// ============================================================================
// find a maximal element 
// ============================================================================
const LHCb::MCParticle* 
LoKi::Dicts::MCAlgs::max_element 
( const LHCb::MCParticle*       particle  , 
  const LoKi::MCTypes::MCFunc&  fun       , 
  const LoKi::MCTypes::MCCuts&  cut       ,
  const  bool                   decayOnly ) 
{ return LoKi::MCAlgs::max_element ( particle , fun , cut , decayOnly ) ; }
// ============================================================================
// find a minimal element 
// ============================================================================
const LHCb::MCParticle* 
LoKi::Dicts::MCAlgs::max_element 
( const LHCb::MCVertex*         vertex    , 
  const LoKi::MCTypes::MCFunc&  fun       , 
  const LoKi::MCTypes::MCCuts&  cut       ,
  const  bool                   decayOnly ) 
{ return LoKi::MCAlgs::max_element ( vertex , fun , cut , decayOnly ) ; }
// ============================================================================
// find a mimimal element 
// ============================================================================
const LHCb::MCParticle* 
LoKi::Dicts::MCAlgs::min_element 
( const LHCb::MCParticle::Container* particles , 
  const LoKi::MCTypes::MCFunc&       fun       , 
  const LoKi::MCTypes::MCCuts&       cut       ) 
{
  if ( 0 == particles              ) { return 0 ; }              // RETURN 
  LHCb::MCParticle::Container::const_iterator ifound = LoKi::Algs::select_min
    ( particles->begin () , particles->end   () , fun , cut ) ;
  if ( particles->end () == ifound ) { return 0 ; }              // RETURN 
  return *ifound ;                                               // RETURN   
}
// ============================================================================
// find a maximal element 
// ============================================================================
const LHCb::MCParticle* 
LoKi::Dicts::MCAlgs::max_element 
( const LHCb::MCParticle::Container* particles , 
  const LoKi::MCTypes::MCFunc&       fun       , 
  const LoKi::MCTypes::MCCuts&       cut       ) 
{
  if ( 0 == particles              ) { return 0 ; }              // RETURN 
  LHCb::MCParticle::Container::const_iterator ifound = LoKi::Algs::select_max
    ( particles->begin () , particles->end   () , fun , cut ) ;
  if ( particles->end () == ifound ) { return 0 ; }              // RETURN 
  return *ifound ;                                               // RETURN   
}
// ============================================================================
// find a mimimal element 
// ============================================================================
const LHCb::MCParticle* 
LoKi::Dicts::MCAlgs::min_element 
( const LHCb::MCParticle::ConstVector& particles , 
  const LoKi::MCTypes::MCFunc&         fun       , 
  const LoKi::MCTypes::MCCuts&         cut       ) 
{
  LHCb::MCParticle::ConstVector::const_iterator ifound = LoKi::Algs::select_min 
    ( particles.begin () , particles.end   () , fun , cut ) ;
  if ( particles.end () == ifound ) { return 0 ; }               // RETURN 
  return *ifound ;                                               // RETURN   
}
// ============================================================================
// find a maximal element 
// ============================================================================
const LHCb::MCParticle* 
LoKi::Dicts::MCAlgs::max_element 
( const LHCb::MCParticle::ConstVector& particles , 
  const LoKi::MCTypes::MCFunc&       fun       , 
  const LoKi::MCTypes::MCCuts&       cut       ) 
{
  LHCb::MCParticle::ConstVector::const_iterator ifound = LoKi::Algs::select_max
    ( particles.begin () , particles.end   () , fun , cut ) ;
  if ( particles.end () == ifound ) { return 0 ; }               // RETURN 
  return *ifound ;                                               // RETURN   
}
// ============================================================================


// ============================================================================
// The END
// ============================================================================
