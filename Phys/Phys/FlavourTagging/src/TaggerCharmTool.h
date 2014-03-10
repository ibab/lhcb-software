// $Id: TaggerCharmTool.h,v 1.0 2013-01-23 $
#ifndef USER_TAGGERCHARMTOOL_H
#define USER_TAGGERCHARMTOOL_H 1

// Include files
#include <list>
#include <utility>
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"

// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"

// interface
#include "Kernel/IParticleDescendants.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/ITriggerTisTos.h"
#include "Kernel/IPVReFitter.h"
#include "Kernel/IVertexFit.h"

// from local
#include "FlavourTagging/ITaggingUtils.h"
#include "INNetTool.h"
#include "TMVAWrapper.h"

// struct

struct CharmDecayMode 
{
  std::string mode;
  int index;
  bool hasK, hasE, hasMu, hasDstar;
  float lowMcut, highMcut;
  float mvaCut, purity;
  CharmDecayMode (){}
  CharmDecayMode (std::string s, int idx, bool h1, bool h2, bool h3, bool h4, float lmc, float hmc, float mvac, float pur) : 
    mode(s), index(idx), hasK(h1), hasE(h2), hasMu(h3), hasDstar(h4), lowMcut(lmc), highMcut(hmc), mvaCut(mvac), purity(pur){}
  
};

struct CharmParticle 
{
  std::string mode;
  const LHCb::Particle *part;
  float mass;
  int flavour;
  float pchi2, fd, fdchi2, tau, bpvdira;
  float maxProbGhostDaus;
  float kaonId, kaonProbnnk, kaonIppvchi2, kaonIpMinchi2;
  float elecProbnne, muonProbnnmu;
  float dstarDm;
  CharmParticle (){}
  CharmParticle (const std::string s, const LHCb::Particle* p, const float m, const int flavour,
                 const float pchi2, const float fd, const float fdchi2, const float tau, const float bpvdira, 
                 const float maxProbGhostDaus, 
                 const float kaonId, const float kaonProbnnk, const float kaonIppvchi2, const float kaonIpMinchi2,
                 const float elecProbnne, const float muonProbnnmu, const float dstarDm) 
    : mode(s), part(p), mass(m), flavour(flavour), 
      pchi2(pchi2), fd(fd), fdchi2(fdchi2), tau(tau), bpvdira(bpvdira),
      maxProbGhostDaus(maxProbGhostDaus),
      kaonId(kaonId), kaonProbnnk(kaonProbnnk), kaonIppvchi2(kaonIppvchi2), kaonIpMinchi2(kaonIpMinchi2),
      elecProbnne(elecProbnne), muonProbnnmu(muonProbnnmu), dstarDm(dstarDm) {}
};

  

/** @class TaggerCharmTool TaggerCharmTool.h
 *
 *  Tool to tag the B flavour with the new Charm Tagger
 *
 *  @author Riccardo Cenci, Jack Wimberley
 *  @date   23/01/2013
 */

class TaggerCharmTool : public GaudiTool,
                        virtual public ITagger {

public:
  /// Standard constructor
  TaggerCharmTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );
  virtual ~TaggerCharmTool( ); ///< Destructor

  StatusCode initialize();    ///<  initialization
  StatusCode finalize();      ///<  finalization

  //-------------------------------------------------------------
  virtual LHCb::Tagger  tag( const LHCb::Particle* signalB,
                             const LHCb::RecVertex* RecVert,
                             const int nPV,
                             LHCb::Particle::ConstVector& tagParticles);

  virtual int addCands(std::vector< CharmParticle> & cands, const std::vector< std::string > & locations,
                       const LHCb::Particle& signalB, const LHCb::RecVertex* RecVert, const int type);

  virtual double getMvaVal(const CharmParticle* cpart, const int nPV, const int multiplicity, const LHCb::Particle& signalB);
  
  virtual double getOmega(const CharmParticle* cpart, const int nPV, const int multiplicity, const LHCb::Particle& signalB);
  
private:

  inline double safe_log( const double x ) const
  {
    return ( x>0 ? std::log(x) : -9e30 );
  }
  
  TMVAWrapper * getMVA( const std::string& mode );

private:

  std::map < std::string, CharmDecayMode> CharmDecayModeMap;

  std::vector<std::string> m_CharmTagLocations;
  std::vector<std::string> m_CharmInclTagLocations;
  std::vector<std::string> m_CharmStarTagLocations;

  float m_P0_Cal_charm, m_P1_Cal_charm, m_Eta_Cal_charm;

  //std::string  m_MvaFileDir;

  typedef std::map< std::string, TMVAWrapper* > Classifiers;
  Classifiers m_classifiers;

  ITaggingUtils* m_util;
  ILifetimeFitter*           m_pLifetimeFitter;
  IParticleDescendants* m_descend;
  INNetTool* m_nnet;

};

//===============================================================//
#endif // USER_TAGGERCHARMTOOL_H
