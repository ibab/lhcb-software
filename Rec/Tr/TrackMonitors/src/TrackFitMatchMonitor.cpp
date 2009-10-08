#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/ToolHandle.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "Event/Track.h"
#include "Event/FitNode.h"


class TrackFitMatchMonitor : public GaudiHistoAlg 
{
public:
  enum ConstrainMethod { All=0, QOverP=1, Projective=2 } ;

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
  void plotDelta(const std::string& name,const LHCb::FitNode& node,bool upstream) ;
  inline void myPlot1D(double x, const std::string& path,const std::string& title, double xmin, double xmax) ;
  inline void myProfile1D(double x, double y, const std::string& path,const std::string& title, 
			  double xmin, double xmax, size_t nbins) ;
private:
  std::string m_trackContainerName;
  int m_constrainMethod ;
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
  declareProperty( "ConstrainMethod", m_constrainMethod = Projective ) ;
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

inline void TrackFitMatchMonitor::myPlot1D(double x, const std::string& path,
					   const std::string& title, double xmin, double xmax) 
{
  std::string id = path + "/" + title ;
  plot( x, id, title, xmin, xmax ) ;
}

inline void TrackFitMatchMonitor::myProfile1D(double x, double y, const std::string& path,
					      const std::string& title, double xmin, double xmax,
					      size_t nbins) 
{
  std::string id = path + "/" + title ;
  profile1D( x, y, id, title, xmin, xmax,nbins ) ;
}


void TrackFitMatchMonitor::plotDelta(const std::string& thisname,
				     const LHCb::FitNode& node,
				     bool upstream)
{
  // get the two predictions
  LHCb::State stateUp   = node.predictedStateForward() ;
  LHCb::State stateDown = node.predictedStateBackward() ;
  // filter one of them
  filter(node, upstream ? stateUp : stateDown ) ;
  // compute the difference
  Gaudi::TrackVector delta  = stateUp.stateVector() - stateDown.stateVector() ;
  Gaudi::TrackSymMatrix cov = stateUp.covariance() + stateDown.covariance() ;

  // now, if we just look at the difference, then the problem is that
  // there are very large correlations: e.g. when you step through the
  // magnet, you don't know the momentum yet. so, if the momentum is
  // off, then so are x and tx. To solve this problem we compute a
  // 'constrained' difference: compute the difference constraining the
  // differenc ein other parameters to zero. There are two modes of operation:
  // a) constrain all 'other' variables (so for 'dx' constraint 'dy=dty=dtx=dqop=0')
  // b) constrain only qop
  // The results are different, mainly because there is something
  // wrong in the fitted tracks already: We find for MC tracks that
  // things don't match very well.

  Gaudi::TrackVector deltac,deltacpull ;
  if( m_constrainMethod == All ) {
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
      if( covc > 0 ) 
	deltacpull(irow) = deltac(irow) / std::sqrt(covc) ;
      else
	warning() << "problem with covc: "
		  << irow << " " << covc << " " << cov(irow,irow) << " "
		  << ROOT::Math::Dot(subcor,tmp) << endmsg ;
    }
  } else {
    int map[4] = {2,3,0,1} ;
    for( size_t irow = 0; irow<4; ++irow) {
      int ref = m_constrainMethod==QOverP ? 4 : map[irow] ;
      deltac(irow) = delta(irow)  - cov(irow,ref)/cov(ref,ref) * delta(ref) ;
      double covc  = cov(irow,irow) - cov(irow,ref)/cov(ref,ref) * cov(ref,irow) ;
      deltacpull(irow) = deltac(irow) / std::sqrt(covc) ;
    }
  }
  
  // these titles are only right if you choose 'Projective'
  myPlot1D(deltac(0),thisname,"dx for dtx==0",-20,20) ;
  myPlot1D(deltac(1),thisname,"dy for dty==0",-20,20) ;
  myPlot1D(deltac(2),thisname,"dtx for dx==0",-0.010,0.010) ;
  myPlot1D(deltac(3),thisname,"dty for dy==0",-0.010,0.010) ;
  //if(!m_constrainQoPOnly) plot(deltac(4),std::string("dqop"), -1e-4,1e-4) ;
  
