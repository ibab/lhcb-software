#ifndef DToKPiTwoProng_H_
#define DToKPiTwoProng_H_

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"


namespace LHCb{

  class Track;
  class Particle;
  class TwoProngVertex;

}

class ILHCbMagnetSvc;
class IHitExpectation;
class ITrackVertexer;
class ITrackExtrapolator;

class ITrackFitter;


class DToKPiTwoProng: public GaudiTupleAlg{

 public:


  virtual ~DToKPiTwoProng();

  DToKPiTwoProng(const std::string& name, ISvcLocator* pSvc);

  virtual StatusCode initialize();
  
  virtual StatusCode execute();

  virtual StatusCode finalize();
  
 private:

  LHCb::Track* track(const LHCb::Particle* part) const;


  LHCb::TwoProngVertex* refittedMass(LHCb::Track* track1, 
                               LHCb::Track* track2,
                               const double zVert ) const;

  double PIDK(const LHCb::Particle* part) const;


  ITrackVertexer* m_vertexer;
  ITrackExtrapolator* m_trackExtrapolator;

  ITrackFitter* m_trackFit;
  ITrackFitter* m_trackPreFit;

  std::string m_particleLocation;
  std::string m_vertexLocation;
  std::string m_trackOutputLocation;
  std::string m_resonanceName;

  double m_pionMass;
  double m_kaonMass;
  double m_deltaMass;
  double m_minMass;
  double m_maxMass;


};

#endif
