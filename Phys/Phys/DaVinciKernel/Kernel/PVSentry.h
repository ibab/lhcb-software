// $Id:$ 
// ============================================================================
#ifndef DAVINCIKERNEL_PVSENTRY_H 
#define DAVINCIKERNEL_PVSENTRY_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Forward declarations 
// ============================================================================
class DaVinciAlgorthm ;
namespace LHCb { class Particle ; }
// ===========================================================================
namespace DaVinci
{
  // ==========================================================================
  /** @class PVSentry
   *  Helper class to guarantee the removal of relations to the
   *  temporary object
   *
   *  The class is moved from CombineParticles 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2009-05-05
   *
   *                    $Reviison:$
   *  Last modification $Date:$
   *                 by $Author:$
   */
  class GAUDI_API PVSentry
  {
    // ========================================================================
  public:
    // ========================================================================
    /** constructor
     *  @param parent   the davinci algorithmthat holds relation tables  
     *  @param particle the particle
     */
    PVSentry ( DaVinciAlgorithm*     parent,
               const LHCb::Particle* particle ) ;
    /// destructor
    ~PVSentry() ; 
    // ========================================================================
  private:
    // ========================================================================
    // the default constructror is disabled
    PVSentry () ; // the default constructror is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the parent
    DaVinciAlgorithm* m_parent ;             
    /// the temporary particle
    const LHCb::Particle* m_particle ;  
    // ========================================================================
  };
  // ==========================================================================
} //                                               the end of namespace DaVinci 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // DAVINCIKERNEL_PVSENTRY_H
// ============================================================================
