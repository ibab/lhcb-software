// $Id$
// ============================================================================
// Include files 
// ===========================================================================
// GaudiKernel
// ===========================================================================
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h" 
// ===========================================================================
// Relations 
// ===========================================================================
#include "Relations/Relation2D.h"
// ===========================================================================
// DaVinciMCKernel
// ===========================================================================
#include "Kernel/Particle2MC.h"
// ===========================================================================
// Local
// ===========================================================================
#include "P2MCBase.h"
// ===========================================================================
/** @file
 *
 *  Simple algorithm for conversion of Particle->MC links from 
 *  "linker" form into usable form of relation table 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-08-17 
 */
// ============================================================================
namespace LoKi 
{
  /** @class P2MC
   *
   *  Simple algorithm for conversion of Particle->MC links from 
   *  "linker" form into usable form of relation table 
   *
   *  Many thanks to Edwin Bos for kind help!
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-08-17 
   */
  class P2MC : public LoKi::P2MCBase
  {
    // ========================================================================
    /// the friend factory for algorithm instantiation 
    friend class AlgFactory<LoKi::P2MC> ;
    // ========================================================================
  public:
    // ========================================================================
    /// execution of the algorithm 
    virtual StatusCode execute    () 
    {
      // avoid long names 
      typedef LHCb::Relation2D<LHCb::Particle,LHCb::MCParticle> Table ;
      // check the inheritance scheme 
      BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::P2MC2D));
      // create the new relation table and register it in TES 
      Table* table = new Table( 100 ) ;
      put ( table , m_output ) ;
      // copy all links from the linker to the table 
      return fillTable ( table ) ;
    }
    // ========================================================================
    /// initialize the algorithm 
    virtual StatusCode initialize () 
    {
      StatusCode sc = LoKi::P2MCBase::initialize() ;
      if ( sc.isFailure() ) {  return sc ; }  
      // check the method 
      switch ( m_method ) 
      {
      case Particle2MCMethod::Chi2      : break ;                   // BREAK 
      case Particle2MCMethod::Links     : break ;                   // BREAK 
      case Particle2MCMethod::Composite : break ;                   // BREAK 
      case Particle2MCMethod::WithChi2  :
        Warning ( "Weighted method is selected" , sc ) ; break ;
      default:
        return Error("Invalid method is choosen!") ;                // RETURN 
      }
      return StatusCode::SUCCESS ;                                  // RETURN   
    }
    // ========================================================================
  protected:
    // ========================================================================
    /// standard constructor 
    P2MC
    ( const std::string& name , 
      ISvcLocator*       pSvc ) 
      : LoKi::P2MCBase ( name , pSvc ) 
    { m_method  = Particle2MCMethod::Chi2 ; } ;
    /// virtual protected destructor 
    virtual ~P2MC() {} ;
    // ========================================================================
  private:
    // ========================================================================
    // default constructor is disabled 
    P2MC () ;
    // copy  constructor is disabled 
    P2MC ( const P2MC& );
    // assignement operator is disabled 
    P2MC& operator=( const P2MC& );
    // ========================================================================
  };
  // ==========================================================================
} // end of namespacee LoKi 
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,P2MC)
// ============================================================================
/// The END 
// ============================================================================
