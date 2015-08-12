// $Id$
// ============================================================================
#ifndef FILTERDESKTOP_H 
#define FILTERDESKTOP_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatEntity.h"
#include "GaudiKernel/IAlgContextSvc.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
// ============================================================================
// DaVinci Kernel
// ============================================================================
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/IPlotTool.h"
// ============================================================================
/** @class FilterDesktop 
 *  LoKi/Bender "Hybrid" (re)implementation of simple algorithm wich 
 *  filters the input particles ("FilterDesktop"). 
 *  Stores either copies of
 *  pointers or shallow clones of the selected input particles and 
 *  their end vertices, as defined by 
 *  InputLocations, depending on the value of CloneFilteredParticles.
 *
 *  The important properties (in addtion to the base class' properties)
 *    - "Factory"   : the type/name of LoKi/Bender 'hybrid' factory
 *    - "Preambulo" : the preambulo, to be used for Bender/Python script
 *    - "Code"      : the actual Bender/Python code 
 *    - "CloneFilteredParticles" : Shallow-clone selected Particles into KeyedContainers. Needed for DST writing. Default: false.
 *    - "InputPlotsTool"   : the type/name of PlotTool for 'input' particles 
 *    - "InputPlotsPath"   : THS path for 'input' plots 
 *    - "OutputPlotsTool"  : the type/name of PlotTool for 'output' particles 
 *    - "OutputPlotsPath"  : THS path for 'output' plots 
 *
 *  The important counters (in addition to counters form DaVinciAlgorithm)
 *    - "#inputs"    : number of input particles 
 *    - "#passed"    : number of particles, which passed criteria
 *    - "efficiency" : the selection efficiency (per particle) 
 *
 *  @see DaVinciAlgorithm 
 *  @see CombineParticles 
 *  @see IHybridFactory
 * 
 * The embedded monitoring:
 *  
 *    - <c>"Monitor"</c>  : swithch on/off the monitoring functors 
 *    - <c>"PreMonitor"</c> : monitor functor for input particles (before cuts)
 *    - <c>"PostMonitor"</c> : monitor functor for output particles (after all cuts)
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
 *  Examples of embedded monitoring:
 *  
 *  @code 
 * 
 *   myAlg = FilterDesktop ( ... ) 
 * 
 *   myAlg.Monitor     = True 
 *   myAlg.PreMonitor  = "  monitor ( SIZE , ... )   > -1 "
 *   myAlg.PostMonitor = """
 *
 *       tee     ( monitor ( SIZE                  ,  ... ) ) >>
 *       tee     ( monitor ( max_value( PT )       ,  ... ) ) >>
 *       process ( monitor (            PT         ,  ... ) ) >>
 *       tee     ( monitor ( count ( PT > 1*GeV )  ,  ... ) ) >> 
 *       EMPTY 
 *
 *  """
 *
 *  @endcode 
 *
 *  The embedded monitoring are described in detail at 
 *  <a href="https://twiki.cern.ch/twiki/bin/view/LHCb/FAQ/LoKiFAQ#How_to_monitor_various_LoKi_func">
 *  here </a>, also see 
 *  <a href="https://twiki.cern.ch/twiki/bin/view/LHCb/FAQ/LoKiFAQ#Helper_streamer_functions">
 *  helper LoKi streamers & filters</a>
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef,nl
 *  @date 2008-09-22
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$ 
 */
