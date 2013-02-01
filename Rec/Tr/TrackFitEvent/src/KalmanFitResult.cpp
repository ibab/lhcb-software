#include "Event/KalmanFitResult.h"
#include "Event/FitNode.h"
#include "Event/OTMeasurement.h"
#include <boost/foreach.hpp>

namespace LHCb
{
  // default constructor. do nothing.
  KalmanFitResult::KalmanFitResult() 
    : m_nTrackParameters(5), m_chi2CacheValid(false),m_errorFlag(0x00),m_bidirectionalSmoother(true) {}
  
  // copy constructor
  KalmanFitResult::KalmanFitResult(const KalmanFitResult& rhs) 
    : TrackFitResult(rhs), m_seedCovariance(rhs.m_seedCovariance),m_nTrackParameters(rhs.m_nTrackParameters)
    , m_chi2Velo(rhs.m_chi2Velo),m_chi2VeloTT(rhs.m_chi2VeloTT)
    , m_chi2VeloTTT(rhs.m_chi2VeloTTT),m_chi2MuonT(rhs.m_chi2MuonT),m_chi2Muon(rhs.m_chi2Muon)
    , m_chi2CacheValid(rhs.m_chi2CacheValid),m_errorFlag(rhs.m_errorFlag)
    ,m_bidirectionalSmoother(rhs.m_bidirectionalSmoother)
  {
    establishNodeLinks() ;
  }
  
  // copy from TrackFitResult
  KalmanFitResult::KalmanFitResult(const TrackFitResult& rhs) 
    : TrackFitResult(rhs),m_nTrackParameters(5), m_chi2CacheValid(false)
    ,m_errorFlag(0x00),m_bidirectionalSmoother(true) {}
		    
  // destructor
  KalmanFitResult::~KalmanFitResult() {}

  // clone
  TrackFitResult* KalmanFitResult::clone() const
  {
    return new KalmanFitResult(*this) ;
  }

  // set the error flag out of direction, algorithm and error type identifiers 
  void KalmanFitResult::setErrorFlag(unsigned short direction, unsigned short algnum , unsigned short errnum) 
  {
    m_errorFlag = (((unsigned short) 1 ) << globalBits)
      +(((unsigned short)direction) << dirBits ) 
      +(((unsigned short)algnum) << algBits ) 
      +(((unsigned short)errnum) << typeBits);  
  }

  // check the global error status of the node
  bool KalmanFitResult::inError() const {
    return m_errorFlag != 0 ;
  }

