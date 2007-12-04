// $Id: GetDVAlgorithm.cpp,v 1.1 2007-12-04 16:54:54 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgContextSvc.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GetAlg.h"
#include "GaudiAlg/GetAlgs.h"
// ============================================================================
// DaVinci Kernel
// ============================================================================
#include "Kernel/DVAlgorithm.h"
#include "Kernel/GetDVAlgorithm.h"
// ============================================================================
/** @file 
 *  Implementation file for function Gaudi::Utils::getDVAlgorithm
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-12-04
 */
// ============================================================================
DVAlgorithm* Gaudi::Utils::getDVAlgorithm ( const IAlgContextSvc* svc ) 
{
  if ( 0 == svc ) { return 0 ; }                                // RETURN 
  // use contetx service:
  IAlgorithm* alg = Gaudi::Utils::getAlgorithm 
    ( svc , Gaudi::Utils::AlgTypeSelector<DVAlgorithm>() ) ;
  if ( 0 == alg ) { return 0 ; }                                // RETURN 
  return dynamic_cast<DVAlgorithm*> ( alg ) ;                   // RETURN 
}
// ============================================================================
// The END 
// ============================================================================
