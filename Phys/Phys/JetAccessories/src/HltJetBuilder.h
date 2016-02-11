#ifndef HLTJETBUILDER_H
#define HLTJETBUILDER_H 1

// STL.
#include <unordered_map>
#include <unordered_set>

// DaVinci.
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/DaVinciHistoAlgorithm.h"

// Jets.
#include "Kernel/IJetMaker.h"
#include "Kernel/JetEnums.h"

// Event.
#include "Event/RecVertex.h"
#include "Event/Particle.h"
#include "Event/Track.h"
#include "Event/CaloCluster.h"

// LoKi.
#include "LoKi/PhysExtract.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/ParticleContextCuts.h"

// Histograms.
#include "GaudiUtils/Aida2ROOT.h"
#include "AIDA/IHistogram3D.h"

// Root.
#include "TH1D.h"
#include "Math/VectorUtil.h"

using namespace std;
using namespace LHCb;

/**
 * Jet building (JB) algorithm class for use in the HLT and offline.
 *
 * JB takes Particle inputs and clusters them into jets using the
 * FastJet package. Optionally, it can then correct the jet energy
 * (JEC), calculate additional information about the jet, and
 * associate tag Particles with the jets.
 *
 * Jets can be built per primary vertex or inclusively by setting
 * 'JetVrt' to true or false respectively. If built inclusively, all
 * inputs that are not banned are used to build jets. If built per PV,
 * then particles are associated to PVs if they contain one or more
 * tracks. Particles without tracks are associated to all PVs. The,
 * jets are built using the input for each PV and the PV location is
 * set as the reference point of the jet. Note that when building per
 * PV, the neutrals are double counted when considering jets from all
 * PVs.
 *
 * Particles, both neutral and charged, can be banned from the jet
 * input via locations set with the 'InputBans' property. Note that
 * the CaloClusters matched to a Track from a charged Particle are not
 * banned.
 *
 * Jets can be tagged via Particles from locations provided with the
 * 'InputTags' property. If the Particle has a vertex, the best PV to
 * vertex flight direction will be used to match it to the jet,
 * otherwise the momentum four-vector will be used. If delta R between
 * the Particle and the jet is less than the 'JetR' property, the jet
 * will be marked as tagged. This is done by setting the extra info
 * for the Particle with key Particle::FirstJetIndex + 100 + tag
 * location index. The value is set as the key of the tagging
 * Particle.
 *
 * @class  HltJetBuilder
 * @file   HltJetBuilder.h
 * @author Victor Coco, Cedric Potterat, and Philip Ilten
 * @date   2016-02-02
 */
class HltJetBuilder : public DaVinciHistoAlgorithm {
public:
  /// Constructor.
  HltJetBuilder(const string &name, ISvcLocator *svc);
  virtual StatusCode initialize(); ///< Initialize.
  virtual StatusCode execute   (); ///< Execute.
  virtual StatusCode finalize  (); ///< Finalize.

private:
  /// Definition of map relating RecVertices to Particles.
  typedef unordered_map<const VertexBase*, vector<const Particle*> > VrtsPrts;

  // Additional methods.
  /// Build jets from #prts and add to the TES.
  void build(const vector<const Particle*> *prts, const VertexBase *vrt);
  void info(Particle *prt);                ///< Add additional information.
  void jec(Particle *prt);                 ///< Perform jet energy correction.
  inline void ban(const Particle *prt);    ///< Ban a Particle.
  inline bool banned(const Particle *prt); ///< Check if a Particle is banned.

  // Input/output property members (Inputs and Output handled by base class).
  vector<string> m_inBans; ///< Input locations of Particles to ban.
  vector<string> m_inTags; ///< Input locations of Particle to tag.

  // Jet property members.
  /// Particle ID to assign the jets.
  int m_jetPid;
  /// Minimum jet pT to build with FastJet.
  double m_jetPtMin;
  /// Jet radius parameter to use with FastJet.
  double m_jetR;
  /// If true, build jets for each primary vertex, otherwise build inclusively.
  bool m_jetVrt;
  /// Jet sorting method: 1 E, 2 pT, or 3 y. 
  int m_jetSort;
  /// Write out extra info for the jets.
  bool m_jetInfo;
  /// If supplied, perform jet energy correction using histograms from
  /// this path.
  string m_jetEcPath;
  /// Shift the jet energy correction by this (in JEC sigma).
  double m_jetEcShift;

  // FastJet property members.
  /// FastJet finder to use: 0 kt, 1 cambridge, 2 anti-kt, ...; see
  /// fasjet::JetAlgorithm for more options.
  int m_fjAlg;
  /// FastJet recombination scheme: 0 E, 1 pT, 2 pT^2, ...; see
  /// fastjet::RecombinationScheme for more options.
  int m_fjScheme;
  /// FastJet clustering strategy: 0 N3Dumb, 1 Best, 2 NlnN, ...; see
  /// fastjet::Strategy for more options.
  int m_fjStrategy;

  // Jet energy correction members.
  static const int m_jecNPvs = 4;  ///< Number of n(PV) JEC bins.
  static const int m_jecNEta = 11; ///< Number of eta JEC bins.
  static const int m_jecNCpf = 4;  ///< Number of (sum charged pT)/pT JEC bins.
  static const int m_jecNPhi = 3;  ///< Number of phi JEC bins.
  static const int m_jecNPt  = 1;  ///< Number of pT JEC bins.
  /// Limits of the JEC eta bins.
  static const double m_jecLimEta[m_jecNEta + 1];
  /// Limits of the JEC (sum charged pT)/pT bins.
  static const double m_jecLimCpf[m_jecNCpf + 1];
  /// Limits of the JEC phi bins.
  static const double m_jecLimPhi[m_jecNPhi + 1];
  /// Limits of the JEC pT bins.
  static const double m_jecLimPt[m_jecNPt + 1];
  /// Number of PVs for the event (try RecVertices, then association).
  int m_jecPvs;
  /// Jet energy correction histograms.
  TH1D *m_jecs[m_jecNPvs][m_jecNEta][m_jecNCpf][m_jecNPhi]; 

  // Additional members (not properties).
  IJetMaker        *m_fj;   ///< The FastJet tool.
  vector<Particle*> m_jets; ///< Vector of output jets.
  /// Set of banned Tracks.
  unordered_set<const Track*> m_trks;
  /// Set of banned CaloClusters.
  unordered_set<const CaloCluster*> m_cals;
};

#endif // HLTJETBUILDER_H
