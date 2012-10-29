#ifndef RECOTRACK_H
#define RECOTRACK_H

#include "GaudiKernel/Vector3DTypes.h"
#include "RecoParticle.h"
#include "Configuration.h"

class ITrackStateProvider ;
namespace LHCb {
  class TrackTraj ;
}

namespace DecayTreeFitter
{

  class RecoTrack : public RecoParticle
  {
  public:
    RecoTrack(const LHCb::Particle& bc, const ParticleBase* mother,
	      const Configuration& config) ;
    virtual ~RecoTrack() ;

    virtual ErrCode initPar2(FitParams*) ;
    virtual ErrCode initCov(FitParams*) const ;
    virtual int dimM() const { return 5 ; }
    virtual int type() const { return kRecoTrack ; }

    virtual ErrCode projectRecoConstraint(const FitParams&, Projection&) const ;
    ErrCode updCache(const FitParams& fitparams) ;
    //tatic void setApplyCovCorrection(bool b=true) { gApplyCovCorrection = b ; }
    //static void correctCov(HepSymMatrix& V) ;
    
    virtual int nFinalChargedCandidates() const { return 1 ; }
    
    virtual void addToConstraintList(constraintlist& alist, int depth) const {
      alist.push_back(Constraint(this,Constraint::track,depth,dimM()) ) ; 
    }
    //ErrCode updFltToMother(const FitParams& fitparams) ;
    void setFlightLength(double flt) { m_flt = flt ; }
    const LHCb::Track& track() const { return *m_track ; }
    const LHCb::State& state() const { return m_state ; }
  private:
    const Gaudi::XYZVector m_bfield ;
    const LHCb::Track* m_track ;
    const ITrackStateProvider* m_stateprovider ;
    bool m_useTrackTraj ;
    const LHCb::TrackTraj* m_tracktraj ;
    bool m_cached ;
    double m_flt ;
    LHCb::State m_state ;
    double m_bremEnergy ;
    double m_bremEnergyCov ;
  } ;

}
#endif
