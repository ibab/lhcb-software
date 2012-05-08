// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <map>
#include <set>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IAlgContextSvc.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/DVAlgorithm.h"
#include "Kernel/IDecodeSimpleDecayString.h"
#include "Kernel/GetDecay.h"
#include "Kernel/GetParticlesForDecay.h"
#include "Kernel/IPlotTool.h"
#include "Kernel/ISetInputParticles.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/ATypes.h"
#include "LoKi/IHybridFactory.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/CompareParticles.h"
// ============================================================================
#include "LoKi/Selected.h"
#include "LoKi/Combiner.h"
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/ATypes.h"
#include "LoKi/Particles0.h"
#include "LoKi/PIDOperators.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/algorithm/string.hpp"
// ============================================================================
namespace
{
  // ==========================================================================
  /// make the plots ?
  inline bool validPlots ( const std::string& name )
  {
    if ( name.empty() ) { return false ; }
    return "none" != boost::to_lower_copy ( name ) ;
  }
  /// local "none"
  typedef LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant       _PBOOL ;
  /// local "none"
  typedef LoKi::BasicFunctors<LHCb::Particle::ConstVector>::BooleanConstant _CBOOL ;
  /// local "none"
  typedef LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant   _ABOOL ;
  ///
  // ==========================================================================
  /** @class PV_Sentry
   *  Helper class to guarantee the removal of relations to the
   *  temporary object
   *
   *  @todo similar "sentry" class to be added into Kernel/Relations package
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-05-05
   */
  class PV_Sentry
  {
    // ========================================================================
  public:
    // ========================================================================
    /** constructor
     *  @param table   the relation table
     *  @param particle the particle
     */
    PV_Sentry
    ( DVAlgorithm*          parent,
      const LHCb::Particle* particle )
      : m_parent   ( parent   )
      , m_particle ( particle )
    {}
    /// destructor
    ~PV_Sentry()
    {
      m_parent->unRelatePV(m_particle);
    }
    // ========================================================================
  private:
    // ========================================================================
    // the default constructror is disabled
    PV_Sentry () ; // the default constructror is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the relation table (REFERENCE!)
    DVAlgorithm* m_parent ;                         // the relation table (REFERENCE!)
    /// the temporary particle
    const LHCb::Particle* m_particle ;                // the temporary particle
    // ========================================================================
  };
  // ==========================================================================
} // end of anonymous namespace
// ============================================================================
/** @class CombineParticles
 *  LoKi-based version of CombineParticles algorithm, developed by
 *  Gerhard Raven, Patrick Koppenburg and Juan Palacios.
 *
 *  The major propertes of the algorithm (in addition to the
 *   properties of class DValgorithm) are:
 *    - <c>"Factory"</c>:   the type/name of hybrid (LoKi/Bender) C++/Python factory
 *    - <c>"Preambulo"</c>: the preambulo to be used for Bender/Python script
 *    - <c>"DecayDescriptors"</c>: the list of decay descriptors
 *    - <c>"DaughtesCuts"</c>:     the map of cuts for daughter particles
 *    - <c>"CombinationCut"</c>:   the cut applied for combination of particles
 *    - <c>"MotherCut"</c>:        the cut applied for mother particle
 *    - <c>"DaughtersPlots"</c>:    the type/name of Plot-Tool for daughter particles
 *    - <c>"CombinationPlots"</c>:  the type/name of Plot-Tool for combination
 *    - <c>"MotherPlots"</c>:       the type/name of Plot-Tool for mother particles
 *    - <c>"DaughtersPlotsPath"</c>:    the path for Daughters Plots
 *    - <c>"CombinationPlotsPath"</c>:  the path for Combination Plots
 *    - <c>"MotherPlotsPath"</c>:       the path for Mother Plots
 *    - <c>"MaxCombinations"</c>: the maximal number of combinations (per channel) to be processes (0:no limit)
 *    - <c>"MaxCandidates"</c>: the maximal number of candidates (per channel) to be processes (0:no limit)
 *    - <c>"StopAtMaxCombinations"</c>: stop or warn when the maximal number of combinations (per channel) is reached
 *    - <c>"StopAtMaxCandidates"</c>: stop or warn when the maximal number of candidates (per channel) is reached
 *    - <c>"StopIncident"</c>: the incident type to be fired in the case of stop processing
 *
 *
 *  Few counters are produced:
 *    - "# <PID>" for each daughter 'PID': number of corresponding daugter particles
 *    - "# <DECAY>" for each <decay>: number of selected decay candidates
 *    - "# selected": total number of selected candidates
 *
 *  The cuts, as well as the quantities plotted, are
 *  <a href="https://twiki.cern.ch/twiki/bin/view/LHCb/MicroDST?topic=LoKiHybridFilters">
 *   LoKi::Hybrid functors</a>, more functors can be found in the
 *  <a href="https://twiki.cern.ch/twiki/bin/view/LHCb/LoKiRefman?topic=LoKiRefMan">
 *   LoKi reference manual</a>, specifically the
 *  <a href="https://twiki.cern.ch/twiki/bin/view/LHCb/LoKiRefMan#LoKi_Particle_Functions">
 *  particle</a> section.
 *
 *
 *  Example python configuration fragment (from DaVinci tutorial):
 *
 *  @code
 *
 *   # 1) make a CombineParticles instance
 *   bs2jpsiphi = CombineParticles("Bs2JpsiPhi")
 *
 *   # 2) give it some input particles ( leading "Phys/" no longer needed)
 *   #    Assuming selections "Jpsi2MuMuSel" and "Phi2KKSel" have been run.
 *   bs2jpsiphiPhys.InputLocations  = [ 'Jpsi2MuMuSel' ,
 *                                      'Phi2KKSel'    ]
 *   # 3) Give it a (mandatory) decay descriptor
 *   bs2jpsiphi.DecayDescriptor = 'B_s0 -> phi(1020) J/psi(1S)'
 *
 *   # 4) Define the cuts on the input particles
 *   bs2jpsiphi.DaughtersCuts = {
 *      "phi(1020)" : "(MAXTREE(ABSID=='mu+',TRCHI2DOF)<10) & (ADMASS('phi(1020)')<30*MeV) & (PT>1000*MeV)",
 *      "J/psi(1S)" : " (ADMASS('J/psi(1S)')<100*MeV)"
 *       }
 *
 *   # 5) Cut on combinations of particles (in this case, J/Psi and Phi)
 *   bs2jpsiphi.CombinationCut = "ADAMASS('B_s0')<2*GeV"
 *
 *   # 6) Cut on the actual created Bs candidate
 *   bs2jpsiphi.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (BPVIPCHI2()<100)"
 *
 *   # 7) Add some plots
 *   bs2jpsiphi.HistoProduce = True
 *   bs2jpsiphi.addTool ( PlotTool ( 'DaughtersPlots' ) )
 *   bs2jpsiphi.DaughtersPlots.Histos = {
 *       'P/Units.GeV'  : ('momentum' , 0 ,100      ) ,
 *       'PT/Units.GeV' : ('pt_%1%'   , 0 ,5   ,500 ) ,
 *       'M'            : ('mass in MeV_%1%_%2%_%3%',0.8*Units.GeV,4*Units.GeV)
 *       }
 *   bs2jpsiphi.addTool( PlotTool ( "MotherPlots" ) )
 *   bs2jpsiphi.MotherPlots.Histos = {
 *        "P/1000"  : ('momentum',0,100) ,
 *        "PT/1000" : ('pt_%1%',0,5,500) ,
 *        "M"      : ('mass in MeV_%1%_%2%_%3%',4*Units.GeV,6*Units.GeV)
 *      }
 *
 *  @endcode
 *
 *
 *    Embedded Monitoring:
 *
 *    - <c>"Monitor"</c>  : swithch on/off the monitoring functors
 *    - <c>"DaughterMonitors"</c> : map of monitor functors for daughetr particles (after selection)
 *    - <c>"CombinationMonitor"</c> : monitor functor for combinations particles (before cuts)
 *    - <c>"MotherMonitor"</c> : monitor functor for mother particles (after all cuts)
 *
 *  Empty string implies no monitorig.
 *
 *  Each monitoring functor has a signature
 *  <code> LHCb::Particle::ConstVector -> book </code>,
 *  that corresponds to internal LoKi type <code>LoKi::types::CutVal</code>
 *   @see LoKi::Types::CutVal
 *   @see LoKi::Types::CutVals
 *   @see LoKi::BasicFunctors
 *
 *
 *  Examples :
 *
 *  @code
 *
 *   myAlg = CombibneParticles ( ....  )
 *
 *   myAlg.Monitor = True
 *   myAlg.MotherMonitor = """
 *
 *       tee     ( monitor ( SIZE                  ,  ... ) ) >>
 *       tee     ( monitor ( max_value( PT )       ,  ... ) ) >>
 *       process ( monitor (            PT         ,  ... ) ) >>
 *       tee     ( monitor ( count ( PT > 1*GeV )  ,  ... ) ) >>
 *       EMPTY
 *
 *   """
 *
 *  myAlg.DaughterMonitors = {
 *   'K+'  : " monitor ( SIZE             , ... ) > 0 " ,
 *   'pi-' : " monitor ( min_value( PT )  , ... ) > 0 " ,
 *  }
 *
 *  @endcode
 *
 *  The embedded monitoring are described in detail at
 *  <a href="https://twiki.cern.ch/twiki/bin/view/LHCb/FAQ/LoKiFAQ#How_to_monitor_various_LoKi_func">
 *  here </a>, also see
 *  <a href="https://twiki.cern.ch/twiki/bin/view/LHCb/FAQ/LoKiFAQ#Helper_streamer_functions">
 *  helper LoKi streamers & filters</a>
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-01
 */
