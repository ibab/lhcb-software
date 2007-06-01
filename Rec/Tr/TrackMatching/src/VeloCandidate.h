// $Id: VeloCandidate.h,v 1.1 2007-06-01 11:53:59 mneedham Exp $
#ifndef TRACKMATCHING_VELOCANDIDATE_H
#define TRACKMATCHING_VELOCANDIDATE_H 1

// from TrackEvent
#include "Event/Track.h"
#include "Event/State.h"

// Gaudi
#include "GaudiKernel/boost_allocator.h"

#include <functional>
#include <algorithm>

class VeloCandidate{

 public:

  VeloCandidate(LHCb::Track* aTrack,
                const LHCb::State& state,
                double yAtT );

  ~VeloCandidate(){}

  LHCb::Track* track() const;

  const LHCb::State& state() const;

  const Gaudi::TrackVector& trackVector() const;

  const Gaudi::TrackSymMatrix& covariance() const; 

  double nLHCbIDs() const;

  double yAtT() const;

  double norm2() const;
 
#ifndef _WIN32

    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(VeloCandidate) == size ?
               boost::singleton_pool<VeloCandidate, sizeof(VeloCandidate)>::malloc() :
               ::operator new(size) );
    }

    /// placement operator new
    /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
    /// it is not needed in libstdc++ >= 3.4
    static void* operator new ( size_t size, void* pObj )
    {
      return ::operator new (size,pObj);
    }

    /// operator delete
    static void operator delete ( void* p )
    {
      boost::singleton_pool<VeloCandidate, sizeof(VeloCandidate)>::is_from(p) ?
      boost::singleton_pool<VeloCandidate, sizeof(VeloCandidate)>::free(p) :
      ::operator delete(p);
    }

    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete (p, pObj);
    }
#endif

 class increasingY  {
  public:
   inline bool operator() ( VeloCandidate* first, VeloCandidate*  second ) const{
     return ( 0 == second ? true : 0 == first ? false : first->yAtT() < second->yAtT());
   }
  };

 class compByY_LB: public std::binary_function<const VeloCandidate*, const double, bool>{
    double testVal;
  public:
    inline bool operator() (const VeloCandidate* obj,const double& testVal) const{
      return ((!obj) ? false : testVal > obj->yAtT());
    }
  };

 private:

  LHCb::Track* m_track; 
  LHCb::State m_state;
  double m_yAtT;
  double m_norm2;
};

inline VeloCandidate::VeloCandidate(LHCb::Track* aTrack, 
                                    const LHCb::State& aState,
                                    double yAtT):
m_track(aTrack),
m_state(aState),
m_yAtT(yAtT)
{
  // constructor
  const Gaudi::TrackVector& trackVector = m_state.stateVector();
  m_norm2 = 1.0 + trackVector[2]* trackVector[2]  + trackVector[3]* trackVector[3];

}

inline LHCb::Track* VeloCandidate::track() const{
  return m_track;
}

const LHCb::State& VeloCandidate::state() const{
  return m_state;
}

const Gaudi::TrackVector& VeloCandidate::trackVector() const{
  return m_state.stateVector();
}

const Gaudi::TrackSymMatrix& VeloCandidate::covariance() const{
  return m_state.covariance();
}

inline double VeloCandidate::nLHCbIDs() const{
  return m_track->nLHCbIDs(); 
}

inline double VeloCandidate::norm2() const{
  return m_norm2;
}

inline double VeloCandidate::yAtT() const {
  return m_yAtT;
}

typedef std::vector<VeloCandidate*> VeloCandidates;

#endif // VeloCandidate
