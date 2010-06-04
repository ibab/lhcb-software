// $Id: ParticleFilters.cpp,v 1.3 2010-06-04 12:35:23 ibelyaev Exp $
// ===========================================================================
// Include files 
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IParticleFilter.h"
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
/*  split the particles according to some criteria
 * 
 *  @param input     (INPUT) the container of input particles 
 *  @param criterion (INPUT) the selection criteria 
 *  @param output    (OUPUT) the container of     selected particles 
 *  @param output2   (OUPUT) the container of non-selected particles 
 *  @return number of added particles    
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-05-14
 */
// ============================================================================
std::size_t DaVinci::filter 
( const LHCb::Particle::Range& input     , 
  const IParticleFilter*       criterion , 
  LHCb::Particle::ConstVector& output    ,
  LHCb::Particle::ConstVector& output2   ) 
{
  if ( 0 == criterion ) { return 0 ; }
  return filter ( input , *criterion , output , output2 ) ;  
}
// ==========================================================================
/*  split the particles according to some criteria
 * 
 *  @param input     (INPUT) the container of input particles 
 *  @param criterion (INPUT) the selection criteria 
 *  @param output    (OUPUT) the container of selected particles 
 *  @param output2   (OUPUT) the container of non-selected particles 
 *  @return number of added particles    
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-05-14
 */
// ============================================================================
std::size_t DaVinci::filter 
( const LHCb::Particle::ConstVector& input     , 
  const IParticleFilter*             criterion , 
  LHCb::Particle::ConstVector&       output    ,
  LHCb::Particle::ConstVector&       output2   ) 
{
  return
    filter ( LHCb::Particle::Range ( input ) , criterion , output , output2 ) ;
}
// ==========================================================================


// ============================================================================
// The END 
// ============================================================================

