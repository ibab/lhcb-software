#include "GaudiKernel/ToolHandle.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "Event/Track.h"
#include "Event/TrackFitResult.h"
#include "Event/FitNode.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram1D.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <boost/foreach.hpp>

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

  /** Algorithm execute */
  virtual StatusCode execute();

private:
  void plotDelta(const std::string& name,const LHCb::FitNode& node,bool upstream) ;
  inline void myPlot1D(double x, const std::string& path,const std::string& title, double xmin, double xmax) ;
  inline void myProfile1D(double x, double y, const std::string& path,const std::string& title, 
			  double xmin, double xmax, size_t nbins) ;
  void plotCurvatureMatch(const LHCb::Track& track) ;
private:
  std::string m_trackContainerName;
  int m_constrainMethod ;
  AIDA::IProfile1D* m_curvatureRatioTToLongPr ;
  AIDA::IProfile1D* m_curvatureRatioVeloTTToLongPr ;
  AIDA::IProfile1D* m_curvatureRatioTToLongVsTxPos ;
  AIDA::IProfile1D* m_curvatureRatioVeloTTToLongVsTxPos ;
  AIDA::IProfile1D* m_curvatureRatioTToLongVsTxNeg ;
  AIDA::IProfile1D* m_curvatureRatioVeloTTToLongVsTxNeg ;
  AIDA::IHistogram1D* m_curvatureRatioTToLongH1 ;
  AIDA::IHistogram1D* m_curvatureRatioVeloTTToLongH1 ;
  AIDA::IHistogram1D* m_curvatureRatioTToLongPullH1 ;
  AIDA::IHistogram1D* m_curvatureRatioVeloTTToLongPullH1 ;
  AIDA::IHistogram1D* m_kickZH1 ;
  AIDA::IProfile1D*  m_kickZVsXPr ;
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackFitMatchMonitor )

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

  setHistoTopDir("Track/") ;
  m_curvatureRatioTToLongPr = bookProfile1D("curvatureRatioTToLongVsQoP", "curvature ratio T to Long versus q/p",-0.4,0.4,40) ;
  m_curvatureRatioVeloTTToLongPr = bookProfile1D("curvatureRatioVeloTTToLongVsQoP", "curvature ratio Velo-TT to Long versus q/p",-0.4,0.4,40) ;
  m_curvatureRatioTToLongVsTxPos = bookProfile1D("curvatureRatioTToLongVsTx", "curvature ratio T to Long versus tx for pos",-0.25,0.25,40) ;
  m_curvatureRatioVeloTTToLongVsTxPos = bookProfile1D("curvatureRatioVeloTTToLongVsTx", "curvature ratio Velo-TTT to Long versus tx for pos",-0.25,0.25,40) ;
  m_curvatureRatioTToLongVsTxNeg = bookProfile1D("curvatureRatioTToLongVsTx", "curvature ratio T to Long versus tx for neg",-0.25,0.25,40) ;
  m_curvatureRatioVeloTTToLongVsTxNeg = bookProfile1D("curvatureRatioVeloTTToLongVsTx", "curvature ratio Velo-TTT to Long versus tx for neg",-0.25,0.25,40) ;

  m_curvatureRatioTToLongH1 = book1D("curvatureRatioTToLong", "curvature ratio T to Long",0,2) ;
  m_curvatureRatioVeloTTToLongH1 = book1D("curvatureRatioVeloTTToLong", "curvature ratio Velo-TT to Long",0,2) ;
  m_curvatureRatioTToLongPullH1 = book1D("curvatureRatioTToLongPull", "curvature ratio T to Long pull",-5,5) ;
  m_curvatureRatioVeloTTToLongPullH1 = book1D("curvatureRatioVeloTTToLongPull", "curvature ratio Velo-TT to Long pull",-5,5) ;
  
  m_kickZH1    = book1D("kickZ","Z position of magnet kick", 4900,5400) ;
  m_kickZVsXPr = bookProfile1D("kickZVsX","Z position of magnet kick versus x",-1500,1500) ;

  return sc;
}

