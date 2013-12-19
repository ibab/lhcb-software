// $Id: $
// ============================================================================
#ifndef LBONIAPAIRS_ONIAPAIRS_H 
#define LBONIAPAIRS_ONIAPAIRS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
/** @namespace OniaPairs 
 *
 *  The main namespace to keep code related to production of Onia-Pairs 
 *
 *  @author Alexey NOVOSELOV Alexey.Novoselov@cern.ch
 *
 *                    $Revision$
 *  Last modification mid/10/2013
 *                 by alexn
 */
namespace OniaPairs 
{
  // ==========================================================================
  GAUDI_API 
  bool   SetPar ( std::string n , double value );
  double GetPar ( std::string n );
  void   PrintCSTable ( ) ;
  // ==========================================================================
} //                                                 end of namespace OniaPairs 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LBONIAPAIRS_ONIAPAIRS_H
// ============================================================================
