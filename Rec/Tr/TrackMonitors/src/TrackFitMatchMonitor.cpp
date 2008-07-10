#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/ToolHandle.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "Event/Track.h"
#include "Event/FitNode.h"


class TrackFitMatchMonitor : public GaudiHistoAlg 
{
public:

   /** Standard construtor */
  TrackFitMatchMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  /** Destructor */
  virtual ~TrackFitMatchMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm finalize */
  virtual StatusCode finalize();

  /** Algorithm execute */
  virtual StatusCode execute();

private:
  void filter(const LHCb::FitNode& node, LHCb::State& state) const ;
  void plotDelta(const std::string& name, const Gaudi::TrackVector& delta,
		 const Gaudi::TrackSymMatrix& cov, const Gaudi::TrackVector& vector) ;
  void plotUnbiased(const std::string& name,const LHCb::FitNode& node,bool upstream) ;
  
private:
  std::string m_trackContainerName;
  bool m_constrainQoPOnly ;
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackFitMatchMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackFitMatchMonitor::TrackFitMatchMonitor( const std::string& name,
					ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator )
{
  declareProperty( "TrackContainer", m_trackContainerName = LHCb::TrackLocation::Default  );
  declareProperty( "ConstrainQoPOnly", m_constrainQoPOnly = false ) ;
}

//=============================================================================
// Destructor
//=============================================================================
TrackFitMatchMonitor::~TrackFitMatchMonitor() 
{
}


//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackFitMatchMonitor::initialize() 
{
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;              // error printed already by GaudiAlgorithm
  return sc;
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackFitMatchMonitor::finalize() 
{
  return GaudiHistoAlg::finalize() ;
}

//=========================================================================
// 
//=========================================================================
void TrackFitMatchMonitor::filter(const LHCb::FitNode& node, LHCb::State& state) const
{
  // get reference to the state vector and cov
  Gaudi::TrackVector&    X = state.stateVector();
  Gaudi::TrackSymMatrix& C = state.covariance();

  // calculate the linearized residual of the prediction and its error
  const Gaudi::TrackProjectionMatrix& H = node.projectionMatrix();
  const double errorMeas2 = node.errMeasure2();
  double res        = node.refResidual() + ( H * (node.refVector().parameters() - X) ) (0) ;
  double errorRes2  = errorMeas2 + Similarity(H,C)(0,0) ;  
  
  // calculate gain matrix K
  ROOT::Math::SMatrix<double,5,1> K = (C * Transpose(H)) / errorRes2;
  
  // update the state vector
  X += K.Col(0) * res ;
  Gaudi::SymMatrix1x1 R ;
  R(0,0) = errorRes2;
  Gaudi::TrackSymMatrix tmp ;
  ROOT::Math::AssignSym::Evaluate(tmp, -2 * K * H * C) ;
  C += tmp + Similarity(K,R) ;  
}

static LHCb::State average( const LHCb::State& stateA, 
			    const LHCb::State& stateB)
{
  // Get the predicted state from the reverse fit
  const Gaudi::TrackVector& predRevX = stateA.stateVector();
  const Gaudi::TrackSymMatrix& predRevC = stateA.covariance();
  const Gaudi::TrackVector& filtStateX = stateB.stateVector();
  const Gaudi::TrackSymMatrix& filtStateC = stateB.covariance();

  // Calculate the gain matrix. Start with inverting the cov matrix of the difference.
  Gaudi::TrackSymMatrix R = filtStateC + predRevC ;
  Gaudi::TrackSymMatrix invR = R ;
  bool OK = invR.Invert() ;
  if(!OK) std::cout << "smoothing error" << std::endl ;
  Gaudi::TrackVector    smoothedX ;
  Gaudi::TrackSymMatrix smoothedC ;
  // Now we need to choose wisely which state is the reference. (In
  // a perfect world it would not make a difference.)
  if( filtStateC(0,0) < predRevC(0,0) ) {
    ROOT::Math::SMatrix<double,5,5> K = filtStateC * invR ;
    smoothedX = filtStateX + K * (predRevX - filtStateX) ;
    // This is the fast expression, but it is not stable enough:
    //
    //     ROOT::Math::AssignSym::Evaluate(smoothedC, K * predRevC ) ; 
    //
    // The inversion of R fails if R has sufficiently large condition
    // number, even if we precondition it (as in invertMatrix). The
    // 'fast' expression for the covariance matrix may then lead to a
    // non-pos-def covariance matrix.  The following works better, but
    // is noticably slower because of the similarity:
    ROOT::Math::AssignSym::Evaluate(smoothedC, -2 * K * filtStateC) ;
    smoothedC += filtStateC + ROOT::Math::Similarity(K,R) ;
  } else {
    ROOT::Math::SMatrix<double,5,5> K = predRevC * invR ;
    smoothedX    = predRevX + K * (filtStateX - predRevX) ;
    ROOT::Math::AssignSym::Evaluate(smoothedC, -2 * K * predRevC) ;
    smoothedC += predRevC + ROOT::Math::Similarity(K,R) ;
  }

  LHCb::State rc = stateA ;
  rc.stateVector() = smoothedX ;
  rc.covariance() = smoothedC ;
  return rc ;
}

void TrackFitMatchMonitor::plotDelta(const std::string& name,
				     const Gaudi::TrackVector& delta,
				     const Gaudi::TrackSymMatrix& cov,
				     const Gaudi::TrackVector& vector)
{
  plot(delta(0),std::string("dx ")  + name,-1,1) ;
  plot(delta(1),std::string("dy " ) + name,-1,1) ;
  plot(delta(2),std::string("dtx ") + name, -0.005,0.005) ;
  plot(delta(3),std::string("dty ") + name, -0.005,0.005) ;

  plot(delta(0)/std::sqrt(cov(0,0)),std::string("dx pull ") + name,-10,10) ;
  plot(delta(1)/std::sqrt(cov(1,1)),std::string("dy pull ") + name,-10,10) ;
  plot(delta(2)/std::sqrt(cov(2,2)),std::string("dtx pull ") + name,-10,10) ;
  plot(delta(3)/std::sqrt(cov(3,3)),std::string("dty pull ") + name,-10,10) ;
  plot(delta(4)/std::sqrt(cov(4,4)),std::string("qop pull ") + name,-10,10) ;
  
  if( delta(2)*delta(2)/cov(2,2)<25) 
    profile1D(vector(2),delta(2)/std::sqrt(cov(2,2)),std::string("dtx pull versus tx") + name,-0.3,0.3) ;
  
}

Gaudi::SymMatrix4x4 constrainMom( const Gaudi::SymMatrix5x5& in)
{
  // recompute the covariance matrix when momentum error is constrained to zero
  Gaudi::SymMatrix4x4 out ;
  for( size_t irow = 0; irow<4; ++irow) 
    for( size_t icol = 0; icol<=irow; ++icol) 
      out(irow,icol) = in(irow,icol) - in(irow,4) * in(4,icol) / in(4,4) ;
  return out ;
}

// void TrackFitMatchMonitor::plotUnbiased(const std::string& name,
// 					const LHCb::FitNode& node,
// 					bool upstream)
// {
//   // I want two filter states, namely at lastVelo and firstTT
//   LHCb::State filteredstate = upstream ? node.predictedStateUp() : node.predictedStateDown() ;
//   filter(node,filteredstate) ; 
  
//   // we only unbiased (x,...ty) subspace, so take all the 4x4 sub parts
//   Gaudi::Vector4      xA = filteredstate.stateVector().Sub<Gaudi::Vector4>(1) ;
//   //Gaudi::SymMatrix4x4 CA = filteredstate.covariance().Sub<Gaudi::SymMatrix4x4>(1,1) ;
//   Gaudi::SymMatrix4x4 CA = constrainMom( filteredstate.covariance() ) ;
//   Gaudi::Vector4      x  = node.state().stateVector().Sub<Gaudi::Vector4>(1) ;
//   //Gaudi::SymMatrix4x4 C  = node.state().covariance().Sub<Gaudi::SymMatrix4x4>(1,1) ;
//   Gaudi::SymMatrix4x4 C = constrainMom( node.state().covariance() ) ;
//   Gaudi::SymMatrix4x4 Cinv = C ;
//   bool OK = Cinv.Invert() ;
//   if(OK) {
//     Gaudi::Vector4 deltabiased   = x - xA ;
//     Gaudi::Vector4 delta         = CA * Cinv * deltabiased ;
//     plot(delta(0),std::string("unbiased dx ")  + name,-1,1) ;
//     plot(delta(1),std::string("unbiased dy " ) + name,-1,1) ;
//     plot(delta(2),std::string("unbiased dtx ") + name, -0.005,0.005) ;
//     plot(delta(3),std::string("unbiased dty ") + name, -0.005,0.005) ;
//   }
// }

void TrackFitMatchMonitor::plotUnbiased(const std::string& name,
					const LHCb::FitNode& node,
					bool upstream)
{
  // I want two filter states, namely at lastVelo and firstTT
  LHCb::State stateUp   = node.predictedStateUp() ;
  LHCb::State stateDown = node.predictedStateDown() ;
  filter(node, upstream ? stateUp : stateDown ) ;

  Gaudi::TrackVector delta  = stateUp.stateVector() - stateDown.stateVector() ;
  Gaudi::TrackSymMatrix cov = stateUp.covariance() + stateDown.covariance() ;

  // two options:
  // - constrain all other variables
  // - constrain only the momentum

  Gaudi::TrackVector deltac,deltacpull ;
 
  if(!m_constrainQoPOnly) {
    // now remove the contribution from the difference in the 'other' variables
    for( size_t irow = 0; irow<5; ++irow) {
      // remove this row from delta and cov
      Gaudi::Vector4 subdelta, subcor ;
      Gaudi::SymMatrix4x4 subcov   ;
      size_t krow(0) ;
      for(size_t jrow = 0; jrow<5; ++jrow) 
	if(jrow != irow) {
	  subdelta(krow) = delta(jrow) ;
	  subcor(krow)   = cov(irow,jrow) ;
	  size_t kcol(0) ;
	  for(size_t jcol = 0; jcol<=jrow; ++jcol) 
	    if(jcol != irow) {
	      subcov(krow,kcol) = cov(jrow,jcol) ;
	      ++kcol ;
	    }
	  ++krow ;
	}
      
      // now invert the subcov
      subcov.Invert() ;
      // compute delta and its covariance
      Gaudi::Vector4 tmp = subcov * subcor ;
      deltac(irow) = delta(irow)  - ROOT::Math::Dot(subdelta,tmp) ;
      double covc = cov(irow,irow) - ROOT::Math::Dot(subcor,tmp) ;
      deltacpull(irow) = deltac(irow) / std::sqrt(covc) ;
    }
  } else {
    for( size_t irow = 0; irow<4; ++irow) {
      deltac(irow) = delta(irow)  - cov(irow,4)/cov(4,4) * delta(4) ;
      double covc  = cov(irow,irow) - cov(irow,4)/cov(4,4) * cov(4,irow) ;
      deltacpull(irow) = deltac(irow) / std::sqrt(covc) ;
    }
  }
  
  plot(deltac(0),std::string("dx ")  + name,-20,20) ;
  plot(deltac(1),std::string("dy " ) + name,-20,20) ;
  plot(deltac(2),std::string("dtx ") + name, -0.005,0.005) ;
  plot(deltac(3),std::string("dty ") + name, -0.005,0.005) ;
  plot(deltac(4),std::string("dqop ") + name, -1e-4,1e-4) ;
  
  plot(deltacpull(0),std::string("dx pull ")  + name,-10,10) ;
  plot(deltacpull(1),std::string("dy pull " ) + name,-10,10) ;
  plot(deltacpull(2),std::string("dtx pull ") + name, -10,10) ;
  plot(deltacpull(3),std::string("dty pull ") + name, -10,10) ;
  plot(deltacpull(4),std::string("dqop pull ") + name, -10,10) ;

  if( fabs(deltacpull(0))<5 ) {
    profile1D(node.state().tx(),deltacpull(0), std::string("dx pull vs tx ")  + name, -0.25,0.25,20) ;
    profile1D(node.state().ty(),deltacpull(0), std::string("dx pull vs ty ")  + name, -0.25,0.25,20) ;
    profile1D(node.state().qOverP(),deltacpull(0), std::string("dx pull vs qop ")  + name, -4e-4,4e-4,20) ;
  }
  if( fabs(deltacpull(1))<5 ) {
    profile1D(node.state().tx(),deltacpull(1), std::string("dy pull vs tx ")  + name, -0.25,0.25,20) ;
    profile1D(node.state().ty(),deltacpull(1), std::string("dy pull vs ty ")  + name, -0.25,0.25,20) ;
    profile1D(node.state().qOverP(),deltacpull(1), std::string("dy pull vs qop ")  + name, -4e-4,4e-4,20) ;
  }
  if( fabs(deltacpull(2))<5 ) {
    profile1D(node.state().tx(),deltacpull(2), std::string("dtx pull vs tx ")  + name, -0.25,0.25,20) ;
    profile1D(node.state().ty(),deltacpull(2), std::string("dtx pull vs ty ")  + name, -0.25,0.25,20) ;
    profile1D(node.state().qOverP(),deltacpull(2), std::string("dtx pull vs qop ")  + name, -4e-4,4e-4,20) ;
  }

  if( node.state().qOverP() > 0 )
    plot(deltacpull(0),std::string("dx pull pos ") + name, -10,10) ;
  else
    plot(deltacpull(0),std::string("dx pull neg ") + name, -10,10) ;
}


StatusCode TrackFitMatchMonitor::execute()
{ 
  // take the set of long tracks, split them at the right places and see how the two pieces match

  // first we need to loop over the notes and determine the break
  // points. we break it between VELO and TT and between TT and T.

  // For VELO_TT we compare parameters at the first TT hit.

  const LHCb::Tracks* tracks = get<LHCb::Tracks>( m_trackContainerName ) ;

  for( LHCb::Tracks::const_iterator itr = tracks->begin() ;
       itr != tracks->end(); ++itr) {
    plot((**itr).chi2PerDoF(),"chi2 per dof",0,10) ;
    plot((**itr).info(LHCb::Track::FitMatchChi2,-1),"match chi2",0,50) ;

    const LHCb::FitNode *lastVelo(0),*firstTT(0),*lastTT(0),*firstT(0) ;
    const LHCb::FitNode *fitnode(0) ;
    for( LHCb::Track::NodeContainer::const_iterator inode = (*itr)->nodes().begin() ;
	 inode != (*itr)->nodes().end(); ++inode) 
      if( (*inode)->hasMeasurement() &&
	  (fitnode = dynamic_cast<const LHCb::FitNode*>(*inode) ) ) {
	switch(fitnode->measurement().type()) {
	case LHCb::Measurement::VeloR:
	case LHCb::Measurement::VeloPhi:
	  if( !lastVelo || (lastVelo->z() < fitnode->z()) ) lastVelo = fitnode ;
	  break ;
	case LHCb::Measurement::TT:
	  if( !firstTT || (firstTT->z() > fitnode->z()) ) firstTT=fitnode ;
	  if( !lastTT || (lastTT->z() < fitnode->z()) ) lastTT=fitnode ;
	  break;
	case LHCb::Measurement::OT:
	case LHCb::Measurement::IT:
	  if( !firstT || (firstT->z() > fitnode->z() ) ) firstT=fitnode ;
	  break;
	default:
	  break ;
	}
      }
    
    // take only long tracks
    if( firstT && lastVelo) {

      // now split this between tracks with and tracks without TT
      if( lastTT ) {

	// you can take either of these two: they give identical results.
	plotUnbiased("Velo-TT",*lastVelo,false) ;
	//plotUnbiased("Velo-TT at TT",*firstTT,true) ;

	plotUnbiased("T-TT",*lastTT,false) ;
	//plotUnbiased("T-TT at T",*firstT,true) ;
	
      } else {
	plotUnbiased("Velo-T",*lastVelo,false) ;
	//plotUnbiased("Velo-T at T",*firstT,true) ;
      }
    }



//     // this is the code for studying the match at one node
//     if( firstTT && lastVelo) {
//       // I want two filter states, namely at lastVelo and firstTT
//       LHCb::State veloState = lastVelo->predictedStateDown() ;
//       filter(*lastVelo,veloState) ;
//       LHCb::State ttState   = firstTT->predictedStateUp() ;
//       filter(*firstTT,ttState) ;
//       // Now the trick: look at the difference with the smoothed
//       // stated. That basically means we ignore the transport matrix.
//       Gaudi::TrackVector delta = 
// 	( veloState.stateVector() - lastVelo->state().stateVector()) -
// 	( ttState.stateVector()   - firstTT->state().stateVector()) ;
//       Gaudi::TrackSymMatrix cov = 
// 	veloState.covariance() + ttState.covariance() ;

//       plot(delta(0),"dx Velo-TT",-0.2,0.2) ;
//       plot(delta(1),"dy Velo-TT",-2,2) ;
//       plot(delta(2),"dtx Velo-TT",-0.005,0.005) ;
//       plot(delta(3),"dty Velo-TT",-0.005,0.005) ;
//       plot(delta(0)/std::sqrt(cov(0,0)),"dx pull Velo-TT",-10,10) ;
//       plot(delta(1)/std::sqrt(cov(1,1)),"dy pull Velo-TT",-10,10) ;
//       plot(delta(2)/std::sqrt(cov(2,2)),"dtx pull Velo-TT",-10,10) ;
//       plot(delta(3)/std::sqrt(cov(3,3)),"dty pull Velo-TT",-10,10) ;
//       plot(delta(4)/std::sqrt(cov(4,4)),"qop pull Velo-TT",-10,10) ;

//       // now we just take the difference between the filtered state
//       // and the prediction. but where? at the end of the velo?
      
//       {
// 	Gaudi::TrackVector delta  = veloState.stateVector() - lastVelo->state().stateVector() ;
// 	Gaudi::TrackSymMatrix cov = veloState.covariance() - lastVelo->state().covariance() ;
// 	plot(delta(0),"dx Velo-TT B",-0.05,0.05) ;
// 	plot(delta(1),"dy Velo-TT B",-0.05,0.05) ;
// 	plot(delta(2),"dtx Velo-TT B",-0.001,0.001) ;
// 	plot(delta(3),"dty Velo-TT B",-0.001,0.001) ;
// 	plot(delta(0)/std::sqrt(cov(0,0)),"dx pull Velo-TT B",-10,10) ;
// 	plot(delta(1)/std::sqrt(cov(1,1)),"dy pull Velo-TT B",-10,10) ;
// 	plot(delta(2)/std::sqrt(cov(2,2)),"dtx pull Velo-TT B",-10,10) ;
// 	plot(delta(3)/std::sqrt(cov(3,3)),"dty pull Velo-TT B",-10,10) ;
// 	plot(delta(4)/std::sqrt(cov(4,4)),"qop pull Velo-TT B",-10,10) ;
//       }

//       // we can do the same thing at the TT side
//       {
// 	Gaudi::TrackVector delta  = ttState.stateVector() - firstTT->state().stateVector() ;
// 	Gaudi::TrackSymMatrix cov = ttState.covariance() - firstTT->state().covariance() ;
// 	plotDelta("Velo-TT C",delta,cov,firstTT->state().stateVector()) ;
//       }

//     }  
    
//     // this is the code for studying the match at one node
//     if( firstT && lastTT) {
//       // I want two filter states, namely at lastTT and firstT
//       LHCb::State ttState = lastTT->predictedStateDown() ;
//       filter(*lastTT,ttState) ;
//       LHCb::State tState   = firstT->predictedStateUp() ;
//       filter(*firstT,tState) ;
//       {
// 	Gaudi::TrackVector delta  = ttState.stateVector() - lastTT->state().stateVector() ;
// 	Gaudi::TrackSymMatrix cov = ttState.covariance() - lastTT->state().covariance() ;
// 	plotDelta("T-TT B",delta,cov,lastTT->state().stateVector()) ;
//       }

//       // we can do the same thing at the TT side
//       {
// 	Gaudi::TrackVector delta  = tState.stateVector() - firstT->state().stateVector() ;
// 	Gaudi::TrackSymMatrix cov = tState.covariance() - firstT->state().covariance() ;
// 	plotDelta("T-TT C",delta,cov,firstT->state().stateVector()) ;
//       }
      

  }
  return StatusCode::SUCCESS ;
}
