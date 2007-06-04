// $Id: PhysAlgsDicts.cpp,v 1.1 2007-06-04 18:25:37 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysAlgs.h"
#include "LoKi/PhysAlgsDicts.h"
// ============================================================================
/** @file 
 *  Implementation file for functions fomr namespace LoKi::Dicts::Algs 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-06-02
 */
// ============================================================================
// count the elements in the tree 
// ============================================================================
std::size_t LoKi::Dicts::Algs::count_if 
( const LHCb::Particle*        particle , 
  const LoKi::PhysTypes::Cuts& cut      ) 
{ return LoKi::PhysAlgs::count_if ( particle , cut ) ; }
// ============================================================================
/// check the presence in the tree 
// ============================================================================
bool        LoKi::Dicts::Algs::found 
( const LHCb::Particle*        particle , 
  const LoKi::PhysTypes::Cuts& cut      ) 
{ return LoKi::PhysAlgs::found  ( particle , cut ) ; }
// ============================================================================
/// find the minimum over the tree 
// ============================================================================
double LoKi::Dicts::Algs::min_value 
( const LHCb::Particle*        particle , 
  const LoKi::PhysTypes::Func& fun      , 
  const LoKi::PhysTypes::Cuts& cut      ,
  double                       result   ) 
{ return LoKi::PhysAlgs::max_value ( particle , fun , cut , result ) ; }
// ============================================================================
/// find the maximum over the tree 
// ============================================================================
double LoKi::Dicts::Algs::max_value 
( const LHCb::Particle*        particle , 
  const LoKi::PhysTypes::Func& fun      , 
  const LoKi::PhysTypes::Cuts& cut      ,
  double                       result   ) 
{ return LoKi::PhysAlgs::max_value ( particle , fun , cut , result ) ; }
// ============================================================================
/// find the minimum element in  the tree 
// ============================================================================
const LHCb::Particle* 
LoKi::Dicts::Algs::min_element
( const LHCb::Particle*        particle , 
  const LoKi::PhysTypes::Func& fun      , 
  const LoKi::PhysTypes::Cuts& cut      ) 
{ return LoKi::PhysAlgs::min_element ( particle , fun , cut ) ; }
// ============================================================================
/// find the maximum element in  the tree 
// ============================================================================
const LHCb::Particle* 
LoKi::Dicts::Algs::max_element
( const LHCb::Particle*        particle , 
  const LoKi::PhysTypes::Func& fun      , 
  const LoKi::PhysTypes::Cuts& cut      ) 
{ return LoKi::PhysAlgs::max_element ( particle , fun , cut ) ; }
// ============================================================================
// accumulate over the tree 
// ============================================================================
double accumulate  
( const LHCb::Particle*        particle , 
  const LoKi::PhysTypes::Func& fun      , 
  const LoKi::PhysTypes::Cuts& cut      , 
  double                       result   ) 
{
  return LoKi::PhysAlgs::accumulate 
    ( particle , fun , cut , result , std::plus<double>()  ) ; 
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
