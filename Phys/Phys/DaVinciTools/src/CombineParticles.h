// $Id: CombineParticles.h,v 1.2 2008-03-04 16:52:47 pkoppenb Exp $
#ifndef COMBINEPARTICLES_H 
#define COMBINEPARTICLES_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"


/** @class CombineParticles CombineParticles.h
 *  
 *  Re-implementation of MakeResonances using three instances of 
 *  the IFilterCriterion tool. It should be configured to use
 *  specifically three instances of Ivan Belyaev's LoKi::Hybrid::FilterCriterion,
 *  but it is possible, if undesirable,  to set other IFilterCriteria 
 *  through options
 *  @see HybridFilterCriterion
 *
 *  The algorithm works in three filtering phases, each with the possibility of plotting with 
 *  an IPlotTool implementation. The phases are
 *  <b>Phase 0</b>: Filter input, or "daughter", particles
 *  <b>Phase 1</b>: Filter on candidate combinations of input particles corresponding to
 *  required decay before full vertex fit is applied. Typically cut on simple kinematic 
 *  variables and invariant mass. The four-momenta and covariance matrices of the candidate 
 *  combinations are "added" using the ParticleAdder implementation of IVertexFit, and so can
 *  be treated as Particles and put through the Phase 1 IFilterCriterion.
 *  <b>Phase 2</b>: Filter output, or "mother", particles, after full vertex fit has been
 *  performed.
 *
 *  <b>General Configuration</b> : Set the three IFilterCriteria to be used
 *  Configurable via property <b>Phase0FilterName</b>, <b>Phase1FilterName</b>, 
 *  and <b>Phase2FilterName</b>
 *  @code
 *     // Set the filter names
 *     MyAlg.Phase0FilterName = "Phase0Filter"; // the default
 *     MyAlg.Phase0FilterName = "Phase1Filter"; // the default
 *     MyAlg.Phase0FilterName = "Phase2Filter"; // the default
 *     // Set the IFilterCriterion implementations
 *     MyAlg.FilterCriteria = { "Phase0Filter" : "LoKi::Hybrid::FilterCriterion/FILTER0",
 *                              "Phase1Filter" : "LoKi::Hybrid::FilterCriterion/FILTER1",
 *                              "Phase2Filter" : "LoKi::Hybrid::FilterCriterion/FILTER2" };
 *     // Configure the HybridFilterCriteria
 *     MyAlg.FILTER0.Code = "(PT>500)&(P>1000)";
 *     MyAlg.FILTER1.Code = "(ADMASS('J/psi(1S)')<400)";
 *     MyAlg.FILTER2.Code = "(VFASPF(VCHI2)<15)";
 *  @endcode
 *
 *
 *  Configure the IPlotTools to be used.
 *  @code
 *     // Switch on histogram making
 *     MyAlg.HistoProduce = true;
 *     // Phase 0 plots
 *     MyAlg.Plots0.Variables =  { "P", "Pt" } ;
 *     // Phase 1 plots
 *     MyAlg.Plots1.Variables =  { "P", "Pt",  "IP", "IPs", "WM", "M""} ;
 *     // Phase 2 plots
 *     MyAlg.Plots2.Variables =  { "P", "Pt",  "IP", "IPs", "WM", "M", "Chi2"} ;
 *
 *  @endcode
 *
 *  @todo Change names of the phases from 0, 1, 2 to something more meaningful
 *  @todo Hard-wire LoKi::Hybrid::FilterCriterion for all three phases, ie disallow configuration
 *  of the implementation of IFilterCriterion
 *
 *  @author Patrick Koppenburg
 *  @author Juan Palacios
 *
 *  @date   2007-07-17
 */

class IFilterCriterion ;
class IPlotTool;
class ICheckOverlap;
class IParticleDescendants;

class CombineParticles : public DVAlgorithm {

