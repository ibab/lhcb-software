#include "Event/KalmanFitResult.h"
#include "Event/FitNode.h"
#include <boost/foreach.hpp>

namespace LHCb
{
  // default constructor. do nothing.
  KalmanFitResult::KalmanFitResult() 
    : m_nTrackParameters(5) {}
  
  // copy constructor
  KalmanFitResult::KalmanFitResult(const KalmanFitResult& rhs) 
    : TrackFitResult(rhs), m_seedCovariance(rhs.m_seedCovariance),
      m_nTrackParameters(rhs.m_nTrackParameters) {}
  
  // copy from TrackFitResult
  KalmanFitResult::KalmanFitResult(const TrackFitResult& rhs) 
    : TrackFitResult(rhs) {}
		    
  // destructor
  KalmanFitResult::~KalmanFitResult() {}

  // clone
  TrackFitResult* KalmanFitResult::clone() const
  {
    return new KalmanFitResult(*this) ;
  }


  void KalmanFitResult::computeChiSquares() const 
  {
    // This routine calculates the chisquare contributions from
    // different segments of the track. It uses the 'delta-chisquare'
    // contributions from the bi-directional kalman fit. Summing these
    // leads to a real chisquare only if the contributions are
    // uncorrelated. For a Velo-TT-T track you can then calculate:
    //
    // - the chisuare of the T segment and the T-TT segment by using the
    // 'upstream' contributions 
    //
    // - the chisquare of the Velo segment and the Velo-TT segment by
    // using the 'downstream' contributions
    //
    // Note that you cannot calculate the contribution of the TT segment
    // seperately (unless there are no T or no Velo hits). Also, if
    // there are Muon hits, you cannot calculate the T station part, so
    // for now this only works for tracks without muon hits.
    
  
    // Compute the chisquare integrals for forward and backward
    double chisqMuonT[2]    = {0,0} ;
    double chisqTT[2]   = {0,0} ;
    double chisqVelo[2] = {0,0} ;
    int    nhitsMuonT(0), nhitsTT(0), nhitsVelo(0) ; 
    BOOST_FOREACH( const LHCb::Node* node, nodes() ) {
      if( node->type() == LHCb::Node::HitOnTrack ) {
	const LHCb::FitNode* fitnode = dynamic_cast<const LHCb::FitNode*>(node) ;
	switch( node->measurement().type() ) {
	case Measurement::VeloR:
	case Measurement::VeloPhi:
	case Measurement::VeloLiteR:
	case Measurement::VeloLitePhi:
	  chisqVelo[0] += fitnode->deltaChi2Forward() ;
	  chisqVelo[1] += fitnode->deltaChi2Backward() ;
	  ++nhitsVelo ;
	  break;
	case Measurement::TT:
	case Measurement::TTLite:
	  chisqTT[0] += fitnode->deltaChi2Forward() ;
	  chisqTT[1] += fitnode->deltaChi2Backward() ;
	  ++nhitsTT ;
	  break;
	case Measurement::Muon:
	case Measurement::OT:
	case Measurement::IT:
	case Measurement::ITLite:
	  chisqMuonT[0] += fitnode->deltaChi2Forward() ;
	  chisqMuonT[1] += fitnode->deltaChi2Backward() ;
	  ++nhitsMuonT ;
	  break;
	case Measurement::Unknown:
	  break ;
	}
      }
    }
    
    bool upstream = nodes().front()->z() > nodes().back()->z() ;
    int nPar = nTrackParameters() ;
    
    m_chi2MuonT  = nhitsMuonT > 0 ?
      ChiSquare(upstream ? chisqMuonT[0] : chisqMuonT[1],nhitsMuonT - nPar) : ChiSquare() ;
    m_chi2Velo   = nhitsVelo > 0 ?
      ChiSquare(upstream ? chisqVelo[1] : chisqVelo[0],nhitsVelo - nPar) : ChiSquare() ;
    m_chi2VeloTT = nhitsVelo + nhitsTT >0 ?
      ChiSquare(upstream ? chisqVelo[1] + chisqTT[1] : chisqVelo[0] + chisqTT[0],
		nhitsVelo + nhitsTT - nPar ) : ChiSquare() ;
    m_chi2 = ChiSquare( std::max( chisqMuonT[0]+chisqTT[0]+chisqVelo[0], chisqMuonT[1]+chisqTT[1]+chisqVelo[1] ),
			nhitsMuonT + nhitsVelo + nhitsTT - nPar) ;
  }
}
