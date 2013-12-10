// $Id: TaggerCharmTool.h,v 1.0 2013-01-23 $
#ifndef USER_TAGGERCHARMTOOL_H
#define USER_TAGGERCHARMTOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"

//from ROOT
//#include "TMVA/Reader.h"

// interface
#include "Kernel/IParticleDescendants.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/ITriggerTisTos.h"
#include "Kernel/IPVReFitter.h"
#include "Kernel/IVertexFit.h"

// mva classes
#include "mva_charmtagger_reco14/train_D0_Kpi_BDT.C"
#include "mva_charmtagger_reco14/train_D0_Kpipipi_BDT.C"
//#include "mva_charmtagger_reco14/train_D0_Kspipi_BDT.C"
#include "mva_charmtagger_reco14/train_D0_Kpipi0_BDT.C"
#include "mva_charmtagger_reco14/train_Dp_Kpipi_BDT.C"
#include "mva_charmtagger_reco14/train_Dp_Kspi_BDT.C"
#include "mva_charmtagger_reco14/train_D0_KpiX_BDT.C"
#include "mva_charmtagger_reco14/train_D0_KeX_BDT.C"
#include "mva_charmtagger_reco14/train_D0_KmuX_BDT.C"
#include "mva_charmtagger_reco14/train_Dstar_D0_Kspipi_BDT.C"
#include "mva_charmtagger_reco14/train_Dp_KpiX_BDT.C"
#include "mva_charmtagger_reco14/train_Dp_KeX_BDT.C"
#include "mva_charmtagger_reco14/train_Dp_KmuX_BDT.C"


// from local
#include <list>
#include <utility>
#include <string>


#include "ITaggingUtils.h"
#include "INNetTool.h"


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
 *  Tool to tag the B flavour with the new KaonOpposite Tagger
 *
 *  @author rcenci
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

  std::map < std::string, CharmDecayMode> CharmDecayModeMap;

  std::vector<std::string> m_CharmTagLocations;
  std::vector<std::string> m_CharmInclTagLocations;
  std::vector<std::string> m_CharmStarTagLocations;

  float m_P0_Cal_charm, m_P1_Cal_charm, m_Eta_Cal_charm;

  //std::string  m_MvaFileDir;

  //std::map< std::string,TMVA::Reader* > m_readers;
  std::map< std::string,IClassifierReader* > m_classifiers;
  
  float m_eval_mode;
  float m_eval_recverts, m_eval_sigBpt, m_eval_ntrks;
  float m_eval_mm, m_eval_p, m_eval_pt, m_eval_dEta;
  float m_eval_probchi2, m_eval_tau, m_eval_fd, m_eval_fdchi2, m_eval_bpvdira;
  float m_eval_maxProbnnGhostDaus, m_eval_kProbnnk, m_eval_kIppvchi2, m_eval_kIpMinchi2;
  float m_eval_eProbnne, m_eval_muProbnnmu;
  float m_eval_dstarDm;

  ITaggingUtils* m_util;
  ILifetimeFitter*           m_pLifetimeFitter;
  IParticleDescendants* m_descend;
  INNetTool* m_nnet;

};

//===============================================================//
#endif // USER_TAGGERCHARMTOOL_H