  class Decay;

public: 
  /// Standard constructor
  CombineParticles( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CombineParticles( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  typedef std::vector<std::string> Strings;

  StatusCode createDecays();
  StatusCode createDecay(const std::string&,const Strings&);
  StatusCode applyFilter(const LHCb::Particle::ConstVector&, 
                         LHCb::Particle::ConstVector&, 
                         IFilterCriterion*) const;
  StatusCode applyDecay(Decay&,LHCb::Particle::ConstVector& );
  const LHCb::Particle* makeMother(const LHCb::Particle::ConstVector&,
                                   const LHCb::ParticleID&);

  inline const bool consideredPID(const int& pid)const
  {
    return 
      std::find(m_allPids.begin(), m_allPids.end(), pid) != m_allPids.end();
  }

private:

  StatusCode initializePlotters();
  StatusCode initializePlotter(IPlotTool*& plotter, 
                               const std::string& plotterName,
                               std::string& plotsPath);
  

  StatusCode makePlots(const LHCb::Particle::ConstVector& PV,
                       IPlotTool* PT);

  /**
   * @todo rename these phases to something sensible.
   */
  std::string m_phase0FilterName ;   ///< Name for phase 0 filter
  std::string m_phase1FilterName ;   ///< Name for phase 1 Filter
  std::string m_phase2FilterName ;   ///< Name for phase 2 Filter
  IFilterCriterion* m_filter0; ///< Pointer to phase 0 filter instance 
  IFilterCriterion* m_filter1; ///< Pointer to phase 1 filter instance 
  IFilterCriterion* m_filter2; ///< Pointer to phase 2 filter instance 

  ICheckOverlap* m_checkOverlap ;

  IParticleDescendants* m_particleDescendants;

  IPlotTool* m_plots0;
  IPlotTool* m_plots1;
  IPlotTool* m_plots2;

  /// Name for the phase0 plot tool (switch on with HistoProduce = true, 
  /// switch off with Plot0Tool = "NONE" )
  std::string m_plot0Tool; 
  /// Name for the phase1 plot tool (switch on with HistoProduce = true, 
  ///  switch off with Plot1Tool = "NONE" )
  std::string m_plot1Tool;    
  /// Name for the phase2 plot tool (switch on with HistoProduce = true, 
  ///  switch off with Plot2Tool = "NONE" )
  std::string m_plot2Tool;    
  /// Histogram path name for daughter plot tool. Default is "D" + algorithm instance name.
  std::string m_plots0Path;    
  /// Histogram path name for mother plot tool. Default is "M" + algorithm instance name.
  std::string m_plots1Path;    
  std::string m_plots2Path;    

  /// DecayDescriptors. In case several need to be given
  std::vector<std::string> m_decayDescriptors;    

  bool       m_killOverlap ;        ///< Kill candidates based on twice the same track
  std::string m_outputLocation ;    ///< Non-standard Output location, gets read by PhysDesktop
  bool m_printStats ;               ///< Print number of candidates
  bool m_printMoreStats ;           ///< Print at each step


  int        m_nEvents ;            ///< Number of events
  int        m_nAccepted ;          ///< Number of events accepted
  int        m_nCandidates ;        ///< Number of candidates

  typedef std::vector<Decay> Decays;
  Decays m_decays;

  std::vector<int> m_allPids ; ///< all daughter PIDs in all decays


//=============================================================================
// Decay class
//=============================================================================
  
  class Decay{
    class PidParticles ; // forward declaration
  public:

    StatusCode initialize(const std::vector<int>& pIDs); ///< give pids

    Decay()
      :
      m_motherPid(0),
      m_checkOverlap(NULL),
      m_particleDescendants(NULL),
      m_checkOrder(false)
    {
      std::vector<int> a;
      initialize(a).ignore();
    }
    
    Decay(const LHCb::ParticleID& pid, 
          const std::vector<int>& dpid,
          ICheckOverlap* checkOverlapTool,
          IParticleDescendants* particleDescendantsTool)
      :
      m_motherPid(pid),
      m_checkOverlap(checkOverlapTool),
      m_particleDescendants(particleDescendantsTool),
      m_checkOrder(false)
    {
      initialize(dpid).ignore();
    };

    ~Decay(){}

    /** 
     * fill maps at each even
     */
    bool fillPidParticles(const LHCb::Particle::ConstVector&);

    /**
     * Return a vector of daughters
     */
    bool getNextCandidates(LHCb::Particle::ConstVector&);
    /**
     * Return a vector of daughters
     */
    bool getFirstCandidates(LHCb::Particle::ConstVector&);

    LHCb::ParticleID getMotherPid()const{return m_motherPid;};

  protected:
    bool iterate(); ///< Iterate to next positions

    bool getCandidates(LHCb::Particle::ConstVector&);  ///< Return a vector of daughters

    bool foundOverlap(const LHCb::Particle::ConstVector&);

    void replaceResonanceByDaughters(LHCb::Particle::ConstVector&);

  private:
    std::vector<PidParticles> m_pidParticles ;
    LHCb::ParticleID m_motherPid;
    ICheckOverlap* m_checkOverlap ;         ///< Kill candidates based on twice the same track
    IParticleDescendants* m_particleDescendants;
    bool m_checkOrder ;

//=============================================================================
// PidParticles class
//=============================================================================
    class PidParticles{
    public:

      PidParticles()
        :
        m_pid(0),
        m_samePID(false),
        m_iterator( m_particles.begin() )
      {      
      };

      PidParticles(int pid)
        :
        m_pid(pid),
        m_samePID(false),
        m_iterator( m_particles.begin() )
      {
      };
      ~PidParticles(){};

      int getPid()const { return m_pid; }

      void setPid(const int& i){ m_pid = i;}

      bool fillParticles(const LHCb::Particle::ConstVector& PV) 
      {
        m_particles.clear();
        for ( LHCb::Particle::ConstVector::const_iterator p=PV.begin(); 
              p!=PV.end(); ++p){
          if ( m_pid==(*p)->particleID().pid() ) m_particles.push_back(*p) ;
        }
        m_iterator = m_particles.begin();
        return ( !m_particles.empty() );  
      }
      

      const LHCb::Particle* getCandidate()const { return *m_iterator; }

      bool iterate() {
        ++m_iterator ;
        if ( m_iterator==m_particles.end() ) {
          m_iterator=m_particles.begin() ;
          return false ;
        }
        return true ; 
      }

      bool samePid()const{ return m_samePID; }

      void setSamePid(bool m){m_samePID = m ;}
      
    private:
      LHCb::Particle::ConstVector m_particles;
      int m_pid ;
      bool m_samePID; // there is another one with same PID in the decay
      LHCb::Particle::ConstVector::const_iterator m_iterator;
    };  

  };
  
  

};
#endif // COMBINEPARTICLES_H
