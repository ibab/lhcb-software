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

// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"

// interface
#include "Kernel/IParticleDescendants.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/ITriggerTisTos.h"
#include "Kernel/IPVReFitter.h"
#include "Kernel/IVertexFit.h"
#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>

// from local
#include "FlavourTagging/ITaggingUtils.h"
//#include "INNetTool.h"
#include "TMVAWrapper.h"
//#include "CharmOSWrapper.h"
// struct

namespace CharmTaggerSpace {
  
  const double charmOmegaBDTCut = 0.45;
  
  
  struct CharmDecay 
  {
    std::string name;
    CharmMode index;
    bool hasP, hasK, hasPi, hasE, hasMu, hasDstar;
    double natMistag;
    CharmDecay (){}
    CharmDecay (std::string s, int idx, bool h1, bool h2, bool h3, bool h4, bool h5, bool h6, double nmt) :
      name(s),
      index(static_cast<CharmTaggerSpace::CharmMode>(idx)),
      hasP(h1),
      hasK(h2),
      hasPi(h3),
      hasE(h4),
      hasMu(h5),
      hasDstar(h6),
      natMistag(nmt) {}
  };
  
  typedef std::map < CharmTaggerSpace::CharmMode, CharmTaggerSpace::CharmDecay> CharmDecayMap;

  struct CharmParticle 
  {
    CharmMode mode;
    const LHCb::Particle *part;
    float mass;
    int flavour;
    float pchi2, fd, fdchi2, tau, bpvdira;
    float maxProbGhostDaus;
    float proId, proProbnnp, proProbnnk, proIppvchi2, proIpMinchi2, proPT;
    float kaonId, kaonProbnnp, kaonProbnnk, kaonIppvchi2, kaonIpMinchi2, kaonPT;
    float pionId, pionProbnnp, pionProbnnk, pionIppvchi2, pionIpMinchi2, pionPT;
    float elecProbnne, elecPT, muonProbnnmu, muonPT;
    float dstarDm;
    CharmParticle (){}
    CharmParticle (CharmTaggerSpace::CharmMode mode, const LHCb::Particle* p, const float m, const int flavour,
                   const float pchi2, const float fd, const float fdchi2, const float tau, const float bpvdira, 
                   const float maxProbGhostDaus, 
                   const float proId, const float proProbnnp, const float proProbnnk, const float proIppvchi2, const float proIpMinchi2, const float proPT,
                   const float kaonId, const float kaonProbnnp, const float kaonProbnnk, const float kaonIppvchi2, const float kaonIpMinchi2, const float kaonPT,
                   const float pionId, const float pionProbnnp, const float pionProbnnk, const float pionIppvchi2, const float pionIpMinchi2, const float pionPT,
                   const float elecProbnne, const float elecPT, const float muonProbnnmu, const float muonPT, const float dstarDm) 
      : mode(mode), part(p), mass(m), flavour(flavour), 
        pchi2(pchi2), fd(fd), fdchi2(fdchi2), tau(tau), bpvdira(bpvdira),
        maxProbGhostDaus(maxProbGhostDaus),
        proId(proId), proProbnnp(proProbnnp), proProbnnk(proProbnnk), proIppvchi2(proIppvchi2), proIpMinchi2(proIpMinchi2), proPT(proPT),
        kaonId(kaonId), kaonProbnnp(kaonProbnnp), kaonProbnnk(kaonProbnnk), kaonIppvchi2(kaonIppvchi2), kaonIpMinchi2(kaonIpMinchi2), kaonPT(kaonPT),
        pionId(pionId), pionProbnnp(pionProbnnp), pionProbnnk(pionProbnnk), pionIppvchi2(pionIppvchi2), pionIpMinchi2(pionIpMinchi2), pionPT(pionPT),
        elecProbnne(elecProbnne), elecPT(elecPT), muonProbnnmu(muonProbnnmu), muonPT(muonPT), dstarDm(dstarDm) {}
  };
}

  

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

  virtual StatusCode initialize();    ///<  initialization
  virtual StatusCode finalize();      ///<  finalization

  //-------------------------------------------------------------
  virtual LHCb::Tagger  tag( const LHCb::Particle* signalB,
                             const LHCb::RecVertex* RecVert,
                             const int nPV,
                             LHCb::Particle::ConstVector& tagParticles);

  virtual int addCands(std::vector< CharmTaggerSpace::CharmParticle> & cands, const std::vector< std::string > & locations,
                       const LHCb::Particle& signalB, const LHCb::RecVertex* RecVert, const int type);

  virtual double getMvaVal(const CharmTaggerSpace::CharmParticle* cpart, const int nPV, const int multiplicity, const LHCb::Particle& signalB);
  
  virtual double getOmega(double bdtMistag, CharmTaggerSpace::CharmMode mode);  
  virtual double calModeOmega(double bdtMistag, CharmTaggerSpace::CharmMode mode);  
  virtual double calOmega(double bdtMistag);  
  
private:

  inline double safe_log( const double x ) const
  {
    return ( x>0 ? std::log(x) : -9e30 );
  }
  
  TMVAWrapper * getMVA( CharmTaggerSpace::CharmMode );

  // classify charm decay modes
  CharmTaggerSpace::CharmMode getCharmDecayMode(const LHCb::Particle*, int);

private:

  //  CharmOSWrapper * m_myDATAreader;

  CharmTaggerSpace::CharmDecayMap decayMap;

  std::vector<std::string> m_CharmTagLocations;
  std::vector<std::string> m_CharmInclTagLocations;
  std::vector<std::string> m_CharmLambdaTagLocations;

  double m_P0_Cal_charm, m_P1_Cal_charm, m_Eta_Cal_charm;

  //std::string  m_MvaFileDir;

  typedef std::map< CharmTaggerSpace::CharmMode, TMVAWrapper* > Classifiers;
  Classifiers m_classifiers;

  ITaggingUtils* m_util;
  std::string m_pLifetimeFitterAlgName;
  const ILifetimeFitter*           m_pLifetimeFitter;
  IParticleDescendants* m_descend;
  IDVAlgorithm* m_dva;

  LHCb::ParticleID ID_Lambda0;
  LHCb::ParticleID ID_Lambda0Bar;
  LHCb::ParticleID ID_PiP;
  LHCb::ParticleID ID_PiM;
  LHCb::ParticleID ID_KP;
  LHCb::ParticleID ID_KM;
  LHCb::ParticleID ID_KS0;
  LHCb::ParticleID ID_PP;
  LHCb::ParticleID ID_PM;
  LHCb::ParticleID ID_EP;
  LHCb::ParticleID ID_EM;
  LHCb::ParticleID ID_MuP;
  LHCb::ParticleID ID_MuM;

};

//===============================================================//
#endif // USER_TAGGERCHARMTOOL_H
