#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/TwoProngVertex.h"
#include "TrackKernel/TrackStateVertex.h"
#include "TrackInterfaces/ITrackVertexer.h"
#include <algorithm>
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IAxis.h"
#include "VeloDet/DeVelo.h"

class TrackVertexMonitor : public GaudiHistoAlg
{
public:

  /** Standard construtor */
  TrackVertexMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  /** Destructor */
  virtual ~TrackVertexMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm finalize */
  virtual StatusCode finalize();

  /** Algorithm execute */
  virtual StatusCode execute();

private:
  std::string m_trackContainerName;
  std::string m_pvContainerName;
  double m_ipmax ;
  double m_ipmaxprof ;
  double m_dzmax ;
  double m_rpvmax ;
  double m_zpvmin ;
  double m_zpvmax ;
  double m_zpvmin_wide ;
  double m_zpvmax_wide ;
  double m_maxLongTrackChisqPerDof ;
  double m_minLongTrackMomentum ;
  size_t m_nprbins ;

  DeVelo* m_velo;
  const DeVeloSensor* leftSensor;
  const DeVeloSensor* rightSensor;


  ToolHandle<ITrackVertexer> m_vertexer ;
  AIDA::IHistogram1D* m_trackXIP ;
  AIDA::IProfile1D* m_trackXIPVsPhi ;
  AIDA::IProfile1D* m_trackXIPVsEta ;
  AIDA::IHistogram1D* m_trackYIP ;
  AIDA::IProfile1D* m_trackYIPVsPhi ;
  AIDA::IProfile1D* m_trackYIPVsEta ;

  AIDA::IHistogram1D* m_fastTrackTransverseIP ;
  AIDA::IProfile1D* m_fastTrackTransverseIPVsPhi ;
  AIDA::IProfile1D* m_fastTrackTransverseIPVsEta ;
  AIDA::IHistogram1D* m_fastTrackLongitudinalIP ;
  AIDA::IProfile1D* m_fastTrackLongitudinalIPVsPhi ;
  AIDA::IProfile1D* m_fastTrackLongitudinalIPVsEta ;
  AIDA::IHistogram1D* m_fastTrackXIP ;
  AIDA::IProfile1D* m_fastTrackXIPVsPhi ;
  AIDA::IProfile1D* m_fastTrackXIPVsEta ;
  AIDA::IHistogram1D* m_fastTrackYIP ;
  AIDA::IProfile1D* m_fastTrackYIPVsPhi ;
  AIDA::IProfile1D* m_fastTrackYIPVsEta ;

  AIDA::IHistogram1D* m_twoprongMass ;
  AIDA::IHistogram1D* m_twoprongMomentum ;
  AIDA::IHistogram1D* m_twoprongDoca ;
  AIDA::IHistogram1D* m_twoprongDocaPull ;
  AIDA::IHistogram1D* m_twoprongDecaylength ;
  AIDA::IHistogram1D* m_twoprongDecaylengthSignificance ;
  AIDA::IHistogram1D* m_twoprongCTau ;
  AIDA::IHistogram1D* m_twoprongTau ;
  AIDA::IHistogram1D* m_twoprongIPChisquare ;
  AIDA::IProfile1D* m_twoprongDocaVsEta ;
  AIDA::IProfile1D* m_twoprongDocaVsPhi ;

} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackVertexMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackVertexMonitor::TrackVertexMonitor( const std::string& name,
                                        ISvcLocator* pSvcLocator)
