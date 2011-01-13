// $Id: IGenDecay.cpp,v 1.1 2009-05-27 18:43:12 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// HepMC
// ============================================================================
#ifdef __INTEL_COMPILER
  #pragma warning (disable:1572) // floating-point equality and inequality comparisons are unreliable
  #pragma warning(push)
#endif
#include "HepMC/GenParticle.h" // To satisfy forward declaration in IGenDecay.h
#ifdef __INTEL_COMPILER
  #pragma warning(pop)
#endif
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IGenDecay.h"
// ============================================================================
/** @file 
 *  Implementation file for class Decays::IGenDecay
 *  @author Ivan BELYAEV
 *  @date   2009-05-22
 */
// ============================================================================
// virtual & protected destructor 
// ============================================================================
Decays::IGenDecay::~IGenDecay () {}           // virtual & protected destructor 
// ============================================================================
// unique interface ID
// ============================================================================
const InterfaceID& Decays::IGenDecay::interfaceID()  // the unique interface ID
{
  static const InterfaceID s_ID ( "Decays::IGenDecay" , 1 , 0 ) ;
  return s_ID ;
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
