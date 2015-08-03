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
#include "GaudiAlg/GaudiAlgorithm.h" 
// ============================================================================
// MCEvent 
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Objects.h"
#include "LoKi/MCDecayChain.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2011-06-03
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class DumpMC    
   *  Simple class to dump MC-information 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-05-04 
   */
  class DumpMC : public GaudiAlgorithm 
  {
    // ========================================================================
    /// the friend factory (needed for instantiations 
    friend class AlgFactory<LoKi::DumpMC> ;
    // ========================================================================
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
    DumpMC ( const std::string& name , // algorithm instance name
             ISvcLocator*       svc  ) // service locator 
      : GaudiAlgorithm ( name , svc ) 
      , m_input ( LHCb::MCParticleLocation::Default )  // input TES-location
      , m_depth   ( 10   )  // the maximal printout depth 
      , m_vertex  ( true )  // print vertex information 
      , m_vertexe ( true )  // print end-vertex information?
      , m_vertexd ( true )  // print decay-only vertices ?
      , m_mode    ( LoKi::DecayChainBase::LV_WITHPT )  // printout mode 
    {
      declareProperty 
        ( "Input" , m_input , "The TES-location of MC data" ) ;
      declareProperty 
        ( "Depth" , m_depth , "The maximal printout depth" ) ;
      declareProperty 
        ( "PrintVertex" , m_vertex , "Print vertex information?" ) ;
      declareProperty 
        ( "PrintEndVertex" , m_vertexe , "Print end-vertex information?" ) ;
      declareProperty 
        ( "DecayOnlyVertices" , m_vertexd , "Decay-only vertices ?" ) ;
      declareProperty 
        ( "Mode" , m_mode , "Printout mode, see LoKi::DecayChainBase::Mode" ) ;
    }
    /// virtual and protected desctructor 
    virtual ~DumpMC() {}
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    // ========================================================================
    DumpMC () ;                                       // no default constructor 
    /// copy constructor is disabled 
    DumpMC ( const DumpMC& right ) ;                     // no copy constructor
    /// assignment operator is disabled 
    DumpMC& operator=( const DumpMC& right ) ;                // no assignement 
    // ========================================================================
  private:
    // ========================================================================
    /// the TES-location of MC data 
    std::string    m_input   ; // the TES-location of MC data 
    /// the maximal printout dephth 
    unsigned short m_depth   ; // the maximal printout dephth 
    /// print vertex information ? 
    bool           m_vertex  ; // print vertex information ? 
    /// print end-vertex information ? 
    bool           m_vertexe ; // print end-vertex information ? 
    /// print only decay-vertices ?
    bool           m_vertexd ; // print only decay-vertices ? 
    /// printout mode  ( @see LoKi::DecayChainBase::Mode )
    unsigned short m_mode    ;  // printout mode 
    // ========================================================================    
  } ;
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// the only one essential method : execution of the algorithm 
// ============================================================================
StatusCode LoKi::DumpMC::execute()  
{
  // get MC data:
  const LHCb::MCParticle::Container* particles = 
    get<LHCb::MCParticle::Container> ( m_input ) ;
  
  //
  LoKi::DecayChainBase::Mode mode = LoKi::DecayChainBase::LV_WITHPT ;
  switch ( m_mode ) 
  {
  case LoKi::DecayChainBase::LV_STD        : 
    mode =  LoKi::DecayChainBase::LV_STD        ; break ;
  case LoKi::DecayChainBase::LV_MASS       : 
    mode =  LoKi::DecayChainBase::LV_MASS       ; break ;
  case LoKi::DecayChainBase::LV_WITHPT     : 
    mode =  LoKi::DecayChainBase::LV_WITHPT     ; break ;
  case LoKi::DecayChainBase::LV_ONLYP      : 
    mode =  LoKi::DecayChainBase::LV_ONLYP      ; break ;
  case LoKi::DecayChainBase::LV_SHORT      : 
    mode =  LoKi::DecayChainBase::LV_SHORT      ; break ;
  case LoKi::DecayChainBase::LV_MPTYPHI    : 
    mode =  LoKi::DecayChainBase::LV_MPTYPHI    ; break ;
  case LoKi::DecayChainBase::LV_MPTETAPHI  : 
    mode =  LoKi::DecayChainBase::LV_MPTETAPHI  ; break ;
  case LoKi::DecayChainBase::LV_MPTY       : 
    mode =  LoKi::DecayChainBase::LV_MPTY       ; break ;
  case LoKi::DecayChainBase::LV_MPTETA     : 
    mode =  LoKi::DecayChainBase::LV_MPTETA     ; break ;
  default: 
    mode =  LoKi::DecayChainBase::LV_WITHPT     ; break ; 
  }
  //
  // get the smart printer 
  //
  LoKi::MCDecayChain printer 
    ( m_depth     , 
      m_vertex    ,   
      mode        , 
      MSG::YELLOW , 
      MSG::RED    ,
      m_vertexe   , 
      m_vertexd   ) ;
  //
  MsgStream&    log    = always()     ;
  //
  log << " MC-particles dump '" << m_input << "' " <<  std::endl ;
  printer.print 
    ( particles             ,     // the input 
      log.stream()          ,     // the stream 
      '\n'                  ,     // the terminator 
      LoKi::Objects::_ALL_  ,     // accept 
      LoKi::Objects::_NONE_ ) ;   // mark
  //
  log << endmsg ;
  //
  return StatusCode::SUCCESS ;
} 
// ============================================================================
/// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,DumpMC)
// ============================================================================
// The END 
// ============================================================================