class CombineParticles
  : public extends2<DVAlgorithm,ISetInputParticles,IIncidentListener>
{
  // ==========================================================================
  // the friend factory, needed for instantiation
  friend class AlgFactory<CombineParticles> ;
  // ==========================================================================
public:
  // ==========================================================================
  /// the standard initialization of the algorithm
  virtual StatusCode initialize () ;                 // standard initialization
  /// the standard execution      of the algorithm
  virtual StatusCode execute    () ;                 //      standard execution
  /// the standard finalization of the algorithm
  virtual StatusCode finalize   () ;                 //  standard  finalization
  // ==========================================================================
public:
  // ==========================================================================
  // IIncindentListener interface
  // ==========================================================================
  /** Inform that a new incident has occured
   *  @see IIncidentListener
   */
  virtual void handle ( const Incident& ) ;
  // ==========================================================================
public:
  // ==========================================================================
  // ISetInputParticles interface
  // ==========================================================================
  /** set the input particles
   *  @see ISetInputParticles
   *  @param input the vector of input particles
   *  @return status code
   */
  virtual StatusCode setInput
  ( const LHCb::Particle::ConstVector& input ) ;
  // ==========================================================================
protected:
  // ==========================================================================
  /** the standard constructor
   *  @param name algorithm instance name
   *  @param pSvc the service locator
   */
  CombineParticles
  ( const std::string& name ,   // the algorithm instance name
    ISvcLocator*       pSvc ) ; // the service locator
  /// virtual and protected destrcutor
  virtual ~CombineParticles() {} // virtual and protected destrcutor
  // ==========================================================================
private:
  // ==========================================================================
  /// the default constructor is disabled
  CombineParticles () ;                            // no default constructor!
  /// the copy constructor is disabled
  CombineParticles ( const CombineParticles& ) ;      // no copy constructor!
  /// the assignement is disabled
  CombineParticles& operator=( const CombineParticles& ) ; // no assignement!
  // ==========================================================================
private:
  // ==========================================================================
  /// decode all cuts
  StatusCode decodeAllCuts ()  ;
  // ==========================================================================
  /// construct the preambulo string
  std::string preambulo() const
  {
    std::string result ;
    for ( std::vector<std::string>::const_iterator iline =
            m_preambulo.begin() ; m_preambulo.end() != iline ; ++iline )
    {
      if ( m_preambulo.begin() != iline ) { result += "\n" ; }
      result += (*iline) ;
    }
    return result ;
  }
  // ==========================================================================
  /// perform the update of the major properties
  StatusCode updateMajor  () ;
  /// perform the update of histogram properties
  StatusCode updateHistos () ;
  /// helper to get histo tools
  StatusCode getHistoTool
  ( IPlotTool*&        histoTool ,
    const std::string& name      ,
    const std::string& path      ) ;
  // ==========================================================================
public:
  // ==========================================================================
  /// the handle for the property update
  void propertyHandler1 ( Property& p ) ;
  void propertyHandler2 ( Property& p ) ;
  // ==========================================================================
public:
  // ==========================================================================
  /// the general flag to switch on/off the monitoring
  bool monitor() const { return m_monitor ; } //   switch on/off the monitoring
  // ==========================================================================
private:
  // ==========================================================================
  // the map of cuts for daughters { "PID" : "cuts" }
  typedef std::map<std::string,std::string>   Map     ;
  // the actual type type for decay descriptors
  typedef std::vector<std::string>            Strings ;
  // ==========================================================================
  /** @class MyCut
   *  helper structure to hold the cuts
   *  Essentially it is a some kind of  bypass the absence of default constructor
   *   for  LoKi::Types::Cut type
   *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
   *  @date 2008-04-19
   */
  class MyCut
  {
  public:
    // ========================================================================
    /// the constructor
    MyCut()
      : m_pid  ()                         // the particle PID
      , m_name ()                         // the particle NAME
      , m_cut     ( _PBOOL ( true  ) )    // the selection predicate
      , m_moni    ( false            )    // monitoring flag
      , m_monitor ( _CBOOL ( false ) )    // the monitoring functor
    {}
    // ========================================================================
  public:
    // ========================================================================
    /// the particle pid
    LHCb::ParticleID        m_pid     ;                 //     the particle pid
    /// the particle name
    std::string             m_name    ;                 //    the particle name
    /// the cut itself
    LoKi::PhysTypes::Cut    m_cut     ;                 //       the cut itself
    // ========================================================================
    /// the monitoring flag
    bool                    m_moni    ;               //    the monitoring flag
    /// the monitoring functor
    LoKi::PhysTypes::CutVal m_monitor ;               // the monitoring functor
    // ========================================================================
  };
  // ==========================================================================
  // the actual type for the vector of daughter cuts
  typedef std::vector<MyCut>  MyCuts ;
  // ==========================================================================
private:   // properties
  // ==========================================================================
  /// The decay descriptors:
  Strings      m_decayDescriptors ; //                    The decay descriptors
  /// The map of the cuts for the daughter particles:
  Map          m_daughterCuts     ; // The map of the cuts for daughter particles
  /// the cut to be applied for combination of good daughters
  std::string  m_combinationCut   ; // the cut to be applied for combination of good daughters
  /// the cut to be applied to the constructed mother particle
  std::string  m_motherCut        ; // the cut to be applied to the constructed mother particle. Must be configured.
  /// the type/name of hybrid C++/Python factory
  std::string  m_factory          ; // the type/name of hybrid C++/Python factory
  /// the preambulo
  std::vector<std::string> m_preambulo ; // the preambulo
  /// "DaughterPlots" tool
  std::string  m_daughtersPlotsName    ; // the type/name of "Daughter Plots" tool
  /// "CombinationPlots" tool
  std::string  m_combinationPlotsName  ; // the type/name of "Combination Plots" tool
  /// "MotherPlots" tool
  std::string  m_motherPlotsName       ; // the type/name of "Mother Plots" tool
  /// "DaughterPlots" path
  std::string  m_daughtersPlotsPath    ; // the path for "Daughter Plots" tool
  /// "CombinationPlots" path
  std::string  m_combinationPlotsPath  ; // the path for "Combination Plots" tool
  /// "MotherPlots" path
  std::string  m_motherPlotsPath       ; // the path for "Mother Plots" tool
  // ==========================================================================
private:
  // ==========================================================================
  /// the general flag to switch on/off monitoring
  bool         m_monitor ;      // the general flag to switch on/off monitoring
  /// The map of monitors for daughters
  Map          m_daughterMonitors ;        // The map of monitors for daughters
  /// the monitoring code for combinations
  std::string  m_combinationMonitorCode ;  //  monitoring code for combinations
  /// the monitoring functor for combinations
  LoKi::Types::CutVal m_combinationMonitor ;     // monitoring for combinations
  /// the monitoring code for mother particles
  std::string  m_motherMonitorCode    ;          //  monitoring code for mother
  /// the monitoring functor for mother particles
  LoKi::Types::CutVal m_motherMonitor ;                // monitoring for mother
  // ==========================================================================
private: // try to be efficient
  // ==========================================================================
  /// the maximal number of combinations (per decay)
  unsigned long m_maxComb ;        // the maximal number of combinations (per decay)
  /// stop the processing after maximal number of combinations reached?
  bool     m_maxCombStop ;
  // the maximal number of candidates   (per decay)
  unsigned long m_maxCand ;        // the maximal number of candidates   (per decay)
  /// stop the processing after maximal number of candidates reached?
  bool     m_maxCandStop ;
  /// incident name
  std::string  m_stopIncidentName ;                            // incident name
  // ==========================================================================
private:   // local stuff
  // ==========================================================================
  /// The actual list of decays
  std::vector<Decays::Decay> m_decays   ; // The actual list of decays
  /// the vector of daughter cuts
  MyCuts            m_cuts     ; // the vector of daughter cuts
  /// the actual cut for combination of good daughters
  LoKi::Types::ACut m_acut     ; // the actual cut for combination of good daughters
  /// the actual 'final' cut for the mother particle
  LoKi::Types::Cut  m_cut      ; // the actual 'final' cut for the mother particle
  // ==========================================================================
  /// "Daughter    Plots" tool
  IPlotTool*        m_daughtersPlots   ; //  the  "Daughter Plots" tool
  /// "Combination Plots" tool
  IPlotTool*        m_combinationPlots ; //  the  "Daughter Plots" tool
  /// "Mother      Plots" tool
  IPlotTool*        m_motherPlots      ; //  the  "Daughter Plots" tool
  // ==========================================================================
private: // some very specific stuff for the special studies
  // ==========================================================================
  /// the vector of input particles to be used instead of the standard
  LHCb::Particle::ConstVector  m_inputParticles    ;
  /// the flag which signal the usage of input particles instead of the standard
  bool                         m_useInputParticles ;
  // ==========================================================================
private: // internal
  // ==========================================================================
  /// the flag to indicate the nesessity of the self-update
  bool m_to_be_updated1 ; // the flag to indicate the nesessity of the self-update
  bool m_to_be_updated2 ; // the flag to indicate the nesessity of the self-update
  // ==========================================================================
protected:
  // ==========================================================================
  IIncidentSvc* incSvc() const
  {
    if ( !m_incSvc ) 
    { m_incSvc = svc<IIncidentSvc> ( "IncidentSvc" , true ); }
    return m_incSvc ;
  }
  // ==========================================================================
private:
  // ==========================================================================
  /// the incident service
  mutable IIncidentSvc* m_incSvc ;                      // the incident service
  // ==========================================================================
} ;
// ============================================================================
using namespace LoKi ;
// ============================================================================
/*  the standard constructor
 *  @param name algorithm instance name
 *  @param pSvc the service locator
 */
