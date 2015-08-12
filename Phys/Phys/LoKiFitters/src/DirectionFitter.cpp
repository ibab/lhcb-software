// $Id$
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartIF.h" 
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IDirectionFit.h"
#include "Kernel/IDVAlgorithm.h"
#include "Kernel/GetIDVAlgorithm.h"
// ============================================================================
// Local 
// ============================================================================
#include "DirectionFitBase.h"
// ============================================================================
/** @file 
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
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2008-02-17
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
 */
namespace LoKi 
{
  // ==========================================================================
  /** @class DirectionFitter
   *
   *  The simplest implementation of abstract interface IDirectionFit 
   *  @see IDirectionFit
   *  
   *  The implementation follows the note by Paul AVERY 
   *    "Directly Determining Lifetime Using a 3-D Fit"
   *
   *  The actual algorithm si described in detail for 
   *  the base class LoKi::DirectionFitBase
   *
   *  @see LoKi::DirectionFitBase 
   *  @see IDirectionFit
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2008-02-17
   */
  class DirectionFitter 
    : public extends1<LoKi::DirectionFitBase,IDirectionFit>
  {
    // ========================================================================
    /// the friend factory needed for instantiation
    friend class ToolFactory<LoKi::DirectionFitter> ;
    // ========================================================================
  public: 
    // ========================================================================
    /** perform a "direction" fit for the particle.
     *  The angle between the momentum of the particle 
     *  and the vectro from primary vertex to the decay 
     *  vertex of the particle is constrained to 0.
     *  
     *  @code 
     * 
     *  // locate the tool:
     *  const IDirectionFit* fitter = ... ;
     *  
     *  // primary vertex:
     *  const LHCb::Vertex* primary = ... ;
     * 
     *  // Bs-candidate:
     *  LHCb::Particle*  Bs = ... ; 
     *
     *  StatusCode sc = fitter -> fit ( *primary , *Bs ) ;
     *  if ( sc.isFailure() )  { ... error here ...  }  
     *
     *  @endcode 
     *
     *  @see IDirectionFit
     *  @see LHCb::Particle
     *  @see LHCb::VertexBase 
     * 
     *  @param particle particle to be constrained (input/output)
     *  @param primary  primary vertex             (input) 
     *  @return status code 
     */
    virtual StatusCode fit 
    ( const LHCb::VertexBase& primary  ,
      LHCb::Particle&         particle ) const ; 
    // ========================================================================
  public:
    // ========================================================================
    /** The basic method for "refit" of the particle
     *
     *  @code 
     *
     *  // locate the tool 
     *  const IParticleReFitter* refitter = ... ;
     * 
     *  // particle to be refit 
     *  LHCb::Particle* p = ... ;
     *
     *  StatusCode sc = refitter -> reFit ( *p ) ;
     *  if ( sc.isFailure() ) { ... error here ...  }
     *
     *  @endcode 
     *
     *  @attention the method is implemented in term of "fit"
     *
     *  @see IParticleReFitter
     *  @see LHCb::Particle
     *
     *  @param particle reference to the particle  (input/output)
     *  @return status code 
     */
    virtual StatusCode reFit ( LHCb::Particle& particle ) const ;
    // ========================================================================
  public:
    // ========================================================================
    /// the standard initialization of the tool 
    virtual StatusCode initialize() 
    { return LoKi::DirectionFitBase::initialize() ; }
    // ========================================================================
  protected:
    // ========================================================================
    /** constructor 
     *  @param type the actual tool type (??)
     *  @param name the tool instance name 
     *  @param parent the parent of the tool
     */
    DirectionFitter  
    ( const std::string& type   , 
      const std::string& name   , 
      const IInterface*  parent ) 
      : base_class ( type , name , parent ) {} 
    /// virtual & protected destructor
    virtual ~DirectionFitter() {}  ;
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The basic method for "refit" of the particle
// ============================================================================
StatusCode LoKi::DirectionFitter::reFit ( LHCb::Particle& particle ) const 
{
  // play a bit with extra-info
  if ( particle.hasInfo ( LHCb::Particle::Chi2OfParticleReFitter ) ) 
  { particle.eraseInfo ( LHCb::Particle::Chi2OfParticleReFitter )  ; }
  
  // try to get the associated primary vertex
  const IDVAlgorithm* dv = Gaudi::Utils::getIDVAlgorithm ( contextSvc() ) ;
  if ( 0 == dv ) 
  { return Error ( "No parent IDVAlgorithm is available!" , NoIDVAlgorithm ) ; }
  //
  // 1 ) get the related primary vertex from the DValgorithm
  const LHCb::VertexBase* primary =  dv -> bestVertex ( &particle );
  if ( 0 == primary ) 
  { return Error ( "No associated primary vertex is found!" , NoPrimaryVertex ) ; }
  //
  // 2) use the regular "fit" method 
  StatusCode sc = fit ( *primary , particle ) ;
  if ( sc.isFailure () ) 
  { return Error ( "reFit(): The errot from fit()" , sc ) ; }
  //
  // in the case of success update the extra-info:
  if ( particle.hasInfo ( LHCb::Particle::Chi2OfDirectionConstrainedFit ) ) 
  { 
    particle.addInfo 
      (  LHCb::Particle::Chi2OfParticleReFitter ,
         particle.info ( LHCb::Particle::Chi2OfDirectionConstrainedFit , -1000 ) ) ;
  }    
  //
  return StatusCode::SUCCESS ;
}
// ========================================================================
// perform a "direction" fit for the particle.
// ========================================================================
StatusCode LoKi::DirectionFitter::fit 
( const LHCb::VertexBase& primary  ,
  LHCb::Particle&         particle ) const 
{
  // play a bit with extra-info
  if ( particle.hasInfo ( LHCb::Particle::Chi2OfDirectionConstrainedFit ) ) 
  { particle.eraseInfo ( LHCb::Particle::Chi2OfDirectionConstrainedFit )  ; }
  
  // get the end-vertex for the particle 
  LHCb::VertexBase* decay = particle.endVertex() ;
  if ( 0 == decay ) 
  { return Error 
      ( "No valid endVertex is found!" , StatusCode ( NoEndVertex , true ) ) ; }
  
  // backup the primary vertex:
  LHCb::VertexBase s_primary ( primary ) ;
  
  double ctau  = 0 ;
  double error = 0 ;
  double chi2  = 0 ;
  
  // make the actual iterations 
  StatusCode sc = fit_ ( &s_primary  , &particle , decay , ctau , error , chi2 ) ;
  if ( sc.isFailure() ) { return Error ( "The error from fit_" , sc ) ; }
  
  // in the case of success update the extra-info:
  particle.addInfo ( LHCb::Particle::Chi2OfDirectionConstrainedFit , chi2 ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( LoKi , DirectionFitter )
// ============================================================================
// The END
// ============================================================================
