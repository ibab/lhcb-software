// $Id: MakeResonances.h,v 1.1 2005-03-11 12:19:13 pkoppenb Exp $
#ifndef MAKERESONANCES_H 
#define MAKERESONANCES_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

/** @class MakeResonances MakeResonances.h
 *  
 *  Re-implementation of CombineParticles using the ByPIDFilterCriterion tool
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-11-29
 */
class IFilterCriterion ;
class IPlotTool;
class ICheckOverlap;

class MakeResonances : public DVAlgorithm {
  class Decay ;
public: 
  /// Standard constructor
  MakeResonances( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MakeResonances( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  typedef std::vector<std::string> strings;
  StatusCode createDecays();
  StatusCode createDecay(const std::string&,const strings&);
  StatusCode applyFilter(const ParticleVector&, ParticleVector&, IFilterCriterion*) const;
  StatusCode applyDecay(Decay&,ParticleVector& );
  StatusCode makeMother(Particle*&,const ParticleVector&,const ParticleID&) const;
  StatusCode makePlots(const ParticleVector&,IPlotTool*); ///< make plots

  // globals

  std::string m_daughterFilterName ;   ///< Name for the daughter filter
  std::string m_motherFilterName ;     ///< Name for the mother Filter
  IFilterCriterion* m_daughterFilter;
  IFilterCriterion* m_motherFilter;
  IPlotTool* m_daughterPlots;
  IPlotTool* m_motherPlots;
  ICheckOverlap* m_checkOverlap ;
  /// Name for the daughter plot tool (switch on with HistoProduce = true, 
  /// switch off with DaughterPlotTool = "none" )
  std::string m_daughterPlotTool; 
  /// Name for the mother plot tool (switch on with HistoProduce = true, 
  ///  switch off with MotherPlotTool = "none" )
  std::string m_motherPlotTool;    
  /// Histogram path name for daughter plot tool. Default is "D" + algorithm instance name.
  std::string m_daughterPlotsPath;    
  /// Histogram path name for mother plot tool. Default is "M" + algorithm instance name.
  std::string m_motherPlotsPath;    
  /// DecayDescriptors. In case several need to be given
  std::vector<std::string> m_decayDescriptors;    

  double     m_massWindow;          ///< Mass Window
  double     m_upperMassWindow;     ///< Mass Window
  double     m_lowerMassWindow;     ///< Mass Window
  double     m_minPt;               ///< Minimum Pt of combination
  double     m_minMomentum;         ///< Minimum P of combination
  bool       m_killOverlap ;        ///< Kill candidates based on twice the same track
  bool       m_makePlots;           ///< Produce Histos. Overrides GaudiHistoAlg variable 

  typedef std::vector<Decay> Decays;
  Decays m_decays;

//=============================================================================
// Decay class
//=============================================================================
  
  class Decay{
    class PidParticles ; // forward declaration
  public:
    StatusCode initialize(const int&, const std::vector<int>& , 
               const double&, const double&, const double&,
                          const double&, ICheckOverlap* ); ///< give pids
    Decay(){
      std::vector<int> a;
      m_checkOrder = false ;
      initialize(0,a,0.,100000.,-1.,-1.,NULL);
    };
    ~Decay(){};
    bool fillPidParticles(const ParticleVector&); ///< fill maps at each even
    bool getNextCandidates(ParticleVector&);  ///< Return a vector of daughters
    bool getFirstCandidates(ParticleVector&);  ///< Return a vector of daughters
    ParticleID getMotherPid()const{return m_motherPid;};
    bool goodFourMomentum(const HepLorentzVector& P)const{
      if (!inMassRange(P.m())) return false;
      if (!m_checkP) return true;
      return goodMomentum(P);
    }

  protected:
    bool iterate(); ///< Iterate to next positions
    bool getCandidates(ParticleVector&);  ///< Return a vector of daughters
    bool inMassRange(const double& m)const{return ((m >= m_minMass) && (m<= m_maxMass));}
    bool goodMomentum(const HepLorentzVector& P)const{return (P.rho()>m_minP && P.perp()>m_minPt);}
    bool foundOverlap(const ParticleVector&);
    bool replaceResonanceByDaughters(ParticleVector&);

  private:
    std::vector<PidParticles> m_pidParticles ;
    ParticleID m_motherPid;
    double m_minMass;
    double m_maxMass;
    double m_minP;
    double m_minPt;
    ICheckOverlap* m_checkOverlap ;         ///< Kill candidates based on twice the same track
    bool m_checkP ;
    bool m_checkOrder ;
    
    
//=============================================================================
// PidParticles class
//=============================================================================
    class PidParticles{
    public:
      PidParticles(){m_pid=0;m_samePID=false;m_iterator=m_particles.begin();};
      PidParticles(int pid){m_pid=pid;m_samePID=false;m_iterator=m_particles.begin();};
      ~PidParticles(){};
      int getPid()const {return m_pid;} ;
      void setPid(const int& i){m_pid = i;}
      bool fillParticles(const ParticleVector&);
      Particle* getCandidate()const {return *m_iterator;};
      bool iterate();
      bool samePid()const{return m_samePID;};
      void setSamePid(bool m){m_samePID = m ;};
      
    private:
      ParticleVector m_particles;
      ParticleVector::const_iterator m_iterator;
      int m_pid ;
      bool m_samePID; // there is another one with same PID in the decay
    };  
    
  };

};
#endif // MAKERESONANCES_H
