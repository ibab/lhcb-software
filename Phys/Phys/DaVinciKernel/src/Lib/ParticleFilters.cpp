// $Id: ParticleFilters.cpp,v 1.1 2010-05-14 15:07:10 ibelyaev Exp $
// ===========================================================================
// Include files 
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IParticleFilter.h"
#include "Kernel/IFilterCriterion.h"
#include "Kernel/IFilterParticles.h"
// ============================================================================
// Local
// ============================================================================
#include "Kernel/ParticleFilters.h"
// ============================================================================
/** @file
 *  implementation of function DaVinci::filter
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-05-14
 */
// ============================================================================
/** filter the particles according to some criteria
 * 
 *  @param input     (INPUT) the container of input particles 
 *  @param criterion (INPUT) the selection criteria 
 *  @param output    (OUPUT) the container of selected particles 
 *  @reutrn number of added particles    
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-05-14
 */
// ==========================================================================
std::size_t DaVinci::filter 
( const LHCb::Particle::Range& input     , 
  const IFilterCriterion*      criterion , 
  LHCb::Particle::ConstVector& output    ) 
{
  if ( 0 == criterion ) { return 0 ; }
  return filter ( input , *criterion , output ) ;
}
// ==========================================================================
/*  filter the particles according to some criteria
 * 
 *  @param input     (INPUT) the container of input particles 
 *  @param criterion (INPUT) the selection criteria 
 *  @param output    (OUPUT) the container of selected particles 
 *  @reutrn number of added particles    
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-05-14
 */
// ============================================================================
std::size_t DaVinci::filter 
( const LHCb::Particle::ConstVector& input     , 
  const IFilterCriterion*            criterion , 
  LHCb::Particle::ConstVector&       output    ) 
{
  if ( 0 == criterion ) { return 0 ; }
  return filter ( input , *criterion , output ) ;
}
// ============================================================================
/** filter the particles according to some criteria
 * 
 *  @param input     (INPUT) the container of input particles 
 *  @param criterion (INPUT) the selection criteria 
 *  @param output    (OUPUT) the container of selected particles 
 *  @reutrn number of added particles    
 *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
 *  @date 2010-05-14
 */
// ============================================================================
std::size_t DaVinci::filter 
( const LHCb::Particle::Range& input     , 
  const IParticleFilter*       criterion , 
  LHCb::Particle::ConstVector& output    )
{
  if ( 0 == criterion ) { return 0 ; }
  return filter ( input , *criterion , output ) ;
}
// ============================================================================
/** filter the particles according to some criteria
 * 
 *  @param input     (INPUT) the container of input particles 
 *  @param criterion (INPUT) the selection criteria 
 *  @param output    (OUPUT) the container of selected particles 
 *  @reutrn number of added particles    
 *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
 *  @date 2010-05-14
 */
// ============================================================================
std::size_t DaVinci::filter 
( const LHCb::Particle::ConstVector& input     , 
  const IParticleFilter*             criterion , 
  LHCb::Particle::ConstVector&       output    )
{
  if ( 0 == criterion ) { return 0 ; }
  return filter ( input , *criterion , output ) ;
}
// ============================================================================



// ============================================================================
// The END 
// ============================================================================

