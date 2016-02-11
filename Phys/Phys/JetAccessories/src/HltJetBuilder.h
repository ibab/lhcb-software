#ifndef HLTJETBUILDER_H
#define HLTJETBUILDER_H 1

// STL.
#include <unordered_map>
#include <unordered_set>

// Gaudi.
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/GaudiTool.h"

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
 * Inputs are set via the 'Inputs' property which must be of the form
 * [[<class name>, <object type>, <location>], ...] where all three
 * arguments for each input are strings. Two class names can be used:
 * 'Particle' and 'RecVertex'.
 *
 * For 'Particle' inputs the three object types 'ban', 'use', and
 * 'tag' can be passed. A 'ban' set of Particles will be banned from
 * the jet input via the associated Tracks and CaloClusters of the
 * object. Note that Track to CaloCluster matching is not
 * performed. If type 'use' is specified, these Particles are used as
 * input. Finally, if type 'tag' is specified, these Particles are
 * used to tag the jets. If a jet has been tagged the extra info with
 * key Particle::FirstJetIndex + 100 + tag input index will be set to
 * one.
 *
 * 'RecVertex' inputs should only be supplied when one wishes to
 * perform JEC, as the number of PVs in the event is needed to
 * determine which JEC to apply. Note that only one 'RecVertex' input
 * should be supplied and the type should always be 'vertex'.
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
 * @class  HltJetBuilder
 * @file   HltJetBuilder.h
 * @author Victor Coco, Cedric Potterat, and Philip Ilten
 * @date   2016-02-02
 */
class HltJetBuilder : public GaudiHistoAlg {
public:
  /// Constructor.
  HltJetBuilder(const string &name, ISvcLocator *svc);
  virtual StatusCode initialize(); ///< Initialize.
  virtual StatusCode execute   (); ///< Execute.
  virtual StatusCode finalize  (); ///< Finalize.

private:
  /**
   * Input structure class for JB. 
   *
   * Used to parse the input lists passed via #m_inLocs.
   */
  class Input {
  public:
    /// Constructor.
    Input(const unsigned int *idx = 0, const vector<string> *prp = 0);
    /// Print the input configuration to #out.
    inline void print(MsgStream &msg, bool idx = true);
    
    // Members.
    string warn;  ///< Any warnings that may have been generated.
    string name;  ///< The class name of objects from this input.
    string type;  ///< The object type for this input.
    string loc;   ///< The TES location for this input.
    bool   valid; ///< Flag if input is valid.
    bool   ban;   ///< Flag if objects from this input should be banned.
    bool   tag;   ///< Flag if objects from this input are tags.
    unsigned int index; ///< Index for this input.
  };

  /// Definition of map relating XYZPoints for PVs to Particles.
  typedef unordered_map<Gaudi::XYZPoint, vector<const Particle*> > VrtsPrts;

  // Additional methods.
  /// Build jets from #prts and add to the TES.
  void build(const vector<const Particle*> *prts, const Gaudi::XYZPoint *vrt);
  void info(Particle *prt);                ///< Add additional information.
  void jec(Particle *prt);                 ///< Perform jet energy correction.
  inline void ban(const Particle *prt);    ///< Ban a Particle.
  inline bool banned(const Particle *prt); ///< Check if a Particle is banned.

  // Input/output property members.
  /// JB inputs of the form [<class name>, <object type>, <location>].
  vector< vector<string> > m_inLocs;
  /// JB output location prefix; objects will be placed in
  /// <Output>/Particles, etc.
  string m_outLoc;

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

  // LoKi functor members.
  LoKi::Types::Fun m_lokBpvX;   ///< Best PV x-position.
  LoKi::Types::Fun m_lokBpvY;   ///< Best PV y-position.
  LoKi::Types::Fun m_lokBpvZ;   ///< Best PV z-position.
  LoKi::Types::Fun m_lokBpvPhi; ///< Best PV phi.
  LoKi::Types::Fun m_lokBpvEta; ///< Best PV eta.
  LoKi::Types::Fun m_lokTrkN;   ///< Number of tracks.

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
  IJetMaker    *m_fj;     ///< The FastJet tool.
  Particles    *m_jets;   ///< Output jet Particles.
  Input         m_inVrts; ///< RecVertex input.
  vector<Input> m_inBans; ///< Vector of Particle ban inputs.
  vector<Input> m_inTags; ///< Vector of Particle tag inputs.
  vector<Input> m_inPrts; ///< Vector of Particle inputs.
  /// Set of banned Tracks.
  unordered_set<const Track*> m_trks;
  /// Set of banned CaloClusters.
  unordered_set<const CaloCluster*> m_cals;
};

namespace std {
  /// Hash function for Gaudi::XYZPoint, needed to use in unordered_map.
  template <> struct hash<Gaudi::XYZPoint> {
    size_t operator()(const Gaudi::XYZPoint &vrt) const {
      return ((hash<double>()(vrt.x()) ^ (hash<double>()(vrt.y()) << 1)) >> 1)
	^ (hash<double>()(vrt.z()) << 1);}
  };
}

#endif // HLTJETBUILDER_H
