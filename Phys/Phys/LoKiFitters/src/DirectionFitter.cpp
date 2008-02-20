// $Id: DirectionFitter.cpp,v 1.1.1.1 2008-02-20 15:48:44 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h" 
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
#include "Kernel/IPhysDesktop.h"
#include "Kernel/DVAlgorithm.h"
#include "Kernel/GetDVAlgorithm.h"
// ============================================================================
// Local 
// ============================================================================
#include "DirectionFitBase.h"
// ============================================================================
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
   *  @author Vanya BELYAEV
   *  @date   2008-02-17
   */
  class DirectionFitter 
    : public virtual IDirectionFit
    , public LoKi::DirectionFitBase
  {
    /// the friend factory needed for instantiation
    friend class ToolFactory<LoKi::DirectionFitter> ;
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
      : DirectionFitBase ( type , name , parent ) 
    {
      declareInterface<IDirectionFit>     ( this ) ;
      declareInterface<IParticleReFitter> ( this ) ;
    } 
    /// virtual & protected destructor
    virtual ~DirectionFitter() {}  ;
    // ========================================================================
  protected:
    // ========================================================================
    /// get desktop from the parents
    inline IPhysDesktop* getDesktop ( const IInterface* p ) const 
    {
      if ( 0 == p  ) { return 0 ; }                               // RETURN 
      const DVAlgorithm* dv = dynamic_cast<const DVAlgorithm*> ( p ) ;
      if ( 0 != dv ) { return dv -> desktop() ; }                 // RETURN 
      SmartIF<IAlgTool> t ( const_cast<IInterface*> ( p ) )  ;
      if ( !t      ) { return 0 ; }                               // RETURN               
      return getDesktop ( t -> parent() ) ;
    }
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The basic method for "refit" of the particle
// ============================================================================
StatusCode LoKi::DirectionFitter::reFit ( LHCb::Particle& particle ) const 
{
  // try to get the associated primary vertex
  // 1) get the desktop from the parent
  IPhysDesktop* desktop = getDesktop ( parent() ) ;
  // 1') or get of from the context:
  if ( 0 == desktop ) 
  {
    const DVAlgorithm* dv = Gaudi::Utils::getDVAlgorithm ( contextSvc() ) ;
    if ( 0 != dv ) { desktop = dv -> desktop() ;  }
  }
  if ( 0 == desktop ) 
  { return Error ( "No IPhysDesktop is available!"          , NoDesktop ) ; }
  // 2 ) get the related primary vertex from the desktop 
  const LHCb::VertexBase* primary =  desktop -> relatedVertex ( &particle ) ;
  if ( 0 == primary ) 
  { return Error ( "No associated primary vertex is found!" , NoPrimaryVertex ) ; }
  // 3) use the regular "fit" method 
  return fit ( *primary , particle ) ;
}
// ========================================================================
// perform a "direction" fit for the particle.
// ========================================================================
StatusCode LoKi::DirectionFitter::fit 
( const LHCb::VertexBase& primary  ,
  LHCb::Particle&         particle ) const 
{
  // get the end-vertex for the particle 
  LHCb::VertexBase* decay = particle.endVertex() ;
  if ( 0 == decay ) { return Error ( "No valid endVertex is found!" , NoEndVertex ) ; }
  
  // backup the primary vertex:
  LHCb::VertexBase s_primary ( primary ) ;
  
  double ctau  = 0 ;
  double error = 0 ;
  double chi2  = 0 ;
  
  // make the actual iterations 
  StatusCode sc = fit_ ( &s_primary  , &particle , decay , ctau , error , chi2 ) ;
  if ( sc.isFailure() ) { return Error ( "The error from fit_" , sc ) ; }
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( LoKi , DirectionFitter );
// ============================================================================
// The END
// ============================================================================