// ============================================================================
CombineParticles::CombineParticles
( const std::string& name ,   // the algorithm instance name
  ISvcLocator*       pSvc )   // the service locator
  : base_class (  name , pSvc )
//
// properties
//
  , m_decayDescriptors ()  // the decay descriptors
  , m_daughterCuts     ()  // the cuts for daughter particles
  , m_combinationCut   ( "ATRUE" ) // the cut for the combination of daughters
  , m_motherCut        ( "Configure me!"  ) // the final cut for the mother particle. Must be configured
  , m_factory  ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC")
  , m_preambulo () // the preambulo
//
// plots:
//
  , m_daughtersPlotsName   ( "LoKi::Hybrid::PlotTool/DaughtersPlots"    )
  , m_combinationPlotsName ( "LoKi::Hybrid::PlotTool/CombinationPlots"  )
  , m_motherPlotsName      ( "LoKi::Hybrid::PlotTool/MotherPlots"       )
//
// plot paths:
//
  , m_daughtersPlotsPath   ( ""  )
  , m_combinationPlotsPath ( ""  )
  , m_motherPlotsPath      ( ""  )
//
// monitoring
//
  , m_monitor                ( false )
  , m_daughterMonitors       ()
  , m_combinationMonitorCode ()
  , m_combinationMonitor     ( _CBOOL ( false ) )
  , m_motherMonitorCode      ()
  , m_motherMonitor          ( _CBOOL ( false ) )