  // get the error description
  std::string KalmanFitResult::getError() const {
    if( m_errorFlag==0 ) return std::string("No error") ;
    unsigned short direction = ( m_errorFlag & dirMask ) >> dirBits ;
    unsigned short algnum = ( m_errorFlag & algMask ) >> algBits ;
    unsigned short errnum = ( m_errorFlag & typeMask ) ;
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
      else if( errnum == AlgError ) errMsg<<"non positive diagonal element in coveriance matrix";
      else if( errnum == Other    )errMsg<<"problem with HCH.";
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

  LHCb::ChiSquare KalmanFitResult::chi2() const
  {
    LHCb::ChiSquare rc ;
    if( !nodes().empty() ) {
      const LHCb::ChiSquare& chi2A = static_cast<const LHCb::FitNode*>(nodes().front())->totalChi2(LHCb::FitNode::Backward) ;
      const LHCb::ChiSquare& chi2B = static_cast<const LHCb::FitNode*>(nodes().back())->totalChi2(LHCb::FitNode::Forward) ;
      rc = chi2A.chi2() > chi2B.chi2() ? chi2A : chi2B ;
    }
    return rc ;
  }

  // return (chisq,dof) out of the differnet contribution
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
    m_chi2Muon = m_chi2MuonT = m_chi2Velo =  m_chi2VeloTT = m_chi2VeloTTT = ChiSquare() ;
    
    if( !nodes().empty() ) {
      ConstFitNodeRange fitnodes = fitNodes() ;
      ConstFitNodeRange::const_iterator firstVelo = fitnodes.end() ;
      ConstFitNodeRange::const_iterator firstTT = fitnodes.end() ;
      ConstFitNodeRange::const_iterator firstT = fitnodes.end() ;
      ConstFitNodeRange::const_iterator firstMuon = fitnodes.end() ;
      ConstFitNodeRange::const_iterator lastVelo = fitnodes.end() ;
      ConstFitNodeRange::const_iterator lastTT = fitnodes.end() ;
      ConstFitNodeRange::const_iterator lastT = fitnodes.end() ;
      ConstFitNodeRange::const_iterator lastMuon = fitnodes.end() ;
      for( ConstFitNodeRange::const_iterator it = fitnodes.begin() ;
	   it != fitnodes.end(); ++it ) {
	if( (*it)->type() == LHCb::Node::HitOnTrack ) {
	  switch( (*it)->measurement().type() ) {
	  case Measurement::VPLite:
    case Measurement::VLR:
    case Measurement::VLPhi:
    case Measurement::VLLiteR:
    case Measurement::VLLitePhi:
	  case Measurement::VeloR:
	  case Measurement::VeloPhi:
	  case Measurement::VeloLiteR:
	  case Measurement::VeloLitePhi:
	  case Measurement::Origin:
	    if( firstVelo==fitnodes.end()) firstVelo = it ;
	    lastVelo = it ;
	    break;
	  case Measurement::TT:
	  case Measurement::TTLite:
	  case Measurement::UT:
	  case Measurement::UTLite:
	    if( firstTT==fitnodes.end()) firstTT = it ;
	    lastTT = it ;
	    break;
	  case Measurement::OT:
	  case Measurement::IT:
	  case Measurement::ITLite:
    case Measurement::FT:
	    if( firstT==fitnodes.end()) firstT = it ;
	    lastT = it ;
	    break;
	  case Measurement::Muon:
	    if( firstMuon==fitnodes.end()) firstMuon = it ;
	    lastMuon = it ;
	    break;
	  case Measurement::Unknown:
	  case Measurement::Calo:
	    break ;
	  }
	}
      }
      
      bool upstream =  nodes().front()->z() > nodes().back()->z() ;
      if( firstMuon != fitnodes.end() )
	m_chi2Muon = upstream ? (**lastMuon).totalChi2(FitNode::Forward) : (**firstMuon).totalChi2(FitNode::Backward) ;
      
      if( firstT != fitnodes.end() ) 
	m_chi2MuonT = upstream ? (**lastT).totalChi2(FitNode::Forward) : (**firstT).totalChi2(FitNode::Backward) ;
      else
	m_chi2MuonT = m_chi2Muon ;

      if( firstVelo != fitnodes.end())
	m_chi2Velo = upstream ? (**firstVelo).totalChi2(FitNode::Backward) : (**lastVelo).totalChi2(FitNode::Forward) ;
      
      if( firstTT != fitnodes.end() ) 
	m_chi2VeloTT = upstream ? (**firstTT).totalChi2(FitNode::Backward) : (**lastTT).totalChi2(FitNode::Forward) ;
      else
	m_chi2VeloTT = m_chi2Velo ;
      
      if( firstT != fitnodes.end() ) 
	m_chi2VeloTTT = upstream ? (**firstT).totalChi2(FitNode::Backward) : (**lastT).totalChi2(FitNode::Forward) ;
      else
	m_chi2VeloTTT = m_chi2VeloTT ;
    }
    m_chi2CacheValid = true ;
  } 
   
  /// setup the link to previous/next fitnode
  void KalmanFitResult::establishNodeLinks() 
  {
    LHCb::FitNode* prev(0) ;
    for( NodeContainer::const_iterator it = nodes().begin() ;
	 it != nodes().end(); ++it ) {
      LHCb::FitNode* fitnode = static_cast<LHCb::FitNode*>(*it) ;
      fitnode->setPreviousNode( prev ) ;
      fitnode->setParent(this) ;
      prev = fitnode ;
    }
  }

  KalmanFitResult::ConstFitNodeRange KalmanFitResult::fitNodes() const
  {
    typedef ConstFitNodeRange::const_iterator iterator ;
    //const NodeContainer& n=nodes() ;
    static NodeContainer::const_iterator _begin, _end ;
    static iterator *begin, *end ;
    _begin = nodes().begin() ;
    _end = nodes().end() ;
    begin = reinterpret_cast<iterator*>(&_begin);
    end   = reinterpret_cast<iterator*>(&_end);
    return ConstFitNodeRange(*begin,*end) ;
  }
  
  KalmanFitResult::FitNodeRange KalmanFitResult::fitNodes() 
  {
    typedef FitNodeRange::iterator iterator ;
    static NodeContainer::iterator _begin, _end ;
    static iterator *begin, *end ;
    _begin = nodes().begin() ;
    _end = nodes().end() ;
    begin = reinterpret_cast<iterator*>(&_begin);
    end   = reinterpret_cast<iterator*>(&_end);
    return FitNodeRange(*begin,*end) ;
  }

