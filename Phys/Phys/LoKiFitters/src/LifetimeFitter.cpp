// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/PhysicalConstants.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/ILifetimeFitter.h"
// ============================================================================
// local
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
 *  @date 2008-02-12
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class LifeTimeFitter
   *  The simple implementation of following major  abstract interfaces:
   *
   *    - ILifetimeFitter
   *
   *  The implementation follows the note by Paul AVERY 
   *    "Directly Determining Lifetime Using a 3-D Fit"
   *   
   *  The actual algorithm si described in detail for 
   *  the base class LoKi::DirectionFitBase
   *
   *  @see LoKi::DirectionFitBase 
   *  @see ILifetimeFitter
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2008-02-12
   */
  class LifetimeFitter 
    : public extends1<LoKi::DirectionFitBase,ILifetimeFitter>
  {
    // ========================================================================
    /// the friend factory needed for instantiation
    friend class ToolFactory<LoKi::LifetimeFitter> ;
    // ========================================================================
  public:
    // ========================================================================
    /** Evaluate the particle  lifetime
     *
     *  @code
     *
     *  // get the tool:
     *  ILifetimeFitter* fitter = ... ;
     *
     *  // get B-candidate:
     *  const LHCb::Particle* myB = ... ;
     *  
     *  // get the corresponidng primary vertex:
     *  const LHCb::VertexBase* primary = ... ;
     *
     *  // use the tool:
     *  double lifetime = -1*e+10 * Gaudi::Units::ns ;
     *  double error    = -1 ;
     *  double chi2     = -1 ;
     *  StatusCode sc = fitter -> fit ( primary , myB , lifetime , error , chi2 ) ;
     *  if ( sc.isFailure() ) { ... error here ... } 
     *
     *  @endcode
     *   
     *  @see ILifetimeFitter
     *
     *  @param primary  the production vertex   (input)
     *  @param particle the particle            (input)
     *  @param lifetime the proper lifetime     (output)
     *  @param error error estgimate for the proper lifetime  (output)
     *  @param chi2 chi2 of the fit            (output)
     */
    virtual StatusCode fit 
    ( const LHCb::VertexBase& primary  , 
      const LHCb::Particle&   particle , 
      double&                 lifetime , 
      double&                 error    ,
      double&                 chi2     ) const ; 
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
    LifetimeFitter  
    ( const std::string& type   , 
      const std::string& name   , 
      const IInterface*  parent ) 
      : base_class ( type , name , parent ) 
    {} 
    /// virtual & protected destructor
    virtual ~LifetimeFitter() {}  ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    LifetimeFitter() ;                   // the default constructor is disabled
    /// the copy constructor is disabled 
    LifetimeFitter ( const LifetimeFitter& ) ;           // no copy constructor
    /// assigmenent operator is disabled
    LifetimeFitter& operator= ( const LifetimeFitter& ) ;     // no assignement
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// Evaluate the particle  lifetime
// ============================================================================
StatusCode LoKi::LifetimeFitter::fit 
( const LHCb::VertexBase& primary  , 
  const LHCb::Particle&   particle , 
  double&                 lifetime , 
  double&                 error    ,
  double&                 chi2     ) const 
{
  const LHCb::VertexBase* decay = particle.endVertex() ;
  if ( NULL == decay ) 
  {
    lifetime = -1.e+10 * Gaudi::Units::nanosecond ;
    error    = -1.e+10 * Gaudi::Units::nanosecond ;
    chi2     = -1.e+10 ;
    return _Error ( "No valid end-vertex is found"  , NoEndVertex ) ;  
  }
  
  // make the actual iterations 
  const StatusCode sc = fitConst_ 
    ( &primary  , &particle , decay , lifetime , error , chi2 ) ;  
  if ( sc.isFailure() ) 
  { return _Warning ( "Error from LoKi::DirectionFitBase" , sc, 0 ) ;  }
  
  // convert c*tau into time 
  lifetime /= Gaudi::Units::c_light ;
  error    /= Gaudi::Units::c_light ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( LoKi , LifetimeFitter )
// ============================================================================
// The END 
// ============================================================================
