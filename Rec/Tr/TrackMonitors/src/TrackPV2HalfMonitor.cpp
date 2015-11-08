#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "TrackKernel/TrackStateVertex.h"
#include "TrackInterfaces/ITrackVertexer.h"
#include "TrackInterfaces/IPVOfflineTool.h"

#include <algorithm>

#include "Event/ODIN.h"

class TrackPV2HalfAlignMonitor : public GaudiHistoAlg 
{
public:

  /** Standard construtor */
  TrackPV2HalfAlignMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  /** Destructor */
  virtual ~TrackPV2HalfAlignMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm finalize */
  virtual StatusCode finalize();

  /** Algorithm execute */
  virtual StatusCode execute();
  
private: 
  ToolHandle<ITrackVertexer> m_vertexer ;
  IPVOfflineTool* m_toolpv;
  std::string m_trackContainerName;
  size_t m_PV_trackmin;
  double m_zpvmin ;
  double m_zpvmax ;
  size_t m_nprbins ;

  double m_limpvx;
  double m_limpvy;
  double m_limpvz;
  double m_limdpvx;
  double m_limdpvy;
  double m_limdpvz;
  double m_limchi2;  
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackPV2HalfAlignMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackPV2HalfAlignMonitor::TrackPV2HalfAlignMonitor( const std::string& name,
                                                            ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_vertexer("TrackVertexer")
{
  declareProperty( "TrackContainer", m_trackContainerName = LHCb::TrackLocation::Default  );
  declareProperty( "MinNumTrPerPV", m_PV_trackmin = 5 ) ;
  declareProperty( "limPx",m_limpvx=1.*Gaudi::Units::mm ) ;
  declareProperty( "limPy",m_limpvy=1.*Gaudi::Units::mm ) ;
  declareProperty( "limPz", m_limpvz=150.*Gaudi::Units::mm ) ;
  declareProperty( "limDPx",m_limdpvx=0.5*Gaudi::Units::mm ) ;
  declareProperty( "limDPy",m_limdpvy=0.5*Gaudi::Units::mm ) ;
  declareProperty( "limDPz",m_limdpvz=1.*Gaudi::Units::mm ) ;
  declareProperty( "limChi2",m_limchi2=10.);
  declareProperty( "MinZPV", m_zpvmin = -20*Gaudi::Units::cm ) ;
  declareProperty( "MaxZPV", m_zpvmax =  20*Gaudi::Units::cm ) ;
  declareProperty( "NumProfileBins", m_nprbins = 20 ) ;
}

//=============================================================================
// Destructor
//=============================================================================
TrackPV2HalfAlignMonitor::~TrackPV2HalfAlignMonitor() 
{
}


//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackPV2HalfAlignMonitor::initialize() 
{
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;              // error printed already by GaudiAlgorithm
  sc = m_vertexer.retrieve() ;
  if( sc.isFailure() ) {
    error() << "Couldn't retrieve vertexing tool" << endmsg ;
    return sc ;
  }

  // book histograms
  static const std::string histoDir = "Track/" ;
  if ( "" == histoTopDir() ) setHistoTopDir(histoDir);

  m_toolpv =tool<IPVOfflineTool>("PVOfflineTool", this);

  return sc;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode TrackPV2HalfAlignMonitor::finalize() 
{
  m_vertexer.release().ignore() ;
  return GaudiHistoAlg::finalize() ;
}

//=============================================================================
// Structure
//=============================================================================

namespace {
  std::vector<const LHCb::Track*> myconvert( const SmartRefVector<LHCb::Track> & tracks )
  {
    std::vector<const LHCb::Track*> rc(tracks.size()) ;
    std::copy(tracks.begin(),tracks.end(),rc.begin()) ;
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
  
  struct TrackVeloSidePredicate
  {
    int m_sign ;
    TrackVeloSidePredicate(int asign) : m_sign(asign) {}
    bool operator()(const LHCb::Track* track) const { 
      return track->firstState().tx()*m_sign*(track->checkFlag(LHCb::Track::Backward)? -1 : 1 )>0 ; }
  } ;
}  


//=============================================================================
// Execute
//=============================================================================

StatusCode TrackPV2HalfAlignMonitor::execute()
{ 
  ulonglong evTimeGps(0) ;     

  LHCb::ODIN* odin = getIfExists<LHCb::ODIN> ( LHCb::ODINLocation::Default );
  if ( NULL != odin ){
    evTimeGps= odin->gpsTime();
  } else {
    Warning("No ODIN Header",StatusCode::SUCCESS,0).ignore();
  }
  
  long long int tzero= 1270064494071424ll; //there is an ll at the end, so that C++ knows this has to be a long long
  float eventTimeGpsMinute = (float) (((evTimeGps  - tzero)/1000000.)/3600.);

  // get the input data
  typedef std::vector<const LHCb::Track*> TrackVector ;
  LHCb::Track::Range alltracks = get<LHCb::Track::Range>(m_trackContainerName);
  TrackVector selectedtracks ;
  for( const LHCb::Track* tr: alltracks) 
    if( tr->hasVelo() && tr->chi2PerDoF() < 10 ) 
      selectedtracks.push_back(tr);
  
  //TrackVector 
  //split the track in right and left, to evalute PV by only right or left tracks
  TrackVector lefttracks = myselect(selectedtracks,TrackVeloSidePredicate(+1)) ;
  TrackVector righttracks =  myselect(selectedtracks,TrackVeloSidePredicate(-1)) ;
  if( lefttracks.size() >= 2 && righttracks.size() >= 2 ) {
    std::vector<LHCb::RecVertex> leftoutvtxvec;
    std::vector<LHCb::RecVertex> rightoutvtxvec;
    m_toolpv->reconstructMultiPVFromTracks(righttracks, rightoutvtxvec).ignore() ;
    m_toolpv->reconstructMultiPVFromTracks(lefttracks, leftoutvtxvec).ignore() ; 
    
    const LHCb::RecVertex* leftvertex(0);
    const LHCb::RecVertex* rightvertex(0) ;
    int n_goodleftPV=0;
    int n_goodrightPV=0;

    for( const LHCb::RecVertex& pv: leftoutvtxvec) {
      plot( pv.tracks().size(), "Left PV Num of track ",0,100,200) ;
      if( pv.tracks().size() >= m_PV_trackmin) {
	++n_goodleftPV ;
	leftvertex = &pv;
	plot( pv.position().x(), "Left PV x position",-m_limpvx,m_limpvx,200) ;
	plot( pv.position().y(), "Left PV y position",-m_limpvy,m_limpvy,200) ;
	plot( pv.position().z(), "Left PV z position",-m_limpvz,m_limpvz,200) ;
	plot( pv.chi2() / pv.nDoF(), "Left PV Chi2 per dof",0,m_limchi2,100) ;
	std::vector<const LHCb::Track*> pvtracks = myconvert( pv.tracks() ) ;
	LHCb::RecVertex* refittedvertex = m_vertexer->fit( pvtracks ) ;
	if(refittedvertex) 
	  plot( refittedvertex->chi2() / refittedvertex->nDoF(), "Left PV Chi2 per dof",0,m_limchi2,100) ;
	delete refittedvertex ;
      }
    }

    for( const LHCb::RecVertex& pv: rightoutvtxvec) {
      plot( pv.tracks().size(), "Right PV Num of track ",0,100,200) ;
      if( pv.tracks().size() >= m_PV_trackmin) {
	++n_goodrightPV;
	rightvertex = &pv;
	plot( pv.position().x(), "Right PV x position",-m_limpvx,m_limpvx,200) ;
	plot( pv.position().y(), "Right PV y position",-m_limpvy,m_limpvy,200) ;
	plot( pv.position().z(), "Right PV z position",-m_limpvz,m_limpvz,200) ;
	std::vector<const LHCb::Track*> pvtracks = myconvert( pv.tracks() ) ;
	LHCb::RecVertex* refittedvertex = m_vertexer->fit( pvtracks ) ;
	if(refittedvertex) 
	  plot( refittedvertex->chi2() / refittedvertex->nDoF(), "Right PV Chi2 per dof",0,m_limchi2,100) ;
	delete refittedvertex ;
      }
    }

    if( leftoutvtxvec.size()==1 && rightoutvtxvec.size()==1 && rightvertex && leftvertex ) {
      
      if(msgLevel(MSG::DEBUG)) debug() << "Found " << n_goodrightPV << " Right PV and "
        << n_goodleftPV << " Left PV" << endmsg;
      
      Gaudi::XYZVector dx = leftvertex->position() - rightvertex->position() ;
      plot( dx.x(), "Left-Right PV delta x ",-m_limdpvx,m_limdpvx,100) ;
      plot( dx.y(), "Left-Right PV delta y ",-m_limdpvy,m_limdpvy,100) ;
      plot( dx.z(), "Left-Right PV delta z ",-m_limdpvz,m_limdpvz,100) ;
      plot( eventTimeGpsMinute, "TimeMinute",0,30000,1000);
      
      if( std::abs( dx.z()) < m_limdpvz ) {
	double z = 0.5*( leftvertex->position().z() + rightvertex->position().z() ) ;
      	if( std::abs( dx.y() ) < m_limdpvy ) 
	  profile1D( z, dx.y(),"PV left-right delta y versus z",m_zpvmin,m_zpvmax,m_nprbins) ;
	if( std::abs( dx.x() ) < m_limdpvx ) 
	  profile1D( z, dx.x(),"PV left-right delta x versus z",m_zpvmin,m_zpvmax,m_nprbins) ;
      }
    }
  }
    
  return StatusCode::SUCCESS ;
}
