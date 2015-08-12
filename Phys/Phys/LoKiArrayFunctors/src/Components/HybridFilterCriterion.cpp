// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <sstream>
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleFilter.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IHybridFactory.h"
#include "LoKi/Primitives.h"
// ============================================================================
// Local 
// ============================================================================
#include "Preambulo.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class FilterCriterion HybridFilterCriterion.cpp
     *  
     *  The first (test) attempt to develop a "hybrid"
     *  solution for LoKi+Hlt
     *  Simple tool (IParticleFilter) which is used in C++ analysis 
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
    // ========================================================================
    class FilterCriterion 
      : public extends1<GaudiTool,IParticleFilter>
    {
      // ======================================================================
      // friend factory for instantiation 
      friend class ToolFactory<LoKi::Hybrid::FilterCriterion> ;
      // ======================================================================
    public:
      // ======================================================================
      /// initialization of the tool 
      virtual StatusCode initialize () 
      {
        // (1) initialize the base 
        StatusCode  sc = GaudiTool::initialize() ;
        if ( sc.isFailure() ) { return sc ; }                        // RETURN 
        //
        return initVar () ;
      }
      /// finalize the tool
      virtual StatusCode finalize () 
      {
        // reset the functor 
        m_cut = LoKi::Constant<const LHCb::Particle*,bool>( false ) ;
        // finalize the base:
        return GaudiTool::finalize () ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      /// initialization of the tool 
      StatusCode initVar     () ;
      // ======================================================================
    public:
      // ======================================================================
      virtual bool operator()   ( const LHCb::Particle* part ) const 
      { return m_cut ( part ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// the preambulo 
      std::string preambulo() const { return _preambulo ( m_preambulo ) ; }      
      // ======================================================================
    public:
      // ======================================================================
      /// the update handler
      void propHandler ( Property& /* p */ )  
      {
        //
        if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
        //
        StatusCode sc = initVar () ;
        Assert ( sc.isSuccess() , "Unable to set 'Code'"   , sc ) ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      /// Standard constructor
      FilterCriterion 
      ( const std::string& type, 
        const std::string& name,
        const IInterface* parent)
        : base_class ( type , name , parent )
        , m_cut ( LoKi::Constant<const LHCb::Particle*,bool>( false ) ) 
        , m_code    ( "NONE")
        , m_factory ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC" ) 
        , m_preambulo()
      {
        //
        if      ( 0 == name.find("Hlt1") ) 
        { m_factory = "LoKi::Hybrid::Tool/Hlt1HybridFactory:PUBLIC" ; }
        else if ( 0 == name.find("Hlt2") ) 
        { m_factory = "LoKi::Hybrid::Tool/Hlt2HybridFactory:PUBLIC" ; }
        //
        declareProperty 
          ( "Code"    , m_code    ,
            "Python pseudocode for the filter criteria" ) ->
          declareUpdateHandler 
          ( &LoKi::Hybrid::FilterCriterion::propHandler , this ) ;
        //
        declareProperty 
          ( "Factory" , m_factory , 
            "Type/Name for C++/Python Hybrid Factory"   ) ->
          declareUpdateHandler 
          ( &LoKi::Hybrid::FilterCriterion::propHandler , this ) ;
        // the preambulo
        declareProperty 
          ( "Preambulo" , 
            m_preambulo , 
            "The preambulo to be used for Bender/Python script" ) ->
          declareUpdateHandler 
          ( &LoKi::Hybrid::FilterCriterion::propHandler , this ) ;
        //
      } 
      /// destructor : virtual and protected
      virtual ~FilterCriterion( ){}       // destructor : virtual and protected
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      FilterCriterion () ;
      /// the copy constructor is disabled 
      FilterCriterion           ( const FilterCriterion& ) ;
      /// the assignement operator is disabled
      FilterCriterion& operator=( const FilterCriterion& ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// the selection functor
      LoKi::Types::Cut  m_cut     ;                    // the selection functor 
      /// python pseudo-code
      std::string       m_code    ;                    //    python pseudo-code
      /// factory type/name
      std::string       m_factory ;                    //     factory type/name
      /// preambulo 
      std::vector<std::string> m_preambulo ;           //             preambulo
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Hybrid 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// initialization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::FilterCriterion::initVar () 
{
  // (1) get the factory:
  IHybridFactory* factory = tool<IHybridFactory> ( m_factory , this ) ;
  if ( 0 == factory ) 
  { return Error ( "Could not locate IHybridFactory" ) ; }               // RETURN 
  // (3) use the factory to get the cuts 
  StatusCode sc = factory->get (  m_code , m_cut , preambulo() ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from IHybridFactory", sc   ) ; }               // RETURN 
  // 
  debug() << "CUT: '" << m_cut << "' "<< endreq ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,FilterCriterion)
// ============================================================================
// The END 
// ============================================================================
