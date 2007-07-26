// $Id: HybridFilterCriterion.cpp,v 1.1 2007-07-26 13:25:09 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <sstream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h" 
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IFilterCriterion.h"
#include "Kernel/FilterCriterionBase.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IHybridFactory.h"
#include "LoKi/Primitives.h"
// ============================================================================
namespace LoKi 
{
  namespace Hybrid 
  {
    /** @class FilterCriterion HybridFilterCriterion.cpp
     *  
     *  The first (test) attempt to develop a "hybrid"
     *  solution for LoKi+Hlt
     *  Simple tool (IFilterCriterion) which is used in C++ analysis 
     *  environment, but the "cuts" are descrived through Python 
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
     *  @date   2004-06-29
     */
    // ============================================================================
    class FilterCriterion : public FilterCriterionBase 
    {
      // friend factory for instantiation 
      friend class ToolFactory<LoKi::Hybrid::FilterCriterion> ;
    public:
      /// initialization of the tool 
      virtual StatusCode initialize () ;
    protected:
      /** The method which actually makes the decision
       *  Must be re-implemented in each real derived filter tool
       *  @see FilterCriterionBase
       */
      virtual bool testParticle( const LHCb::Particle* const & part ) 
      { return m_cut ( part ) ; }
  protected:
      /// Standard constructor
      FilterCriterion 
      ( const std::string& type, 
        const std::string& name,
        const IInterface* parent)
        : FilterCriterionBase ( type , name , parent )
        , m_cut ( LoKi::BooleanConstant<const LHCb::Particle*>( false ) ) 
        , m_code    ( "NONE")
        , m_factory ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC" ) 
      {
        // why it is not declared in base class?
        declareInterface<IFilterCriterion> ( this ) ;
        declareProperty 
          ( "Code"    , m_code    , "Python pseudocode for the filter criteria" ) ;
        declareProperty 
          ( "Factory" , m_factory , "Type/Name for C++/Python Hybrid Factory"   ) ;
      } ;
      /// destructor : virtual and protected
      virtual ~FilterCriterion( ){}
    private:
      /// the default constructor is disabled 
      FilterCriterion () ;
      /// the copy constructor is disabled 
      FilterCriterion           ( const FilterCriterion& ) ;
      /// the assignement operator is disabled
      FilterCriterion& operator=( const FilterCriterion& ) ;
    private:
      // the selection functor
      LoKi::Types::Cut  m_cut     ; ///< the selection functor 
      // python pseudo-code
      std::string       m_code    ; ///< python pseudo-code
      // factory type/name
      std::string       m_factory ; ///< factory type/name
    } ;
  } // end of namespace LoKi::Hybrid 
} // end of namespace LoKi 
// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,FilterCriterion);
// ============================================================================
// initialization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::FilterCriterion::initialize () 
{
  // (1) initialize the base 
  StatusCode  sc = FilterCriterionBase::initialize() ;
  if ( sc.isFailure() ) { return sc ; }                                  // RETURN 
  // (2) get the factory:
  IHybridFactory* factory = tool<IHybridFactory> ( m_factory , this ) ;
  if ( 0 == factory ) 
  { return Error ( "Could not locate IHybridFactory" ) ; }               // RETURN 
  // (3) use the factory to get the cuts 
  sc = factory->get (  m_code , m_cut ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from IHybridFactory", sc   ) ; }               // RETURN 
  // 
  info() << "CUT: '" << m_cut << "' "<< endreq ;
  //
  // use FilterCriterionBase:
  setActive() ;                               ///< use FilteraCriterionBase:
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
// The END 
// ============================================================================

