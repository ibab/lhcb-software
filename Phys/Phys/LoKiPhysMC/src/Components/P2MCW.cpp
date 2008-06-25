// $Id: P2MCW.cpp,v 1.1 2008-06-25 17:27:39 ibelyaev Exp $
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
#include "Relations/RelationWeighted2D.h"
// ===========================================================================
// DaVinciMCKernel
// ===========================================================================
#include "Kernel/Particle2MC.h"
// ===========================================================================
// LoKi
// ===========================================================================
#include "P2MCBase.h"
// ===========================================================================
/** @file
 *
 *  Simple algorithm for conversion of Particle->MC links from 
 *  "linker" form into usable form of relation table with weight 
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
  /** @class P2MCW
   *
   *  Simple algorithm for conversion of Particle->MC links from 
   *  "linker" form into usable form of relation table with weight 
   *
   *  Many thanks to Edwin Bos for kind help!
   *
   *  @todo replace 'include Particle2MCLinker' with 
   *   'include Particle2MCmethod' from new package DaVinciMCTools 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-08-17 
   */
  class P2MCW : public LoKi::P2MCBase
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class AlgFactory<LoKi::P2MCW> ;
    // ========================================================================
  public:
    // ========================================================================
    /// execution of the algorithm 
    virtual StatusCode execute() 
    {
      // avoid long names 
      typedef LHCb::RelationWeighted2D<LHCb::Particle,LHCb::MCParticle,double> Table ;
      // check the inheritance scheme 
      BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::P2MC2DW));
      // create the new relation table and register it in TES 
      Table* table = new Table( 100 ) ;
      put ( table , m_output ) ;
      // copy all links from the linker to the table 
      return fillTable ( table ) ;
    }
    /// initialize the algorithm 
    virtual StatusCode initialize () 
    {
      // inittialize the base 
      StatusCode sc = LoKi::P2MCBase::initialize() ;
      if ( sc.isFailure() ) {  return sc ; }
      // check the method 
      switch ( m_method ) 
      {
      case Particle2MCMethod::WithChi2  : break ;
      case Particle2MCMethod::Chi2      : 
      case Particle2MCMethod::Links     : 
      case Particle2MCMethod::Composite : 
        Warning ( "Unweighted method is selected!" , sc ) ; break ;
      default:
        return Error ( "Invalid method is choosen!" ) ;
      }
      //
      return StatusCode::SUCCESS ;                       // RETURN 
    }
    // ========================================================================
  protected:
    // ========================================================================
    /// standard constructor 
    P2MCW
    ( const std::string& name , 
      ISvcLocator*       pSvc ) 
      : LoKi::P2MCBase ( name , pSvc ) 
    {
      m_method = Particle2MCMethod::WithChi2 ;
    } 
    /// virtual protected destructor 
    virtual ~P2MCW() {} ;
    // ========================================================================    
  private:
    // ========================================================================
    // default constructor is disabled 
    P2MCW () ;
    // copy  constructor is disabled 
    P2MCW ( const P2MCW& );
    // assignement operator is disabled 
    P2MCW& operator=( const P2MCW& );
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,P2MCW);
// ============================================================================
/// The END 
// ============================================================================
