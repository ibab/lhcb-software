// Include files:
// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"
#include "DetDesc/Condition.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
// Local
#include "PVOfflineTool.h"

DECLARE_TOOL_FACTORY(PVOfflineTool)

namespace {

template <typename Container, typename ValueType, typename UnaryFcn>
ValueType min_value(const Container& c, ValueType init, UnaryFcn fcn) {
    using cref = typename Container::const_reference;
    return std::accumulate( std::begin(c), std::end(c), init,
                             [&]( ValueType v, cref r) {
             return std::min(v,fcn(r));
    });
}


double zCloseBeam(const LHCb::Track& track) {
  Gaudi::XYZVector unitVect = track.firstState().slopes().Unit();
  const LHCb::State& stateG = track.firstState();
  return stateG.z() - unitVect.z() *
    (unitVect.x() * stateG.x() + unitVect.y() * stateG.y()) /
    (1.0 - std::pow(unitVect.z(),2));
}
//=============================================================================
// Store dummy vertices
//=============================================================================
std::vector<LHCb::RecVertex>
storeDummyVertices(const std::vector<Gaudi::XYZPoint>& seeds,
                   const std::vector<const LHCb::Track*>& rtracks)
{
  std::vector<LHCb::RecVertex> out; out.reserve(seeds.size());
  for (const auto& seed : seeds) {
    LHCb::RecVertex tVertex;
    tVertex.setPosition( seed );
    Gaudi::SymMatrix3x3 errMat;
    for(int i = 0; i < 3; i++) {
      for(int j = 0; j < 3; j++) {
        errMat(i,j) = 1.0;
      }
    }
    tVertex.setCovMatrix(errMat);
    tVertex.setNDoF(1);
    tVertex.setChi2(99999.0);
    // Fill close tracks
    for(const auto& trk : rtracks) {
      if(fabs(zCloseBeam(*trk) - seed.Z()) < 3.0 * Gaudi::Units::mm) {
        tVertex.addToTracks(trk);
      }
    }
    out.push_back(tVertex);
  }
  return out;
}


class velo_overlap_with {
    std::vector<LHCb::LHCbID> m_ref;
public:
    velo_overlap_with(const std::vector<LHCb::LHCbID>& ids) {
        m_ref.reserve(ids.size());
        std::copy_if( ids.begin(), ids.end(),
                      std::back_inserter(m_ref),
                      [](const LHCb::LHCbID& id) { return id.isVelo(); });
    }
    velo_overlap_with(const LHCb::Track& trk) : velo_overlap_with( trk.lhcbIDs() ) {
    }

    std::pair<int,int> operator()(const std::vector<LHCb::LHCbID>& ids) const {
      auto first1 = m_ref.begin() ;
      auto last1  = m_ref.end() ;
      auto first2 = ids.begin() ;
      auto last2  = ids.end() ;
      auto n = std::make_pair(0,0);
      while (first1 != last1 && first2 != last2) {
        if ( *first1 < *first2 ) {
          ++first1;
        } else if ( *first2 < *first1 ) {
          if (first2->isVelo()) ++n.second;
          ++first2;
        } else {
          ++n.first;
          ++n.second;
          ++first1;
          ++first2;
        }
      }
      return n;
    }
    std::pair<int,int> operator()(const LHCb::Track& trk) const
    { return (*this)(trk.lhcbIDs()); }


};


}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVOfflineTool::PVOfflineTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent)
  : base_class(type,name,parent)
{
  declareInterface<IPVOfflineTool>(this);
  declareProperty("RequireVelo"      , m_requireVelo   = true);
  declareProperty("SaveSeedsAsPV"    , m_saveSeedsAsPV = false);
  declareProperty("InputTracks"      , m_inputTracks);
  declareProperty("InputVerticesName", m_inputVerticesName = LHCb::RecVertexLocation::Velo3D);
  declareProperty("PVFitterName"     , m_pvFitterName  = "LSAdaptPVFitter");
  declareProperty("PVSeedingName"    , m_pvSeedingName = "PVSeed3DTool");
  declareProperty("LookForDisplaced" , m_lookForDisplaced = false);
  declareProperty("PVsChi2Separation", m_pvsChi2Separation = 25.);
  declareProperty("PVsChi2SeparationLowMult", m_pvsChi2SeparationLowMult = 91.);
  declareProperty("UseBeamSpotRCut",   m_useBeamSpotRCut  = false );
  declareProperty("BeamSpotRCut",      m_beamSpotRCut = 0.3 );
  declareProperty("ResolverBound",     m_resolverBound = 5 * Gaudi::Units::mm );
  declareProperty("TimingMeasurement", m_doTiming       = false     );
}

