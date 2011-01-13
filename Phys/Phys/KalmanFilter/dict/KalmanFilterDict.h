// $Id: $
// ============================================================================
#ifndef DICT_KALMANFILTERDICT_H 
#define DICT_KALMANFILTERDICT_H 1
// ============================================================================
// Include files
// ============================================================================
#include "KalmanFilter/FastVertex.h"
#include "KalmanFilter/ErrorCodes.h"
#include "KalmanFilter/ParticleTypes.h"
#include "KalmanFilter/VertexFit.h"
#include "KalmanFilter/VertexFitWithTracks.h"
// ============================================================================
/** @file
 *  Helper file to build Reflex dictionry for Kalman filter package 
 *
 *  This file is a part of 
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *  ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2011-01-13
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
 */
// ============================================================================
namespace
{
  // ==========================================================================
  struct _Instantiations 
  {
    // ========================================================================
    LoKi::KalmanFilter::Entries    m_entries  ;
    LoKi::KalmanFilter::TrEntries4 m_entries4 ;
    // ========================================================================
  } ;  
  // ==========================================================================
}
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // DICT_KALMANFILTERDICT_H
// ============================================================================
