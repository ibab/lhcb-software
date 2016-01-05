// Include files

// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiAlg/ISequencerTimerTool.h"

// from LHCbKernel
#include "Kernel/LHCbID.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/StateVector.h"
#include "Event/State.h"

#include "MuonInterfaces/MuonLogHit.h"
#include "MuonInterfaces/MuonLogPad.h"
#include "MuonInterfaces/MuonHit.h"
#include "MuonInterfaces/IMuonHitDecode.h"
#include "MuonInterfaces/IMuonPadRec.h"
#include "MuonInterfaces/IMuonClusterRec.h"
#include "MuonInterfaces/IMuonTrackMomRec.h"
#include "MuonDet/DeMuonDetector.h"
#include "Kernel/MuonTileID.h"
//tools
// local
#include "MuonCombRec.h"
using namespace LHCb;
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : MuonCombRec
//
// 2009-10-07 : Giovanni Passaleva
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MuonCombRec )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonCombRec::MuonCombRec( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : base_class ( type, name , parent )
{
  declareInterface<IMuonTrackRec>(this);
  declareProperty( "MeasureTime"      , m_measureTime = false );
  declareProperty( "CloneKiller"      , m_optCloneKiller = true );
  declareProperty( "StrongCloneKiller", m_optStrongCloneKiller = true );
  declareProperty( "SkipStation"      , m_optSkipStation = -1 );
  declareProperty( "SeedStation"      , m_optSeedStation = 4); // default seed: M5

  declareProperty( "xFOIs"          , m_xFOIs = {{
  //                 R1     R2     R3     R4
                     100.0F,200.0F,300.0F,400.0F,      // M1
                     100.0F,200.0F,300.0F,400.0F,      // M2
                     100.0F,200.0F,300.0F,400.0F,      // M3
                     400.0F,400.0F,400.0F,400.0F }});  // M4

  declareProperty( "yFOIs"          , m_yFOIs = {{
  //                 R1     R2    R3     R4
                     30.0F, 60.0F,100.0F,150.0F,       // M1
                     60.0F,120.0F,180.0F,240.0F,       // M2
                     60.0F,120.0F,240.0F,480.0F,       // M3
                     60.0F,120.0F,240.0F,480.0F, }});  // M4

  declareProperty( "PhysicsTiming"    , m_physicsTiming = true );
  declareProperty( "AssumeCosmics"    , m_assumeCosmics = false );
  declareProperty( "AssumePhysics"    , m_assumePhysics = true );
  declareProperty( "AddXTalk"         , m_XTalk = true);
  declareProperty( "DecodingTool"     , m_decToolName = "MuonHitDecode");
  declareProperty( "PadRecTool"       , m_padToolName = "MuonPadRec");
  declareProperty( "ClusterTool"      , m_clusterToolName = "MuonFakeClustering");
  declareProperty( "TracksOutputLocation", m_trackOutputLoc  = "Rec/Muon/Track" );
}

//=============================================================================
// Destructor
//=============================================================================
MuonCombRec::~MuonCombRec() = default;
//=============================================================================
// clear memory
//=============================================================================
void MuonCombRec::clear() {

  if( !m_hitsDone ) m_trackhits.clear() ; // clear the array of pointers to MuonHit's
  if( !m_sortDone ) for( auto& hitVector : m_sortedHits ) hitVector.clear();
  m_tracks.clear() ;      // clear the array of pointers to MuonTrack's

  m_recDone = m_recOK = false;
}
//=============================================================================
// Incident handling: EndEvent
//=============================================================================
void MuonCombRec::handle ( const Incident& incident ){

  if ( IncidentType::EndEvent == incident.type() ) {
    if ( msgLevel(MSG::VERBOSE) ) verbose() << "End Event: clear everything"<<endmsg;
    m_hitsDone = m_sortDone = false;

    // WARNING ! this resets m_recDone to "false"
    setSkipStation(m_optSkipStation); // reset the station to be skipped
    // this is needed because if M5 is skipped the seed is taken in M4
    setSeedStation(m_optSeedStation);

    clear() ;
  }
}
//=============================================================================
// Finalization
//=============================================================================
StatusCode MuonCombRec::finalize (){

  if ( msgLevel(MSG::DEBUG) ) debug()<<" MuonCombRec::finalize"<<endmsg;
  return GaudiTool::finalize() ;
}
//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonCombRec::initialize() {

  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // -- initialize tools

  // -- retrieve the pointer to the muon detector
  if ( msgLevel(MSG::DEBUG) ) debug() << "Retrieve MuonDet" << endmsg;
  m_muonDetector=getDet<DeMuonDetector>(DeMuonLocation::Default);
  if(!m_muonDetector){
    error()<<"error retrieving Muon Detector geometry "<<endmsg;
    return StatusCode::FAILURE;
  }

  // -- hit decoding
  if ( msgLevel(MSG::DEBUG) ) debug() << "Retrieve hit decoding" << endmsg;
  m_decTool = tool<IMuonHitDecode>(m_decToolName, this);
  if(!m_decTool){
    error()<<"error retrieving the muon decoding tool "<<endmsg;
    return StatusCode::FAILURE;
  }

  // -- create pads
  if ( msgLevel(MSG::DEBUG) ) debug() << "Retrieve pad decoding" << endmsg;
  m_padTool = tool<IMuonPadRec>(m_padToolName, this );
  if(!m_padTool){
    error()<<"error retrieving the muon pad rec. tool "<<endmsg;
    return StatusCode::FAILURE;
  }


  // -- hit clustering algorithm
  if ( msgLevel(MSG::DEBUG) ) debug()<<"Retrieve pad position tool"<<endmsg;
  m_clusterTool = tool<IMuonClusterRec>(m_clusterToolName, this);
  if(!m_clusterTool){
    error()<<"error retrieving the cluster rec. tool "<<endmsg;
    return StatusCode::FAILURE;
  }

  // switch off xtalk code if we're doing real clustering
  //if (m_clusterToolName == "MuonClusterRec") m_XTalk=false;

  // -- calculate the transverse momentum
  m_momentumTool = tool<IMuonTrackMomRec>("MuonTrackMomRec");
  if(!m_momentumTool){
    error()<<"error retrieving the momentum rec. tool "<<endmsg;
    return StatusCode::FAILURE;
  }

  // services

  // incident service
  incSvc()->addListener( this, IncidentType::EndEvent );

  // geometry stuff --->
  m_nStation= m_muonDetector->stations();
  if(m_nStation != 5){
    err()<<"Wrong station number: "<<m_nStation<<endmsg;
    return StatusCode::FAILURE;
  }

  m_nRegion = m_muonDetector->regions()/m_nStation;
  if ( msgLevel(MSG::DEBUG) ) debug() << "Basic geometry: Stations = "
				      << m_nStation << " Regions = " << m_nRegion<<endmsg;
  if(m_nRegion != 4){
    err()<<"Wrong region number: "<<m_nRegion<<endmsg;
    return StatusCode::FAILURE;
  }

  // -- get the z position of stations
  for (int station=0; station<m_nStation; ++station){
    m_zStations[station] = m_muonDetector->getStationZ(station);
    if ( msgLevel(MSG::DEBUG) ) debug() << "Z of station M" << station+1 << ": " << m_zStations[station] << endmsg;
  }
  // <---

  //== Timing information
  if ( m_measureTime ) {
    m_timer = tool<ISequencerTimerTool>( "SequencerTimerTool", this );
    m_timeTotal = m_timer->addTimer("MuonCombRecTime");
    m_timer->increaseIndent();
    m_timeLoadHits = m_timer->addTimer( "Get hits" );
    m_timeBuildLogicalPads = m_timer->addTimer( "Build logical pads" );
    m_timeClusters = m_timer->addTimer( "Create clusters" );
    m_timeMuon  = m_timer->addTimer( "Reconstruct muons" );
    m_timeCloneKilling = m_timer->addTimer( "Clone killing" );
    m_timeFitting = m_timer->addTimer( "Fitting " );
    m_timer->decreaseIndent();
  }

  // local stuff
  if ( msgLevel(MSG::DEBUG) ){
    debug()<<"xFOIs: "<<endmsg;
    for(int is = 0; is < m_nStation-1; is++){
      for(int ir = 0; ir < m_nRegion; ir++){
        int key = is*4+ir;
        debug()<< m_xFOIs[key] <<" ";
      }
      debug()<<endmsg;
    }
    debug()<<"yFOIs: "<<endmsg;
    for(int is = 0; is < m_nStation-1; is++){
      for(int ir = 0; ir < m_nRegion; ir++){
        int key = is*4+ir;
        debug()<< m_yFOIs[key] <<" ";
      }
      debug()<<endmsg;
    }
  }

  // -- initialize the pad size. Hardwired to speed up.
  //             R1         R2         R3         R4
  m_padX ={{  10.1667 ,  20.1667 ,  40.333  ,  80.6667 ,   // M1
               6.41667,  12.75   ,  25.5    ,  51.     ,   // M2
               6.91667,  13.75   ,  27.5    ,  55.     ,   // M3
              29.6667 ,  59.     , 118.     , 236.     ,   // M4
              31.6667 ,  63.     , 126.     , 252       }}; // M5

  //             R1         R2         R3         R4
  m_padY = {{ 24.7586 ,  49.7584 ,  99.7581 , 199.757  ,   // M1
              31.3585 ,  62.9583 , 126.158  , 252.557  ,   // M2
              33.8585 ,  67.     , 136.158  , 272.557  ,   // M3
              36.3585 ,  72.9583 , 146.158  , 292.557  ,   // M4
              38.8585 ,  77.9582 , 156.158  , 312.557   }}; // M5


  if ( msgLevel(MSG::DEBUG) ){
    debug()<< "x pad sizes: " << endmsg;
    for(int is = 0; is < m_nStation; is++){
      for(int ir = 0; ir < m_nRegion; ir++){
        int key = is*4+ir;
        debug()<< m_padX[key] << " ";
      }
      debug() << endmsg;
    }
    debug()<< "y pad sizes: " << endmsg;
    for(int is = 0; is < m_nStation; is++){
      for(int ir = 0; ir < m_nRegion; ir++){
        int key = is*4+ir;
        debug()<< m_padY[key] << " ";
      }
      debug()<<endmsg;
    }
  }

  // -- create a local container for sorted hits.
  // -- Hits are sorted by station/region to easy the track/hit matching algorithm

  if ( msgLevel(MSG::DEBUG)) debug() << "Create the sorted hits container" << endmsg;

  // -- Set tolerances for hit search in region
  m_tolForRegion = {{ 2.0, 4.0, 8.0, 10.0 }};

  // -- clear all object containers
  clear();

  // -- set tool options
  setPhysicsTiming(m_physicsTiming);
  setAssumeCosmics(m_assumeCosmics);
  setAssumePhysics(m_assumePhysics);
  setSeedStation(m_optSeedStation);
  setSkipStation(m_optSkipStation); // WARNING ! this resets m_recDone to "false"

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main reconstruction steering routine
//=============================================================================
StatusCode MuonCombRec::muonTrackFind() {

  // -- Don't do the track reconstruction if already done
  if( m_recDone == true ) {
    return StatusCode::SUCCESS;
  }

  if( m_measureTime)  m_timer->start( m_timeTotal );

  if ( msgLevel(MSG::DEBUG) ){
    debug() << "Running MuonCombRec::muonTrackFind" << endmsg;
    debug()<< "log bits: recdone, hitsdone, sortdone "
	   << m_recDone << " "
	   << m_hitsDone << " "
	   << m_sortDone << " " << endmsg;
  }




  // -- Load hits
  if(m_measureTime) m_timer->start( m_timeLoadHits );
  const std::vector<MuonLogHit*>* myHits = m_decTool->hits();

  if(m_measureTime){
    m_timer->stop( m_timeLoadHits );
    m_timer->start( m_timeBuildLogicalPads );
  }

  // -- Build logical pads
  if(myHits) {
    if ( msgLevel(MSG::DEBUG) ) debug() << "Loaded " << myHits->size() << " hits" << endmsg;
    StatusCode sc = m_padTool->buildLogicalPads(myHits);
    if (!sc) {
      if( m_measureTime)  m_timer->stop( m_timeTotal );
      return sc;
    }
  }else{
    if ( msgLevel(MSG::INFO) ) info() << "Could not load hits" << endmsg;
  }


  if(m_measureTime){
    m_timer->stop( m_timeBuildLogicalPads );
    m_timer->start( m_timeClusters );
  }

  // -- Create pads
  const std::vector<MuonLogPad*>* coords = m_padTool->pads();

  // -- Call clustering algorithm
  if( !m_hitsDone ){
    m_trackhits = *(m_clusterTool->clusters(coords));
    m_hitsDone = true;
  }

  if(m_measureTime){
    m_timer->stop( m_timeClusters );
    m_timer->start( m_timeMuon );
  }

  // -- Clear the containers
  clear();

  // -- At this stage the minimal requirements (hits and pads) are present
  m_recDone = true;

  if( !m_sortDone ){
    // -- Fill a temporary array of MuonHit* sorted by station/region
    StatusCode sh = sortMuonHits();
    if (sh.isFailure()) {
      err()<<"error in sorting hits"<<endmsg;
      if( m_measureTime)  m_timer->stop( m_timeTotal );
      return StatusCode::FAILURE;
    }
    m_sortDone = true; // sorted MuonHit array successfully filled
  }

  // -- Find the tracks of muon candidates
  StatusCode sm = muonSearch();
  if (sm.isFailure()) {
    err()<<"error in muon reconstruction"<<endmsg;
    if( m_measureTime)  m_timer->stop( m_timeTotal );
    return StatusCode::FAILURE;
  }

  // -- The minimal tasks i.e. filling of hit and track containers are OK
  m_recOK = true;

  if( m_measureTime){
    m_timer->stop( m_timeMuon );
    m_timer->start( m_timeCloneKilling );
  }

  // -- Defines a clone if two traks share pads on M2,M3,M4
  if (m_cloneKiller) {
    StatusCode sd = cloneKiller();
    if (sd.isFailure()) {
      err()<<"error detectClone"<<endmsg;
      if( m_measureTime)  m_timer->stop( m_timeTotal );
      return StatusCode::FAILURE;
    }
  }

  // -- Defines a clone if two traks share pads on M2,M3
  if (m_strongCloneKiller){
    StatusCode sk = strongCloneKiller();
    if (sk.isFailure()) {
      err()<<"error in clone killer"<<endmsg;
      if( m_measureTime)  m_timer->stop( m_timeTotal );
      return StatusCode::FAILURE;
    }
  }

  // -- Start track fitting
  if( m_measureTime){
    m_timer->stop( m_timeCloneKilling );
    m_timer->start( m_timeFitting );
  }

  StatusCode scf = trackFit();
  if(!scf) {
    warning()<<"WARNING: problem in track fitting"<<endmsg;
  }


  if( m_measureTime){
    m_timer->stop( m_timeFitting );
    m_timer->stop( m_timeTotal );
  }

  return StatusCode::SUCCESS;
}

//========================================================================
// track reconstruction
//========================================================================
StatusCode MuonCombRec::muonSearch() {

  // -- Local array of matched hits
  std::array< std::vector<MuonHit*>,5 > candidates;

  // -- Local array of Seed Station seeds
  std::vector<MuonHit*> SSseeds;

  // -- Local array x-talk hits
  std::vector<MuonHit*> xtHits;

  int SS = m_seedStation; // SS = Seed Station

  if ( msgLevel(MSG::DEBUG) ) debug() << "Seed Station is: " << SS << endmsg;

  // -- Loop over SS hits and copy them in a local array with SS seeds
  for ( int region = 0; region < m_nRegion; ++region){
    int keySS = SS*4+region;
    SSseeds.insert( SSseeds.end(), m_sortedHits[keySS].begin(), m_sortedHits[keySS].end());
  }

  // -- Main loop over all SS seeds. Merge xtalk hits in single seeds
  for( auto isSS = SSseeds.begin(); isSS != SSseeds.end(); isSS++){
    MuonHit* seedHit = *isSS;

    // -- A new seed is a new SS candidate
    candidates[SS].clear();
    candidates[SS].push_back(seedHit);

    if ( msgLevel(MSG::DEBUG) ) debug() << "Seed : " << seedHit->station() << " "
					<< seedHit->region() << " "
					<< seedHit->X() << " "
					<< seedHit->Y() << " "
					<< seedHit->Z() << endmsg;

    double SSSeedX = seedHit->X();
    double SSSeedY = seedHit->Y();
    int regionSS = seedHit->region();

    if( m_XTalk ) {

      // -- Look for xtalk hits around the seed and merge them if required
      // -- Avoid double counting, skipping hits already merged to a previous seed

      auto used = std::find_if(xtHits.begin(),xtHits.end(),
                                     [&](const MuonHit* h) {
                      return h->tile() == seedHit->tile();
      });
      if(used!=xtHits.end()) {
          if ( msgLevel(MSG::DEBUG) ) debug() << " Hit already used for another seed "
                                              << (*used)->station() << " "
                                              << (*used)->region() << " "
                                              << (*used)->X() << " "
                                              << (*used)->Y() << " "
                                              << (*used)->Z() << " alreadyUsed= " << true << endmsg;
          continue;
      }

      // -- Merge the x-talk hits if any
      if ( msgLevel(MSG::DEBUG) ) debug() << "Now merge the x-talk hits to the current seed" << endmsg;

      float SSx = seedHit->x();
      float SSy = seedHit->y();
      int n = 1;

      // -- look for x-talk hits
      std::vector<MuonHit*> seedXTHits; // array of xt hits for the current seed

      for( auto itSSn = isSS+1; itSSn != SSseeds.end(); itSSn++){

        MuonHit* seedHit2 = *itSSn;

        int istam = seedHit->station();
        int iregm = seedHit->region();
        int keym = istam*4+iregm;

        int istan = seedHit2->station();
        int iregn = seedHit2->region();
        int keyn = istan*4+iregn;

        if( fabs(seedHit->x()-seedHit2->x())<
            0.55*(m_padX[keym]+m_padX[keyn])&&
            fabs(seedHit->y()-seedHit2->y())<
            0.55*(m_padY[keym]+m_padY[keyn])){

          SSx += (float) (seedHit2->x());
          SSy += (float) (seedHit2->y());

          ++n;
          xtHits.push_back(seedHit2);
          seedXTHits.push_back(seedHit2);
          candidates[SS].push_back(seedHit2);
        }
      }

      seedXTHits.push_back(seedHit);

      // -- debug statements
      if ( msgLevel(MSG::DEBUG) ){
        debug() << "Accumulated x-talk points for this seed: -->" << endmsg;
        std::vector<MuonHit*>::iterator sxtit;

        for( MuonHit* hit : seedXTHits ){
          debug() << hit->station() << " " << hit->region()
                  << " " << hit->X()
                  << " " << hit->Y()
                  << " " << hit->Z() <<endmsg;
        }
        debug() << "<--" << endmsg;
      }

      // -- SSSeedx,SSseedy in SS are the center of mass of the various xtalk
      // -- hits if any.
      // -- Now find x,y extrapolated to M4 (pointing to IP)

      SSSeedX =  SSx/n;
      SSSeedY =  SSy/n;

      if ( msgLevel(MSG::DEBUG) ) debug() << "original seed x: " << seedHit->x()
                                          << " average x: " << SSSeedX << endmsg;
      if ( msgLevel(MSG::DEBUG) ) debug() << "original seed y: " << seedHit->y()
                                          << " average y: " << SSSeedY << endmsg;

      // -- Here defines in which region lies the average x in case of xtalk cluster
      // -- by construction, the region is that of the hit closer to the hit cm
      //double distance = 0;
      int kSS = SS*4; // pointer to SS pad size values
      double sumPadX = m_padX[kSS+R1]+m_padX[kSS+R2]+m_padX[kSS+R3]+m_padX[kSS+R4];
      double sumPadY = m_padY[kSS+R1]+m_padY[kSS+R2]+m_padY[kSS+R3]+m_padY[kSS+R4];

      double mindistance = sqrt(sumPadX*sumPadX+sumPadY*sumPadY);
      regionSS = seedHit->region();

      for( MuonHit* sxtHit :  seedXTHits){
        double distance = sqrt( ( SSSeedX-sxtHit->x() )*( SSSeedX-sxtHit->x() )+
                                ( SSSeedY-sxtHit->y() )*( SSSeedY-sxtHit->y() ));
        if(distance<mindistance){
          mindistance=distance;
          regionSS=sxtHit->region();
        }
      }

    }

    if ( msgLevel(MSG::DEBUG) ) debug() << " +++++++++++++++++++ new seed " << endmsg;
    if ( msgLevel(MSG::DEBUG) ) debug() << "seed " << SSSeedX << " " << SSSeedY << " REGION " << regionSS << endmsg;


    std::vector<MuonHit*> bestCandidate(5); // best candidates in M1,..,M4
    std::array<int,5> seedRegion; // seed region
    std::array<double,5> bcX; // best candidates x
    std::array<double,5> bcY; // best candidates y

    // -- special settings for SS -->
    bestCandidate[SS] = candidates[SS].at(0);
    // -- update the x,y coordinates of the SS seed to the cm of the hits
    bcX[SS]=SSSeedX;
    bcY[SS]=SSSeedY;
    seedRegion[SS]=regionSS;
    // <--

    // -- Now extrapolate to the next station and start the matching loop

    int nextStation = SS-1;
    if(nextStation == m_skipStation) nextStation = SS-2;

    double x = bcX[SS] * m_zStations[nextStation]/m_zStations[SS];
    double y = bcY[SS] * m_zStations[nextStation]/m_zStations[SS];

    bool matchingFound = true;

    for(int is = nextStation; is >= 0; --is){

      if(is == m_skipStation) continue; // skip a station

      int prevStat = is+1;
      int nextStat = is-1;
      if(prevStat == m_skipStation) prevStat = is+2;

      if ( msgLevel(MSG::DEBUG) ) debug() << "Search a matching in station: M" << is+1 << " prev station: M"
                                          << prevStat+1 << " next station: M" << nextStat+1 << " skip station: M"
                                          << m_skipStation+1 << endmsg;
      candidates[is].clear();

      // -- Call the method that finds matching hits in the other stations
      StatusCode scFindMatch = findMatching(x,y, is, seedRegion[prevStat], candidates[is]);

      if (scFindMatch.isFailure()) {
        err()<<"error in match finding procedure"<<endmsg;
        return StatusCode::FAILURE;
      }

      if(candidates[is].empty()) {
        if ( msgLevel(MSG::DEBUG) ) debug() << "No matching for this seed. Go to next one" << endmsg;
        matchingFound = false;
        break;
      }

      if(!m_XTalk && candidates[is].size() > 1) {
        if ( msgLevel(MSG::DEBUG) ) debug() << "Matching with multiple hit. Go to next one" << endmsg;
        matchingFound = false;
        break;
      }

      bestCandidate[is] = candidates[is].at(0);
      bcX[is] = bestCandidate[is]->x();
      bcY[is] = bestCandidate[is]->y();

      if ( msgLevel(MSG::DEBUG) ) debug() << "M" << is+1 << " extrapolation  x = " << x << " y = " << y
                                          << " BCx = "<< bestCandidate[is]->x()
                                          << " BCy = "<< bestCandidate[is]->y() <<endmsg;

      // -- extrapolate the best candidate to the next station.
      // -- extrapolation is along the previous pair of hits in x and towards
      // -- the IP in y
      if(nextStat == M1 && nextStat == m_skipStation) break;
      if(nextStat == m_skipStation) {
        nextStat = is-2;
      }
      if( nextStat < 0 ) break; // no need to extrapolate beyond M1

      if ( msgLevel(MSG::DEBUG) ) debug() << "Extrapolate station: M" << is+1 << " from station: M"
                                          << prevStat+1 << " to station: M" << nextStat+1 << " skip station: M"
                                          << m_skipStation+1 << endmsg;

      double zslope = (m_zStations[nextStat]-m_zStations[is])/(m_zStations[prevStat]-m_zStations[is]);

      if(is == M3){
        if(fabs(m_momentumTool->getBdl()) > 0.1){
          x = zslope * ( bcX[prevStat]-bcX[is] ) + bcX[is];
        } else {
          x = bcX[is] * m_zStations[nextStat]/m_zStations[is];
        }
      }else{
        x = zslope * ( bcX[prevStat]-bcX[is] ) + bcX[is];
      }
      y = bcY[is] * m_zStations[nextStat]/m_zStations[is];
      seedRegion[is]=bestCandidate[is]->region();
    }// -- end of matching loop

    if( !matchingFound ) continue; // no matching in at least 1 station, go to the next seed

    std::unique_ptr<MuonTrack> muonTrack{ new MuonTrack() };
    for( std::vector<MuonHit*> candidate : candidates){
      for( MuonHit* hit : candidate){
        muonTrack->insert( hit->hitID(), hit );
      }
    }
    // -- Store the best candidate array
    muonTrack->setBestCandidate(bestCandidate);
    m_tracks.push_back(std::move(muonTrack));

    if ( msgLevel(MSG::DEBUG) ) debug() << "++ This seed has been processed, go to the next one ++" << endmsg;

  }

  if ( msgLevel(MSG::DEBUG) ) debug() << "Number of muon tracks: " << m_tracks.size() << endmsg;

  counter("#MuonTracks") += m_tracks.size();

  return StatusCode::SUCCESS;

}

//========================================================================
// match hits with the track extrapolation
//========================================================================
StatusCode MuonCombRec::findMatching( const double x, const double y,
                                      const int station, const int regionBefore,
                                      std::vector<MuonHit*> &candidates ){

  double toll = m_tolForRegion[regionBefore];

  double deltaYmin = 9999.;
  double deltaXmin = 9999.;
  MuonHit* candidate = nullptr;

  if ( msgLevel(MSG::DEBUG) ) debug() << "Entering hit matching routine for station M" << station+1
                                      << "; seed was in region: " << regionBefore << " Extrapolation is at: "
                                      << x << ", " << y << endmsg;

  // -- Loop over the regions of the station considered
  int keyBefore = station*4+regionBefore;

  for(int region = 0; region<4 ; ++region){ //loop over regions
    int key = station*4+region;

    // -- Loop over the hits in this region
    for( MuonHit* hit : m_sortedHits[key]){
      // -- Get the distance of the hit from the extrapolation point
      double deltaX = fabs( x - hit->x() );
      double deltaY = fabs( y - hit->y() );

      if ( msgLevel(MSG::DEBUG) ) debug() << "This hit is at: " << hit->x() << ", " << hit->y()
                                          << " dx, dy: " << deltaX << ", " << deltaY << endmsg;

      // -- Check if the hit is within the FOI;
      // -- in any case a xFOI>1000 is not considered
      if( deltaY < m_yFOIs[keyBefore]&&
          deltaX < m_xFOIs[keyBefore]&&
          deltaX < 1000 ){
        if( deltaY < (deltaYmin - toll) ){
          deltaXmin = deltaX;
          deltaYmin = deltaY;
          candidate = hit;
        }else if( deltaY < deltaYmin + toll ){
          if( deltaX < (deltaXmin - toll) ){
            deltaXmin = deltaX;
            deltaYmin = deltaY;
            candidate = hit;
          }else if( fabs( deltaXmin - deltaX )<0.1 ){
            deltaXmin = deltaX;
            deltaYmin = deltaY;
            candidate = hit;
          }
        }
      }
    } // end of loop over hits
  } // end of loop over regions


  if(candidate != nullptr){
    if ( msgLevel(MSG::DEBUG) ) debug() << "Best candidate is at: " << candidate->x() << ", "
                                        << candidate->y() << endmsg;

    // -- Now merge the x-talk hits if any
    candidates.push_back(candidate);

    for(int region=0;region<4;region++){ //loop over regions
      int key = station*4+region;
      // -- Loop over the hits in this region
      for( MuonHit* hit : m_sortedHits[key]){

        double deltaX = fabs(candidate->x()-hit->x());
        double deltaY = fabs(candidate->y()-hit->y());

        if(deltaX<2&&deltaY<2) continue; // same pad...;-)

        int csta = candidate->station();
        int creg = candidate->region();
        int ckey = csta*4+creg;

        int ista = hit->station();
        int ireg = hit->region();
        int ikey = ista*4+ireg;

        if(deltaX < ( (m_padX[ckey]+m_padX[ikey])/2. )*1.1 &&
           deltaY < ( (m_padY[ckey]+m_padY[ikey])/2. )*1.1 ){
          candidates.push_back(hit);
        }
      }
    }
  } else {
    if ( msgLevel(MSG::DEBUG) ) debug() << "No matching found for this hit !" << endmsg;
  }
  return StatusCode::SUCCESS;
}
//========================================================================
// tag clone tracks if M2, M3, M4 hits are shared
//========================================================================
StatusCode MuonCombRec::cloneKiller()
{
  for(auto it1 = m_tracks.begin(); it1 != m_tracks.end(); ++it1){ // 1st loop over tracks
    std::array<bool,5> sameM={false,false,false,false,false};
    auto hitsT1 = (*it1)->getHits(); // get hits of 1st track
    for(auto it2 = it1+1; it2 != m_tracks.end(); ++it2){ // 2nd loop over tracks
      auto hitsT2 = (*it2)->getHits(); // get hits of 2nd track

      for(const auto& hitT1 : hitsT1) { // loop over hits of 1st trk

        int st1 = hitT1->station();
        if(st1 != M2 && st1 != M3 && st1 != M4) continue;//look only in M2,M3,M4

        for(const auto& hitT2 : hitsT2){ // loop over hits of 2nd trk

          if(hitT1->tile().key() == hitT2->tile().key() ) { // compare the hits
            sameM[st1] = true; // count common hits in station st1
          }
        }
      }

      if(sameM[M2]&&sameM[M3]&&sameM[M4]){

        double zslope = (m_zStations[M5]-m_zStations[M4])/
                        (m_zStations[M3]-m_zStations[M4]);


        double x_extra5=zslope*((*it1)->bestCandidate()[M3]->x() -
                                (*it1)->bestCandidate()[M4]->x()) +
                                (*it1)->bestCandidate()[M4]->x();
        double y_extra5=(*it1)->bestCandidate()[M4]->y()*m_zStations[M5]/m_zStations[M4];

        double deltaX1 = (*it1)->bestCandidate()[M5]->x() - x_extra5;
        double deltaY1 = (*it1)->bestCandidate()[M5]->y() - y_extra5;
        double deltaX2 = (*it2)->bestCandidate()[M5]->x() - x_extra5;
        double deltaY2 = (*it2)->bestCandidate()[M5]->y() - y_extra5;

        double dist1=std::hypot(deltaX1,deltaY1);
        double dist2=std::hypot(deltaX2,deltaY2);

        if(dist1>dist2){
          (*it1)->setClone();
          debug()<< "First track is a clone"<<endmsg;
        }else{
          (*it2)->setClone();
          debug()<< "Second track is a clone"<<endmsg;
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

//========================================================================
// tag clone tracks if M2, M3 hits are shared
//========================================================================

StatusCode MuonCombRec::strongCloneKiller()
{
  for(auto it1 = m_tracks.begin(); it1 != m_tracks.end(); it1++){ // 1stloop over tracks
    std::array<bool,5> sameM={false,false,false,false,false};
    auto hitsT1 = (*it1)->getHits(); // get hits of 1st track

    for(auto it2 = it1+1; it2 != m_tracks.end(); it2++){ // 2nd loop over tracks
      auto hitsT2 = (*it2)->getHits(); // get hits of 2nd track

      for(const auto& hitT1 : hitsT1){ // loop over hits of 1st trk

        int st1 = hitT1->station();
        if(st1 != M2 && st1 != M3) continue; // look only in M2,M3

        for(const auto& hitT2 : hitsT2){ // loop over hits of 2nd trk

          if(hitT1->tile().key() == hitT2->tile().key() ) { // compare the hits
            sameM[st1] = true; // count common hits in station st1
          }
        }
      }

      if(sameM[M2]&&sameM[M3]){

        double zslope = (m_zStations[M5]-m_zStations[M3])/
                        (m_zStations[M2]-m_zStations[M3]);

        double x_extra5=zslope*((*it1)->bestCandidate()[M2]->x() -
                                (*it1)->bestCandidate()[M3]->x()) +
                                (*it1)->bestCandidate()[M3]->x();
        double y_extra5=(*it1)->bestCandidate()[M3]->y()*m_zStations[M5]/m_zStations[M3];

        double deltaX1 = (*it1)->bestCandidate()[M5]->x() - x_extra5;
        double deltaY1 = (*it1)->bestCandidate()[M5]->y() - y_extra5;
        double deltaX2 = (*it2)->bestCandidate()[M5]->x() - x_extra5;
        double deltaY2 = (*it2)->bestCandidate()[M5]->y() - y_extra5;

        double dist1=std::hypot(deltaX1,deltaY1);
        double dist2=std::hypot(deltaX2,deltaY2);

        if(dist1>dist2){
          (*it1)->setClone();
          debug()<< "First track is a clone"<<endmsg;
        }else{
          (*it2)->setClone();
          debug()<< "Second track is a clone"<<endmsg;
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}
//========================================================================
// create a temporary array of hits sorted by station/region
//========================================================================
StatusCode MuonCombRec::sortMuonHits(){

  // -- Fill the sorted hit array for later track reconstruction
  for( MuonHit* hit : m_trackhits ){
    int station = hit->station();
    int region  = hit->region();
    int key = station*4+region;
    m_sortedHits[key].push_back(hit);
  }

  if ( msgLevel(MSG::DEBUG) ){
    for( const auto& hitVec : m_sortedHits){
      for( const MuonHit* hit : hitVec ){
        debug() << "sorted hit stored station: " << hit->station()
                <<" sorted hit stored  region: "<< hit->region()
                <<" x,y,z: "
                << hit->X() << " "
                << hit->Y() << " "
                << hit->Z() << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}
//========================================================================
// create a temporary array of hits sorted by station/region
//========================================================================
StatusCode MuonCombRec::copyToLHCbTracks(){

  if (exist<LHCb::Tracks>(m_trackOutputLoc)) {
    //already called with this output location, exit
    return StatusCode::SUCCESS;
  }

  std::unique_ptr<Tracks> otracks{ new Tracks() };
  for ( const auto& t : *tracks() ) {
    /// New track
    std::unique_ptr<Track> track{ new Track() };
    m_momentumTool->recMomentum( t, track.get() );
    otracks->insert( track.release() );
  }
  debug()<< " copying container to TES"<<endmsg;
  put( otracks.release(), m_trackOutputLoc );

  return StatusCode::SUCCESS;
}


//=============================================================================
//  track fitting
//=============================================================================
StatusCode MuonCombRec::trackFit(){

  for( auto& track : m_tracks ){
    StatusCode tsc = track->linFit();
    if(!tsc){
      err() << "Error fitting track" << endmsg;
      return StatusCode::FAILURE ;
    }
    if ( msgLevel(MSG::DEBUG) ){
      debug() << "track fitted" << endmsg;
      debug() << "sx : "<<track->sx() << " bx : "<< track->bx() << endmsg;
      debug() << "sy : "<<track->sy() << " by : "<< track->by() << endmsg;
      debug() << "esx : "<<track->errsx() << " ebx : " << track->errbx() << endmsg;
      debug() << "esy : "<< track->errsy() << " eby : " << track->errby() << endmsg;
    }
  }// end loop on tracks
  return StatusCode::SUCCESS;
}