//
// for "heavy" events
//
  , m_maxComb      ( 0     )
  , m_maxCombStop  ( false )
  , m_maxCand      ( 0     )
  , m_maxCandStop  ( false )
  , m_stopIncidentName ()
//
// locals
//
// the actual list of the decays
  , m_decays ()  // the actual list of the decays
// the list of cuts for daughter particles
  , m_cuts   ()  // the list of cuts for daughter particles
// the actual cut for combination of good daughters
  , m_acut ( _ABOOL ( true  ) )
// the final cut the constructed mother particle
  , m_cut  ( _PBOOL ( false ) )
// plots:
  , m_daughtersPlots   ( NULL )
  , m_combinationPlots ( NULL )
  , m_motherPlots      ( NULL )
// special
  , m_inputParticles    (       )
  , m_useInputParticles ( false )
// update:
  , m_to_be_updated1    ( true )
  , m_to_be_updated2    ( true )
//
  , m_incSvc ( NULL )
{
  //
  //
  declareProperty
    ( "DecayDescriptors" , m_decayDescriptors ,
      "The list of (simple) decays to be recontructed/selected"   )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  {
    Property* p = Gaudi::Utils::getProperty ( this , "DecayDescriptor" ) ;
    if ( 0 != p && 0 == p->updateCallBack() )
    { p -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ; }
  }
  //
  declareProperty
    ( "Factory"          , m_factory          ,
      "Type/Name for C++/Python Hybrid Factory"                   )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  declareProperty
    ( "Preambulo"        , m_preambulo        ,
      "The preambulo to be used for Bender/Python script"         )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  declareProperty
    ( "DaughtersCuts"    , m_daughterCuts     ,
      "The map of cuts, to be applied for daughter particles"     )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  declareProperty
    ( "CombinationCut"   , m_combinationCut   ,
      "The cut to be applied for combination of good daughters"   )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  declareProperty
    ( "MotherCut"        , m_motherCut        ,
      "The cut to be applied for the constructed mother particle. Must be configured." )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  declareProperty
    ( "DaughtersPlots"   , m_daughtersPlotsName   ,
      "The type/name for 'Daughters   Plots' tool " )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty
    ( "CombinationPlots" , m_combinationPlotsName ,
      "The type/name for 'Combination Plots' tool " )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty
    ( "MotherPlots"      , m_motherPlotsName      ,
      "The type/name for 'Mother      Plots' tool " )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty
    ( "DaughtersPlotsPath"   , m_daughtersPlotsPath  ,
      "The path for 'Daughters   Plots'" )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty
    ( "CombinationPlotsPath" , m_combinationPlotsPath ,
      "The path for 'Combination Plots'" )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty
    ( "MotherPlotsPath"      , m_motherPlotsPath      ,
      "The path for 'Mother      Plots'" )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  // The general flag to switch on/off the monitoring
  declareProperty
    ( "Monitor" ,
      m_monitor ,
      "The general flag to switch on/off the monitoring" )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  // The map of monitoring functors for daughter particles
  declareProperty
    ( "DaughtersMonitors" ,
      m_daughterMonitors  ,
      "The map of monitoring functors for daughter particles")
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  // The monitoring functor for combinations
  declareProperty
    ( "CombinationMonitor"     ,
      m_combinationMonitorCode ,
      "The monitoring functor for combinations before the cuts" )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  // The monitoring functor for mother particles
  declareProperty
    ( "MotherMonitor"     ,
      m_motherMonitorCode ,
      "The monitoring functor for mother particles after all cuts" )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  setProperty ( "HistoProduce" , false ) ;
  {
    Property* p = Gaudi::Utils::getProperty ( this , "HistoProduce" ) ;
    if ( 0 != p && 0 == p->updateCallBack() )
    { p -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ; }
  }
  //
  declareProperty
    ( "MaxCombinations" ,
      m_maxComb         ,
      "The maximal number of combinations (per channel) to be processed" ) ;
  declareProperty
    ( "StopAtMaxCombinations" ,
      m_maxCombStop,
      "Stop at the maximal number of combinations (per channel) " ) ;
  declareProperty
    ( "MaxCandidates"   ,
      m_maxCand         ,
      "The maximal number of candidates   (per channel) to be processed" ) ;
  declareProperty
    ( "StopAtMaxCandidates" ,
      m_maxCandStop,
      "Stop at the maximal number of candidates (per channel) " ) ;
  declareProperty
    ( "StopIncidentType" ,
      m_stopIncidentName ,
      "The type of incident to be fired for problematic processing" ) ;
  //
}
// ============================================================================
// decode all cuts
// ============================================================================
StatusCode CombineParticles::decodeAllCuts()
{
  m_cuts.clear() ;

  // locate the factory
  IHybridFactory* factory = tool<IHybridFactory> ( m_factory , this ) ;

  // 1) decode "daughter" cuts
  for ( Map::const_iterator ic = m_daughterCuts.begin () ;
        m_daughterCuts.end() != ic ; ++ic )
  {
    if ( ic->first.empty() ) { continue ; }
    //
    { // check the correctness of particle name
      Assert ( 0 !=  pid ( ic->first ) ,
               " Invalid Particle Name is used as the key : '"  + (ic->first) + "'" ) ;
    }
    //
    MyCut item ;
    StatusCode sc = factory->get ( ic->second , item.m_cut , preambulo () ) ;
    if ( sc.isFailure() )
    { return Error ( "Unable to  decode the cut for '" + ic->first + "':" + ic->second , sc ) ; }
    item.m_name = ic->first ;
    item.m_pid  = LoKi::Particles::pidFromName ( ic->first ) ;
    item.m_moni = false     ;
    //
    debug () << "The decoded cuts for '"+ ( ic->first ) +"' are: " << item.m_cut << endreq ;
    //
    if ( monitor() && !m_daughterMonitors.empty() )
    {
      Map::const_iterator imoni = m_daughterMonitors.find ( ic->first ) ;
      if ( m_daughterMonitors.end() != imoni && !imoni->second.empty() )
      {
        sc = factory->get ( imoni->second , item.m_monitor , preambulo() ) ;
        if ( sc.isFailure() )
        { return Error ( "Unable to  decode the monitor for '" + 
                         ic->first + "':" + imoni->second , sc ) ; }
        item.m_moni = true ;
        debug () << "The decoded monitor for '"+ ( ic->first )
          + "' is: " << item.m_moni << endreq ;
      }
    }
    m_cuts.push_back ( item ) ;
  }
  // 2) decode the cut for the combination of good daughters
  {
    StatusCode sc = factory -> get ( m_combinationCut , m_acut , preambulo () ) ;
    if ( sc.isFailure () )
    { return Error ( "Unable to  decode the 'A'-cut: " + m_combinationCut  , sc ) ; }
    debug () << "The decoded cut for 'combination' is: " << m_acut << endreq ;
  }
  // 3) decode the final cut for the constructed mother particle
  {
    StatusCode sc = factory -> get ( m_motherCut , m_cut , preambulo () ) ;
    if ( sc.isFailure () )
    { return Error ( "Unable to  decode 'mother'-cut: " + m_motherCut  , sc ) ; }
    debug () << "The decoded cut for 'mother' is: " << m_motherCut << endreq ;
  }
  // monitor combinations ?
  if ( monitor() && !m_combinationMonitorCode.empty() )
  {
    StatusCode sc = factory -> get ( m_combinationMonitorCode ,
                                     m_combinationMonitor     , preambulo () ) ;
    if ( sc.isFailure () )
    { return Error ( "Unable to  decode 'combination-monitor': "
                     + m_combinationMonitorCode  , sc ) ; }
    debug () << "The decoded 'combination-monitor' is: "
             << m_combinationMonitor << endreq ;
  }
  // monitor mothers ?
  if ( monitor() && !m_motherMonitorCode.empty() )
  {
    StatusCode sc = factory -> get ( m_motherMonitorCode ,
                                     m_motherMonitor     , preambulo () ) ;
    if ( sc.isFailure () )
    { return Error ( "Unable to  decode 'mother-monitor': "
                     + m_motherMonitorCode  , sc ) ; }
    debug () << "The decoded 'mother-monitor' is: "
             << m_motherMonitor << endreq ;
  }
  //
  release ( factory ) ;
  //
  return StatusCode::SUCCESS ;                                        // RETURN
}
// ============================================================================
// the standard initialization of the algorithm
// ============================================================================
StatusCode CombineParticles::initialize ()  // standard initialization
{
  StatusCode sc = DVAlgorithm::initialize () ;
  if ( sc.isFailure() ) { return sc ; }

  // check for LoKi service
  svc<IService> ( "LoKiSvc" , true ) ;

  // subscribe the incident
  incSvc() -> addListener ( this , IncidentType::BeginEvent ) ;

  // (re)lock the context again:

  // register for the algorithm context service
  IAlgContextSvc* ctx = 0 ;
  if ( registerContext() ) { ctx = contextSvc() ; }
  // setup sentry/guard
  Gaudi::Utils::AlgContext sentry ( ctx , this ) ;

  // the actual tuning/decoding
  sc = updateMajor () ;
  if ( sc.isFailure () )
  { return Error ( "Error from updateMajor", sc ) ; }  // RETURN

  // deal with the histograms:
  sc = updateHistos () ;

  // return
  return sc ;
}
// ============================================================================
// the standard execution      of the algorithm
// ============================================================================
StatusCode CombineParticles::execute()  // standard execution
{
  setFilterPassed ( false ) ;

  if ( m_to_be_updated1 )
  {
    const StatusCode sc = updateMajor() ;
    if ( sc.isFailure() )
    { return Error ( "Error from updateMajor"  , sc ) ; }  // RETURN
  }

  if ( m_to_be_updated2 )
  {
    const StatusCode sc = updateHistos () ;
    if ( sc.isFailure() )
    { return Error ( "Error from updateHistos" , sc ) ; }  // RETURN
  }

  // the local storage of selected particles
  typedef LoKi::Selected_<LHCb::Particle::ConstVector>  Selected ;

  // the looping/combiner engine
  typedef LoKi::Combiner_<LHCb::Particle::ConstVector>  Combiner ;

  // get the particles from the local storage
  const LHCb::Particle::ConstVector& particles
    = m_useInputParticles ? m_inputParticles : i_particles () ;

  if ( m_useInputParticles && msgLevel ( MSG::INFO ) )
  {
    info() << " The external set of " << particles.size()
           << " particles is used instead of InputLocations "
           << endreq ;
  }

  if ( msgLevel ( MSG::VERBOSE ) )
  {
    for ( LHCb::Particle::ConstVector::const_iterator i = particles.begin();
          i != particles.end(); ++i )
    {
      verbose() << "Input :  " << (*i)->key() << " " << (*i)->particleID().pid() << " "
                << (*i)->momentum() << endmsg ;
    }
  }

  // flag to monitor combinations
  const bool monitorCombinations  = monitor() && !m_combinationMonitorCode.empty() ;
  // flag to monitor saved mothers
  const bool monitorMother        = monitor() && !m_motherMonitorCode.empty() ;

  // (pre)select all daughters:
  Selected daughters ;
  for ( MyCuts::const_iterator idau = m_cuts.begin() ; m_cuts.end() != idau ; ++idau )
  {
    Selected::Range r = daughters.add
      ( idau->m_name       ,
        particles.begin () ,
        particles.end   () ,
        LoKi::Particles::Identifier() == idau->m_pid && idau->m_cut  ) ;
    // some statistics:
    counter( "# " + r.name () ) += r.size() ;
    if (msgLevel(MSG::VERBOSE))
    {
      for ( Selected::Range::const_iterator i = r.begin(); i!=r.end();++i)
        verbose() << "Select: " << (*i)->key() << " " << (*i)->particleID().pid() << " "
                  << (*i)->momentum() << endmsg ;
    }
    // make plots ?
    if ( m_daughtersPlots )
    {
      StatusCode sc = m_daughtersPlots -> fillPlots ( r.begin() , r.end() , r.name() ) ;
      if ( sc.isFailure() ) { Warning ("Error from DaughterPlots" , sc ) ; }
    }
    // monitor
    if ( monitor() && idau->m_moni )
    { idau->m_monitor ( LHCb::Particle::ConstVector( r.begin() , r.end() ) ) ; }
    //
  }

  /** get the default particle combiner/creator
   *  @attention Particle Combiner is used for creation of Mother Particle!
   */
  const IParticleCombiner* combiner = particleCombiner() ; // get the particle combiner

  // the counter of recontructed/selected decays:
  size_t nTotal = 0 ;

  // Flag to indicate if processing is aborted
  bool problem = false ;

  LHCb::Particle::ConstVector saved ;
  saved.reserve ( 100 ) ; // CRJ : Was 1000. Seems a bit big ?

  // loop over all decays
  for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin() ;
        m_decays.end() != idecay && !problem ; ++idecay )
  {
    // the counter of "good" selected decays
    size_t nGood = 0 ;

    // comparison criteria to remove of double counts for the same pid
    const LoKi::Particles::PidCompare compare = LoKi::Particles::PidCompare () ;

    // create the actual object for looping
    Combiner loop ;

    // fill it with the input data
    const Decays::Decay::Items& items = idecay->children() ;
    for ( Decays::Decay::Items::const_iterator child = items.begin() ;
          items.end() != child ; ++child ) { loop.add ( daughters ( child->name() ) ) ; }

    // check for max
    if ( msgLevel( MSG::DEBUG ) || 0 < m_maxComb )
    { counter ( "# max size" + idecay->toString() ) += loop.size() ; }
    if ( 0 < m_maxComb && m_maxComb <= loop.size() )
    {
      Warning ( "MaxCombinations exceeded for '" + idecay->toString() + "'",
                StatusCode::FAILURE, 0 ).ignore() ;
      if ( m_maxCombStop ) { problem = true ; continue ; }        // CONTINUE
    }

    // here we can start the actual looping
    for ( ; loop.valid() ; loop.next() )  // we are in the loop!
    {

      if ( 0 < m_maxCand && m_maxCand <= nGood )
      {
        Warning ( "MaxCandidates exceeded for '" + idecay->toString() + "'",
                  StatusCode::FAILURE, 0 ).ignore() ;
        if ( m_maxCandStop ) { problem = true ; break ; }         //  BREAK
      }

      if ( !loop.unique ( compare ) ) { continue ; }              // CONTINUE

      // get the actual('current') combination:
      LHCb::Particle::ConstVector combination ( loop.dim() ) ;

      loop.current ( combination.begin() ) ;

      if ( msgLevel(MSG::VERBOSE) )
      {
        for ( LHCb::Particle::ConstVector::const_iterator i = combination.begin();
              i != combination.end(); ++i )
        {
          verbose() << "New Com: " << (*i)->key() << " " << (*i)->particleID().pid() << " "
                    << (*i)->momentum() << endmsg ;
        }
      }
      if ( checkOverlap()->foundOverlap ( combination ) )
      {
        if ( msgLevel( MSG::VERBOSE ) ) verbose() << "    Overlap!" << endmsg ;
        continue ;
      }
      if ( msgLevel ( MSG::VERBOSE ) ) verbose() << "    No Overlap" << endmsg ;

      // monitor combinations?
      if ( monitorCombinations ) { m_combinationMonitor ( combination ) ; }

      // here we have the combination and can apply the cut:
      if ( !m_acut ( combination ) )
      {
        if ( msgLevel ( MSG::VERBOSE ) ) verbose() << "    Failed Cut!" << endmsg ;
        continue ;
      }               // CONTINUE
      if ( msgLevel ( MSG::VERBOSE ) ) verbose() << "    Good combination" << endmsg ;

      // here we can create the particle (and the vertex)

      LHCb::Vertex   vertex   ;
      LHCb::Particle mother ( idecay->mother().pid() ) ;

      // lock the relation table Particle -> Primary Vertex:
      PV_Sentry lock ( this , &mother ) ;

      // use default particle combiner to create the composed particle
      const StatusCode sc = combiner -> combine ( combination , mother , vertex ) ;
      if ( sc.isFailure() )
      {
        const std::string mess = "Error from IParticleCombiner, skip the combination";
        Print( mess, sc, MSG::DEBUG ) ;
        ++counter(mess);
        continue ;                                                 // CONTINUE
      }

      if ( msgLevel ( MSG::VERBOSE ) ) verbose() << "    Passed IParticleCombiner" << endmsg ;

      if ( m_combinationPlots )
      {
        const StatusCode scc = m_combinationPlots->fillPlots ( &mother ) ;
        if ( scc.isFailure() ) { Warning ( "Error from CombinationPlots", scc ).ignore() ; }
      }

      // apply the cut on "mother" particle
      if ( !m_cut( &mother ) )  { continue ; }                    // CONTINUE

      // keep the good candidate:
      const LHCb::Particle* particle = this->markTree( &mother ) ;
      if ( monitorMother ) { saved.push_back( particle ) ; }

      if ( m_motherPlots )
      {
        const StatusCode scc = m_motherPlots->fillPlots ( particle ) ;
        if ( scc.isFailure() ) { Warning ( "Error from MotherPlots" , scc ).ignore() ; }
      }

      // increment number of good decays
      ++nGood;

    } // the loop over combinations

    // some statistics
    if ( problem ) nGood = 0; // Nothing saved if a problem occurred
    counter ( "#" + idecay->toString() ) += nGood;
    nTotal += nGood;

  } // the loop over the decays

  // If a problem occurred, do not save anything
  if ( problem ) nTotal = 0;

  // the final statistics
  counter ( "# selected" ) += nTotal;

  // monitor mother particles
  if ( monitorMother && !problem ) { m_motherMonitor ( saved ) ; }

  // reset the "use external input" flag
  m_useInputParticles = false ;

  // the final decision
  setFilterPassed ( 0 != nTotal && !problem ) ;

  // Finally, if configured fire an incident
  if ( problem )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() <<  "A problem with combinatorics has occured"  << endmsg;
    if ( !m_stopIncidentName.empty() )
    { incSvc()->fireIncident( Incident ( name() , m_stopIncidentName ) ) ; }
  }

  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  Inform that a new incident has occured
 *  @see IIncidentListener
 */
