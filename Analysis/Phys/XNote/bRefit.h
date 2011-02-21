#ifndef bRefit_H_
#define bRefit_H_

#include "Kernel/DVAlgorithm.h"
#include "Event/TrackTypes.h"

class IParticle2State;
class ITrackFitter;
class ITrackStateInit;

namespace LHCb{
  class Track;
}

class ITransportSvc;
class ITrackManipulator;
class ITrackExtrapolator;

class bRefit: public DVAlgorithm{

 public:

  virtual ~bRefit();

  bRefit(const std::string& name, ISvcLocator* pSvc);

  virtual StatusCode initialize();
  
  virtual StatusCode execute();

  virtual StatusCode finalize();

 private:

  ITrackExtrapolator* m_trackExtrapolator;
  IParticle2State* m_pToState;
  ITrackFitter* m_trackFit;
  ITrackStateInit* m_trackStateInit;
  LHCb::Track* track(const LHCb::Particle* part) const;
  void refit(std::vector<LHCb::Particle*>& daughters, bool isBasic, double z) const;

  void scaleErrors(LHCb::State* state) const;

  double m_txScale;
  double m_tyScale;
  double m_qpScale;
  bool m_scaleErrors;
  
  Gaudi::TrackMatrix m_scaleMatrix;
  
};




#endif