//=========================================================================
// Initialize
//=========================================================================
StatusCode PVOfflineTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;

  m_beamSpotCond = "/dd/Conditions/Online/Velo/MotionSystem";
  if (m_useBeamSpotRCut ) {
    IUpdateManagerSvc* m_updMgrSvc = svc<IUpdateManagerSvc>("UpdateManagerSvc", true);
    m_updMgrSvc->registerCondition(this, m_beamSpotCond, &PVOfflineTool::UpdateBeamSpot);
    StatusCode scu = m_updMgrSvc->update(this);
    if(!scu.isSuccess())
      return Error("Failed to update conditions!",StatusCode::FAILURE);
  }

  // Access PVFitterTool
  m_pvfit = tool<IPVFitter>(m_pvFitterName,this);
  if(!m_pvfit) {
    err() << "Unable to retrieve the PVFitterTool" << endmsg;
    return  StatusCode::FAILURE;
  }
  // Access PVSeedTool
  m_pvSeedTool = tool<IPVSeeding>(m_pvSeedingName,this);
  if(!m_pvSeedTool) {
    err() << "Unable to retrieve the PV seeding tool " << m_pvSeedingName << endmsg;
    return  StatusCode::FAILURE;
  }

  // Access PVOFflineRecalculate tool
  m_pvRecalc = tool<PVOfflineRecalculate>("PVOfflineRecalculate", this);
  //  m_pvRecalc = tool<PVOfflineRecalculateWG1>("PVOfflineRecalculateWG1", this);
  if(!m_pvRecalc) {
    err() << "Unable to retrieve the PV recalculation tool " << endmsg;
    return  StatusCode::FAILURE;
  }


  // Input tracks
  if(m_inputTracks.size() == 0) {
    m_inputTracks.push_back(LHCb::TrackLocation::Default);
  }

  if ( m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool/Timer", this );
    m_timeTotal   = m_timerTool->addTimer( "PatPV total" );
    m_timerTool->increaseIndent();
    m_timeSeeding = m_timerTool->addTimer( "PatPV seeding" );
    m_timeFitting = m_timerTool->addTimer( "PatPV fitting" );
    m_timerTool->decreaseIndent();
  }

  return StatusCode::SUCCESS;
}


//=============================================================================
// reconstruct PV for a given seed
//=============================================================================
StatusCode PVOfflineTool::reconstructSinglePV(const Gaudi::XYZPoint xyzseed,
                                              LHCb::RecVertex& outvtx)
{
  std::vector<const LHCb::Track*> tracks2remove;
  StatusCode scvfit = m_pvfit->fitVertex( xyzseed, readTracks(), outvtx, tracks2remove );
  return scvfit;
}
//=============================================================================
// reconstruct PV for a given seed using tracks from the list
//=============================================================================
StatusCode PVOfflineTool::reconstructSinglePVFromTracks(const Gaudi::XYZPoint xyzseed,
                                                        std::vector<const LHCb::Track*>& tracks2use,
                                                        LHCb::RecVertex& outvtx)
{
  std::vector<const LHCb::Track*> tracks2remove;
  StatusCode scvfit = m_pvfit->fitVertex( xyzseed, tracks2use, outvtx, tracks2remove );
  return scvfit;
}