// ============================================================================
void CombineParticles::handle ( const Incident& )
{
  m_inputParticles.clear()    ;      // clear the input container
  m_useInputParticles = false ;      // reset the flag
}
// ============================================================================
/** set the input particles
 *  @see ISetInputParticles
 *  @param input the vector of input particles
 *  @return status code
 */
// ============================================================================
StatusCode CombineParticles::setInput
( const LHCb::Particle::ConstVector& input )
{
  m_inputParticles    = input ;
  m_useInputParticles = true  ;
  info() << "The external set of " << input.size()
         << " particles will be used as input instead of InputLocations "
         << endreq ;
  return StatusCode ( StatusCode::SUCCESS , true ) ;
}
// =============================================================================
// the handle for the property update
// =============================================================================
void CombineParticles::propertyHandler1 ( Property& p )
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  /// mark as "to-be-updated"
  m_to_be_updated1 = true ;
  Warning ( "The structural property '" + p.name() +
            "' is updated. It will take affect at the next 'execute'" ,
            StatusCode( StatusCode::SUCCESS, true ) ) ;
  info () << "The updated property is: " << p << endreq ;
}
// =============================================================================
// the handle for the property update
// =============================================================================
void CombineParticles::propertyHandler2 ( Property& p )
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  /// mark as "to-be-updated"
  m_to_be_updated2 = true ;
  debug () << "The histogramming property is updated: " << p << endreq ;
}
// ============================================================================
// perform the update of the major properties
// ============================================================================
StatusCode CombineParticles::updateMajor  ()
{
  // 1) decode all decay descriptors:
  {
    if ( m_decayDescriptors.empty() )
    { m_decayDescriptors.push_back ( getDecayDescriptor() ) ; }
    // locate the decoder tool:
    IDecodeSimpleDecayString* decoder =
      tool <IDecodeSimpleDecayString> ( "DecodeSimpleDecayString:PUBLIC" , this ) ;
    //
    m_decays = Decays::decays ( m_decayDescriptors , decoder ) ;
    if ( m_decays.empty() )
    { return Error ( "Unable to decode DecayDescriptor(s) " ) ; }    // RETURN
    // release the tool (not needed anymore)
    release ( decoder );
  }
  // 2) check the list of daughters, involved into all decays:
  {
    typedef std::set<std::string> PIDs ;
    PIDs pids ;
    for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin() ;
          m_decays.end()  != idecay ; ++idecay )
    {
      const Decays::Decay::Items& ds= idecay->daughters () ;
      for ( Decays::Decay::Items::const_iterator ic = ds.begin() ;
            ds.end ()  != ic ; ++ic ) { pids.insert ( ic->name()  ) ; }
    }
    // default cut: accept all
    if  ( m_daughterCuts.end() == m_daughterCuts.find ("") ) { m_daughterCuts[""] = "ALL" ; }
    // check that all daughters has the cuts associated:
    for ( PIDs::const_iterator ipid = pids.begin() ; pids.end () != ipid ; ++ipid )
    {
      // get the antiparticle:
      const std::string anti = LoKi::Particles::antiParticle ( *ipid ) ;

      { // play with daughter cuts
        // find the cuts for particle
        Map::const_iterator i1 = m_daughterCuts.find ( *ipid ) ;
        // find the cuts for antiparticle
        Map::const_iterator i2 = m_daughterCuts.find (  anti ) ;
        //
        // both cuts are already specified explicitly?
        if      ( m_daughterCuts.end() != i1 &&  m_daughterCuts.end() != i2 ) { } // nothing to do
        else if ( m_daughterCuts.end() != i1 ) // the cut for particle is already specified
        { m_daughterCuts[ anti ] = m_daughterCuts[ *ipid ] ; } // use the same cust for antiparticle
        else if ( m_daughterCuts.end() != i2 ) // the cut for anti-particle is already specified
        { m_daughterCuts[ *ipid ] = m_daughterCuts[ anti ] ; } // use the same cuts as particle
        else // none cuts for particle&antiparticle are specified: use the default cuts
        {
          m_daughterCuts [ *ipid ] = m_daughterCuts[""] ;  // the default cuts for particle
          m_daughterCuts [ anti  ] = m_daughterCuts[""] ;  // the default cuts for antiparticle
        }
      }

      // play the same game with daughter monitors
      if ( monitor() && !m_daughterMonitors.empty() )
      {
        // find the cuts for particle
        Map::const_iterator j1 = m_daughterMonitors.find ( *ipid ) ;
        // find the cuts for antiparticle
        Map::const_iterator j2 = m_daughterMonitors.find (  anti ) ;
        //
        // both monitors are already specified explicitly?
        if      ( m_daughterMonitors.end() != j1 &&
                  m_daughterMonitors.end() != j2 ) { } // nothing to do
        // the monitor for particle is already specified
        else if ( m_daughterMonitors.end() != j1 )
        { m_daughterMonitors[ anti  ] = m_daughterMonitors[ *ipid ] ; } // use the same monitor for antiparticle
        // the monitor for anti-particle is already specified
        else if ( m_daughterMonitors.end() != j2 )
        { m_daughterMonitors[ *ipid ] = m_daughterMonitors[ anti  ] ; } // use the same monitor as particle
      }

    }

    debug () << "The updated map of daughter cuts is : "
             << Gaudi::Utils::toString ( m_daughterCuts ) << endreq ;

    if ( monitor() && !m_daughterMonitors.empty() )
    {
      debug () << "The updated map of daughter monitors is : "
               << Gaudi::Utils::toString ( m_daughterMonitors ) << endreq ;
    }

  }
  // 3) decode the cuts for the daughters:
  {
    StatusCode sc = decodeAllCuts () ;
    if ( sc.isFailure() )
    { return Error ( "The error from decodeAlLCuts", sc ) ; }       // RETURN
  }
  //
  m_to_be_updated1 = false ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// perform the update of the histogram properties
