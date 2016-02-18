// $Id:$ 
// ============================================================================
#ifndef DAVINCIKERNEL_PVSENTRY_H 
#define DAVINCIKERNEL_PVSENTRY_H 1
// ============================================================================
// Include files
// ============================================================================
// STD& STL 
// ============================================================================
#include <set>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Forward declarations 
// ============================================================================
class IDVAlgorthm ;
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
     *  @param parent   the davinci algorithm that holds relation tables  
     *  @param particle the particle
     */
    PVSentry ( const IDVAlgorithm*   parent           ,
               const LHCb::Particle* particle         , 
               const bool            tree     = false ) ;
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
    const IDVAlgorithm*             m_parent    ;             
    /// the temporary particle
    std::vector<const LHCb::Particle*> m_particles ;  
    // ========================================================================
  };
  // ==========================================================================
} //                                               the end of namespace DaVinci 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // DAVINCIKERNEL_PVSENTRY_H
// ============================================================================
