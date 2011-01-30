#include "Event/KalmanFitResult.h"
#include "Event/FitNode.h"
#include <boost/foreach.hpp>

namespace LHCb
{
  // default constructor. do nothing.
  KalmanFitResult::KalmanFitResult() 
    : m_nTrackParameters(5), m_chi2CacheValid(false),m_errorFlag(0x00) {}
  
  // copy constructor
  KalmanFitResult::KalmanFitResult(const KalmanFitResult& rhs) 
    : TrackFitResult(rhs), m_seedCovariance(rhs.m_seedCovariance),
      m_nTrackParameters(rhs.m_nTrackParameters), m_chi2CacheValid(false),m_errorFlag(0x00)
  {
    establishNodeLinks() ;
  }
  
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


  void KalmanFitResult::setErrorFlag(ushort direction, ushort algnum , ushort errnum) 
  {
    m_errorFlag = (((ushort) 1 ) << globalBits)
      +(((ushort)direction) << dirBits ) 
      +(((ushort)algnum) << algBits ) 
      +(((ushort)errnum) << typeBits);  
  }

  bool KalmanFitResult::inError(){
    if ( m_errorFlag == 0 ) return false;
    else return true;
  }

  std::string KalmanFitResult::getError(){
    ushort direction = ( m_errorFlag & dirMask ) >> dirBits ;
    ushort algnum = ( m_errorFlag & algMask ) >> algBits ;
    ushort errnum = ( m_errorFlag & typeMask ) ;
    std::ostringstream errMsg;
    std::ostringstream dir;
    // Set the direction
    switch (direction){
    case Forward:
      dir<<"forward "; break;
    case Backward:
      dir<<"backward "; break;
    default : dir<<""; break;
    }
    // Set the algorithm
    switch (algnum){
    case Predict:
      errMsg<<"Error in predict "<<dir.str()<<"function: ";
      if(errnum == Initialization) errMsg<<"seed covariance is not set!";
      else if(errnum == AlgError) errMsg<<"something goes wrong in the prediction";
      else errMsg<<"unknown error";
      break;
    case Filter:
      errMsg<<"Error in filter "<<dir.str()<<"function: ";
      if(errnum == Initialization) errMsg<<"projection matrix is not set!";
      else if(errnum == AlgError) errMsg<<"something goes wrong in the filtering";
      else errMsg<<"unknown error";
      break;
    case Smooth:
      errMsg<<"Error in smooth function: ";
      if( errnum == MatrixInversion )errMsg<<"error in matrix inversion";
      if( errnum == Other )errMsg<<"problem with HCH.";
      else errMsg<<"unknown error";
      break;
    case ComputeResidual:
      errMsg<<"Error in compute residual: ";
      if( errnum == Other )errMsg<<" non positive variance.";
      else errMsg<<"unknown error";
      break;
    case WeightedAverage:
      errMsg<<"Error in weighted average: ";
      if( errnum == Other )errMsg<<" non positive variance.";
      else errMsg<<"unknown error";
      break;  
    default:
      errMsg<<"Unknown error...";
      break;
    }
    return errMsg.str();
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
    
    // first reset everything
    m_chi2Muon = m_chi2MuonT = m_chi2Velo =  m_chi2VeloTT = m_chi2VeloTTT = m_chi2 = ChiSquare() ;

    if( !nodes().empty() ) {

      // Compute the chisquare integrals for forward and backward
      double chisqMuon[2] = {0,0} ;
      double chisqT[2]    = {0,0} ;
      double chisqTT[2]   = {0,0} ;
      double chisqVelo[2] = {0,0} ;
      int    nhitsMuon(0), nhitsT(0), nhitsTT(0), nhitsVelo(0) ; 
      BOOST_FOREACH( const LHCb::Node* node, nodes() ) {
	if( node->type() == LHCb::Node::HitOnTrack ) {
	  const LHCb::FitNode* fitnode = static_cast<const LHCb::FitNode*>(node) ;
	  
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
	  case Measurement::OT:
	  case Measurement::IT:
	  case Measurement::ITLite:
	    chisqT[0] += fitnode->deltaChi2Forward() ;
	    chisqT[1] += fitnode->deltaChi2Backward() ;
	    ++nhitsT ;
	    break;
	  case Measurement::Muon:
	    chisqMuon[0] += fitnode->deltaChi2Forward() ;
	    chisqMuon[1] += fitnode->deltaChi2Backward() ;
	    ++nhitsMuon ;
	    break;
	  case Measurement::Unknown:
	  case Measurement::Calo:
	  case Measurement::VeloPixLite:
	    break ;
	  }
	}
      }
      
      bool upstream =  nodes().front()->z() > nodes().back()->z() ;
      int nPar = nTrackParameters() ;
      
      if( nhitsMuon > 0 )
	m_chi2Muon  = ChiSquare(upstream ? chisqMuon[0] : chisqMuon[1],nhitsMuon - nPar) ;

      if( nhitsT + nhitsMuon > 0 ) 
	m_chi2MuonT  = ChiSquare(upstream ? (chisqT[0]+ chisqMuon[0]) : (chisqT[1] +chisqMuon[0]),nhitsT + nhitsMuon - nPar) ;

      if( nhitsVelo > 0 ) 
	m_chi2Velo   =  ChiSquare(upstream ? chisqVelo[1] : chisqVelo[0],nhitsVelo - nPar) ;

      if( nhitsVelo + nhitsTT >0 )
	m_chi2VeloTT = ChiSquare(upstream ? chisqVelo[1] + chisqTT[1] : chisqVelo[0] + chisqTT[0],nhitsVelo + nhitsTT - nPar ) ;
      
      if( nhitsVelo + nhitsTT + nhitsT >0 )
	m_chi2VeloTTT = ChiSquare(upstream ? chisqVelo[1] + chisqTT[1] + chisqT[1] : chisqVelo[0] + chisqTT[0] + chisqT[0],
				  nhitsVelo + nhitsTT + nhitsT - nPar )  ;
      
      if( nhitsMuon + nhitsT + nhitsVelo + nhitsTT > 0 )
	m_chi2 = ChiSquare( std::max( chisqMuon[0] + chisqT[0]+chisqTT[0]+chisqVelo[0], chisqMuon[1]+chisqT[1]+chisqTT[1]+chisqVelo[1] ),
			    nhitsMuon+ nhitsT + nhitsVelo + nhitsTT - nPar) ;
    }
    
