#ifndef HLTJETBUILDER_H
#define HLTJETBUILDER_H 1

// STL.
#include <unordered_map>
#include <unordered_set>

// Gaudi.
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/GaudiTool.h"

// Relations.
#include "Kernel/Particle2Vertex.h"

// Jets.
#include "Kernel/IJetMaker.h"

// Event.
#include "Event/RecVertex.h"
#include "Event/Particle.h"
#include "Event/Track.h"
#include "Event/CaloCluster.h"

// LoKi.
#include "LoKi/PhysExtract.h"
#include "LoKi/ParticleCuts.h"

using namespace std;
using namespace LHCb;

/**
 * TO-DO.
 *
 * Particle: ban, use
 * Particle2Vertex: table
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
    inline void print(MsgStream &msg);
    
    // Members.
    string warn;  ///< Any warnings that may have been generated.
    string name;  ///< The class name of objects from this input.
    string type;  ///< The object type for this input.
    string loc;   ///< The TES location for this input.
    bool   valid; ///< Flag if input is valid.
    bool   ban;   ///< Flag if objects from this input should be banned.
  };

  /// Definition of map relating RecVertices to Particles.
  typedef unordered_map<const VertexBase*, vector<const Particle*> > VrtsPrts;

  // Ban or check a Particle.
  inline void ban(const Particle *prt);    //< Ban a Particle.
  inline bool banned(const Particle *prt); //< Check if a Particle is banned.

  // Input/output property members.
  /// JB inputs of the form [<class name>, <object type>, <location>].
  vector< vector<string> > m_inLocs;
  /// JB output location prefix; objects will be placed in
  /// <Output>/Particles and <Output>/Particle2Vertex.
  string m_outLoc;

  // Jet property members.
  /// Particle ID to assign the jets.
  int m_jetPid;
  /// Minimum jet pT to build with FastJet.
  double m_jetPtMin;
  /// Jet radius parameter to use with FastJet.
  double m_jetR;
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

  // Additional members (not properties).
  IJetMaker    *m_fj;         ///< The FastJet tool.
  vector<Input> m_inPrts;     ///< Vector of Particle inputs.
  vector<Input> m_inPrtsVrts; ///< Vector of Particle to RecVertex inputs.
  /// Set of banned Tracks.
  unordered_set<const Track*> m_trks;
  /// Set of banned CaloClusters.
  unordered_set<const CaloCluster*> m_cals;
};

#endif // HLTJETBUILDER_H