//=============================================================================
// reconstruct PV for a given seed with a list of tracks to be excluded
//=============================================================================
StatusCode PVOfflineTool::reDoSinglePV(const Gaudi::XYZPoint xyzseed,
                                       std::vector<const LHCb::Track*>& tracks2exclude,
                                       LHCb::RecVertex& outvtx)
{
  auto rtracks = readTracks();
  if(tracks2exclude.size()>0) {
    removeTracksByLHCbIDs(rtracks, tracks2exclude);
  }
  std::vector<const LHCb::Track*> tracks2remove;
  return m_pvfit->fitVertex( xyzseed, rtracks, outvtx, tracks2remove );

}


//=============================================================================
// multi vtx search and fit. Return new vtx after track removal
//=============================================================================
StatusCode PVOfflineTool::reDoMultiPV(const LHCb::RecVertex& invtx,
                                      std::vector<const LHCb::Track*>& tracks2exclude,
                                      LHCb::RecVertex& outvtx)
{
  std::vector<LHCb::RecVertex> outvtxVec;
  auto rtracks = readTracks();

  if(tracks2exclude.size()>0) {
    removeTracksByLHCbIDs(rtracks, tracks2exclude);
  }

  StatusCode scvfit = reconstructMultiPVFromTracks(rtracks,outvtxVec);

  if(scvfit != StatusCode::SUCCESS) return scvfit;
  // check which vtx corresponds to input vtx
  StatusCode scmatch = matchVtxByTracks(invtx, outvtxVec, outvtx);
  return scmatch;
}

//=============================================================================
// multi vtx search and fit with tracks from default location
//=============================================================================
StatusCode PVOfflineTool::reconstructMultiPV(std::vector<LHCb::RecVertex>& outvtxvec)
{
  auto r = readTracks();
  return reconstructMultiPVFromTracks(r, outvtxvec);
}


//=============================================================================
// multi vtx search and fit with tracks specified
//=============================================================================
StatusCode PVOfflineTool::reconstructMultiPVFromTracks(std::vector<const LHCb::Track*>& tracks2use,
                                                       std::vector<LHCb::RecVertex>& outvtxvec)
{

  if ( m_doTiming ){
    m_timerTool->start( m_timeTotal );
  }

  auto rtracks = tracks2use;

  outvtxvec.clear();

  if (m_saveSeedsAsPV) {
    auto seeds = m_pvSeedTool->getSeeds(rtracks, {m_beamSpotX, m_beamSpotY, 0.0 });
    outvtxvec = storeDummyVertices(seeds, rtracks);
    return StatusCode::SUCCESS;
  }

  int nvtx_before = -1;
  int nvtx_after  =  0;
  while ( nvtx_after > nvtx_before ) {
    nvtx_before = outvtxvec.size();
    // reconstruct vertices

    if ( m_doTiming ){
      m_timerTool->start( m_timeSeeding );
    }

    // seeding
    auto seeds = m_pvSeedTool->getSeeds(rtracks, {m_beamSpotX, m_beamSpotY, 0.0} );

    if ( m_doTiming ){
      m_timerTool->stop( m_timeSeeding );
    }

    //TIMEBOMB fixed
    if (!m_pvfit)
      return Warning("m_pvfit is null ", StatusCode::FAILURE);

    if(msgLevel(MSG::DEBUG)) {
      debug() << " seeds  " << seeds.size()
              << " pos: " << seeds << endmsg;
    }
    for (const auto& seed : seeds) {
      LHCb::RecVertex recvtx;
      if(msgLevel(MSG::VERBOSE)) {
        verbose() << "ready to fit" << endmsg;
      }

      std::vector<const LHCb::Track*> tracks2remove;
      if ( m_doTiming ){
        m_timerTool->start( m_timeFitting );
      }

      // fitting
      StatusCode scvfit = m_pvfit->fitVertex( seed, rtracks, recvtx, tracks2remove );

      if ( m_doTiming ){
        m_timerTool->stop( m_timeFitting );
      }

      if(scvfit.isSuccess()) {
        bool isSepar = separatedVertex(recvtx,outvtxvec);
        bool inR = true;
        if ( m_useBeamSpotRCut && m_veloClosed ) {
          const auto& pos = recvtx.position();
          if ( (std::pow(pos.x()-m_beamSpotX,2) + std::pow( pos.y()-m_beamSpotY,2) )
              > std::pow(m_beamSpotRCut,2) ) inR = false;
        }
        if ( isSepar && inR ) {
          outvtxvec.push_back(recvtx);
          removeTracks(rtracks, tracks2remove);
        }
      }
    }//iterate on seeds
    nvtx_after = outvtxvec.size();
  }//iterate on vtx

  if ( m_doTiming ) m_timerTool->stop( m_timeTotal );

  return StatusCode::SUCCESS;

}

