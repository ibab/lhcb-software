// $Id:$ 
// ============================================================================
#ifndef COMBINEPARTICLES_H 
#define COMBINEPARTICLES_H 1
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IAlgContextSvc.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/DaVinciAlgorithm.h"
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
// ============================================================================
/** @class CombineParticles
 *  LoKi-based version of CombineParticles algorithm, developed by
 *  Gerhard Raven, Patrick Koppenburg and Juan Palacios.
 *
 *  The major propertes of the algorithm (in addition to the
 *   properties of class DaVinciAlgorithm) are:
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
 * 
 *  Version           $Revision:$
 *  Last modification $Date:$
 *                 by $Author:$
 */
class CombineParticles
  : public extends2<DaVinciAlgorithm,ISetInputParticles,IIncidentListener>
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
  virtual ~CombineParticles() ; // virtual and protected destrcutor
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
protected:
  // ==========================================================================
  /// decode all cuts
  virtual StatusCode decodeAllCuts ()  ;
  /** helper function for further decoding 
   *  @param factory the hybrid factory for decoding 
   */
  virtual StatusCode decodeRest    ( LoKi::IHybridFactory* factory ) ;  
  // ==========================================================================
  /// construct the preambulo string
  std::string preambulo     () const ;
  // ==========================================================================
private:
  // ==========================================================================
  /// perform the update of the major properties
  StatusCode updateMajor    () ;  
  /// perform the update of histogram properties
  StatusCode updateHistos   () ;
  // ==========================================================================
protected:
  // ==========================================================================
  /** pre-action
   *  -check that everythinhy is decoded properly 
   *  @return status code 
   */
  StatusCode pre_action  () ;
  // ==========================================================================
  /// the local storage of selected particles
  typedef LoKi::Selected_<LHCb::Particle::ConstVector>  Selected ;
  // ==========================================================================
  /// get all input particles
  StatusCode get_daughters ( Selected& daughters ) ;
  /** treat combination
   *  - check overlap 
   *  - check cuts  
   *  return true if combination is good 
   */
  bool treat_combination 
  ( const LHCb::Particle::ConstVector& combination ) const ;
  // ==========================================================================
  /** make and check mother 
   *  - create mother prticles 
   *  - check it 
   *  - save if good 
   *  - monitor 
   *  @param pid         (INPUT) the PID for mother particle 
   *  @param combination (INPUT) the list of daughters 
   *  @param combiner    (INPUT) the combiner tool 
   *  @param saved       (UPDATE) the list of saved mother particles for monitoring 
   *  @return true if good mother particle is saved 
   */
  bool treat_mother  
  ( const LHCb::ParticleID&            pid         , 
    const LHCb::Particle::ConstVector& combination , 
    const IParticleCombiner*           combiner    , 
    LHCb::Particle::ConstVector&       saved       ) ;  
  // ==========================================================================
  /** post-action
   *  - monitor number of total saved decays , 
   *  - monitor mother particles (if requred)
   *  - make the final algorithm decision 
   *  - fire the problem incident    (if needed)
   *  @param nTotal   (INPUT) total number of saved decays 
   *  @param problem  (INPUT) problem-flag 
   *  @param saved    (INPUT) saved mother particles for monitorin
   *  @return status code 
   */
  StatusCode post_action 
  ( size_t                             nTotal  ,
    const bool                         problem ,
    const LHCb::Particle::ConstVector& saved   ) ;
  // ==========================================================================
  /// monitor combinations ?
  bool monitorCombinations () const 
  { return monitor() && !m_combinationMonitorCode.empty() ; }
  /// monitor mother       ?
  bool monitorMother       () const 
  { return monitor() &&      !m_motherMonitorCode.empty() ; }
  // ==========================================================================
  /// have we picked up already too much candidates ?
  bool tooMuchCandidates ( const size_t         nGood , 
                           const Decays::Decay& decay ) const 
  {
    if ( 0 < m_maxCand && m_maxCand <= nGood )
    {
      Warning ( "MaxCandidates exceeded for '" + decay.toString() + "'",
                StatusCode::FAILURE, 0 ).ignore() ;
      if ( m_maxCandStop ) { return true ; }  // RETURN 
    }
    return false ;
  }
  // ==========================================================================
  /// have we picked up already too much combinations  ?
  bool tooMuchCombinations ( const double         ncomb , 
                             const Decays::Decay& decay ) const 
  {
    if ( 0 < m_maxComb && m_maxComb <= ncomb )
    {      
      Warning ( "MaxCombinations exceeded for '" + decay.toString() + "'",
                StatusCode::FAILURE, 0 ).ignore() ;
      if ( m_maxCombStop ) { return true ; } 
    }
    return false ;  
  }
  // ==========================================================================
private:
  // ==========================================================================
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
   *  for  LoKi::Types::Cut type
   *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
   *  @date 2008-04-19
   */
  class MyCut
  {
  public:
    // ========================================================================
    /// the constructor
    MyCut() ;
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
  /// Turn on/off histograms
  bool m_produceHistos;
  // ==========================================================================
private:
  // ==========================================================================
  /// the general flag to switch on/off monitoring
  bool         m_monitor ;      // the general flag to switch on/off monitoring
  /// The map of monitors for daughters
  Map          m_daughterMonitors ;        // The map of monitors for daughters
  /// the monitoring code for combinations
  std::string  m_combinationMonitorCode ;  //  monitoring code for combinations
  /// the monitoring code for mother particles
  std::string  m_motherMonitorCode    ;          //  monitoring code for mother
  // ==========================================================================
protected:
  // ==========================================================================
  /// the monitoring functor for combinations
  LoKi::Types::CutVal m_combinationMonitor ;     // monitoring for combinations
  /// the monitoring functor for mother particles
  LoKi::Types::CutVal m_motherMonitor ;                // monitoring for mother
  // ==========================================================================
protected: // try to be efficient
  // ==========================================================================
  /// the maximal number of combinations (per decay)
  unsigned long m_maxComb ;        // the maximal number of combinations (per decay)
  /// stop the processing after maximal number of combinations reached?
  bool     m_maxCombStop ;
  // the maximal number of candidates   (per decay)
  unsigned long m_maxCand ;        // the maximal number of candidates   (per decay)
  /// stop the processing after maximal number of candidates reached?
  bool     m_maxCandStop ;
  // ==========================================================================
private: 
  // ==========================================================================
  /// incident name
  std::string  m_stopIncidentName ;                            // incident name
  // ==========================================================================
protected:   // local stuff
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
private:
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
// The END 
// ============================================================================
#endif // COMBINEPARTICLES_H
// ============================================================================