class FilterDesktop : public DaVinciAlgorithm
{
  // ==========================================================================
  /// friend factory for instantiation 
  friend class AlgFactory<FilterDesktop> ;
  // ==========================================================================
protected:
  // ==========================================================================
  /// Base class type
  typedef DaVinciAlgorithm BaseClass;
  // ==========================================================================
public:
  // ==========================================================================
  /// the specific initialization 
  virtual StatusCode initialize () ;           // the specific initialization 
  /// the most interesting method 
  virtual StatusCode execute    () ;           // the most interesting method
  // finalize /reset functors 
  virtual StatusCode finalize   () ;
  // ==========================================================================
protected:
  // ==========================================================================
  /** standard constructor 
   *  @see DaVinciAlgorithm
   *  @see GaudiTupleAlg
   *  @see GaudiHistoAlg
   *  @see GaudiAlgorithm
   *  @see Algorithm
   *  @see AlgFactory
   *  @see IAlgFactory
   *  @param name the algorithm instance name 
   *  @param pSvc pointer to Service Locator 
   */
  FilterDesktop                                        // standard contructor 
  ( const std::string& name ,                  // the algorithm instance name 
    ISvcLocator*       pSvc ) ;                 // pointer to Service Locator
  // ==========================================================================
  /// virtual & protected destructor 
  virtual ~FilterDesktop() ;               // virtual & protected destructor 
  // ==========================================================================
public:
  // ==========================================================================
  /** the major method for filter input particles 
   *  @param input    (INPUT) the input  container of particles
   *  @param filtered (OUPUT) the output container of particles
   *  @return Status code 
   *  @attention mark for particles/tree must be done inside this method
   */
  virtual StatusCode filter 
  ( const LHCb::Particle::ConstVector& input    , 
    LHCb::Particle::ConstVector&       filtered ) ;
  // ==========================================================================
public:
  // ==========================================================================
  /// update the structural property  
  void updateHandler1 ( Property& p ) ;
  /// update the histogram property  
  void updateHandler2 ( Property& p ) ;
  // ==========================================================================
private:
  // ==========================================================================
  /// the default constructor is disabled 
  FilterDesktop () ;                    // the default consructor is disabled
  /// copy constructor is disabled 
  FilterDesktop ( const FilterDesktop& ) ;    // copy constructor is disabled 
  /// assignement operator is disabled 
  FilterDesktop& operator=( const FilterDesktop& ) ;        // no assignement
  // ==========================================================================
public:
  // ==========================================================================
  /// general flag to switch on/off the monitoring
  bool monitor() const { return m_monitor ; }
  // ==========================================================================
protected:
  // ==========================================================================
  /// construct the preambulo string 
  std::string preambulo   () const ;          // construct the preambulo string 
  // ==========================================================================
  /// update the major properties 
  StatusCode updateMajor  () ;                //    update the major properties 
  // ==========================================================================
  /// update histos 
  StatusCode updateHistos () ;                //                  update histos 
  // ==========================================================================
  /// decode the code 
  virtual StatusCode decodeCode () ;          //                decode the code 
  // ==========================================================================
protected:
  // ==========================================================================
  /// Get the related PV of particle and relate it to clone.
  void cloneP2PVRelation
  ( const LHCb::Particle*   particle ,
    const LHCb::Particle*   clone      ) const;
  /// Write empty container if selection fails.
  void writeEmptyKeyedContainers  ( ) const;
  /// Write empty container if selection fails.
  void writeEmptySharedContainers ( ) const;
  // ============================================================================
private:
  // ============================================================================
  /// save (clone if needed) selected particles in TES 
  template <class PARTICLES, class VERTICES, class CLONER>
  StatusCode _save () const ;
  // ============================================================================
protected :
  // ==========================================================================
  /// get the actual predicate: 
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& predicate() const 
  { return m_cut ;}
  // ==========================================================================
  /// access to the code-string
  const std::string& code() const { return m_code ; }
  // ==========================================================================
  /// CloneFilteredParticles ? 
  bool cloneFilteredParticles() const { return m_cloneFilteredParticles ; }
  /// CloneFilteredParticles ? 
  void setCloneFilteredParticles ( const bool value ) 
  { m_cloneFilteredParticles = value ; }
  // ==========================================================================
protected:
  // ==========================================================================
  const std::string& factory() const { return m_factory ; }
  // ==========================================================================
private:
  // ==========================================================================
  /// save (clone if needed) selected particles in TES 
  /// Success if number of saved particles == number saved
  /// to TES.
  /// Overwritten from DaVinciAlgorithm. Is called automatically.
  virtual StatusCode _saveInTES () ;
  /// Write empty containers if selection fails.
  virtual void writeEmptyTESContainers() ;
  // ==========================================================================
private:
  // ==========================================================================
  /// LoKi/Bender "hybrid" factory name 
  std::string m_factory ; // LoKi/Bender "hybrid" factory name 
  /// the preambulo 
  std::vector<std::string> m_preambulo ; // the preambulo 
  /// the code for the functor/predicate 
  std::string m_code    ; // the code for the functor/predicate
  // ==========================================================================
  /// the predicate itself 
  LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate m_cut ;
  //
  // ==========================================================================
protected:
  // ==========================================================================
  /// general flag to switch on/off monitoring
  bool                m_monitor ;   // general flag to switch on/off monitoring
  /// (pre-monitoring) code 
  std::string         m_preMonitorCode  ;         // (pre-monitoring)      code
  /// (pre-monitoring) functor 
  LoKi::Types::CutVal m_preMonitor      ;         // (pre-monotoring)   functor 
  /// (post-monitoring) code 
  std::string         m_postMonitorCode ;         // (post-monitoring)     code
  /// (post-monitoring) functor 
  LoKi::Types::CutVal m_postMonitor     ;         // (post-monitoring)  functor 
  //
  // ==========================================================================
  // local storage
  LHCb::Particle::ConstVector m_accepted;
  //
  // input plots
  //
  /// type/name for the input plots tool 
  std::string m_inputPlotsTool  ;           // type-name for input plots tool 
  /// the inputplots tool itself 
  IPlotTool*  m_inputPlots      ;              // the input plots tool itself
  /// the THS path for the input plots 
  std::string m_inputPlotsPath  ;                     // the input plots path 
  //
  // ==========================================================================
private:
  // ==========================================================================
  //  // output plots 
  //
  /// type/name for the output plots tool 
  std::string m_outputPlotsTool ;          // type-name for output plots tool 
  /// the inputplots tool itself 
  IPlotTool*  m_outputPlots     ;             // the output plots tool itself
  /// the THS path for the output plots 
  std::string m_outputPlotsPath ;                    // the output plots path 
  //
  // ==========================================================================
protected:
  // ==========================================================================
  /// the flag to indicate the nesessity of update 
  bool m_to_be_updated1 ; // the flag to indicate the nesessity of update 
  /// the flag to indicate the nesessity of update 
  bool m_to_be_updated2 ; // the flag to indicate the nesessity of update 
  // ==========================================================================
private:
  // ==========================================================================
  /// CloneFilteredParticles ? 
  bool m_cloneFilteredParticles;                    // CloneFilteredParticles ? 
  // ==========================================================================
private:
  /// Turn on/off histograms
  bool m_produceHistos;
protected:
  /// Access the histogramming flag
  inline bool produceHistos() const { return m_produceHistos; }
};
// ============================================================================

// ============================================================================ 
// The END 
// ============================================================================
#endif // FILTERDESKTOP_H
// ============================================================================


