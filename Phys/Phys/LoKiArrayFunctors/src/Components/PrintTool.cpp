// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IPrintDecay.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ParticleProperties.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/IHybridFactory.h"
#include "LoKi/PrintDecay.h"
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
    /** @class PrintTool 
     *  The  simplest "LoKi::Hybrid"-based implementation of 
     *  the abstract interface IPrintDecay 
     *  @see IPrintDecay
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-30
     */
    class PrintTool 
      : public virtual IPrintDecay 
      , public          GaudiTool
    {
      // ======================================================================
      /// the friend factory needed for instantiation
      friend class ToolFactory<LoKi::Hybrid::PrintTool> ;
      // ======================================================================
    public:
      // ======================================================================
      /** print the decay tree of the particle 
       *  @param particle the particle to printed 
       *  @param maxDepth the maximal depth
       */
      void printTree 
      ( const LHCb::Particle* particle , 
        int                   maxDepth ) ;
      // ======================================================================
    public:
      // ======================================================================
      /** initialize the tool
       *  @see IAlgTool
       *  @return status code 
       */
      virtual StatusCode initialize () 
      {
        StatusCode sc = GaudiTool::initialize() ;
        if ( sc.isFailure() ) { return sc ; }           // RETURN 
        // decode the variables:
        sc = initVars() ;
        if ( sc.isFailure() ) { return sc ; }           // RETURN 
        //
        return initTable() ;
      }
      /** finalize the tool
       *  @see IAlgTool
       *  @return status code 
       */
      virtual StatusCode finalize  () 
      {
        // reset all functors:
        m_funcs.clear() ;
        return GaudiTool::finalize () ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      /// init variables 
      StatusCode initVars () 
      {
        // decode the variables:
        StatusCode sc = decodeVars() ;
        if ( sc.isFailure() ) { return sc ; }           // RETURN 
        //
        if ( m_funcs.size() != m_vars.size() ) { return Error ( "Wrong decoding?" ) ; }
        //
        return StatusCode::SUCCESS ;
      }
      /// init the table 
      StatusCode initTable ()
      {
        //
        if ( m_format.empty() ) 
        {
          m_format = "|" ;
          for ( unsigned int i = 0 ; i < m_funcs.size() ; ++i ) 
          { m_format += "%|=15.5g||" ; } 
        }
        if ( m_header.empty() ) 
        { 
          // predefine:
          m_header = "|" ;
          for ( unsigned int i = 0 ; i < m_funcs.size() ; ++i ) { m_header += "%|=15.15s||" ; }
          boost::format fmt ( m_header ) ;
          // make the printouts:
          using namespace boost::io ;
          fmt.exceptions ( all_error_bits ^ ( too_many_args_bit | too_few_args_bit ) ) ;
          for ( Funcs::const_iterator ifun = m_funcs.begin() ; 
                m_funcs.end() != ifun ; ++ifun ) 
          { fmt % ifun->m_fun.printOut() ; }    
          // get the format back
          m_header = fmt.str() ;
        } 
        //
        return StatusCode::SUCCESS ;                    // RETURN        
      }
      // ======================================================================
    public:
      // ======================================================================
      /// the preambulo
      std::string preambulo() const { return _preambulo( m_preambulo ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// the update handler: variables , preambulo & factory
      void propHandler1 ( Property& /* p */ )  
      {
        //
        if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
        //
        StatusCode sc = initVars () ;
        Assert ( sc.isSuccess() , "Unable to set 'Variables'"   , sc ) ;
      }
      /// the update handler: format & header 
      void propHandler2 ( Property& /* p */ )  
      {
        //
        if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
        //
        StatusCode sc = initTable () ;
        Assert ( sc.isSuccess() , "Unable to set 'Table'"   , sc ) ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      /** standard constructor 
       *  @param type tool type (?)
       *  @param name the instance name 
       *  @param parent the parent
       */
      PrintTool 
      ( const std::string& type   ,          // tool type (?)
        const std::string& name   ,          // the instance name 
        const IInterface*  parent )          // the parent  
        : GaudiTool ( type, name , parent )
        , m_vars        (   ) 
        , m_funcs       (   ) 
        , m_format      (   )
        , m_header      (   )
        , m_factory     ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC") 
        , m_preambulo    () 
      {
        declareInterface<IPrintDecay> ( this ) ;
        //
        if      ( 0 == name.find("Hlt1") ) 
        { m_factory = "LoKi::Hybrid::Tool/Hlt1HybridFactory:PUBLIC" ; }
        else if ( 0 == name.find("Hlt2") ) 
        { m_factory = "LoKi::Hybrid::Tool/Hlt2HybridFactory:PUBLIC" ; }
        //
        declareProperty 
          ( "Format"    , 
            m_format    , 
            "The table format " ) ->
          declareUpdateHandler 
          ( &LoKi::Hybrid::PrintTool::propHandler2 , this ) ;
        //
        declareProperty 
          ( "Header"    , 
            m_header    , 
            "The table header " ) ->
          declareUpdateHandler
          ( &LoKi::Hybrid::PrintTool::propHandler2 , this ) ;
        //
        declareProperty 
          ( "Variables" , 
            m_vars      , 
            "The list of variables " ) ->
          declareUpdateHandler 
          ( &LoKi::Hybrid::PrintTool::propHandler1 , this ) ;
        //
        declareProperty 
          ( "Factory"   , 
            m_factory   , 
            "Type/Name for C++/Python Hybrid Factory" ) ->
          declareUpdateHandler 
          ( &LoKi::Hybrid::PrintTool::propHandler1 , this ) ;
        //
        declareProperty 
          ( "Preambulo" , 
            m_preambulo , 
            "The preambulo to be used for Bender/Python script" ) ->
          declareUpdateHandler 
          ( &LoKi::Hybrid::PrintTool::propHandler1 , this ) ;
      } 
      /// virtual & proptected destructor 
      virtual ~PrintTool() {}
      // ======================================================================      
    private:
      // ======================================================================      
      /// the default constructor is disabled  
      PrintTool () ;                    // the default constructor is disabled  
      /// the copy constructor is disabled  
      PrintTool            ( const PrintTool& ) ;   // no default constructor 
      /// the assignement operator is disabled  
      PrintTool& operator= ( const PrintTool& ) ;   // no assignement  
      // ======================================================================      
    protected:
      // ======================================================================      
      /** decode the variables 
       *  @return status code
       */
      StatusCode decodeVars () ;
      // ======================================================================
      /** perform the actual printpout 
       *  @param particle the particle 
       *  @param stream   the stream 
       *  @param prefix   the prefix 
       *  @param depth    the depth  (stop at depth equal to 0 )
       */
      inline std::ostream& print 
      ( const LHCb::Particle* particle       , 
        std::ostream&         stream         , 
        const int             depth          ,
        const int             maxdepth = -1  ,
        const std::string&    prefix   = ""  ) const ;
      // ======================================================================
      /** perform the actual printpout 
       *  @param begin the begin of the particles sequence 
       *  @param end   the end   of the particles sequence 
       *  @param stream   the stream 
       *  @param prefix   the prefix 
       *  @param depth    the depth  (stop at depth equal to 0 )
       */
      template <class PARTICLE>
      inline std::ostream& print
      ( PARTICLE           begin    , 
        PARTICLE           end      , 
        std::ostream&      stream   ,
        const int          depth    ,
        const int          maxdepth , 
        const std::string& prefix   ) const 
      {
        for ( ; begin != end ; ++begin ) 
        { print ( *begin , stream , depth , maxdepth , prefix ) ; }
        return stream ;
      }
      // ======================================================================
      /// make the actual printout 
      std::string eval ( const LHCb::Particle* particle ) const ;      
      // ======================================================================
    protected:
      // ======================================================================
      /** @struct MyFunc 
       *  the small helper structure to keep the function
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-03-30
       */
      struct MyFunc
      {
      public:
        // ====================================================================
        /// the default constriuctor
        MyFunc () 
        : m_fun ( LoKi::BasicFunctors<const LHCb::Particle*>::Constant( -1.e+10 ) ) 
        {}
        // ====================================================================
      public:
        // ====================================================================
        /// the function itself
        LoKi::PhysTypes::Fun m_fun ; // the function to be evaluated 
        // ====================================================================
      } ;
      /// the actual type for the vector of functions
      typedef std::vector<MyFunc> Funcs ;
      // ======================================================================
    private:
      // ======================================================================
      typedef std::vector<std::string> Vars ;
      /// the vector of variables (code)
      Vars        m_vars    ;                 // the vector of variables (code)
      /// the actual vector of functions 
      Funcs       m_funcs   ;                 // the actual vector of functions
      /// the table format 
      std::string m_format  ;                               // the table format
      /// the table header 
      std::string m_header  ;                               // the table header 
      /// the factory 
      std::string m_factory ;                                 //    the factory
      /// the preambulo
      std::vector<std::string> m_preambulo ;                  //  the preambulo
      // ======================================================================
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Hybrid
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
/* decode the variables 
 *  @return status code
 */
// ============================================================================
StatusCode LoKi::Hybrid::PrintTool::decodeVars () 
{
  // clear the existing vector of variables:
  m_funcs.clear() ;
  // locate the factory 
  IHybridFactory* factory = tool<IHybridFactory> ( m_factory , this ) ;
  
  for ( Vars::const_iterator ivar = m_vars.begin() ; 
        m_vars.end() != ivar ; ++ivar )
  {
    MyFunc fun ;
    StatusCode sc = factory->get ( *ivar , fun.m_fun , preambulo() ) ;
    if ( sc.isFailure() ) 
    { return Error ( "Unable to decode '" + (*ivar) + "'" , sc) ; } // RETURN
    // add the decoded functor into the list 
    m_funcs.push_back ( fun ) ;
  }
  // release the factory
  release ( factory ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  print the decay tree of the particle 
 *  @param particle th eparticle to printed 
 *  @param maxDepth the maximal depth
 */
// ============================================================================
void LoKi::Hybrid::PrintTool::printTree 
( const LHCb::Particle* particle , 
  int                   maxDepth ) 
{
  if ( 0 == particle ) 
  {
    Error ( "LHCb::Particle* (the head of the tree ) points to NULL") ;
    return ;                                                          // RETURN 
  }
  MsgStream& msg = info () ;
  if ( msg.isActive() ) 
  {    
    msg << "Print the decay tree: " 
        << LoKi::Print::printDecay ( particle ) 
        << std::endl ;
    boost::format fmt ( " |#%1$=22.22s%2$s" ) ;
    fmt % "Decay" % m_header ;
    const std::string header = fmt.str() ;
    msg << " " << std::string( header.size()-1 , '-' ) << std::endl ;
    msg << header << std::endl ;
    msg << " " << std::string( header.size()-1 , '-' ) << std::endl ;
    print ( particle , msg.stream() , 0 , maxDepth ) ;
    msg << " " << std::string( header.size()-1 , '-' ) << std::endl ;
    msg << endreq ;
  }
}
// ===========================================================================
/*  perform the actual printpout 
 *  @param particle the particle 
 *  @param stream   the stream 
 *  @param prefix   the prefix 
 *  @param depth    the depth  
 *  @param maxdepth the depth  (stop at depth equal to 0 )
 */
// ===========================================================================
inline std::ostream& LoKi::Hybrid::PrintTool::print 
( const LHCb::Particle* particle  , 
  std::ostream&         stream    , 
  const int             depth     ,
  const int             maxdepth  ,
  const std::string&    prefix    ) const 
{
  //
  if ( 0 == particle ) { return stream ; }             // RETURN 
  //
  // the format of the row 
  boost::format fmt ( " |%1$-2d%2$s|--> %3$s %|25t|%4$s" ) ;
  // allow various number of arguments 
  using namespace boost::io ;
  fmt.exceptions ( all_error_bits ^ ( too_many_args_bit | too_few_args_bit ) ) ;
  // format the row 
  fmt 
    % depth 
    % prefix 
    % LoKi::Particles::nameFromPID ( particle -> particleID () ) 
    % eval ( particle ) ;
  // make the actual printout 
  stream << fmt << std::endl ;
  //
  if ( 0 <= maxdepth && maxdepth < depth ) { return stream ; }  // RETURN 
  // print the daughters
  const SmartRefVector<LHCb::Particle>& daughters = particle->daughters() ;
  //
  return
    print ( daughters.begin () , 
            daughters.end   () , 
            stream             , 
            depth  + 1         , 
            maxdepth           , 
            prefix + "   "     ) ;
}
// ============================================================================
// make the actual printout 
// ============================================================================
std::string LoKi::Hybrid::PrintTool::eval 
( const LHCb::Particle* particle ) const 
{
  if ( 0 == particle ) { return "<NULL>" ; }
  // the format of the row 
  boost::format fmt ( m_format ) ;
  // allow the various number of arguments 
  using namespace boost::io ;
  fmt.exceptions ( all_error_bits ^ ( too_many_args_bit | too_few_args_bit ) ) ;
  // format the row 
  for ( Funcs::const_iterator ifun = m_funcs.begin() ; 
        m_funcs.end() != ifun ; ++ifun ) 
  {
    /// evaluate the function 
    const double result = ifun->m_fun ( particle ) ;
    /// print the result:
    fmt % result ;
  }
  return fmt.str() ;                                            // RETURN 
}
// ============================================================================
/// the factory: needed for instantistion
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,PrintTool) 
// ============================================================================
// The END 
// ============================================================================