bool PVOfflineTool::separatedVertex( const LHCb::RecVertex& rvtx, const std::vector<LHCb::RecVertex>& outvtxvec) const {

  auto rz = rvtx.position().z();
  auto sigma2z = rvtx.covMatrix()(2,2);
  double chi2min = min_value( outvtxvec, 1e10,
                              [=] (const LHCb::RecVertex& v) { //TODO: C++14: use generalized capture
            return std::pow(rz - v.position().z(), 2)/(sigma2z+v.covMatrix()(2,2));
  });

  bool vsepar = true;
  if ( chi2min < m_pvsChi2Separation ) vsepar = false;
  // protect secondary vertices of B signal
  if ( chi2min < m_pvsChi2SeparationLowMult && rvtx.tracks().size() < 7 ) vsepar = false;
  return vsepar;
}

//=============================================================================
// Read tracks
//=============================================================================
std::vector<const LHCb::Track*> PVOfflineTool::readTracks() const
{
  if(msgLevel(MSG::VERBOSE)) verbose() << "readTracks method" << endmsg;

  std::vector<const LHCb::Track*> rtracks;

  for(const auto& trName : m_inputTracks) {
    // Retrieving tracks
    LHCb::Tracks* stracks = get<LHCb::Tracks>(trName);
    if(!stracks) {
      warning() << "Tracks not found at location: " << trName << endmsg;
      continue;
    }
    // Read parameters
    std::copy_if( stracks->begin(), stracks->end(),
                  std::back_inserter(rtracks),
                  [=](const LHCb::Track *trk) { return !m_requireVelo || trk->hasVelo(); } );
  }

  //remove tracks used by the other vertices to look for displaced vertices
  if(m_lookForDisplaced){
    const auto inputVertices = get<LHCb::RecVertices>(m_inputVerticesName);
    if(!inputVertices) {
      warning() << "Vertices not found at location: " << m_inputVerticesName << endmsg;
    }
    for (auto& vrt:  *inputVertices) {
      removeTracksUsedByVertex(rtracks, *vrt);
    }
  }

  if (msgLevel(MSG::DEBUG)) debug() << "readTracks: " << rtracks.size() << endmsg;

  return rtracks;
}

//=============================================================================
// removeTracks
//=============================================================================
void PVOfflineTool::removeTracks(std::vector<const LHCb::Track*>& tracks,
                                 const std::vector<const LHCb::Track*>& tracks2remove) const
{
  if(msgLevel(MSG::VERBOSE)) verbose() << " removeTracks method" << endmsg;

  if(msgLevel(MSG::DEBUG)) {
    debug() << "removeTracks. Input number of tracks: " << tracks.size()
            << "  To remove:" << tracks2remove.size() << endmsg;
  }

  for(const auto& trk : tracks2remove) {
    auto i = std::find(tracks.begin(),tracks.end(), trk);
    if(i != tracks.end()) tracks.erase(i);
  }

  if(msgLevel(MSG::DEBUG)) {
    debug() << "removeTracks. Output number of tracks: " << tracks.size() << endmsg;
  }
}