//=============================================================================
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
  const LHCb::State& stateUp   = upstream ? node.filteredStateForward()  : node.predictedStateForward() ;
  const LHCb::State& stateDown = upstream ? node.predictedStateBackward() : node.filteredStateBackward() ;

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

  if( std::abs(deltacpull(0))<5 ) {
    myProfile1D(node.state().tx(),deltacpull(0),thisname,"dx pull vs tx", -0.25,0.25,20) ;
    myProfile1D(node.state().tx(),deltac(0),thisname,"dx vs tx", -0.25,0.25,20) ;
    myProfile1D(node.state().ty(),deltacpull(0),thisname,"dx pull vs ty", -0.25,0.25,20) ;
    myProfile1D(node.state().qOverP()*Gaudi::Units::GeV,deltacpull(0),thisname,"dx pull vs qop", -0.2,0.2,40) ;
    myProfile1D(node.state().qOverP()*Gaudi::Units::GeV,deltac(0),thisname,"dx vs qop", -0.2,0.2,40) ;
    myProfile1D(node.state().x(),deltacpull(0),thisname,"dx pull vs x", -2400,2400,48) ;
    myProfile1D(node.state().x(),deltac(0),thisname,"dx vs x", -2400,2400,48) ;
    //profile1D(node.state().qOverP(),deltacpull(0), std::string("dx pull vs qop"), -4e-4,4e-4,20) ;
  }
  if( std::abs(deltacpull(1))<5 ) {
    myProfile1D(node.state().tx(),deltacpull(1),thisname,"dy pull vs tx", -0.25,0.25,20) ;
    myProfile1D(node.state().ty(),deltacpull(1),thisname,"dy pull vs ty", -0.25,0.25,20) ;
    //profile1D(node.state().qOverP(),deltacpull(1), std::string("dy pull vs qop"), -4e-4,4e-4,20) ;
  }
  if( std::abs(deltacpull(2))<5 ) {
    myProfile1D(node.state().x(),deltac(2), thisname,"dtx vs x", -2400, 2400, 48) ;
    myProfile1D(node.state().x(),deltacpull(2), thisname,"dtx pull vs x", -2400, 2400, 48) ;
    myProfile1D(node.state().qOverP()*Gaudi::Units::GeV,deltac(2),thisname,"dtx vs qop", -0.2, 0.2, 40) ;
    myProfile1D(node.state().qOverP()*Gaudi::Units::GeV,deltacpull(2),thisname,"dtx pull vs qop", -0.2, 0.2, 40) ;
  }
  if( fullDetail() ) {
    if( std::abs(deltacpull(2))<5 ) {
      myProfile1D(node.state().tx(),deltacpull(2),thisname,"dtx pull vs tx", -0.25,0.25,20) ;
      myProfile1D(node.state().ty(),deltacpull(2),thisname,"dty pull vs ty", -0.25,0.25,20) ;
      //profile1D(node.state().qOverP(),deltacpull(2), std::string("dtx pull vs qop"), -4e-4,4e-4,20) ;
    }
  }
}


StatusCode TrackFitMatchMonitor::execute()
{ 
  setHistoTopDir("Track/") ;

  LHCb::Track::Range tracks = get<LHCb::Track::Range>( m_trackContainerName ) ;
  
  BOOST_FOREACH( const LHCb::Track* track, tracks) {
    if( track->fitResult() && track->fitResult()->nodes().size()>0 ) {
      plotCurvatureMatch( *track ) ;
      
      const LHCb::FitNode *lastVelo(0),*firstTT(0),*lastTT(0),*firstT(0) ;
      const LHCb::FitNode *fitnode(0) ;
      LHCb::Track::ConstNodeRange nodes = track->nodes() ;
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
	  case LHCb::Measurement::TTLite:
	    if( !firstTT || (firstTT->z() > fitnode->z()) ) firstTT=fitnode ;
	    if( !lastTT || (lastTT->z() < fitnode->z()) ) lastTT=fitnode ;
	    break;
	  case LHCb::Measurement::OT:
	  case LHCb::Measurement::IT:
	  case LHCb::Measurement::ITLite:
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
	  plotDelta("Velo-TT",*firstTT,true) ;
	  //plotDelta("Velo-TT",*lastVelo,false) ;
	  
	  if( firstT )
	    plotDelta("T-TT",*firstT,true) ; 
	  //plotDelta("T-TT",*lastTT,false) ;
	  
	} else if (firstT) {
	  plotDelta("Velo-T",*firstT,true) ;
	  //plotDelta("Velo-T",*lastVelo,false) ;
	}
      }
    }
  }
  return StatusCode::SUCCESS ;
}


