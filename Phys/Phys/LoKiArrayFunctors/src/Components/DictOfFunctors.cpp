//#ifndef MULTITOOLDICTTOOL_H 
//#define MULTITOOLDICTTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/VectorMap.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/Tuples.h"
#include "GaudiAlg/TupleObj.h"
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleTupleTool.h"
#include "Kernel/IParticleDictTool.h"            // Interface
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/IHybridFactory.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/format.hpp"
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
    /** @class DictOfFunctors DictOfFunctors.h
     *  copies Hybrid::TupleTool functionality to
     *  create a dictionary of LoKi functors
     *  Hybrid::TupleTool should probably be implemented by using this class
     *
     *  @author Sebastian Neubert
     *  @date   2013-07-08
     */
    class DictOfFunctors 
      :         public GaudiTool
      , virtual public IParticleDictTool 
    {
    public:
      // ======================================================================
      /// friend class factory for instantiation
      friend class ToolFactory<LoKi::Hybrid::DictOfFunctors>;
      // ======================================================================
    public:
      // ======================================================================
      /** helper class to keep the dictionary items
       *  it is needed due to absence of the default constructor for
       *  the class LoKi::PhysTypes::Fun
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-03-19
       */
      class Item
      {
      public:
        // ====================================================================
        // the default constructor
        Item ()
          : m_name ()
          , m_fun  ( LoKi::BasicFunctors<const LHCb::Particle*>::Constant ( -1.e+10 ) )
        {}
        // ====================================================================
      public:
        /// the variable name
        std::string           m_name ; // the variable name
        /// the functor
        LoKi::PhysTypes::Fun  m_fun  ; /// the functor
      } ;
      // ======================================================================
    public:
      // ======================================================================
      // typedef GaudiUtils::VectorMap<std::string,double> DICT;
      // ======================================================================
      /** Fill the dictionary
       *  @see IParticleDictTool
       *  @param particle the particle about which some info are filled.
       *  @param head prefix for the dictionary keys
       *  @param dict the dictionary to fill
       *  @return status code
       */
      virtual StatusCode  fill 
      ( const LHCb::Particle* p ,
        IParticleDictTool::DICT& dict ) const;
      // ======================================================================
    public:
      // ======================================================================
      /// initialization of the tool
      virtual StatusCode initialize ()
      {
        StatusCode sc = GaudiTool::initialize() ;
        if ( sc.isFailure() ) {  return sc ; }               // RETURN
        svc<IService> ( "LoKiSvc" , true ) ;
        //
        return initVariables () ;
      }
      // ======================================================================
      /// finalization of the tool
      virtual StatusCode finalize ()
      {
        // reset all functors:
        m_items.clear() ;
        // finalize the base
        return GaudiTool::finalize () ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      /// initialization of the tool
      virtual StatusCode initVariables ()
      {
        // get the factory
        IHybridFactory* factory = tool<IHybridFactory> ( m_factory , this ) ;
        //
        m_items.clear() ;
        m_items.reserve ( m_vars.size() ) ;
        // variables are getting configured in the configurable
        // for each variable ask the factory to produce the corresponding functor
        for ( Map::const_iterator ivar = m_vars.begin() ;
              m_vars.end() != ivar ; ++ivar )
        {
          Item item ;
          StatusCode sc = factory->get ( ivar->second , item.m_fun , preambulo() ) ;
          if ( sc.isFailure() )
          { return Error
              ("Unable to decode " + ivar->first + " : " + ivar->second , sc ) ; }
          //
          item.m_name = ivar->first ;
          m_items.push_back  ( item ) ;
          //
          debug() << "The decoded variable name is '"
                  << m_items.back().m_name << "'\t, the functor : '"
                  << m_items.back().m_fun  << "'" << endreq ;
        } // end loop over variables
        //
        if ( m_items.empty() ) { Warning ( "No variables/items are defined" ) ; }
        //
        release ( factory ) ; // we do not need the factory anymore
        //
        return StatusCode::SUCCESS ;
      }
      // ======================================================================
    public:
      // ======================================================================
      /// the update handler
      void propHandler ( Property& /* p */ )
      {
        //
        if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
        //
        Warning( "Reintialization of Variables/Factory&Preambulo" ).ignore() ;
        //
        StatusCode sc = initVariables () ;
        Assert ( sc.isSuccess() , "Unable to set 'Variables'"   , sc ) ;
      }
      // ======================================================================
    public:
      // ======================================================================
      /// the preambulo
      std::string preambulo() const { return _preambulo ( m_preambulo ) ; }
      // ======================================================================
    protected:
      // ======================================================================
      DictOfFunctors
      ( const std::string& type   ,
        const std::string& name   ,
        const IInterface*  parent )
        : GaudiTool ( type , name , parent )
        , m_factory  ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC")
        , m_vars      ()
        , m_items     ()
        , m_preambulo ()
      {
        declareInterface<IParticleDictTool> ( this ) ;
        ///
        if      ( 0 == name.find ( "Hlt1" ) ) 
        { m_factory =  "LoKi::Hybrid::Tool/Hlt1HybridFactory:PUBLIC" ; }
        else if ( 0 == name.find ( "Hlt2" ) ) 
        { m_factory =  "LoKi::Hybrid::Tool/Hlt2HybridFactory:PUBLIC" ; }
        ///
        declareProperty
          ( "Factory" , m_factory ,
            "Type/Name for C++/Python Hybrid Factory" ) ->
          declareUpdateHandler ( &LoKi::Hybrid::DictOfFunctors::propHandler , this ) ;
        //
        declareProperty
          ( "Variables" , m_vars ,
            "The {'name':'functor'}-map defining the dictionary " ) ->
          declareUpdateHandler ( &LoKi::Hybrid::DictOfFunctors::propHandler , this ) ;
        // the preambulo
        declareProperty
          ( "Preambulo" ,
            m_preambulo ,
            "The preambulo to be used for Bender/Python script" ) ->
          declareUpdateHandler ( &LoKi::Hybrid::DictOfFunctors::propHandler , this ) ;
        //
      }
      // ======================================================================
      /// virtual & protected destructor
      virtual ~DictOfFunctors() {}
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      DictOfFunctors () ;                              // no default constructor
      /// the copy  constructor is disabled
      DictOfFunctors ( const DictOfFunctors& ) ;            // no copy constructor
      /// the assigenement operator is disabled
      DictOfFunctors& operator=( const DictOfFunctors& ) ;  // no assignement
      // ======================================================================
    private:
      // ======================================================================
      /// the actual type of {"name":"functor"} map
      typedef std::map<std::string,std::string>  Map ;
      /// the actual type of containter of items
      typedef std::vector<Item> Items ;
      // ======================================================================
    private:
      // ======================================================================
      /// the typename of the hybrid factory
      std::string             m_factory ; // the typename of the hybrid factory
      /// { "name":"functor"} map
      Map                      m_vars       ;        // { "name":"functor"} map
      /// n-tuple columns
      Items                    m_items      ;        //         N-tuple columns
      /// preambulo
      std::vector<std::string> m_preambulo  ;        //               preambulo
      // ======================================================================
    }; // class MultiTooldictTool
    // ========================================================================
  } // end namespace Hybrid
  // ==========================================================================
} // end namespace LoKi
// ============================================================================
/** Fill the dictionary
 *  @see IParticleDictTool
 *  @param particle the particle about which some info are filled.
 *  @param dict the dictionary to fill
 *  @return status code
 */
StatusCode LoKi::Hybrid::DictOfFunctors::fill
( const LHCb::Particle*   p    ,
  IParticleDictTool::DICT& dict ) const
{
  if ( 0 == p  ) { Warning ( "LHCb::Particle*       points to NULL" ) ; }
  //
  for ( Items::const_iterator item = m_items.begin();
        m_items.end() != item ; ++item ) 
  {
    // fill dictionary
    dict.insert( item->m_name , item->m_fun ( p ) ) ;
  }// end loop over items
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,DictOfFunctors)
// ============================================================================
// The END
// ============================================================================