//=============================================================================
// removeTracks
//=============================================================================
void PVOfflineTool::removeTracksByLHCbIDs(std::vector<const LHCb::Track*>& tracks,
                                          const std::vector<const LHCb::Track*>& tracks2remove) const
{
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << " removeTracks method" << endmsg;
  }

  if(msgLevel(MSG::DEBUG)) {
    debug() << "removeTracksByLHCbIDs. Input number of tracks: " << tracks.size() << endmsg;
  }

  for(const auto& ptr1 : tracks2remove) {
    auto compute_overlap = velo_overlap_with(*ptr1);
    auto i = std::find_if( tracks.begin(), tracks.end(),
                           [&](const LHCb::Track* trk) {
        auto n = compute_overlap(*trk);
        return 1.*n.first > 0.99*n.second;
    });
    if (i!=tracks.end()) tracks.erase(i);
  } // over tracks2remove

  if(msgLevel(MSG::DEBUG)) {
    debug() << "removeTracksByLHCbIDs. Output number of tracks: " << tracks.size() << endmsg;
  }
}

//=============================================================================
// removeTracksUsedByVertex
//=============================================================================
void PVOfflineTool::removeTracksUsedByVertex(std::vector<const LHCb::Track*>& tracks,
                                             LHCb::RecVertex& rvtx) const

{
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << " removeTracksUsedByVertex method" << endmsg;
  }

  const auto& vtx_tracks = rvtx.tracks();
  removeTracks(tracks, { std::begin(vtx_tracks), std::end(vtx_tracks) });

  if(msgLevel(MSG::VERBOSE)) {
    verbose() << " leaving removeTracksUsedByVertex method" << endmsg;
  }
}

//=============================================================================
// Match vtx in vector of vtx by matching tracks
//=============================================================================
StatusCode PVOfflineTool::matchVtxByTracks(const LHCb::RecVertex& invtx,
                                           std::vector<LHCb::RecVertex>& outvtxvec,
                                           LHCb::RecVertex& outvtx)
{
  auto& tracksIn =  invtx.tracks();
  int ntrin = tracksIn.size();

  if( ntrin == 0 ) return StatusCode::FAILURE;

  double lastrate = 0.;
  auto itbest = outvtxvec.end();
  for (auto itv = outvtxvec.begin(); itv != outvtxvec.end(); itv++) {
    const auto& tracksOut = itv->tracks();
    auto imatch = std::count_if( tracksIn.begin(), tracksIn.end(),
                                 [&](const LHCb::Track* t) {
      return std::find_if(tracksOut.begin(),tracksOut.end(), //force SmartRef-> * conversion...
                          [&](const LHCb::Track* to) { return to==t;})!=tracksOut.end();
    });
    double rate = 1.0*imatch/ntrin;
    if (rate > lastrate ) {
      lastrate = rate;
      itbest = itv;
    }
  }

  if ( itbest == outvtxvec.end() && lastrate < 0.3 ) {
    return StatusCode::FAILURE;
  }

  outvtx = *itbest;
  debug() << " vtx succesfully matched at tracks rate: " << lastrate << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode PVOfflineTool::removeTracksAndRecalculatePV(const LHCb::RecVertex* pvin,
					const std::vector<const LHCb::Track*>& tracks2remove,
					LHCb::RecVertex& vtx)
{
  return m_pvRecalc->RecalculateVertex(pvin, tracks2remove, vtx);
}

//=============================================================================
// Update of the beam spot position
//=============================================================================
StatusCode PVOfflineTool::UpdateBeamSpot()
{
  if (! exist<Condition>(detSvc(), m_beamSpotCond )){
    Warning( "Unable to locate beam spot condition" ) ;
    return StatusCode::FAILURE;
  }
  Condition *myCond =  get<Condition>(detSvc(), m_beamSpotCond );
  //
  const double xRC = myCond -> paramAsDouble ( "ResolPosRC" ) ;
  const double xLA = myCond -> paramAsDouble ( "ResolPosLA" ) ;
  const double   Y = myCond -> paramAsDouble ( "ResolPosY"  ) ;
  //
  m_beamSpotX = ( xRC + xLA ) / 2;
  m_beamSpotY = Y ;

  m_veloClosed = (std::abs ( xRC - m_beamSpotX ) < m_resolverBound &&
                  std::abs ( xLA - m_beamSpotX ) < m_resolverBound);

  return StatusCode::SUCCESS;
}
