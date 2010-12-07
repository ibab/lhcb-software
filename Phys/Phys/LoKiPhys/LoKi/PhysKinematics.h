// $Id$
// ============================================================================
#ifndef LOKI_PHYSKINEMATICS_H 
#define LOKI_PHYSKINEMATICS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Kinematics.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Track.h"
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
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-09 
 *
 *                    $Revision$
 *  Last Modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace PhysKinematics 
  {
    // ========================================================================
    /** trivial function to evaluate the mass LHCb::Particle
     *  @param  p particle 
     *  @return invariant mass 
     *  @see LHCb::Particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    GAUDI_API 
    double mass 
    ( const LHCb::Particle* p ) ;
    // ========================================================================    
    /** trivial function to evaluate the mass LHCb::Particle
     *  @param  p1 the first particle 
     *  @param  p2 the second particle 
     *  @return invariant mass 
     *  @see LHCb::Particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    GAUDI_API
    double mass 
    ( const LHCb::Particle* p1 , 
      const LHCb::Particle* p2 ) ;
    // ========================================================================    
    /** trivial function to evaluate the mass LHCb::Particle
     *  @param  p1 the first particle 
     *  @param  p2 the third particle 
     *  @param  p3 the second particle 
     *  @return invariant mass 
     *  @see LHCb::Particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    GAUDI_API
    double mass 
    ( const LHCb::Particle* p1 , 
      const LHCb::Particle* p2 ,
      const LHCb::Particle* p3 ) ;
    // ========================================================================    
    /** trivial function to evaluate the mass LHCb::Particle
     *  @param  p1 the first particle 
     *  @param  p2 the second particle 
     *  @param  p3 the third  particle 
     *  @param  p3 the fourth  particle 
     *  @return invariant mass 
     *  @see LHCb::Particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    GAUDI_API
    double mass 
    ( const LHCb::Particle* p1 , 
      const LHCb::Particle* p2 ,
      const LHCb::Particle* p3 ,
      const LHCb::Particle* p4 ) ;
    // ========================================================================
    /** trivial function to get the lorentz-vector for the track using
     *  mass-hypthesis 
     *  @param track (INPUT) the track 
     *  @param mass  (INPUT) the mass hypothesis 
     *  @return 4-vector 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-07
     */ 
    GAUDI_API 
    Gaudi::LorentzVector
    momentum ( const LHCb::Track* track , 
               const double       mass  ) ;
    // ========================================================================
    /** trivial function to get the mass for the tracks using
     *  the mass-hypotheses 
     *  @param track1 (INPUT) the first  track 
     *  @param mass1  (INPUT) the first  mass hypothesis 
     *  @param track2 (INPUT) the second track 
     *  @param mass2  (INPUT) the second mass hypothesis 
     *  @return mass 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-07
     */ 
    GAUDI_API 
    double mass 
    ( const LHCb::Track* track1 , 
      const double       mass1  ,
      const LHCb::Track* track2 , 
      const double       mass2  ) ;
    // ========================================================================
    /** trivial function to get the mass for the tracks using
     *  the mass-hypotheses 
     *  @param track1 (INPUT) the first  track 
     *  @param mass1  (INPUT) the first  mass hypothesis 
     *  @param track2 (INPUT) the second track 
     *  @param mass2  (INPUT) the second mass hypothesis 
     *  @param track3 (INPUT) the third  track 
     *  @param mass3  (INPUT) the third  mass hypothesis 
     *  @return mass 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-07
     */ 
    GAUDI_API 
    double mass 
    ( const LHCb::Track* track1 , 
      const double       mass1  ,
      const LHCb::Track* track2 , 
      const double       mass2  ,
      const LHCb::Track* track3 , 
      const double       mass3  ) ;
    // ========================================================================
    /** trivial function to get the mass for the tracks using
     *  the mass-hypotheses 
     *  @param tracks (INPUT) the vetcor of tracks
     *  @param masses (INPUT) the vector of masses 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-07
     */ 
    GAUDI_API 
    double mass 
    ( const LHCb::Track::ConstVector& tracks , 
      const std::vector<double>&      masses ) ;
    // ========================================================================
    /** trivial function to get the mass for the tracks using
     *  the mass-hypotheses 
     *  @param tracks (INPUT) the vetcor of tracks
     *  @param masses (INPUT) the vector of masses 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-07
     */ 
    GAUDI_API 
    double mass 
    ( const SmartRefVector<LHCb::Track>& tracks , 
      const std::vector<double>&         masses ) ;
    // ========================================================================
  } //                                    end of namespace LoKi::PhysKinematics
  // ==========================================================================
  namespace Kinematics 
  {
    // ========================================================================
    /// import the namespace into the proper scope 
    using namespace LoKi::PhysKinematics ;
    // ========================================================================
  } //                                        end of namespace LoKi::Kinematics
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSKINEMATICS_H
// ============================================================================