    m_chi2CacheValid = true ;
  } 

  ChiSquare KalmanFitResult::computeChiSquareForwardFit()
  {
    LHCb::FitNode* lastnode(0) ;
    double chisq(0) ; int ndof(0) ;
    BOOST_FOREACH( LHCb::Node* node, nodes()) {
      if( node->type()==LHCb::Node::HitOnTrack ) {
	LHCb::FitNode* fitnode = static_cast<FitNode*>(node) ;
	chisq   += fitnode->deltaChi2Forward() ;
	lastnode = fitnode ;
	++ndof ;
      }
    }
    // Count the number of active track parameters. For now, just look at the momentum.
    if(lastnode) {
      const double threshold = 0.1 ;
      size_t npar = (lastnode->filteredState(LHCb::FitNode::Forward).covariance()(4,4) 
		     / m_seedCovariance(4,4) < threshold ? 5 : 4) ;
      setNTrackParameters( npar ) ;
      ndof -= npar ;
    }
    return ChiSquare( chisq, ndof ) ;
  }
  
  void KalmanFitResult::establishNodeLinks() 
  {
    LHCb::FitNode* prev(0) ;
    for( NodeContainer::const_iterator it = nodes().begin() ;
	 it != nodes().end(); ++it ) {
      LHCb::FitNode* fitnode = static_cast<FitNode*>(*it) ;
      fitnode->setPreviousNode( prev ) ;
      fitnode->setParent(this) ;
      prev = fitnode ;
    }
  }
    
}
