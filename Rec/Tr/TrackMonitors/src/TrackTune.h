#ifndef TrackTune_H_
#define TrackTune_H_

#include "GaudiAlg/GaudiTupleAlg.h"
#include "Event/Track.h"
#include "Event/Particle.h"

namespace LHCb{
  class Particle;
}

class TrackTune: public GaudiTupleAlg{

 public:

  virtual ~TrackTune();

  TrackTune(const std::string& name, ISvcLocator* pSvc);

  virtual StatusCode initialize();  

  virtual StatusCode execute();


 private:

  const LHCb::Track* track(const LHCb::Particle* part) const;

  bool isFound(const LHCb::Tracks* tracks, const LHCb::Particle* part) const;

  bool select(std::vector<const LHCb::Particle* >& output, const LHCb::Particle::Container* input) const; 

  bool inMassRange(const LHCb::Particle* particle) const;

  std::string m_particleLocation;
  std::string m_trackLocation;
  std::string m_resonanceName;

  double m_deltaMass;
  double m_minMass;
  double m_maxMass;
  bool m_selectBest;
  double m_minPurityCut;


};


#endif
