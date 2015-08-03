// $Id: EvtTupleTool.cpp,v 1.7 2010-06-04 13:44:37 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/TupleObj.h"
#include "GaudiAlg/Tuple.h"
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IEventTupleTool.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ICoreFactory.h"   // void & numbers 
#include "LoKi/IHltFactory.h"    // ODIN, HDR & L0 
// ============================================================================
// Event 
// ============================================================================
#include "Event/ODIN.h"
#include "Event/L0DUReport.h"
#include "Event/HltDecReports.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class EvtTupleTool
     *  Simple LoKi-Hybryd tool for "event"-level information
     *
     *   - ODIN           -> double 
     *   - L0DUReport     -> double 
     *   - HltDecReports  -> double 
     *   - void           -> double 
     *
     * 
     *  @code 
     * 
     *    XXXTool.ODIN_Variables = 
     *    { 
     *      "Run" : "ODIN_RUN"
     *    }
     *
     *    XXXTool.ODIN_Variables = 
     *    { 
     *      "L0" : " switch ( L0_DECISION , 1 ,  0 ) "
     *    }
     *
     *    XXXTool.HLT_Variables = 
     *    { 
     *      "size" : " HLT_SIZE "      
     *    }
     *
     *    XXXTool.VOID_Variables = 
     *    { 
     *      "nPV" : " CONTAINS ( 'Rex/Vertex/Primary') "
     *    }
     *
     *  @endcode
     * 
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2010-02-16
     */
    class EvtTupleTool 
      : public               GaudiTool 
      , public virtual IEventTupleTool 
    {
      // ======================================================================
      /// friend factory for instantiation 
      friend class ToolFactory<LoKi::Hybrid::EvtTupleTool> ;
      // ======================================================================
    public:
      // ======================================================================
      /**  Fill the tuple
       *   @see IEventTupleTool
       */
      virtual StatusCode fill( Tuples::Tuple& t ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// standard initialization of the tool 
      virtual StatusCode initialize () ;
      /// standard finalization   of the tool 
      virtual StatusCode finalize   () ;
      // ======================================================================      
    protected:
      // ======================================================================
      /** constructor
       *  @param type   the tool type (??)
       *  @param name   the tool instance name 
       *  @param parent the tool parent 
       */
      EvtTupleTool 
      ( const std::string&  type   ,                      // tool type ??
        const std::string&  name   ,                      // tool instance name 
        const IInterface*   parent ) ;                    // the parent 
      /// vitual & protected destructor 
      virtual ~EvtTupleTool () ;               // vitual & protected destructor 
      // ======================================================================      
    private:
      // ======================================================================      
      /// the default constructor is disabled
      EvtTupleTool () ;                  // the default constructor is disabled
      /// the copy constructor is disabled 
      EvtTupleTool ( const EvtTupleTool& ) ; // the copy constructor is disabled
      /// assignement operator is disabled 
      EvtTupleTool& operator=( const EvtTupleTool& ) ;        // no assignement
      // ======================================================================      
    public:
      // ======================================================================      
      /// the update handler for 'Preambulo'
      void handlePreambulo ( Property& /* p */ )  ;
      /// the update handler for 'CoreFactory'
      void handleCore      ( Property& /* p */ )  ;
      /// the update handler for 'HltFactory'
      void handleHlt       ( Property& /* p */ )  ;
      /// the update handler for ODIN_Variables 
      void handleODIN      ( Property& /* p */ )  ;
      /// the update handler for L0DU_Variables 
      void handleL0DU      ( Property& /* p */ )  ;
      /// the update handler for  HLT_Variables 
      void handleHLT       ( Property& /* p */ )  ;
      /// the update handler for VOID_Variables 
      void handleVOID      ( Property& /* p */ )  ;
      // ======================================================================      
    protected:
      // ======================================================================      
      /// update ODIN-variables 
      StatusCode updateODIN () ;                      // update ODIN-variables 
      /// update L0DU-variables 
      StatusCode updateL0DU () ;                      // update L0DU-variables 
      /// update HLT-variables 
      StatusCode updateHLT  () ;                      // update  HLT-variables 
      /// update VOID-variables 
      StatusCode updateVOID () ;                      // update VOID-variables 
      // ======================================================================      
    protected:
      // ======================================================================
      /// the preambulo 
      std::string preambulo() const 
      {
        const std::vector<std::string>& lines  = m_preambulo ;
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
      /** helper class to keep the N-tuple items
       *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
       *  @date 2010-02-16
       */
      template <class TYPE>
      class Item
      {
      public:
        // ====================================================================
        // the default constructor 
        Item () 
          : m_name () 
          , m_fun  ( typename LoKi::BasicFunctors<TYPE>::Constant ( -1.e+10 ) )
        {}
        // ====================================================================
      public:
        // ====================================================================
        /// the variable name 
        std::string           m_name ; // the variable name 
        /// the functor itself 
        typename LoKi::BasicFunctors<TYPE>::FunctionFromFunction m_fun ;
        // ====================================================================
      } ;
      // ======================================================================      
      /// the actual type of {"name":"functor"} map 
      typedef std::map<std::string,std::string>  Map ;
      /// the actual type of containter of items 
      typedef std::vector<Item<const LHCb::ODIN*> >          ODIN_Items ;
      /// the actual type of containter of items 
      typedef std::vector<Item<const LHCb::L0DUReport*> >    L0_Items   ;
      /// the actual type of containter of items 
      typedef std::vector<Item<const LHCb::HltDecReports*> > HLT_Items  ;
      /// the actual type of containter of items 
      typedef std::vector<Item<void> >                       VOID_Items ;
      // ======================================================================
    private:
      // ======================================================================      
      /// Preambulo:
      std::vector<std::string>  m_preambulo ;                      // Preambulo
      /// CoreFactory name 
      std::string               m_core      ;               // CoreFactory name 
      /// HltFactory  name 
      std::string               m_hlt       ;               //  HltFactory name 
      // ======================================================================      
      /// the { "name" : "functor" } map ( property) 
      Map  m_map_odin ;           // the { "name" : "functor" } map ( property) 
      /// the { "name" : "functor" } map ( property) 
      Map  m_map_l0   ;           // the { "name" : "functor" } map ( property) 
      /// the { "name" : "functor" } map ( property) 
      Map  m_map_hdr  ;           // the { "name" : "functor" } map ( property) 
      /// the { "name" : "functor" } map ( property) 
      Map  m_map_void ;           // the { "name" : "functor" } map ( property) 
      // ======================================================================      
    private:
      // ======================================================================      
      /// the items: ODIN 
      ODIN_Items m_odin ;                                    // the items: ODIN 
      /// the items: L0
      L0_Items   m_l0   ;                                    // the items:   L0
      /// the items: HLT
      HLT_Items  m_hdr  ;                                    // the items:  HLT
      /// the items: void
      VOID_Items m_void ;                                    // the items: void
      // ======================================================================      
    private:
      // ======================================================================      
      /// the TES-location of ODIN
      std::string  m_ODINLocation ;        //          the TES-location of ODIN
      /// the TES-location of L0DUReports  
      std::string  m_L0DULocation ;        //   the TES-location of L0DUReports
      /// the TES-location of HltDecReports 
      std::string  m_HDRLocation  ;        // the TES-location of HltDecReports
      // ======================================================================      
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Hybrid 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
/*  constructor
 *  @param type   the tool type (??)
 *  @param name   the tool instance name 
 *  @param parent the tool parent 
 */
// ============================================================================
LoKi::Hybrid::EvtTupleTool::EvtTupleTool 
( const std::string&  type   ,                      // tool type ??
  const std::string&  name   ,                      // tool instance name 
  const IInterface*   parent )                      // the parent 
  : GaudiTool ( type , name , parent ) 
// preambulo:
  , m_preambulo ( ) 
  , m_core      ( "LoKi::Hybrid::CoreFactory/CoreFactory:PUBLIC" )
  , m_hlt       ( "LoKi::Hybrid::HltFactory/HltFactory:PUBLIC"   ) 
// maps:
  , m_map_odin  ( ) 
  , m_map_l0    ( ) 
  , m_map_hdr   ( ) 
  , m_map_void  ( ) 
// items 
  , m_odin      ( ) 
  , m_l0        ( ) 
  , m_hdr       ( ) 
  , m_void      ( ) 
// TES-locations 
  , m_ODINLocation ( LHCb::ODINLocation          :: Default )
  , m_L0DULocation ( LHCb::L0DUReportLocation    :: Default )
  , m_HDRLocation  ( LHCb::HltDecReportsLocation :: Default ) 
{
  // interface
  declareInterface<IEventTupleTool> ( this ) ;
  //
  if      ( 0 == name.find ( "Hlt1" ) ) 
  { m_core = "LoKi::Hybrid::CoreFactory/Hlt1CoreFactory:PUBLIC"; }
  else if ( 0 == name.find ( "Hlt2" ) ) 
  { m_core = "LoKi::Hybrid::CoreFactory/Hlt1CoreFactory:PUBLIC"; }
  //
  if      ( 0 == name.find ( "Hlt1" ) ) 
  { m_hlt  = "LoKi::Hybrid::HltFactory/Hlt1HltFactory:PUBLIC" ; }
  else if ( 0 == name.find ( "Hlt2" ) ) 
  { m_hlt  = "LoKi::Hybrid::HltFactory/Hlt2HltFactory:PUBLIC" ; }
  //
  declareProperty 
    ( "Preambulo"  , 
      m_preambulo  , 
      "The preambulo lines to be used for the temporary python script" ) 
    -> declareUpdateHandler  ( &LoKi::Hybrid::EvtTupleTool::handlePreambulo , this ) ;
  //
  declareProperty 
    ( "CoreFactory" , 
      m_core        , 
      "The type/name of LoKiBender \"hybrid\" factory: void->double" ) 
    -> declareUpdateHandler  ( &LoKi::Hybrid::EvtTupleTool::handleCore      , this ) ;
  //
  declareProperty 
    ( "HltFactory" , 
      m_hlt        , 
      "The type/name of LoKiBender \"hybrid\" factory: ODIN->double, L0DUReport->double & HltDecReports->double" ) 
    -> declareUpdateHandler  ( &LoKi::Hybrid::EvtTupleTool::handleHlt      , this ) ;
  //
  declareProperty 
    ( "ODIN_Variables" , 
      m_map_odin       , 
      "The map { 'name' : 'functor'} of variables: LHCb::ODIN -> double " ) 
    -> declareUpdateHandler  ( &LoKi::Hybrid::EvtTupleTool::handleODIN , this ) ;
  //
  declareProperty 
    ( "L0DU_Variables" , 
      m_map_l0         , 
      "The map { 'name' : 'functor'} of variables: LHCb::L0DUReport -> double   " ) 
    -> declareUpdateHandler  ( &LoKi::Hybrid::EvtTupleTool::handleL0DU , this ) ;
  //
  declareProperty 
    ( "HLT_Variables"  , 
      m_map_hdr        , 
      "The map { 'name' : 'functor'} of variables:  LHCb::HltDecReports-> double " ) 
    -> declareUpdateHandler ( &LoKi::Hybrid::EvtTupleTool::handleHLT , this ) ;
  //
  declareProperty 
    ( "VOID_Variables" , 
      m_map_void       , 
      "The map { 'name' : 'functor'} of variables: void -> double "     ) 
    -> declareUpdateHandler ( &LoKi::Hybrid::EvtTupleTool::handleVOID , this ) ;
  // 
  // TES-locations 
  //
  declareProperty 
    ( "ODIN_Location" , 
      m_ODINLocation  , 
      "TES-location of LHCb::ODIN object"          ) ;
  //
  declareProperty 
    ( "L0DU_Location" , 
      m_L0DULocation  , 
      "TES-location of LHCb::L0DUReport object"    ) ;
  //
  declareProperty 
    ( "HLT_Location"  , 
      m_HDRLocation   , 
      "TES-location of LHCb::HltDecReports object" ) ;
  //
}
// ============================================================================
// virtual & protected destructor 
// ============================================================================
LoKi::Hybrid::EvtTupleTool::~EvtTupleTool()
{
  m_odin . clear() ;
  m_l0   . clear() ;
  m_hdr  . clear() ;
  m_void . clear() ;
}
// ============================================================================
// standard initialization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EvtTupleTool::initialize () 
{
  StatusCode sc = GaudiTool::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  svc<ILoKiSvc> ( "LoKiSvc" , true ) ;
  // decode variables:
  if  ( ! m_map_odin.empty() ) 
  {
    sc = updateODIN () ;
    if ( sc.isFailure () ) { return Error ( "Unable to decode ODIN_Variables", sc ) ; }  
  }
  // ==========================================================================
  if  ( ! m_map_l0  .empty() ) 
  {
    sc = updateL0DU () ;
    if ( sc.isFailure () ) { return Error ( "Unable to decode L0DU_Variables", sc ) ; } 
  }
  // ==========================================================================
  if  ( ! m_map_hdr .empty() ) 
  {
    sc = updateHLT  () ;
    if ( sc.isFailure () ) { return Error ( "Unable to decode  HLT_Variables", sc ) ; } 
  }
  // ==========================================================================
  if  ( ! m_map_void.empty() ) 
  {
    sc = updateVOID () ;
    if ( sc.isFailure () ) { return Error ( "Unable to decode VOID_Variables", sc ) ; } 
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================ 
// standard finalization   of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EvtTupleTool::finalize   () 
{
  m_odin . clear() ;
  m_l0   . clear() ;
  m_hdr  . clear() ;
  m_void . clear() ;
  //
  return GaudiTool::finalize () ;
}
// ============================================================================
// the update handler for Preambulo
// ============================================================================
void LoKi::Hybrid::EvtTupleTool::handlePreambulo ( Property& /* p */ )  
{
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  Warning ( "Reintialization of Preambulo" ).ignore() ;
  //
  StatusCode sc = updateODIN () ;
  Assert ( sc.isSuccess () , "Unable to set 'ODIN_Variables'"   , sc ) ;
  sc = updateL0DU () ;
  Assert ( sc.isSuccess () , "Unable to set 'L0DU_Variables'"   , sc ) ;
  sc = updateHLT  () ;
  Assert ( sc.isSuccess () , "Unable to set  'HLT_Variables'"   , sc ) ;
  sc = updateVOID () ;
  Assert ( sc.isSuccess () , "Unable to set 'VOID_Variables'"   , sc ) ;
}    
// ============================================================================
// the update handler for HltFactory
// ============================================================================
void LoKi::Hybrid::EvtTupleTool::handleHlt ( Property& /* p */ )  
{
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  Warning ( "Reintialization of HltFactory" ).ignore() ;
  //
  StatusCode sc = updateODIN () ;
  Assert ( sc.isSuccess() , "Unable to set 'ODIN_Variables'"   , sc ) ;
  sc = updateL0DU () ;
  Assert ( sc.isSuccess() , "Unable to set 'L0DU_Variables'"   , sc ) ;
  sc = updateHLT  () ;
  Assert ( sc.isSuccess() , "Unable to set  'HLT_Variables'"   , sc ) ;
}    
// ============================================================================
// The update handler for CoreFactory
// ============================================================================
void LoKi::Hybrid::EvtTupleTool::handleCore ( Property& /* p */ )  
{
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  Warning ( "Reintialization of CoreFactory" ).ignore() ;
  //
  StatusCode sc = updateVOID () ;
  Assert ( sc.isSuccess() , "Unable to set 'VOID_Variables'"   , sc ) ;
}    
// ============================================================================
// The update handler for VOID_Variables
// ============================================================================
void LoKi::Hybrid::EvtTupleTool::handleVOID ( Property& /* p */ )  
{
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  Warning ( "Reintialization of VOID_Varaibles" ).ignore() ;
  //
  StatusCode sc = updateVOID () ;
  Assert ( sc.isSuccess() , "Unable to set 'VOID_Variables'"   , sc ) ;
}    
// ============================================================================
// The update handler for ODIN_Variables
// ============================================================================
void LoKi::Hybrid::EvtTupleTool::handleODIN ( Property& /* p */ )  
{
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  Warning ( "Reintialization of ODIN_Varaibles" ).ignore() ;
  //
  StatusCode sc = updateODIN () ;
  Assert ( sc.isSuccess() , "Unable to set 'ODIN_Variables'"   , sc ) ;
}    
// ============================================================================
// The update handler for L0DU_Variables
// ============================================================================
void LoKi::Hybrid::EvtTupleTool::handleL0DU ( Property& /* p */ )  
{
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  Warning ( "Reintialization of L0DU_Varaibles" ).ignore() ;
  //
  StatusCode sc = updateL0DU () ;
  Assert ( sc.isSuccess() , "Unable to set 'L0DU_Variables'"   , sc ) ;
}    
// ============================================================================
// The update handler for HLT_Variables
// ============================================================================
void LoKi::Hybrid::EvtTupleTool::handleHLT ( Property& /* p */ )  
{
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  Warning ( "Reintialization of HLT_Variables" ).ignore() ;
  //
  StatusCode sc = updateHLT  () ;
  Assert ( sc.isSuccess() , "Unable to set  'HLT_Variables'"   , sc ) ;
}    
// ============================================================================
// update ODIN-variables 
// ============================================================================
StatusCode LoKi::Hybrid::EvtTupleTool::updateODIN ()  // update ODIN-variables 
{
  // get the factory
  IHltFactory* factory = tool<IHltFactory> ( m_hlt , this ) ;
  //
  m_odin.clear() ;
  m_odin.reserve ( m_map_odin.size() ) ;
  for ( Map::const_iterator ivar = m_map_odin.begin() ; 
        m_map_odin.end() != ivar ; ++ivar ) 
  {
    Item<const LHCb::ODIN*> item ;
    StatusCode sc = factory->get ( ivar->second , item.m_fun , preambulo() ) ;
    if ( sc.isFailure() )
    { return Error
        ("Unable to decode " + ivar->first + " : " + ivar->second , sc ) ; }
    //
    item.m_name = ivar->first ;
    m_odin.push_back  ( item ) ; 
    //
    debug() << "The decoded variable name is '"
            << m_odin.back().m_name << "'\t, the functor : '"
            << m_odin.back().m_fun  << "'" << endmsg ;
  }  
  // 
  release ( factory ) ; // we do not need the factory anymore 
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// update L0DU-variables 
// ============================================================================
StatusCode LoKi::Hybrid::EvtTupleTool::updateL0DU ()  // update ODIN-variables 
{
  // get the factory
  IHltFactory* factory = tool<IHltFactory> ( m_hlt , this ) ;
  //
  m_l0.clear() ;
  m_l0.reserve ( m_map_l0.size() ) ;
  for ( Map::const_iterator ivar = m_map_l0.begin() ; 
        m_map_l0.end() != ivar ; ++ivar ) 
  {
    Item<const LHCb::L0DUReport*> item ;
    StatusCode sc = factory->get ( ivar->second , item.m_fun , preambulo() ) ;
    if ( sc.isFailure() )
    { return Error
        ("Unable to decode " + ivar->first + " : " + ivar->second , sc ) ; }
    //
    item.m_name = ivar->first ;
    m_l0.push_back  ( item ) ; 
    //
    debug() << "The decoded variable name is '"
            << m_l0 .back().m_name << "'\t, the functor : '"
            << m_l0 .back().m_fun  << "'" << endmsg ;
  }  
  // 
  release ( factory ) ; // we do not need the factory anymore 
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// update  HLT-variables 
// ============================================================================
StatusCode LoKi::Hybrid::EvtTupleTool::updateHLT ()  // update HLT-variables 
{
  // get the factory
  IHltFactory* factory = tool<IHltFactory> ( m_hlt , this ) ;
  //
  m_hdr.clear() ;
  m_hdr.reserve ( m_map_hdr.size() ) ;
  for ( Map::const_iterator ivar = m_map_hdr.begin() ; 
        m_map_hdr.end() != ivar ; ++ivar ) 
  {
    Item<const LHCb::HltDecReports*> item ;
    StatusCode sc = factory->get ( ivar->second , item.m_fun , preambulo() ) ;
    if ( sc.isFailure() )
    { return Error
        ("Unable to decode " + ivar->first + " : " + ivar->second , sc ) ; }
    //
    item.m_name = ivar->first ;
    m_hdr.push_back  ( item ) ; 
    //
    debug() << "The decoded variable name is '"
            << m_hdr .back().m_name << "'\t, the functor : '"
            << m_hdr .back().m_fun  << "'" << endmsg ;
  }  
  // 
  release ( factory ) ; // we do not need the factory anymore 
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// update  Void-variables 
// ============================================================================
StatusCode LoKi::Hybrid::EvtTupleTool::updateVOID ()  // update VOID-variables 
{
  // get the factory
  ICoreFactory* factory = tool<ICoreFactory> ( m_core , this ) ;
  //
  m_void.clear() ;
  m_void.reserve ( m_map_void.size() ) ;
  for ( Map::const_iterator ivar = m_map_void.begin() ; 
        m_map_void.end() != ivar ; ++ivar ) 
  {
    Item<void> item ;
    StatusCode sc = factory->get ( ivar->second , item.m_fun , preambulo() ) ;
    if ( sc.isFailure() )
    { return Error
        ("Unable to decode " + ivar->first + " : " + ivar->second , sc ) ; }
    //
    item.m_name = ivar->first ;
    m_void.push_back  ( item ) ; 
    //
    debug() << "The decoded variable name is '"
            << m_void.back().m_name << "'\t, the functor : '"
            << m_void.back().m_fun  << "'" << endmsg ;
  }  
  // 
  release ( factory ) ; // we do not need the factory anymore 
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  Fill the tuple
 *   @see IEventTupleTool
 */
// ============================================================================
StatusCode LoKi::Hybrid::EvtTupleTool::fill( Tuples::Tuple& t ) 
{
  if ( !t.valid() ) { return Error ( "Tuples::Tuple object is invalid!" ) ; }
  //
  StatusCode sc = StatusCode ( StatusCode::SUCCESS , true ) ;
  // ==========================================================================
  // ODIN
  if ( !m_odin.empty() ) 
  {
    const LHCb::ODIN* odin = get<LHCb::ODIN> ( m_ODINLocation ) ;
    //
    for ( ODIN_Items::const_iterator item = m_odin.begin() ; 
          m_odin.end() != item ; ++item ) 
    {
      StatusCode code = t->column ( item->m_name , item->m_fun ( odin ) ) ;
      if ( code.isFailure() ) 
      {
        Warning ( "Error in column('" + item -> m_name + "')" , code ).ignore() ;
        sc = code ;
      }
    }
  }
  // ==========================================================================
  // L0 
  if ( !m_l0.empty() ) 
  {
    const LHCb::L0DUReport* l0 = get<LHCb::L0DUReport> ( m_L0DULocation ) ;
    //
    for ( L0_Items::const_iterator item = m_l0.begin() ; 
          m_l0.end() != item ; ++item ) 
    {
      StatusCode code = t->column ( item->m_name , item->m_fun ( l0 ) ) ;
      if ( code.isFailure() ) 
      {
        Warning ( "Error in column('" + item -> m_name + "')" , code ).ignore() ;
        sc = code ;
      }
    }
  }
  // ==========================================================================
  // HLT
  if ( !m_hdr.empty() ) 
  {
    const LHCb::HltDecReports* hdr = get<LHCb::HltDecReports> ( m_HDRLocation ) ;
    //
    for ( HLT_Items::const_iterator item = m_hdr.begin() ; 
          m_hdr.end() != item ; ++item ) 
    {
      StatusCode code = t->column ( item->m_name , item->m_fun ( hdr ) ) ;
      if ( code.isFailure() ) 
      {
        Warning ( "Error in column('" + item -> m_name + "')" , code ).ignore() ;
        sc = code ;
      }
    }
  }
  // ==========================================================================
  // VOID
  for ( VOID_Items::const_iterator item = m_void.begin() ; 
        m_void.end() != item ; ++item ) 
  {
    StatusCode code = t->column ( item->m_name , item->m_fun () ) ;
    if ( code.isFailure() ) 
    {
      Warning ( "Error in column('" + item -> m_name + "')" , code ).ignore() ;
      sc = code ;
    }
  }
  //
  return sc ;
}
// ============================================================================




// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,EvtTupleTool)
// ============================================================================


// ============================================================================
// The END 
// ============================================================================



