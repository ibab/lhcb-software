#ifndef TRACKFITEVENT_KALMANFITRESULT_H
#define TRACKFITEVENT_KALMANFITRESULT_H

#include "Event/TrackFitResult.h"
#include "Event/TrackTypes.h"

namespace LHCb 
{
  class KalmanFitResult : public TrackFitResult
  {
  public:
    
    // default constructor. do nothing.
    KalmanFitResult() ;
    
    // copy constructor
    KalmanFitResult(const KalmanFitResult& rhs) ;
		    
    // copy from TrackFitResult
    KalmanFitResult(const TrackFitResult& rhs) ;
		    
    // destructor
    virtual ~KalmanFitResult() ;

    // clone
    virtual TrackFitResult* clone() const ;

    // get the seed covariance
    const Gaudi::TrackSymMatrix& seedCovariance() const {
      return m_seedCovariance ;
    }
    
    // set the seed covariance
    void setSeedCovariance(const Gaudi::TrackSymMatrix& rhs) {
      m_seedCovariance = rhs ;
    }

  private:
    Gaudi::TrackSymMatrix m_seedCovariance ;
  } ;

}

#endif





