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
/** @namespace ObniaPairs 
 *
 *  The main namespace to keep code related to production of Onia-Pairs 
 *
 *  @author Alexey NONOSELOV Alexey.Novoselov@cern.ch
 *
 *                     $Revision$
 *  Last modiffication $Date$
 *                  by $Author$
 */
namespace OniaPairs 
{
  // ==========================================================================
  GAUDI_API 
  bool   SetPar ( std::string n , double value );
  double GetPar ( std::string n );
  // ==========================================================================
} //                                                 end of namespace OniaPairs 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LBONIAPAIRS_ONIAPAIRS_H
// ============================================================================
