#ifndef DISPLVERTICES_DISPLACEDVERTEXJETCANDIDATEMAKERS20P3_H
#define DISPLVERTICES_DISPLACEDVERTEXJETCANDIDATEMAKERS20P3_H 1

#include "Kernel/DaVinciAlgorithm.h"

#include "Kernel/IJetMaker.h"
#include "Kernel/IExtraInfoTool.h"

#include "LoKi/PhysTypes.h"

/** @class DisplacedVertexJetCandidateMakerS20p3 DisplacedVertexJetCandidateMakerS20p3.h DisplVertices/DisplacedVertexJetCandidateMakerS20p3.h
 *
 * A class to create N-jet candidates out of displaced vertex candidates, Stripping20p3 version
 *
 * (merged PFJetMakerForDVAlg and MakeNJetsCandidate for Stripping20r*p3)
 * For each input DV candidate (passed through Inputs), the particle flow inputs
 * are filtered using the input cut code, and partitioned according to the
 * specified IP and IPCHI2 cut values.
 * Next, jets are reconstructed using the jet maker tool,
 * their energy is corrected using a JEC tool (particle refitter with nickname "JEC")
 * and the JetID variables are calculated.
 * Then the JetID selection is applied using the code,
 * and the remaining jets, if there are enough, are combined into a new candidate,
 * which is added to the output container.
 *
 * Inputs: DV candidates
 * Output: N-jet candidates
 * PF inputs should be passed as "PFInputs"
 *
 * @author Victor Coco
 * @author Wouter Hulsbergen
 * @author Pieter David
 * @date   2013-12-18
 */
class DisplacedVertexJetCandidateMakerS20p3 : public DaVinciAlgorithm {
public:
  /// Standard constructor
  DisplacedVertexJetCandidateMakerS20p3( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DisplacedVertexJetCandidateMakerS20p3();  ///< Destructor

  virtual StatusCode initialize();              ///< Algorithm initialization
  virtual StatusCode execute   ();              ///< Algorithm execution
  virtual StatusCode finalize  ();              ///< Algorithm finalization
private: // TODO clean up and reorder
  std::string m_pfInputLocation;
  std::string m_pfInputCode;
  // partitioning
  LoKi::Types::Fun PFTYPE;
  LoKi::Types::Cut ISGOODINPUT;
  LoKi::Types::Cut ISPERVINPUT;
  LoKi::Types::Cut ISALLVINPUT;
  LoKi::Types::Cut HASPOINTINGINFO;
  double m_maxIP2DV;
  double m_minIp2PV;
  double m_minIpChi22PV;
  double m_maxIpChi22DVDown;
  LoKi::Types::Fun BestPVIPChi2;
  LoKi::Types::Fun MinPVIP;

  const IDistanceCalculator* m_dist;

  std::string m_jetMakerName;
  const IJetMaker* m_jetMaker;

  const IParticleReFitter* m_JECtool;

  // JET ID
  std::string m_jetIDInfoToolName;
  IExtraInfoTool* m_jetIDInfoTool;
  void addJetIDInfo( LHCb::Particle* jet );
  std::string m_jetIDCode;
  LoKi::Types::Cut JETIDCUT;

  unsigned int m_minJet;                               ///< Minimum number of jets per candidates
  const IParticleCombiner* m_combiner;
  std::string m_candPIDName;
  const LHCb::ParticleProperty* m_candProp;
  bool makeCandidate( const LHCb::Particle::Vector& daughters, const LHCb::Particle* strippingCandidate );

  // for parsing cuts
  std::string m_factory;
  std::vector<std::string> m_preambulo;
  std::string preambulo() const;
  StatusCode updateCuts();
  void updateHandlerCuts( Property& p );
};
#endif // DISPLVERTICES_DISPLACEDVERTEXJETCANDIDATEMAKERS20P3_H
