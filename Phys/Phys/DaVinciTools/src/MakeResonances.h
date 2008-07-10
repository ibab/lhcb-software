// $Id: MakeResonances.h,v 1.16 2008-07-10 15:03:24 pkoppenb Exp $
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
 * 
 * 
 */
class IFilterCriterion ;
class ICheckOverlap;
class IParticleDescendants;

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

  typedef std::vector<std::string> strings;
  StatusCode createDecays();
  StatusCode createDecay(const std::string&,const strings&);
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
  // globals

  std::string m_daughterFilterName ;   ///< Name for the daughter filter
  std::string m_motherFilterName ;     ///< Name for the mother Filter
  IFilterCriterion* m_daughterFilter;
  IFilterCriterion* m_motherFilter;
  ICheckOverlap* m_checkOverlap ;
  IParticleDescendants* m_particleDescendants;
  std::vector<std::string> m_decayDescriptors;    

  double     m_massWindow;          ///< Mass Window
  double     m_upperMassWindow;     ///< Mass Window
  double     m_lowerMassWindow;     ///< Mass Window
  double     m_minPt;               ///< Minimum Pt of combination
  double     m_minMomentum;         ///< Minimum P of combination
  bool       m_killOverlap ;        ///< Kill candidates based on twice the same track
  int        m_nEvents ;            ///< Number of events
  int        m_nAccepted ;          ///< Number of events accepted
  int        m_nCandidates ;        ///< Number of candidates
  std::string m_outputLocation ;    ///< Non-standard Output location, gets read by PhysDesktop
  bool m_printStats ;               ///< Print number of daughters and mothers

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
      m_minMass(0.),
      m_maxMass(100000.),
      m_minP(-1.),
      m_minPt(-1.),
      m_checkOverlap(NULL),
      m_particleDescendants(NULL),
      m_checkP(false),
      m_checkOrder(false)
    {
      std::vector<int> a;
      initialize(a).ignore();
    }
    
    Decay(const LHCb::ParticleID& pid, 
          const std::vector<int>& dpid, 
          const double& minMass, 
          const double& maxMass, 
          const double& minMomentum,
          const double& minPt, 
          ICheckOverlap* checkOverlapTool,
          IParticleDescendants* particleDescendantsTool)
      :
      m_motherPid(pid),
      m_minMass(minMass),
      m_maxMass(maxMass),
      m_minP(minMomentum),
      m_minPt(minPt),
      m_checkOverlap(checkOverlapTool),
      m_particleDescendants(particleDescendantsTool),
      m_checkP(( minPt>0.) || (minMomentum>0.)),
      m_checkOrder(false)
    {
      initialize(dpid).ignore();
    };

    ~Decay(){};

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

    bool goodFourMomentum(const Gaudi::XYZTVector& P)const{
      if (!inMassRange(P.M())) return false;
      if (!m_checkP) return true;
      return goodMomentum(P);
    }

  protected:
    bool iterate(); ///< Iterate to next positions

    bool getCandidates(LHCb::Particle::ConstVector&);  ///< Return a vector of daughters

    bool inMassRange(const double& m) const
    {
      return ((m >= m_minMass) && (m<= m_maxMass));
    }

    bool goodMomentum(const Gaudi::XYZTVector& P) const
    {
      return (P.R()>m_minP && P.Pt()>m_minPt);
    }

    bool foundOverlap(const LHCb::Particle::ConstVector&);

    void replaceResonanceByDaughters(LHCb::Particle::ConstVector&);

  private:
    std::vector<PidParticles> m_pidParticles ;
    LHCb::ParticleID m_motherPid;
    double m_minMass;
    double m_maxMass;
    double m_minP;
    double m_minPt;
    ICheckOverlap* m_checkOverlap ;         ///< Kill candidates based on twice the same track
    IParticleDescendants* m_particleDescendants;
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
      bool fillParticles(const LHCb::Particle::ConstVector&);
      const LHCb::Particle* getCandidate()const {return *m_iterator;};
      bool iterate();
      bool samePid()const{return m_samePID;};
      void setSamePid(bool m){m_samePID = m ;};
      
    private:
      LHCb::Particle::ConstVector m_particles;
      LHCb::Particle::ConstVector::const_iterator m_iterator;
      int m_pid ;
      bool m_samePID; // there is another one with same PID in the decay
    };  
    
  };

};
#endif // MAKERESONANCES_H
