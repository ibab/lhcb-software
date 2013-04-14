// $Id: $
// ============================================================================
#ifndef LOKI_GENPARTICLES5_H 
#define LOKI_GENPARTICLES5_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Kinematics.h"
#include "LoKi/GenTypes.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-28 
 *
 *                    $Revision: 134315 $
 *  Last modification $Date: 2012-01-29 14:43:08 +0100 (Sun, 29 Jan 2012) $
 *                 by $Author: ibelyaev $
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace GenParticles
  {
    // ========================================================================
    /** @class FlightDistance 
     *  evaluator of flight  distance of particle 
     *  @see LoKi::Cuts::GPVFLIGHT
     *  @see LoKi::Cuts::GFLIGHTPV
     *  @see LoKi::Cuts::GFLIGHT
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2013-04-14
     */
    class GAUDI_API Flight : public LoKi::GenTypes::GFunc 
    {
    public:
      // ======================================================================
      /** constructor from the flag  
       *  @param primary use primary vertex, otherwise use own vertex 
       */
      Flight ( const bool primary = false ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Flight () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  Flight* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// use primary vertex ?
      bool primary() const { return m_primary ; } // use primary vertex ?
      // ======================================================================
    private:
      // ======================================================================
      /// use primary vertex 
      bool m_primary ; // use primary vertex 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                      end of namespace LoKi::GenParticles 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @var GFLIGHTPV 
     *  evaluator of flight distance from PV 
     *  @see LoKi::GenParticles::Flight
     *  @see LoKi::Cuts::GPVFLIGHT
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2013-04-14
     */
    const LoKi::GenParticles::Flight                      GFLIGHTPV ( true  ) ;
    // ========================================================================
    /** @var GPVFLIGHT
     *  evaluator of flight distance from PV 
     *  @code
     *  @endcode 
     *  @see LoKi::GenParticles::Flight
     *  @see LoKi::Cuts::GFLIGHTPV
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2013-04-14
     */
    const LoKi::GenParticles::Flight                      GPVFLIGHT ( true ) ;
    // ========================================================================
    /** @var GFLIGHT
     *  evaluator of flight distance
     *  @code
     *  @endcode 
     *  @see LoKi::GenParticles::Flight
     *  @see LoKi::Cuts::GFLIGHTPV 
     *  @see LoKi::Cuts::GPVFLIGHT
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2013-04-14
     */
    const LoKi::GenParticles::Flight                        GFLIGHT ( false ) ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_GENPARTICLES5_H
// ============================================================================
 