// ============================================================================
StatusCode CombineParticles::getHistoTool
( IPlotTool*&        histoTool ,
  const std::string& name      ,
  const std::string& path      )
{
  if ( validPlots ( name ) )
  {
    histoTool = tool<IPlotTool>( name, this ) ;
    if ( !path.empty() )
    {
      StatusCode sc = m_daughtersPlots -> setPath ( path ) ;
      if ( sc.isFailure() )
      { return Error ( "Unable to set Plots Path "+path+" for tool "+name , sc ) ; }
    }
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
// perform the update of the histogram properties
// ============================================================================
StatusCode CombineParticles::updateHistos ()
{
  // ==========================================================================
  if ( m_daughtersPlots   )
  { releaseTool ( m_daughtersPlots   ) ; m_daughtersPlots   = 0 ; }
  if ( m_combinationPlots )
  { releaseTool ( m_combinationPlots ) ; m_combinationPlots = 0 ; }
  if ( m_motherPlots      )
  { releaseTool ( m_motherPlots      ) ; m_motherPlots      = 0 ; }
  // ==========================================================================
  if ( produceHistos() )
  {
    StatusCode sc = getHistoTool( m_daughtersPlots, m_daughtersPlotsName, m_daughtersPlotsPath );
    if (sc) sc = getHistoTool( m_combinationPlots, m_combinationPlotsName, m_combinationPlotsPath );
    if (sc) sc = getHistoTool( m_motherPlots, m_motherPlotsName, m_motherPlotsPath );
    if (!sc) return sc ;
  }
  // ========================================================================
  //
  m_to_be_updated2 = false ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// the standard finalization of the algorithm
// ============================================================================
StatusCode CombineParticles::finalize   ()           //  standard  finalization
{
  m_incSvc = 0 ;
  // reset functors:
  m_acut               = _ABOOL ( true  ) ;
  m_cut                = _PBOOL ( false ) ;
  m_cuts.clear() ;
  m_combinationMonitor = _CBOOL ( false ) ;
  m_motherMonitor      = _CBOOL ( false ) ;
  m_to_be_updated1     = true ;

  // finalize the base class
  return DVAlgorithm::finalize () ;                 // finalize the base class
}
// ============================================================================
/// The factory (needed for the proper instantiation)
DECLARE_ALGORITHM_FACTORY(CombineParticles)
// ============================================================================
// The END
// ============================================================================
