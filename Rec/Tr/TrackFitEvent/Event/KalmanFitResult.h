#ifndef TRACKFITEVENT_KALMANFITRESULT_H
#define TRACKFITEVENT_KALMANFITRESULT_H

#include "Event/TrackFitResult.h"
#include "Event/TrackTypes.h"
#include "Event/ChiSquare.h"

namespace LHCb 
{
  class KalmanFitResult : public TrackFitResult
  {
  public:
    enum algoType{ Predict,
		   Filter,
		   Smooth,
		   ComputeResidual,
		   WeightedAverage
    };
    enum errorType{ Initialization,
		    MatrixInversion,
		    AlgError,
		    Other
    };
    enum directionType{Forward ,
		       Backward,
		       BiDirection
    };
    
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

    // set links between nodes and to parent
    void establishNodeLinks() ;
    
    // get the seed covariance
    const Gaudi::TrackSymMatrix& seedCovariance() const {
      return m_seedCovariance ;
    }
    
    // set the seed covariance
    void setSeedCovariance(const Gaudi::TrackSymMatrix& rhs) {
      m_seedCovariance = rhs ;
    }

    // return the number of track parameters
    int nTrackParameters() const { return m_nTrackParameters ; }
    
    // set the number of track parameters and reset the cache
    void setNTrackParameters(int n) { 
      m_nTrackParameters = n ; 
      m_chi2CacheValid = false ; }
    
    // return (chisq,dof) for the velo part of this track
    const ChiSquare& chi2() const {
      if( !m_chi2CacheValid ) computeChiSquares() ;
      return m_chi2 ;
    }
    
    // return (chisq,dof) for the velo part of this track
    const ChiSquare& chi2Velo() const {
      if( !m_chi2CacheValid ) computeChiSquares() ;
      return m_chi2Velo ;
    }
    
    // return (chisq,dof) for the segment downstream of the magnet (T + Muon)
    const ChiSquare& chi2Downstream() const {
      if( !m_chi2CacheValid ) computeChiSquares() ;
      return m_chi2MuonT ;
    }
    
    // return (chisq,dof) for the segment downstream of the magnet
    const ChiSquare& chi2Upstream() const {
      if( !m_chi2CacheValid ) computeChiSquares() ;
      return m_chi2VeloTT ;
    }
    
    // return (chisq,dof) for upstream versus downstream segment
    ChiSquare chi2Match() const {
      if( !m_chi2CacheValid ) computeChiSquares() ;
      return m_chi2 - m_chi2VeloTT - m_chi2MuonT ;
    }
    
    // return (chisq,dof) for the velo-TT-T segment, so everything excluding muon
    const ChiSquare& chi2Long() const {
      if( !m_chi2CacheValid ) computeChiSquares() ;
      return m_chi2VeloTTT ;
    }
    
    // return (chisq,dof) for the muon segment
    const ChiSquare& chi2Muon() const {
      if( !m_chi2CacheValid ) computeChiSquares() ;
      return m_chi2Muon ;
    }
    
    // return (chisq,dof) for the muon - T match
    ChiSquare chi2MuonTMatch() const {
      if( !m_chi2CacheValid ) computeChiSquares() ;
      return m_chi2 - m_chi2VeloTTT - m_chi2Muon ;
    }

    // return (chisq,dof) for the forward direction fit
    ChiSquare computeChiSquareForwardFit() ;

    // set the error flag
    void setErrorFlag(ushort extrainfo ,ushort algnum , ushort errnum);
    
    // check if there is an error
    bool inError( );

    // check the type of error
    std::string getError( );

    // check if the fit is bidirectionnal or not
    bool biDirectionnal( ){return m_biDirectionnal;};

    // set the type of fit: bidirectionnal or not?
    void setBiDirectionnal( bool bidir ){ m_biDirectionnal=bidir;};

  private:
    void computeChiSquares() const ;

  private: 
     enum errorBits{ typeBits = 0,   // error type bit position
                     algBits  = 2,   // function bit position
                     dirBits  = 5,   // direction bit position
                     globalBits  = 7   // global error bit position
     };
     enum errorMasks{ typeMask = 0x03,    // error type mask
		      algMask  = 0x1C,   // function mask  
		      dirMask  = 0x60,   // direction mask  
		      globalMask  = 0x80   // direction mask          
     };


  private:
    Gaudi::TrackSymMatrix m_seedCovariance ;
    int m_nTrackParameters ;
    mutable ChiSquare m_chi2 ;
    mutable ChiSquare m_chi2Velo ;
    mutable ChiSquare m_chi2VeloTT ;
    mutable ChiSquare m_chi2VeloTTT ;
    mutable ChiSquare m_chi2MuonT ;
    mutable ChiSquare m_chi2Muon ;
    mutable bool m_chi2CacheValid ;
    bool m_biDirectionnal ;
    ushort m_errorFlag;
  } ;  
}

#endif

