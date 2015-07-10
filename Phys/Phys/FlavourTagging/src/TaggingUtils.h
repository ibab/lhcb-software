#ifndef USER_TAGGINGUTILS_H 
#define USER_TAGGINGUTILS_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event

class IDistanceCalculator;
class IPVReFitter;
class IDVAlgorithm;

#include "FlavourTagging/ITaggingUtils.h"
#include "MultiplePersonalityCall.h"

#include "Kernel/IParticleDescendants.h"

/** @class TaggingUtils TaggingUtils.h
 *
 *  Tool to tag the B flavour with a Electron Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggingUtils : public GaudiTool,
                     virtual public ITaggingUtils
{

public:

  /// Standard constructor
  TaggingUtils( const std::string& type,
                const std::string& name,
                const IInterface* parent );

  virtual ~TaggingUtils( ); ///< Destructor
  StatusCode initialize();  ///<  initialization
  StatusCode finalize();    ///<  finalization

  //----------------------------------------------------------------------------
  // acessors to fitters
  const IPVReFitter*          getPVReFitter() const { return m_PVReFitter; }
  const ILifetimeFitter*      getLifetimeFitter() const { return m_LifetimeFitter; }
  const IVertexFit*           getVertexFitter() const { return m_VertexFitter; }
  const IDistanceCalculator*  getDistanceCalculator() const { return m_DistanceCalculator; }
  IParticleDescendants*       getParticleDescendants() const { return m_ParticleDescendants; }
  
  //-------------------------------------------------------------
  StatusCode calcIP( const LHCb::Particle* axp,
                     const LHCb::VertexBase* v,
                     double& ip, double& iperr) ;

  StatusCode calcIP( const LHCb::Particle*,
                     const LHCb::RecVertex::ConstVector& ,
                     double& , double& );

  StatusCode calcDOCAmin( const LHCb::Particle* axp,
                          const LHCb::Particle* p1,
                          const LHCb::Particle* p2,
                          double& doca, double& docaerr) ;

  int countTracks(const LHCb::Particle::ConstVector& );

  bool isinTree(const LHCb::Particle*,
                 const LHCb::Particle::ConstVector& , double& );

  // classify charm decay modes
  CharmTaggerSpace::CharmMode getCharmDecayMode(const LHCb::Particle*, int);
  
  // remove any charm cand that has descendents in common with the signal B
  LHCb::Particle::ConstVector purgeCands(const LHCb::Particle::Range& cands, const LHCb::Particle& BS);

  //-------------------------------------------------------------



private:

  IDVAlgorithm* m_dva;
  std::string m_personality;

  std::string m_PVSelCriterion;

  std::string m_algNamePVReFitter;
  std::string m_algNameLifetimeFitter;
  std::string m_algNameVertexFitter;
  std::string m_algNameDistanceCalculator;
  std::string m_algNameParticleDescendants;

  const IPVReFitter*          m_PVReFitter;
  const ILifetimeFitter*      m_LifetimeFitter;
  const IVertexFit*           m_VertexFitter;
  const IDistanceCalculator*  m_DistanceCalculator;
  IParticleDescendants* m_ParticleDescendants; // cannot be const, as 
                                               // descendants() is not a const 
                                               // function.

  MultiplePersonalityCall<boost::function<
      int(const LHCb::Particle::ConstVector&)> > m_countTracks;
  MultiplePersonalityCall<boost::function<
      bool(const LHCb::Particle*,
	      const LHCb::Particle::ConstVector&, double&) > > m_isinTree;

  int countTracksReco12(const LHCb::Particle::ConstVector& );
  int countTracksReco14(const LHCb::Particle::ConstVector& );

  bool isinTreeReco12(const LHCb::Particle*,
                 const LHCb::Particle::ConstVector& , double& );

  bool isinTreeReco14(const LHCb::Particle*,
                 const LHCb::Particle::ConstVector& , double& );

  unsigned int lambda_pid, pi_pid, pi0_pid, k_pid, ks_pid, p_pid, e_pid, mu_pid, d0_pid, d_pid, lambdaC_pid;

};

//===============================================================//
#endif // USER_TAGGINGUTILS_H