: GaudiHistoAlg( name , pSvcLocator ),
  m_vertexer("TrackVertexer")
{
  declareProperty( "TrackContainer", m_trackContainerName = LHCb::TrackLocation::Default  );
  declareProperty( "PVContainer", m_pvContainerName = LHCb::RecVertexLocation::Primary ) ;
  declareProperty( "MaxIP", m_ipmax = 0.5*Gaudi::Units::mm ) ;
  declareProperty( "MaxIPProfile", m_ipmaxprof = 0.1*Gaudi::Units::mm ) ;
  declareProperty( "MaxDz", m_dzmax =   5*Gaudi::Units::mm ) ;
  declareProperty( "MaxRPV", m_rpvmax = 1*Gaudi::Units::mm ) ;
  declareProperty( "MinZPV", m_zpvmin = -20*Gaudi::Units::cm ) ;
  declareProperty( "MaxZPV", m_zpvmax =  20*Gaudi::Units::cm ) ;
  declareProperty( "MinZPV_Wide", m_zpvmin_wide = -150*Gaudi::Units::cm , "Wide z window for PV plot" ) ;
  declareProperty( "MaxZPV_Wide", m_zpvmax_wide =  150*Gaudi::Units::cm , "Wide z window for PV plot" ) ;
  declareProperty( "MaxLongTrackChisqPerDof", m_maxLongTrackChisqPerDof = 5 ) ;
  declareProperty( "MinLongTrackMomentum", m_minLongTrackMomentum = 5 ) ;
  declareProperty( "NumProfileBins", m_nprbins = 20 ) ;
}
//=============================================================================
// Destructor
//=============================================================================
TrackVertexMonitor::~TrackVertexMonitor()
{
}