  myPlot1D(deltacpull(0),thisname,"dx pull",-10,10) ;
  myPlot1D(deltacpull(1),thisname,"dy pull",-10,10) ;
  myPlot1D(deltacpull(2),thisname,"dtx pull",-10,10) ;
  myPlot1D(deltacpull(3),thisname,"dty pull",-10,10) ;
  //if(!m_constrainQoPOnly) plot(deltacpull(4),std::string("dqop pull"), -10,10) ;

  if( fabs(deltacpull(0))<5 ) {
    myProfile1D(node.state().tx(),deltacpull(0),thisname,"dx pull vs tx", -0.25,0.25,20) ;
    myProfile1D(node.state().ty(),deltacpull(0),thisname,"dx pull vs ty", -0.25,0.25,20) ;
    //profile1D(node.state().qOverP(),deltacpull(0), std::string("dx pull vs qop"), -4e-4,4e-4,20) ;
  }
  if( fabs(deltacpull(1))<5 ) {
    myProfile1D(node.state().tx(),deltacpull(1),thisname,"dy pull vs tx", -0.25,0.25,20) ;
    myProfile1D(node.state().ty(),deltacpull(1),thisname,"dy pull vs ty", -0.25,0.25,20) ;
    //profile1D(node.state().qOverP(),deltacpull(1), std::string("dy pull vs qop"), -4e-4,4e-4,20) ;
  }
  if( fullDetail() ) {
    if( fabs(deltacpull(2))<5 ) {
      myProfile1D(node.state().tx(),deltacpull(2),thisname,"dtx pull vs tx", -0.25,0.25,20) ;
      myProfile1D(node.state().ty(),deltacpull(2),thisname,"dty pull vs ty", -0.25,0.25,20) ;
      //profile1D(node.state().qOverP(),deltacpull(2), std::string("dtx pull vs qop"), -4e-4,4e-4,20) ;
    }
  }
}


StatusCode TrackFitMatchMonitor::execute()
{ 
  setHistoTopDir("Track/") ;

  if (!exist<LHCb::Tracks>(m_trackContainerName)) 
    return Warning( m_trackContainerName+" not found", StatusCode::SUCCESS, 0);
  const LHCb::Tracks* tracks = get<LHCb::Tracks>( m_trackContainerName ) ;
  for( LHCb::Tracks::const_iterator itr = tracks->begin() ;
       itr != tracks->end(); ++itr) {
    //plot((**itr).chi2PerDoF(),"chi2 per dof",0,10) ;
    plot((**itr).info(LHCb::Track::FitMatchChi2,-1),"match chi2",0,50) ;

    const LHCb::FitNode *lastVelo(0),*firstTT(0),*lastTT(0),*firstT(0) ;
    const LHCb::FitNode *fitnode(0) ;
    LHCb::Track::ConstNodeRange nodes = (*itr)->nodes() ;
    for( LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin() ;
	 inode != nodes.end(); ++inode) 
      if( (*inode)->hasMeasurement() &&
	  (fitnode = dynamic_cast<const LHCb::FitNode*>(*inode) ) ) {
	switch(fitnode->measurement().type()) {
	case LHCb::Measurement::VeloR:
	case LHCb::Measurement::VeloPhi:
	case LHCb::Measurement::VeloLiteR:
	case LHCb::Measurement::VeloLitePhi:
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
    
    // take only tracks with VELO
    if( lastVelo) {

      // now split this between tracks with and tracks without TT
      if( lastTT ) {

	// you can take either of these two: they give identical results.
	//plotDelta("Velo-TT at TT",*firstTT,true) ;
	plotDelta("Velo-TT",*lastVelo,false) ;

	if( firstT )
	  //plotDelta("T-TT at T",*firstT,true) ;
	  plotDelta("T-TT",*lastTT,false) ;
	
      } else if (firstT) {
	//plotDelta("Velo-T at T",*firstT,true) ;
	plotDelta("Velo-T",*lastVelo,false) ;
      }
    }
  }
  return StatusCode::SUCCESS ;
}
