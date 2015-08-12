// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IService.h" 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleReFitter.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
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
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-03-16
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
  /** @class FakeReFitter 
   *  The most trivial implementation of 
   *  the abstract interface IParticleReFitter
   *
   *  It is "refitter" which essentially does nothing.
   *
   *  @see IParticleReFitter
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-03-16
   */
  class FakeReFitter : public extends1<GaudiTool,IParticleReFitter>
  {
    // ========================================================================
    // the friend factory for instantiation 
    friend class ToolFactory<LoKi::FakeReFitter> ;
    // ========================================================================
  public:
    // ========================================================================
    /** The basic method for "refit" of the particle
     *  @see IParticleReFitter 
     */
    virtual StatusCode reFit ( LHCb::Particle&  particle ) const ;
    // ========================================================================
  public:
    // ========================================================================
    /// initialize the tool 
    virtual StatusCode initialize () ;
    // ========================================================================    
  protected:
    // ========================================================================
    /** standard constructor 
     *  @param type tool type (?)
     *  @param name toll instance name 
     *  @param parent the parent 
     */
    FakeReFitter 
    ( const std::string& type   , // tool type 
      const std::string& name   , // tool instance name 
      const IInterface*  parent ) // the parent  
      : base_class ( type , name , parent ) 
    {}
    // ========================================================================
    /// virtual and protected destructor 
    virtual ~FakeReFitter(){} // virtual and protected destructor
    // ========================================================================
  private:
    /// the default constructor is disabled  
    FakeReFitter () ; // no default constructor 
    /// the copy constructor is disabled  
    FakeReFitter ( const FakeReFitter& ) ; // no copy constructor 
    /// the assignement operator is disabled  
    FakeReFitter& operator=( const FakeReFitter& ) ; // no assignment
  };
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// initialize the tool 
// ============================================================================
StatusCode LoKi::FakeReFitter::initialize () 
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  // get LoKi-service 
  svc<IService> ("LoKiSvc" , true ) ;
  return StatusCode::SUCCESS ;
}
// ========================================================================
// The basic method for "refit" of the particle
// ========================================================================
StatusCode LoKi::FakeReFitter::reFit ( LHCb::Particle&  particle ) const 
{
  // erase the corresponding info
  if ( particle.hasInfo ( LHCb::Particle::Chi2OfParticleReFitter ) )
  { particle.eraseInfo(  LHCb::Particle::Chi2OfParticleReFitter ) ; }
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,FakeReFitter)
// ============================================================================
// The END 
// ============================================================================
