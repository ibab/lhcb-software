#ifndef INTERNALRECOTRACK_H
#define INTERNALRECOTRACK_H

#include "InternalParticle.h"
#include "Event/State.h"

class ITrackStateProvider ;
namespace LHCb {
  class TrackTraj ;
  class Track ;
}


namespace DecayTreeFitter 
{

  class InternalRecoTrack : public InternalParticle
  {
  public:
    InternalRecoTrack(const LHCb::Particle& bc, const ParticleBase* mother, 
		      const Configuration& config) ;
    ~InternalRecoTrack() ;
    virtual int type() const { return kInternalRecoTrack ; }
    virtual int dimM() const { return 5 ; }

    virtual ErrCode projectRecoConstraint(const FitParams&, Projection&) const ;
    ErrCode updCache(const FitParams& fitparams) ;
    
    virtual void addToConstraintList(constraintlist& alist, int depth) const {
      InternalParticle::addToConstraintList( alist, depth ) ;
      alist.push_back(Constraint(this,Constraint::track,depth,dimM()) ) ; 
    }

    virtual ErrCode projectConstraint(Constraint::Type, const FitParams&, Projection&) const ;
    
  private:
    //ErrCode updFltToMother(const FitParams& fitparams) ;
    void setFlightLength(double flt) { m_flt = flt ; }
    const LHCb::Track& track() const { return *m_track ; }
    const LHCb::State& state() const { return m_state ; }

  private:
    const LHCb::Track* m_track ;
    const ITrackStateProvider* m_stateprovider ;
    bool m_useTrackTraj ;
    const LHCb::TrackTraj* m_tracktraj ;
    bool m_cached ;
    double m_flt ;
    LHCb::State m_state ;
  } ;
}


#endif