void TrackFitMatchMonitor::plotCurvatureMatch(const LHCb::Track& track)
{
  // inspired by the problems we see in the field. see also TT field study 
  if( track.hasT() && track.hasVelo() && track.hasTT() && std::abs(track.firstState().qOverP()) > 0 ) {
    
    // first make sure that we have hits in all 3 T stations
    int hitsInStation[3] = {0,0,0} ;
    BOOST_FOREACH( LHCb::Node* node, track.fitResult()->nodes() ) {
      if(node->type() == LHCb::Node::HitOnTrack ) {
	LHCb::LHCbID id = node->measurement().lhcbID() ;
	if( id.isOT() ) {
	  hitsInStation[ id.otID().station()-1 ] += 1 ;
	} else if( id.isIT() ) {
	  hitsInStation[ id.stID().station()-1 ] += 1 ;
	}
      }
    }
      
    if( hitsInStation[0] >=3 && hitsInStation[1] >=3 && hitsInStation[2] >=3 ) {

      // first get the 3 measurements of the curvature with error
      //nodes are sorted in decreasing z. find the nodes around the magnet
      const LHCb::Node *nodeAfter(0), *nodeBefore(0), *firstNodeAfterT1(0) ;
      BOOST_FOREACH( LHCb::Node* node, track.fitResult()->nodes() ) {
	if( node->z() > 5200 ) {
	  if( nodeAfter==0 || nodeAfter->z() > node->z() ) nodeAfter = node ;
	} else {
	  if( nodeBefore==0 || nodeBefore->z() < node->z() ) nodeBefore = node ;
	}
	if( node->z() > 8100 )
	  if( firstNodeAfterT1 ==0 || firstNodeAfterT1->z() > node->z() ) firstNodeAfterT1 = node ;
	
      }
      const LHCb::FitNode *fitNodeAfter = dynamic_cast<const LHCb::FitNode*>(nodeAfter) ;
      const LHCb::FitNode *fitNodeBefore = dynamic_cast<const LHCb::FitNode*>(nodeBefore) ;

      if ((fitNodeBefore != NULL) && (fitNodeAfter != NULL)) {
      
	// NOTE: we dont have the filtered states, so we take the
	// predicted state at the next node! for q/p this is okay. don't
	// do this for any of the other parameters!!

	double qop       = fitNodeBefore->state().qOverP() ;
	double tx        = fitNodeBefore->state().tx() ;
	
	// extract the 'upstream' filtered state of T segment
	bool upstream = track.fitResult()->nodes().front()->z() > track.fitResult()->nodes().back()->z() ;
	const LHCb::State& stateT = upstream ? fitNodeAfter->filteredStateForward() : fitNodeAfter->filteredStateBackward() ;
	double qopT      = stateT.qOverP() ;
	double qoperrT   = std::sqrt(stateT.covariance()(4,4));

	// extract the 'downstream' filtered state of Velo-TT segment
	const LHCb::State& stateVeloTT = upstream ? fitNodeBefore->filteredStateBackward() : fitNodeBefore->filteredStateForward() ;
	double qopVeloTT = stateVeloTT.qOverP() ;
	double qoperrVeloTT = std::sqrt(stateVeloTT.covariance()(4,4));
	
	m_curvatureRatioTToLongH1->fill(qopT / qop ) ;
	m_curvatureRatioVeloTTToLongH1->fill(qopVeloTT / qop ) ;
	m_curvatureRatioTToLongPullH1->fill( (qopT - qop) / qoperrT ) ;
	m_curvatureRatioVeloTTToLongPullH1->fill( (qopVeloTT - qop) / qoperrVeloTT ) ;
	
	if( std::abs(qopT / qop - 1 ) < 1 ) {
	  m_curvatureRatioTToLongPr->fill(qop * Gaudi::Units::GeV, qopT / qop ) ;
	  if( qop>0 ) 
	    m_curvatureRatioTToLongVsTxPos->fill(tx, qopT / qop ) ;
	  else
	    m_curvatureRatioTToLongVsTxNeg->fill(tx, qopT / qop ) ;
	}
	
	if( std::abs(qopVeloTT / qop - 1 ) < 1 ) {
	  m_curvatureRatioVeloTTToLongPr->fill(qop * Gaudi::Units::GeV, qopVeloTT / qop ) ;
	  
	  if( std::abs(qopT / qop - 1 ) < 1 ) 
	    m_curvatureRatioTToLongPr->fill(qop * Gaudi::Units::GeV, qopT / qop ) ;
	  if( std::abs(qopVeloTT / qop - 1 ) < 1 ) 
	    m_curvatureRatioVeloTTToLongPr->fill(qop * Gaudi::Units::GeV, qopVeloTT / qop ) ;
	}
	
	if( qop>0 ) 
	  m_curvatureRatioVeloTTToLongVsTxPos->fill(tx, qopVeloTT / qop ) ;
	else
	  m_curvatureRatioVeloTTToLongVsTxNeg->fill(tx, qopVeloTT / qop ) ;

	// compute the (x,z) point of the intersection of the 2 segments for linear propagation
	// FIXME: it must be better to take a fixed z position in T.
	if( 1/std::abs(qop) > 5*Gaudi::Units::GeV ) {
	  double zkick = ( stateVeloTT.z()*stateVeloTT.tx() - stateVeloTT.x() + stateT.x() - stateT.z()*stateT.tx() ) / ( stateVeloTT.tx() - stateT.tx() ) ;
	  double xkick = stateT.x() + (zkick - stateT.z()) * stateT.tx() ;
    //	  double xkickprime = stateVeloTT.x() + (zkick - stateVeloTT.z()) * stateVeloTT.tx() ;
	  m_kickZH1->fill( zkick) ;
	  if( 5000*Gaudi::Units::mm < zkick && zkick < 5300*Gaudi::Units::mm ) 
	    m_kickZVsXPr->fill(xkick,zkick) ;
	}
      }
    }
  }
}
