#ifndef USER_TAGGINGUTILS_H 
#define USER_TAGGINGUTILS_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event

class IDistanceCalculator;
class IPVReFitter;
class IDVAlgorithm;

#include "ITaggingUtils.h"
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

  // remove any charm cand that has descendents in common with the signal B
  LHCb::Particle::ConstVector purgeCands(const LHCb::Particle::Range& cands, const LHCb::Particle& BS);

  // classify charm decay modes
  std::string getCharmDecayMode(const LHCb::Particle*, int);

  //-------------------------------------------------------------

private:

  std::string m_ChoosePV ;
  std::string m_personality;
  const IDistanceCalculator *m_Dist;
  IDVAlgorithm* m_dva;
  const IPVReFitter* m_pvReFitter;

  IParticleDescendants*      m_descend;

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

  const IDistanceCalculator* getDistanceCalculator() { return m_Dist;}

};

//===============================================================//
#endif // USER_TAGGINGUTILS_H