  unsigned int KalmanFitResult::nActiveOTTimes() const 
  {
    unsigned int rc(0) ;
    for( NodeContainer::const_iterator inode = nodes().begin() ; 
	 inode != nodes().end() ; ++inode) 
      if( (*inode)->type() == LHCb::Node::HitOnTrack &&
	  (*inode)->measurement().type() == LHCb::Measurement::OT ) {
	const LHCb::OTMeasurement* otmeas = 
	  static_cast< const LHCb::OTMeasurement* > (&(*inode)->measurement()) ;
	if( otmeas->driftTimeStrategy() == LHCb::OTMeasurement::FitDistance ||
	    otmeas->driftTimeStrategy() == LHCb::OTMeasurement::FitTime )
	  ++rc ;
      }
    return rc ;
  }

  LHCb::ChiSquare KalmanFitResult::chi2VeloTMatch() const
  {
    LHCb::ChiSquare velo = chi2Velo() ;
    LHCb::ChiSquare T    = chi2Downstream() ;
    LHCb::ChiSquare tot  = chi2() - chi2TTHits() ;
    return tot - T - velo ;
  }
  
  LHCb::ChiSquare KalmanFitResult::chi2TTHits() const
  {
    LHCb::ChiSquare rc ;
    ConstFitNodeRange fitnodes = fitNodes() ;
    const LHCb::FitNode *lastnode(0), *firstnode(0) ;
    for( ConstFitNodeRange::const_iterator inode = fitnodes.begin() ; 
	 inode != fitnodes.end() ; ++inode)
      if( (*inode)->type()==LHCb::Node::HitOnTrack &&
	  (*inode)->measurement().type()==LHCb::Measurement::TT ) {
	lastnode = (*inode) ;
	if(firstnode==0) firstnode = (*inode) ;
      }
    if( firstnode ) {
      LHCb::ChiSquare chi2WithOutTTHits ;
      // first add the contributions untill the TT hits
      if( lastnode->hasInfoUpstream( LHCb::FitNode::Backward) )
	chi2WithOutTTHits += lastnode->prevNode(LHCb::FitNode::Backward)->totalChi2(LHCb::FitNode::Backward) ;
      if( firstnode->hasInfoUpstream( LHCb::FitNode::Forward) )
	chi2WithOutTTHits += firstnode->prevNode(LHCb::FitNode::Forward)->totalChi2(LHCb::FitNode::Forward) ;
      
      // now compute the contribution of the velo-T match, if there is any
      if( lastnode->hasInfoUpstream( LHCb::FitNode::Backward) &&
	  firstnode->hasInfoUpstream( LHCb::FitNode::Forward) ) {
	// that means we need to take off the hits.
	
	// for now, let's do it the lazy way: make a clone, then flag
	// all hits as outliers. the cloning is real slow .
	KalmanFitResult* copy = static_cast<KalmanFitResult*>(this->clone()) ;
	
	// set all TT hits as ourliers
	FitNodeRange fitnodes = copy->fitNodes() ;
	LHCb::FitNode *copylastnode(0), *copyfirstnode(0) ;
	for( FitNodeRange::iterator inode = fitnodes.begin() ; 
	     inode != fitnodes.end() ; ++inode)
	  if( (*inode)->type()==LHCb::Node::HitOnTrack &&
	      (*inode)->measurement().type()==LHCb::Measurement::TT ) {
	    (*inode)->deactivateMeasurement() ;
	    copylastnode = (*inode) ;
	    if(copyfirstnode==0) copyfirstnode = (*inode) ;
	  }
	LHCb::State stateA = copylastnode->predictedState(LHCb::FitNode::Forward) ;
	LHCb::State stateB = copylastnode->predictedState(LHCb::FitNode::Backward) ;
	Gaudi::SymMatrix5x5 weight = stateA.covariance()+stateB.covariance() ;
	weight.InvertChol() ;
	Gaudi::Vector5 res = stateA.stateVector() - stateB.stateVector() ;
	chi2WithOutTTHits += LHCb::ChiSquare( ROOT::Math::Similarity(res,weight), 5 ) ;
	delete copy ;
      }
      rc = chi2() - chi2WithOutTTHits ;
    }
    return rc ;
  }
  
}
