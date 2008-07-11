// $Id: ISetInputParticles.cpp,v 1.1 2008-07-11 13:11:28 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// PhysEvent 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// Local
// ============================================================================
#include "Kernel/ISetInputParticles.h"
// ============================================================================
/** @file
 *  Implementation file for the class ISetInputParticles
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-07-11
 */
// ============================================================================
// return the unique interface identifier
// ============================================================================
const InterfaceID& ISetInputParticles::interfaceID() 
{
  // the unique interface identifier
  static const InterfaceID s_IID ( "ISetInputParticles", 1, 0 ) ;
  return s_IID ;
}
// ============================================================================
// protected and virtual destructor 
// ============================================================================
ISetInputParticles::~ISetInputParticles() {}
// ============================================================================
/*  set the input particles for some component
 *  @param component the component inself
 *  @param input the intut data 
 *  @return status code 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-07-11
 */
// ============================================================================
StatusCode DaVinci::Utils::setInput 
( IInterface*                        component , 
  const LHCb::Particle::ConstVector& input     ) 
{
  if ( 0 == component ) { return StatusCode ( 300 ) ; }        // RETURN 
  // get the interface 
  SmartIF<ISetInputParticles> alg ( component ) ;
  // valid ? 
  if ( !alg           ) { return StatusCode ( 301 ) ; }        // RETURN  
  // use the valid inyterface 
  return alg->setInput ( input ) ;                             // RETURN 
}
// ============================================================================
// The END 
// ============================================================================
