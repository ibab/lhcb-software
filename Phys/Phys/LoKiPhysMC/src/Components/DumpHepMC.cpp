// $Id: DumpHepMC.cpp,v 1.3 2008-06-25 17:27:39 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IService.h" 
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h" 
// ============================================================================
// GenEvent 
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Objects.h"
#include "LoKi/DecayChain.h"
#include "LoKi/BuildGenTrees.h"
#include "LoKi/GenExtract.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class DumpHepMC    
   *  Simple class to dump HepMC-infomration 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-05-04 
   */
  class DumpHepMC : public GaudiAlgorithm 
  {
    /// the friend factory (needed for instantiations 
    friend class AlgFactory<LoKi::DumpHepMC> ;
  public:
    // ========================================================================
    /// standard initialization of the algorithm
    virtual StatusCode initialize () 
    {
      StatusCode sc = GaudiAlgorithm::initialize() ;
      if ( sc.isFailure() ) { return sc ; }
      /// get LoKi service 
      svc<IService>( "LoKiSvc" , true ) ;
      return StatusCode::SUCCESS ;
    }
    /// the only one essential method : execution of the algorithm 
    virtual StatusCode execute()  ;
    // ========================================================================
  protected:
    // ========================================================================
    /** The standard constructor 
     *  @param name algorithm instance name 
     *  @param svc  Service Locator 
     */
    DumpHepMC ( const std::string& name , // algorithm instance name
                ISvcLocator*       svc  ) // service locator 
      : GaudiAlgorithm ( name , svc ) 
      , m_input ( LHCb::HepMCEventLocation::Default )  // input TES-location
      , m_depth   ( 7    )  // the maximal printout depth 
      , m_vertex  ( true )  // print vertex information 
      , m_vertexe ( true )  // print end-vertex information?
      , m_mode    ( LoKi::DecayChain::LV_WITHPT )  // printout mode 
    {
      declareProperty 
        ( "Input" , m_input , "The TES-location of HepMC data" ) ;
      declareProperty 
        ( "Depth" , m_depth , "The maximal printout depth" ) ;
      declareProperty 
        ( "PrintVertex" , m_vertex , "Print vertex information?" ) ;
      declareProperty 
        ( "PrintEndVertex" , m_vertexe , "Print end-vertex information?" ) ;
      declareProperty 
        ( "Mode" , m_mode , "Printout mode, see LoKi::DecayChain::Mode" ) ;
    }
    /// virtual and protected desctructor 
    virtual ~DumpHepMC() {}
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    // ========================================================================
    DumpHepMC () ;                                    // no default constructor 
    /// copy constructor is disabled 
    DumpHepMC ( const DumpHepMC& right ) ;               // no copy constructor
    /// assignment operator is disabled 
    DumpHepMC& operator=( const DumpHepMC& right ) ;          // no assignement 
    // ========================================================================
  private:
    // ========================================================================
    /// the TES-location of HepMC data 
    std::string    m_input   ; // the TES-location of HepMC data 
    /// the maximal printout dephth 
    unsigned short m_depth   ; // the maximal printout dephth 
    /// print vertex information ? 
    bool           m_vertex  ; // print vertex information ? 
    /// print end-vertex information ? 
    bool           m_vertexe ; // print end-vertex information ? 
    /// printout mode  ( @see LoKi::DecayChain::Mode )
    unsigned short m_mode    ;  // printout mode 
    // ========================================================================    
  } ;
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
//the only one essential method : execution of the algorithm 
StatusCode LoKi::DumpHepMC::execute()  
{
  // get HepMC data:
  const LHCb::HepMCEvent::Container* events = 
    get<LHCb::HepMCEvent::Container> ( m_input ) ;
  
  LoKi::DecayChain::Mode mode = LoKi::DecayChain::LV_WITHPT ;
  
  switch ( m_mode ) 
  {
  case LoKi::DecayChain::LV_STD    : 
    mode =  LoKi::DecayChain::LV_STD    ; break ;
  case LoKi::DecayChain::LV_MASS   : 
    mode =  LoKi::DecayChain::LV_MASS   ; break ;
  case LoKi::DecayChain::LV_WITHPT : 
    mode =  LoKi::DecayChain::LV_WITHPT ; break ;
  case LoKi::DecayChain::LV_ONLYP  : 
    mode =  LoKi::DecayChain::LV_ONLYP  ; break ;
  case LoKi::DecayChain::LV_SHORT  : 
    mode =  LoKi::DecayChain::LV_SHORT  ; break ;
  default: 
    mode =  LoKi::DecayChain::LV_WITHPT ; break ; 
  }
  
  // get the smart printer 
  LoKi::DecayChain printer 
    ( m_depth   , 
      m_vertex  , 
      m_vertexe , 
      mode      ) ;
  
  MsgStream&    log    = always()     ;
  //
  log << " HepMC event dump '" << m_input << "' " 
      << std::endl ;
  
  printer.print 
    ( events       , // input data 
      log.stream() , // the stream 
      '\n'         , // the terminator 
      LoKi::Objects::_ALL<const void*>  () ,  // accept 
      LoKi::Objects::_NONE<const void*> () ); // mark
  
  log << endreq ;

  //
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
/// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,DumpHepMC)
// ============================================================================
// The END 
// ============================================================================