//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackVertexMonitor::initialize()
{
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;              // error printed already by GaudiAlgorithm
  sc = m_vertexer.retrieve() ;

  // book histograms
  static const std::string histoDir = "Track/" ;
  if ( "" == histoTopDir() ) setHistoTopDir(histoDir);

  // impact parameters of the hightest Pt track wrt the vertex
  m_trackXIP = book1D("track IP X","track IP X (biased)",-m_ipmax,m_ipmax) ;
  m_trackYIP = book1D("track IP Y","track IP Y (biased)",-m_ipmax,m_ipmax) ;
  m_trackXIPVsPhi = bookProfile1D("track IP X vs phi","track IP X vs phi (biased)",-Gaudi::Units::pi,Gaudi::Units::pi,m_nprbins) ;
  m_trackXIPVsEta = bookProfile1D("track IP X vs eta","track IP X vs eta (biased)",2.0,5.0,m_nprbins) ;
  m_trackYIPVsPhi = bookProfile1D("track IP Y vs phi","track IP Y vs phi (biased)",-Gaudi::Units::pi,Gaudi::Units::pi,m_nprbins) ;
  m_trackYIPVsEta = bookProfile1D("track IP Y vs eta","track IP Y vs eta (biased)",2.0,5.0,m_nprbins) ;

  m_fastTrackTransverseIP   = book1D("fast track transverse IP",-m_ipmax,m_ipmax) ;
  m_fastTrackLongitudinalIP = book1D("fast track longitudinal IP",-m_ipmax,m_ipmax) ;
  m_fastTrackTransverseIPVsPhi = bookProfile1D("fast track transverse IP vs phi",-Gaudi::Units::pi,Gaudi::Units::pi,m_nprbins) ;
  m_fastTrackTransverseIPVsEta = bookProfile1D("fast track transverse IP vs eta",2.0,5.0,m_nprbins) ;
  m_fastTrackLongitudinalIPVsPhi = bookProfile1D("fast track longitudinal IP vs phi",
                                                 -Gaudi::Units::pi,Gaudi::Units::pi,m_nprbins) ;
  m_fastTrackLongitudinalIPVsEta = bookProfile1D("fast track longitudinal IP vs eta",2.0,5.0,m_nprbins) ;

  m_fastTrackXIP = book1D("fast track IP X","fast track IP X",-m_ipmax,m_ipmax) ;
  m_fastTrackYIP = book1D("fast track IP Y","fast track IP Y",-m_ipmax,m_ipmax) ;
  m_fastTrackXIPVsPhi = bookProfile1D("fast track IP X vs phi","fast track IP X vs phi",
                                      -Gaudi::Units::pi,Gaudi::Units::pi,m_nprbins) ;
  m_fastTrackXIPVsEta = bookProfile1D("fast track IP X vs eta","fast track IP X vs eta",2.0,5.0,m_nprbins) ;
  m_fastTrackYIPVsPhi = bookProfile1D("fast track IP Y vs phi","fast track IP Y vs phi",
                                      -Gaudi::Units::pi,Gaudi::Units::pi,m_nprbins) ;
  m_fastTrackYIPVsEta = bookProfile1D("fast track IP Y vs eta","fast track IP Y vs eta",2.0,5.0,m_nprbins) ;

  // impact parameter and vertex chisquare of the two highest Pt tracks
  m_twoprongMass      = book1D("twoprong mass (GeV)",0,10) ;
  m_twoprongMomentum  = book1D("twoprong momentum (GeV)",0,200) ;
  m_twoprongDoca      = book1D("twoprong doca",-m_ipmax,m_ipmax) ;
  m_twoprongDocaPull  = book1D("twoprong doca pull",-5,5) ;
  m_twoprongDecaylength = book1D("twoprong decaylength",-2,2) ;
  m_twoprongDecaylengthSignificance = book1D("twoprong decaylength significance",-5,5) ;
  m_twoprongCTau = book1D("twoprong ctau",-0.1,0.1) ;
  m_twoprongTau  = book1D("twoprong proper lifetime (ps)",-0.2,0.2) ;
  m_twoprongIPChisquare = book1D("twoprong IP chi2 per dof",0,10) ;

  m_twoprongDocaVsEta = bookProfile1D("twoprong doca vs eta",2.0,5.0,m_nprbins) ;
  m_twoprongDocaVsPhi = bookProfile1D("twoprong doca vs phi",-Gaudi::Units::pi,Gaudi::Units::pi,m_nprbins) ;

  m_velo = getDetIfExists<DeVelo>("/dd/Structure/LHCb/BeforeMagnetRegion/Velo" );
  leftSensor  = m_velo ? (m_velo->sensor(0)) : 0 ;
  rightSensor = m_velo ? (m_velo->sensor(1)) : 0 ;

  return sc;
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackVertexMonitor::finalize()
{
  return m_vertexer.release() && GaudiHistoAlg::finalize() ;
}

namespace {

  std::vector<const LHCb::Track*> myconvert( const SmartRefVector<LHCb::Track> & tracks )
  {
    std::vector<const LHCb::Track*> rc ;
    rc.reserve( tracks.size() ) ;
    for(const LHCb::Track* t: tracks )
      if( t ) rc.push_back( t ) ;
    return rc ;
  }

  template< class TrackContainer, class Predicate >
  std::vector<const LHCb::Track*> myselect( const TrackContainer& tracks, Predicate selector)
  {
    std::vector<const LHCb::Track*> rc ;
    for( typename TrackContainer::const_iterator itrack = tracks.begin() ;
         itrack != tracks.end(); ++itrack)
      if( selector(*itrack) ) rc.push_back(*itrack) ;
    return rc ;
  }

  struct TrackTypePredicate
  {
    int m_type ;
    TrackTypePredicate(int atype) : m_type(atype) {}
    bool operator()(const LHCb::Track* track) const { return track->type()==m_type ; }
  } ;

  struct TrackFlagPredicate
  {
    LHCb::Track::Flags m_flag ;
    bool m_positive ;
    TrackFlagPredicate(LHCb::Track::Flags flag, bool positive=true) : m_flag(flag), m_positive(positive) {}
    bool operator()(const LHCb::Track* track) const { return m_positive ? track->checkFlag(m_flag) : !track->checkFlag(m_flag) ; }
  } ;

  struct TrackVeloSidePredicate
  {
    int m_sign ;
    TrackVeloSidePredicate(int asign) : m_sign(asign) {}
    bool operator()(const LHCb::Track* track) const {
      return track->firstState().tx()*m_sign*(track->checkFlag(LHCb::Track::Backward)? -1 : 1 )>0 ; }
  } ;

  struct TrackChisqPredicate
  {
    double m_maxchisq ;
    TrackChisqPredicate(double maxChisqPerDof) : m_maxchisq(maxChisqPerDof) {}
    bool operator()(const LHCb::Track* track) const { return track->chi2PerDoF() < m_maxchisq  ; }
  } ;
}

StatusCode TrackVertexMonitor::execute()
{

  LHCb::RecVertex::Range pvcontainer  = get<LHCb::RecVertex::Range>(m_pvContainerName) ;
  LHCb::Track::Range alltracks = get<LHCb::Track::Range>( m_trackContainerName );

  TrackTypePredicate isLong( LHCb::Track::Long ) ;
  TrackFlagPredicate isBackward( LHCb::Track::Backward ) ;
  TrackFlagPredicate isForward( LHCb::Track::Backward,false ) ;

  // lists needed
  // - primary vertices
  // - all tracks
  // - long tracks
  // - backward tracks
  // for now I'll just create the track lists from the Best container
  typedef std::vector<const LHCb::Track*> TrackVector ;

  // number of primary vertices
  plot(pvcontainer.size(),"NumPrimaryVertices",-0.5,10.5,11) ;

  for( const LHCb::RecVertex* pv: pvcontainer ) {
    TrackVector tracks = myconvert(pv->tracks()) ;
    TrackVector forwardtracks = myselect(tracks,isForward) ;
    TrackVector backwardtracks =  myselect(tracks,isBackward) ;
    TrackVector longtracks =  myselect(tracks,isLong) ;

    // number of tracks per primary vertex
    plot( tracks.size(), "NumTracksPerPV",-0.5,99.5,50) ;
    // number of long tracks per primary vertex
    plot( longtracks.size(), "NumLongTracksPerPV",-0.5,99.5,50) ;
    // number of backward tracks per primary vertex
    plot( backwardtracks.size(), "NumBackTracksPerPV",-0.5,99.5,50) ;
    // chisquare
    plot( pv->chi2() / pv->nDoF(), "PV chisquare per dof",0.,3.,150) ;
    // position with crap hack for vertices at exactly 0
    if(std::abs(pv->position().x()) > 0.00001 && std::abs(pv->position().y()) > 0.00001 ){
      //info() << "pvx " << pv->position().x() << endmsg;
      plot( pv->position().x(), "PV x position",-m_rpvmax,m_rpvmax) ;
      plot( pv->position().y(), "PV y position",-m_rpvmax,m_rpvmax) ;
      plot( pv->position().z(), "PV z position", m_zpvmin,m_zpvmax) ;
      plot( pv->position().z(), "PV z position (wide)", m_zpvmin_wide,m_zpvmax_wide) ;
    }

    if( std::abs( pv->position().y() ) < m_rpvmax )
      profile1D( pv->position().z(), pv->position().y(),"PV y versus z",m_zpvmin,m_zpvmax,m_nprbins) ;
    if( std::abs( pv->position().x() ) < m_rpvmax )
      profile1D( pv->position().z(), pv->position().x(),"PV x versus z",m_zpvmin,m_zpvmax,m_nprbins) ;

    // refit the primary vertex with only the long tracks
    if(longtracks.size()>=2) {
      auto longvertex = std::unique_ptr<LHCb::RecVertex>(m_vertexer->fit( longtracks ) );
      if(longvertex) plot( longvertex->chi2() / longvertex->nDoF(), "PV long chisquare per dof",0,10) ;
    }

    // now split the primary vertex in left and right tracks
    TrackVector lefttracks = myselect(tracks,TrackVeloSidePredicate(+1)) ;
    TrackVector righttracks =  myselect(tracks,TrackVeloSidePredicate(-1)) ;
    if( lefttracks.size() >= 2 && righttracks.size() >= 2 ) {
      // fit two vertices
      auto leftvertex  = std::unique_ptr<LHCb::RecVertex>(m_vertexer->fit( lefttracks ) );

      if( leftvertex ) {
        plot( leftvertex->position().x(), "PV left x",-m_rpvmax,m_rpvmax) ;
        plot( leftvertex->position().y(), "PV left y",-m_rpvmax,m_rpvmax) ;
        plot( leftvertex->position().z(), "PV left z", m_zpvmin,m_zpvmax) ;
        if( leftSensor ) {
          plot( -(leftSensor->globalToVeloHalfBox(leftvertex->position())).x(), "PV left-Left half x",-m_rpvmax/2,m_rpvmax/2) ;
          plot( -(leftSensor->globalToVeloHalfBox(leftvertex->position())).y(), "PV left-Left half y",-m_rpvmax/2,m_rpvmax/2) ;
        }
      }
      auto  rightvertex = std::unique_ptr<LHCb::RecVertex>(m_vertexer->fit( righttracks ));
      if( rightvertex) {
        plot( rightvertex->position().x(), "PV right x",-m_rpvmax,m_rpvmax) ;
        plot( rightvertex->position().y(), "PV right y",-m_rpvmax,m_rpvmax) ;
        plot( rightvertex->position().z(), "PV right z", m_zpvmin,m_zpvmax) ;
        if( rightSensor ) {
          plot( -(rightSensor->globalToVeloHalfBox(rightvertex->position())).x(), "PV right-Right half x",-m_rpvmax/2,m_rpvmax/2) ;
          plot( -(rightSensor->globalToVeloHalfBox(rightvertex->position())).y(), "PV right-Right half y",-m_rpvmax/2,m_rpvmax/2) ;
        }
      }
      if( leftvertex && rightvertex) {
        // draw the difference
        Gaudi::XYZVector dx = leftvertex->position() - rightvertex->position() ;

        plot( dx.x(), "PV left-right delta x",-0.1,0.1) ;
        plot( dx.y(), "PV left-right delta y",-0.1,0.1) ;
        plot( dx.z(), "PV left-right delta z",-1,1) ;
        if( std::abs( dx.y() ) < m_ipmaxprof )
          profile1D( pv->position().z(), dx.y(),"PV left-right delta y versus z",m_zpvmin,m_zpvmax,m_nprbins) ;
        if( std::abs( dx.x() ) < m_ipmaxprof )
          profile1D( pv->position().z(), dx.x(),"PV left-right delta x versus z",m_zpvmin,m_zpvmax,m_nprbins) ;

        // draw the pull of the difference
        Gaudi::SymMatrix3x3 cov = leftvertex->covMatrix() + rightvertex->covMatrix() ;

        // cov(0,0)
        if(cov(0,0) > 1e-10 ){
          plot( dx.x()/std::sqrt(cov(0,0)), "PV left-right delta x pull",-5,5) ;
        }else{
          Info("cov(0,0) too small", StatusCode::SUCCESS,10).ignore();
        }
        // cov(1,1)
        if(cov(1,1) > 1e-10 ){
          plot( dx.y()/std::sqrt(cov(1,1)), "PV left-right delta y pull",-5,5) ;
        }else{
          Info("cov(1,1) too small", StatusCode::SUCCESS,10).ignore();
        }
        // cov(2,2)
        if(cov(2,2) > 1e-10 ){
          plot( dx.z()/std::sqrt(cov(2,2)), "PV left-right delta z pull",-5,5) ;
        }else{
          Info("cov(2,2) too small", StatusCode::SUCCESS,10).ignore();
        }
        
        // draw the chisquares
        if( leftvertex->nDoF() > 0 ){
          plot( leftvertex->chi2() / leftvertex->nDoF(), "PV left chisquare per dof",0,10) ;
        }else{
          Info("left ndof = 0", StatusCode::SUCCESS,10).ignore();
        }
        if( rightvertex->nDoF() > 0 ){
           plot( rightvertex->chi2() / rightvertex->nDoF(), "PV right chisquare per dof",0,10) ;
        }else{
          Info("right ndof = 0", StatusCode::SUCCESS,10).ignore();
        }
      }
    }

    if( forwardtracks.size() >= 2 && backwardtracks.size() >= 2 ) {
      // fit two vertices
      auto forwardvertex  = std::unique_ptr<LHCb::RecVertex>( m_vertexer->fit( forwardtracks ) );
      auto backwardvertex = std::unique_ptr<LHCb::RecVertex>( m_vertexer->fit( backwardtracks ) );
      if( forwardvertex && backwardvertex) {
        Gaudi::XYZVector dx = forwardvertex->position() - backwardvertex->position() ;

        // draw the difference
        plot( dx.x(), "PV forward-backward delta x",-m_ipmax,m_ipmax) ;
        plot( dx.y(), "PV forward-backward delta y",-m_ipmax,m_ipmax) ;
        plot( dx.z(), "PV forward-backward delta z",-m_dzmax,m_dzmax) ;
        if( std::abs( dx.y() ) < m_ipmaxprof )
          profile1D( pv->position().z(), dx.y(),"PV forward-backward delta y versus z",m_zpvmin,m_zpvmax,m_nprbins) ;
        if( std::abs( dx.x() ) < m_ipmaxprof )
          profile1D( pv->position().z(), dx.x(),"PV forward-backward delta x versus z",m_zpvmin,m_zpvmax,m_nprbins) ;

        // draw the pull of the difference
        Gaudi::SymMatrix3x3 cov = forwardvertex->covMatrix() + backwardvertex->covMatrix() ;
        // cov(0,0)
        if(cov(0,0) > 1e-10 ){
          plot( dx.x()/std::sqrt(cov(0,0)), "PV forward-backward delta x pull",-5,5) ;
        }else{
          Info("cov(0,0) too small", StatusCode::SUCCESS,10).ignore();
        }
        // cov(1,1)
        if(cov(1,1) > 1e-10 ){
          plot( dx.y()/std::sqrt(cov(1,1)), "PV forward-backward delta y pull",-5,5) ;
        }else{
          Info("cov(1,1) too small", StatusCode::SUCCESS,10).ignore();
        }
        // cov(2,2)
        if(cov(2,2) > 1e-10 ){
          plot( dx.z()/std::sqrt(cov(2,2)), "PV forward-backward delta z pull",-5,5) ;
        }else{
          Info("cov(2,2) too small", StatusCode::SUCCESS,10).ignore();
        }
        // draw the chisquares
        if( forwardvertex->nDoF() > 0 ){
          plot( forwardvertex->chi2() / forwardvertex->nDoF(), "PV forward chisquare/dof",0,10) ;
        }else{
          Info("forward ndof = 0", StatusCode::SUCCESS,10).ignore();
        }
        if( backwardvertex->nDoF() > 0 ){
          plot( backwardvertex->chi2() / backwardvertex->nDoF(), "PV backward chisquare/dof",0,10) ;
        }else{
          Info("backward ndof = 0", StatusCode::SUCCESS,10).ignore();
        }
      }
    }

    // for events with a single vertex, do something with IP of
    // highest momentum track, as function of phi and eta.
    if( pvcontainer.size()==1 && tracks.size()>=10 ) {

      // now get all good long tracks from the best container:
      TrackVector goodlongtracks ;
      for( const LHCb::Track* tr: alltracks )
        if( isLong(tr) && tr->chi2PerDoF()<m_maxLongTrackChisqPerDof &&
            tr->p()>m_minLongTrackMomentum)
          goodlongtracks.push_back( tr ) ;

      for( const LHCb::Track* tr: goodlongtracks ) {
        const LHCb::State& firststate = tr->firstState() ;
        double dz  = pv->position().z() - firststate.z() ;
        double dx  = firststate.x() + dz * firststate.tx() - pv->position().x() ;
        double dy  = firststate.y() + dz * firststate.ty() - pv->position().y() ;
        Gaudi::XYZVector p3 = firststate.momentum() ;
        m_trackXIP->fill( dx ) ;
        m_trackYIP->fill( dy ) ;
        // apply a cut for the profiles
        if( std::abs(dx) < m_ipmaxprof && std::abs(dy) < m_ipmaxprof ) {
          double phi = p3.phi() ;
          double eta = p3.eta() ;
          m_trackXIPVsEta->fill(eta,dx) ;
          m_trackXIPVsPhi->fill(phi,dx) ;
          m_trackYIPVsEta->fill(eta,dy) ;
          m_trackYIPVsPhi->fill(phi,dy) ;
        }
      }

      if( goodlongtracks.size()>=2 ) {

        std::sort(goodlongtracks.begin(), goodlongtracks.end(),
                  [](const LHCb::Track* lhs, const LHCb::Track* rhs) {
                      return lhs->firstState().pt() < rhs->firstState().pt();
                  } );

        const LHCb::Track* firsttrack = goodlongtracks.back() ;
        goodlongtracks.pop_back() ;

        // now pick a 2nd track that makes the highest possible invariant mass with this one
        double highestmass2(0) ;
        const LHCb::Track* secondtrack = nullptr;
        Gaudi::XYZVector firstp3 = firsttrack->firstState().momentum() ;
        for( const auto& t : goodlongtracks ) {
          Gaudi::XYZVector p3 = t->firstState().momentum() ;
          double mass2= p3.r() * firstp3.r() - p3.Dot( firstp3 ) ;
          if(secondtrack==0 || highestmass2 < mass2 ) {
            highestmass2 = mass2 ;
            secondtrack = t ;
          }
        }

        // recompute the vertex without these tracks
        auto newend = tracks.end() ;
        newend = std::remove(tracks.begin(),newend,firsttrack) ;
        newend = std::remove(tracks.begin(),newend,secondtrack) ;
        tracks.erase(newend,tracks.end()) ;
        auto restvertex  = std::unique_ptr<LHCb::RecVertex>( m_vertexer->fit( tracks ) );
        if( restvertex && firsttrack->nStates()!=0 ) {
          const LHCb::State& firststate = firsttrack->firstState() ;
          double dz  = restvertex->position().z() - firststate.z() ;
          double dx  = firststate.x() + dz * firststate.tx() - restvertex->position().x()  ;
          double dy  = firststate.y() + dz * firststate.ty() - restvertex->position().y() ;
          double nt  = std::sqrt( firststate.tx()*firststate.tx() + firststate.ty()*firststate.ty() ) ;
          // transverse and longitudinal impact parameter
          double iptrans = (dx * firststate.ty() - dy * firststate.tx())/nt ;
          double iplong  = (dx * firststate.tx() + dy * firststate.ty())/nt ;
          Gaudi::XYZVector p3 = firststate.momentum() ;
          double phi = p3.phi() ;
          double eta = p3.eta() ;

          m_fastTrackTransverseIP->fill(iptrans ) ;
          m_fastTrackLongitudinalIP->fill(iplong ) ;
          m_fastTrackXIP->fill( dx ) ;
          m_fastTrackYIP->fill( dy ) ;
          // apply a cut for the profiles
          if( std::abs(iptrans) < m_ipmaxprof && std::abs(iplong) < m_ipmaxprof ) {
            m_fastTrackTransverseIPVsEta->fill(eta,iptrans) ;
            m_fastTrackTransverseIPVsPhi->fill(phi,iptrans) ;
            m_fastTrackLongitudinalIPVsEta->fill(eta,iplong) ;
            m_fastTrackLongitudinalIPVsPhi->fill(phi,iplong) ;
          }
          if( std::abs(dx) < m_ipmaxprof && std::abs(dy) < m_ipmaxprof ) {
            m_fastTrackXIPVsEta->fill(eta,dx) ;
            m_fastTrackXIPVsPhi->fill(phi,dx) ;
            m_fastTrackYIPVsEta->fill(eta,dy) ;
            m_fastTrackYIPVsPhi->fill(phi,dy) ;
          }

          // The two-track cuts we only make for relatively heavy objects
          double mass = std::sqrt(highestmass2) ;
          m_twoprongMass->fill(mass / Gaudi::Units::GeV ) ;
          if( mass > 1*Gaudi::Units::GeV ) {
            // compute doca of two tracks
            Gaudi::XYZVector dx3 = firsttrack->firstState().position() - secondtrack->firstState().position() ;
            Gaudi::XYZVector n3  = firsttrack->firstState().slopes().Cross( secondtrack->firstState().slopes() ) ;
            double doca = dx3.Dot(n3) / n3.R() ;
            m_twoprongDoca->fill(doca) ;
            if( std::abs(doca) < m_twoprongDoca->axis().upperEdge() ) {
              m_twoprongDocaVsEta->fill(firstp3.eta(),doca) ;
              m_twoprongDocaVsPhi->fill(firstp3.phi(),doca) ;
            }
            // the easiest way to compute the pull is with a vertex fit
            auto twoprong = std::unique_ptr<LHCb::TwoProngVertex>(m_vertexer->fit(firsttrack->firstState(),secondtrack->firstState()) );
            if(twoprong) {
              double pc = twoprong->p3().R() ;
              m_twoprongMomentum->fill( pc / Gaudi::Units::GeV ) ;
              m_twoprongDocaPull->fill(std::sqrt(twoprong->chi2()) * (doca>0 ? 1 : -1)) ;
              double chi2, decaylength,decaylengtherr ;
              m_vertexer->computeDecayLength( *twoprong, *restvertex, chi2, decaylength,decaylengtherr ) ;
              m_twoprongDecaylength->fill( decaylength ) ;
              m_twoprongDecaylengthSignificance->fill( decaylength/decaylengtherr ) ;
              m_twoprongIPChisquare->fill( chi2 / 2 ) ;
              m_twoprongCTau->fill( decaylength * mass / pc ) ;
              m_twoprongTau->fill( decaylength * mass / (pc * Gaudi::Units::c_light * Gaudi::Units::picosecond) ) ;
            }
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS ;
}
