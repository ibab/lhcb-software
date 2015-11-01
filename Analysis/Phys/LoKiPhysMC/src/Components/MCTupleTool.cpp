// $Id$
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
// ============================================================================
// DaVinciMCKernel
// ============================================================================
#include "Kernel/IMCParticleTupleTool.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// LoKiMC 
// ============================================================================
#include "LoKi/IMCHybridFactory.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hybrid
  {
    // ========================================================================
    /** @class MCTupleTool 
     *  Simple implementation of IMCParticleTupleTool interface   
     *  @see IMCParticleTupleTool
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2010-02-18
     */
    class MCTupleTool 
      : public                    GaudiTool 
      , public virtual IMCParticleTupleTool 
    {
      // ======================================================================
      /// friend factory for instantiation 
      friend class ToolFactory<LoKi::Hybrid::MCTupleTool> ;
      // ======================================================================
    public: 
      // ======================================================================
      /** Fill the tuple. 
       *  @see IMCParticleTupleTool
       *  @param top  : may be used to provide additional information about 
       *                "part", like the top particle of the decay.
       *  @param part : the particle about which some info are filled.
       *  @param head : prefix for the tuple column name.
       *  @param tuple: the tuple to fill
       *  @return status code 
       */
      virtual StatusCode fill
      ( const LHCb::MCParticle* top   , 
        const LHCb::MCParticle* part  , 
        const std::string&      head  , 
        Tuples::Tuple&          tuple ) ;
      // ======================================================================
    public: 
      // ======================================================================
      /// initializaton of the tool 
      virtual StatusCode initialize () ;           // initializaton of the tool 
      /// finalization  of the tool
      virtual StatusCode finalize   () ;           // finalization  of the tool
      // ======================================================================
    protected:
      // ======================================================================
      /** Standard constructor 
       *  @param type the tool type (?)
       *  @param name the tool instance name 
       *  @param parent the parent 
       */
      MCTupleTool 
      ( const std::string& type   ,  
        const std::string& name   , 
        const IInterface*  parent ) ;
      /// virtual & protected destructor 
      virtual ~MCTupleTool() ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MCTupleTool () ; // the default constrcutor is disabled 
      /// the copy constructor is disabled 
      MCTupleTool ( const MCTupleTool& ) ; // the copy constructor is disabled 
      /// the assignement operator is disabled 
      MCTupleTool& operator=( const MCTupleTool& ) ; // no assignement
      // ======================================================================
    public:
      // ======================================================================
      /// the update handler
      void propHandler ( Property& /* p */ ) ;
      // ======================================================================      
    protected:
      // ======================================================================
      /** helper class to keep the N-tuple items
       *  it is needed due to absence f the default constructor for 
       *  the class LoKi::MCTypes::MCFun 
       *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
       *  @date 2010-02-18
       */
      class Item
      {
      public:
        // ====================================================================
        // the default constructor 
        Item () 
          : m_name () 
          , m_fun  ( LoKi::BasicFunctors<const LHCb::MCParticle*>::Constant ( -1.e+10 ) )
        {}
        // ====================================================================
      public:
        /// the variable name 
        std::string           m_name ; // the variable name 
        /// the functor
        LoKi::MCTypes::MCFun  m_fun  ; /// the functor
      } ;
      // ======================================================================
      /// the actual type for list of items
      typedef std::vector<Item>                                         Items ;
      /// map { "name" : "functor" } 
      typedef std::map<std::string,std::string>                           Map ;
      // ======================================================================
    protected:
      // ======================================================================
      /// build the preambulo-string from preambulo-list 
      std::string preambulo() const 
      {
        const std::vector<std::string>& lines = m_preambulo ;
        //
        std::string result ;
        for ( std::vector<std::string>::const_iterator iline = 
                lines.begin() ; lines.end() != iline ; ++iline ) 
        {
          if ( lines.begin() != iline ) { result += "\n" ; }
          result += (*iline) ;
        }
        return result ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      /// initialization of the tool 
      virtual StatusCode initVariables () ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual type/name of the factory 
      std::string               m_factory   ;               // the factory name
      /// "Preambulo" list 
      std::vector<std::string>  m_preambulo ;               // "Preambulo" list
      /// map { "name" : "functor" }
      Map                       m_map       ;     // map { "name" : "functor" }
      // ======================================================================
    private:
      // ======================================================================
      /// the actual functors for n-tuple
      Items m_items ;                        // the actual functors for n-tuple
      // ======================================================================
    };
    // ========================================================================
  } //                                            end of namespace LoKi::Hybrid
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
/*  Standard constructor 
 *  @param type the tool type (?)
 *  @param name the tool instance name 
 *  @param parent the parent 
 */
// ============================================================================
LoKi::Hybrid::MCTupleTool::MCTupleTool 
( const std::string& type   ,  
  const std::string& name   , 
  const IInterface*  parent ) 
  : GaudiTool   ( type , name , parent ) 
  , m_factory   ( "LoKi::Hybrid::MCTool/MCHybridFactory:PUBLIC" ) 
  , m_preambulo () 
  , m_map       () 
  , m_items     () 
{
  //
  // interface!
  //
  declareInterface<IMCParticleTupleTool> ( this ) ;
  //
  // properties
  //
  // factory
  declareProperty 
    ( "Factory" ,
      m_factory , 
      "Type/Name for C++/Python Hybrid Factory" ) -> 
    declareUpdateHandler ( &LoKi::Hybrid::MCTupleTool::propHandler , this ) ;
  // varibales 
  declareProperty 
    ( "Variables" , 
      m_map       , 
      "The {'name':'functor'}-map of columns for N-tuple " ) -> 
    declareUpdateHandler ( &LoKi::Hybrid::MCTupleTool::propHandler , this ) ;
  // the preambulo
  declareProperty 
    ( "Preambulo" , 
      m_preambulo , 
      "The preambulo to be used for Bender/Python script" ) ->
    declareUpdateHandler ( &LoKi::Hybrid::MCTupleTool::propHandler , this ) ;
  //
}
// ============================================================================
// virtual & protected destructor 
// ============================================================================
LoKi::Hybrid::MCTupleTool::~MCTupleTool(){}
// ============================================================================
/*  Fill the tuple. 
 *  @see IMCParticleTupleTool
 *  @param top  : may be used to provide additional information about 
 *                "part", like the top particle of the decay.
 *  @param part : the particle about which some info are filled.
 *  @param head : prefix for the tuple column name.
 *  @param tuple: the tuple to fill
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::Hybrid::MCTupleTool::fill
( const LHCb::MCParticle* top      , 
  const LHCb::MCParticle* particle , 
  const std::string&      head     , 
  Tuples::Tuple&          tuple    ) 
{
  if ( 0 == top       ) 
  { Warning ( "LHCb::MCParticle* 'Top' points to NULL" ) . ignore () ; }
  if ( 0 == particle  ) 
  { Warning ( "LHCb::MCParticle*       points to NULL" ) . ignore () ; }
  //
  if ( !tuple.valid() ) { return Error ( "Invalid tuple " ) ; } 
  //
  std::string head_ = head + "_" ;
  for ( Items::const_iterator item = m_items.begin() ; 
        m_items.end() != item ; ++item ) 
  {
    // fill N-tuple 
    tuple -> column ( head_ + item->m_name , item->m_fun ( particle ) ) ; 
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// standard initialization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::MCTupleTool::initialize () 
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) {  return sc ; }               // RETURN 
  svc<IService> ( "LoKiSvc" , true ) ;
  //
  return initVariables () ;
}
// ============================================================================
// finalization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::MCTupleTool::finalize () 
{
  // reset all functors:
  m_items.clear() ;
  // finalize the base 
  return GaudiTool::finalize () ;
}
// ============================================================================
// initialization of variables 
// ============================================================================
StatusCode LoKi::Hybrid::MCTupleTool::initVariables () 
{
  // get the factory
  IMCHybridFactory* factory = tool<IMCHybridFactory> ( m_factory , this ) ;
  //
  m_items.clear() ;
  m_items.reserve ( m_map.size() ) ;
  for ( Map::const_iterator ivar = m_map.begin() ; 
        m_map.end() != ivar ; ++ivar ) 
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
            << m_items.back().m_fun  << "'" << endmsg ;
  }  
  // 
  if ( m_items.empty() ) { Warning ( "No variables/items are defined" ) ; }
  //
  release ( factory ) ; // we do not need the factory anymore 
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// the update handler
// ============================================================================
void LoKi::Hybrid::MCTupleTool::propHandler ( Property& /* p */ )  
{
  //
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  Warning( "Reintialization of Variables/Factory&Preambulo" ).ignore() ;
  //
  StatusCode sc = initVariables () ;
  Assert ( sc.isSuccess() , "Unable to set 'Variables'"   , sc ) ;
}
// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,MCTupleTool)
// ============================================================================
// The END 
// ============================================================================
