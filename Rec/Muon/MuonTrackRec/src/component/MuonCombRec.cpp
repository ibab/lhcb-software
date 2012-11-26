// Include files 
#include <fstream>

#ifdef _WIN32
#pragma warning ( disable : 4244 ) // conversion double to float in boost
#endif

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
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
#include "boost/assign/list_of.hpp"
//tools
// local
#include "MuonCombRec.h"
using namespace LHCb; 
using namespace Gaudi::Units;
using namespace boost::assign;

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
  : GaudiTool ( type, name , parent ),
    m_muonDetector(NULL), m_hitsDone(0), m_sortDone(0)
{
  declareInterface<IMuonTrackRec>(this);
  declareProperty( "MeasureTime"      , m_measureTime = true );
  declareProperty( "CloneKiller"      , m_optCloneKiller = true );
  declareProperty( "StrongCloneKiller", m_optStrongCloneKiller = true );
  declareProperty( "SkipStation"      , m_optSkipStation = -1 );
  declareProperty( "SeedStation"      , m_optSeedStation = 4); // default seed: M5

  std::vector<float> tmp1 = list_of
  //                 R1     R2    R3    R4
                   (100.0F)(200.0F)(300.0F)(400.0F)   // M1
                   (100.0F)(200.0F)(300.0F)(400.0F)   // M2
                   (100.0F)(200.0F)(300.0F)(400.0F)   // M3
                   (400.0F)(400.0F)(400.0F)(400.0F);  // M4
  declareProperty( "xFOIs"          , m_xFOIs = tmp1); // M4

  std::vector<float> tmp2 = list_of
  //                 R1     R2    R3    R4
                   ( 30.0F)( 60.0F)(100.0F)(150.0F)   // M1
                   ( 60.0F)(120.0F)(180.0F)(240.0F)   // M2
                   ( 60.0F)(120.0F)(240.0F)(480.0F)   // M3
                   ( 60.0F)(120.0F)(240.0F)(480.0F);  // M4
  declareProperty( "yFOIs"          , m_yFOIs = tmp2);
  
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

MuonCombRec::~MuonCombRec() {
  clear();
} 

//=============================================================================
// clear memory
//=============================================================================

void MuonCombRec::clear() {
  
  std::vector<MuonHit*>::iterator ih;
  std::vector<MuonTrack*>::iterator it;
  std::vector< std::vector<MuonHit*>* >::iterator ish;

  if( !m_hitsDone )
    m_trackhits.clear() ; // clear the array of pointers to MuonHit's

  for(it=m_tracks.begin() ; it !=m_tracks.end() ; it++)
    delete (*it);         // delete all the allocated MuonTrack's
  m_tracks.clear() ;      // clear the array of pointers to MuonTrack's

  if(!m_sortDone){
    for(ish=m_sortedHits.begin() ; ish !=m_sortedHits.end() ; ish++)
      (*ish)->clear();    // clear the sortedHits
  }
  
  m_recDone = m_recOK = false;
}

//=============================================================================
// Incident handling: EndEvent
//=============================================================================

void MuonCombRec::handle ( const Incident& incident )
{ 
  if ( IncidentType::EndEvent == incident.type() ) {
    verbose() << "End Event: clear everything"<<endmsg;
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

StatusCode MuonCombRec::finalize ()

{
  debug()<<" MuonCombRec::finalize"<<endmsg;
  return   GaudiTool::finalize() ;
}

//=============================================================================
// Initialization
//=============================================================================

StatusCode MuonCombRec::initialize() { 

  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) return sc;
  
  debug() << "==> Initialize" << endmsg;

  // initialize tools

  //retrieve the pointer to the muon detector 
  debug()<<"Retrieve MuonDet"<<endmsg;
  m_muonDetector=getDet<DeMuonDetector>(DeMuonLocation::Default);
  if(!m_muonDetector){
    error()<<"error retrieving Muon Detector geometry "<<endmsg;
    return StatusCode::FAILURE;
  }
  
  // hit decoding
  debug()<<"Retrieve hit decoding"<<endmsg;
  m_decTool = tool<IMuonHitDecode>(m_decToolName);
  if(!m_decTool){
    error()<<"error retrieving the muon decoding tool "<<endmsg;
    return StatusCode::FAILURE;
  }

  // create pads
  debug()<<"Retrieve pad decoding"<<endmsg;
  m_padTool = tool<IMuonPadRec>(m_padToolName);
  if(!m_padTool){
    error()<<"error retrieving the muon pad rec. tool "<<endmsg;
    return StatusCode::FAILURE;
  }


  // hit clustering algorithm 
  debug()<<"Retrieve pad position tool"<<endmsg;
  m_clusterTool = tool<IMuonClusterRec>(m_clusterToolName);
  if(!m_clusterTool){
    error()<<"error retrieving the cluster rec. tool "<<endmsg;
    return StatusCode::FAILURE;
  }

  // switch off xtalk code if we're doing real clustering
  //if (m_clusterToolName == "MuonClusterRec") m_XTalk=false;

  //calculate the transverse momentum  
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
  debug()<<"Basic geometry: Stations = "
         <<m_nStation<<" Regions = "<<m_nRegion<<endmsg;
  if(m_nRegion != 4){
    err()<<"Wrong region number: "<<m_nRegion<<endmsg;
    return StatusCode::FAILURE;
  }
  
  // get the z position of stations
  for ( int station=0;station<m_nStation;station++){
    m_zStations[station] = (float) (m_muonDetector->getStationZ(station));
    debug()<<"Z of station M"<<station+1<<": "<<m_zStations[station]<<endmsg;
  }
  // <---

  //== Timing information
  if ( m_measureTime ) {
    m_timer = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timer->increaseIndent();
    m_timeLoad = m_timer->addTimer( "Load from Buffer" );
    m_timePad  = m_timer->addTimer( "Make pad from strip" );   
    m_timeMuon  = m_timer->addTimer( "Reconstruct muons" );
    m_timeMuonStore  = m_timer->addTimer( "Store muons" );
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

  // initialize the pad size. Hardwired to speed up.

  //                     R1         R2         R3         R4
  double vx[20]={ 10.1667 ,  20.1667 ,  40.333  ,  80.6667 ,   // M1
                   6.41667,  12.75   ,  25.5    ,  51.     ,   // M2
                   6.91667,  13.75   ,  27.5    ,  55.     ,   // M3
                  29.6667 ,  59.     , 118.     , 236.     ,   // M4
                  31.6667 ,  63.     , 126.     , 252       }; // M5

  //                     R1         R2         R3         R4
  double vy[20]={ 24.7586 ,  49.7584 ,  99.7581 , 199.757  ,   // M1
                  31.3585 ,  62.9583 , 126.158  , 252.557  ,   // M2
                  33.8585 ,  67.     , 136.158  , 272.557  ,   // M3 
                  36.3585 ,  72.9583 , 146.158  , 292.557  ,   // M4
                  38.8585 ,  77.9582 , 156.158  , 312.557   }; // M5
  
  for(int i = 0; i < 20; i++){
    m_pad_x.push_back(vx[i]);
    m_pad_y.push_back(vy[i]);
  }
  if ( msgLevel(MSG::DEBUG) ){
    debug()<<"x pad sizes: "<<endmsg;
    for(int is = 0; is < m_nStation; is++){
      for(int ir = 0; ir < m_nRegion; ir++){
        int key = is*4+ir;
        debug()<< m_pad_x[key] <<" ";
      }
      debug()<<endmsg;
    }
    debug()<<"y pad sizes: "<<endmsg;
    for(int is = 0; is < m_nStation; is++){
      for(int ir = 0; ir < m_nRegion; ir++){
        int key = is*4+ir;
        debug()<< m_pad_y[key] <<" ";
      }
      debug()<<endmsg;
    }
  }
  
  // create a local container for sorted hits. 
  // Hits are sorted by station/region to easy the track/hit matching algorithm

  debug()<<"Create the sorted hits container"<<endmsg;

  for ( int station=0;station<m_nStation;station++){
    for ( int region = 0 ; region < m_nRegion ; region++ ){      
      std::vector<MuonHit*>* newHitArray = new std::vector<MuonHit*>;
      m_sortedHits.push_back(newHitArray);
    }
  }



  // clear all object containers
  clear();

  // set tool options
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
  
  // don't do the track reconstruction if already done
  if( m_recDone == true ) {
    return StatusCode::SUCCESS;
  }

  debug()<<"running MuonCombRec::muonTrackFind"<<endmsg;
  debug()<<"log bits: recdone, hitsdone, sortdone "
         <<m_recDone<<" "
         <<m_hitsDone<<" "
         <<m_sortDone<<" "<<endmsg;
  
  // call decoding and pad reconstruction
  // timing
  m_timer->start( m_timeLoad );

  // load hits
  const std::vector<MuonLogHit*> *myhits = m_decTool->hits();
  if(myhits) {
    StatusCode sc = m_padTool->buildLogicalPads(myhits);
    if (!sc) return sc;
  }

  // timing
  m_timer->start( m_timePad );

  // create pads
  const std::vector<MuonLogPad*>* coords =m_padTool->pads(); 
  
  // call clustering algorithm
  if( !m_hitsDone ){
    m_trackhits = *(m_clusterTool->clusters(coords));
    m_hitsDone = true;
  }

  // clear the containers
  clear();

  // At this stage the minimal requirements (hits and pads) are present
  m_recDone = true;


  if( !m_sortDone ){
    //fill a temporary array of MuonHit* sorted by station/region
    StatusCode sh = sortMuonHits();
    if (sh.isFailure()) {
      err()<<"error in sorting hits"<<endmsg;
      return StatusCode::FAILURE;
    }
    m_sortDone = true; // sorted MuonHit array successfully filled
  }
  
  //find the tracks of muon candidates
  StatusCode sm = muonSearch();
  if (sm.isFailure()) {
    err()<<"error in muon reconstruction"<<endmsg;
    return StatusCode::FAILURE;
  }

  // the minimal tasks i.e. filling of hit and track containers are OK
  m_recOK = true;

  //defines a clone if two traks share pads on M2,M3,M4 
  if (m_cloneKiller) {
    StatusCode sd = cloneKiller();
    if (sd.isFailure()) {
      err()<<"error detectClone"<<endmsg;
      return StatusCode::FAILURE;
    }
  }
  
 //defines a clone if two traks share pads on M2,M3
  if (m_strongCloneKiller){
    StatusCode sk = strongCloneKiller();
    if (sk.isFailure()) {
      err()<<"error in clone killer"<<endmsg;
    return StatusCode::FAILURE;
    }
  }
  // start track fitting
  
  StatusCode scf = trackFit();
  if(!scf) {
    warning()<<"WARNING: problem in track fitting"<<endmsg;
  }

  return StatusCode::SUCCESS;
}

//========================================================================
// track reconstruction 
//========================================================================

StatusCode MuonCombRec::muonSearch() {

  int lookInStation;
  int lookInRegion;
  StatusCode scFindMatch;
  float zslope;

   // local array of matched hits
  std::vector< std::vector<MuonHit*> > candidates(5);

  // local array of Seed Station seeds
  std::vector<MuonHit*> SSseeds;

  // local array x-talk hits
  std::vector<MuonHit*> xt_Hits;

  int SS = m_seedStation; // SS = Seed Station

  debug()<<"Seed Station is: "<<SS<<endmsg;

  // loop over SS hits and copy them in a local array with SS seeds
  for ( int region=0;region<m_nRegion;region++){
    int keySS = SS*4+region;
    std::vector<MuonHit*>::iterator ish = m_sortedHits[keySS]->begin();
    for( ; ish!=m_sortedHits[keySS]->end(); ish++){
      SSseeds.push_back(*ish); // local array of M5 seeds
    }
  }
  
  //main loop over all SS seeds. Merge xtalk hits in single seeds

  std::vector<MuonHit*>::iterator isSS;
  for( isSS = SSseeds.begin(); isSS != SSseeds.end(); isSS++){
    // a new seed is a new SS candidate
    candidates[SS].clear();
    candidates[SS].push_back(*isSS);

    debug()<<"Seed : "<<(*isSS)->station()<<" "
           <<(*isSS)->region()<<" "
           <<(*isSS)->X()<<" "
           <<(*isSS)->Y()<<" "
           <<(*isSS)->Z()<<endmsg;

    double SSSeedX = (*isSS)->X();
    double SSSeedY = (*isSS)->Y();
    int regionSS = (*isSS)->region();
    if( m_XTalk ) {
      
      // look for xtalk hits around the seed and merge them if required
      
      // avoid double counting, skipping hits already merged to a previous seed
      bool alreadyUsed = false;
      std::vector<MuonHit*>::iterator xtit = xt_Hits.begin();
      for( ; xtit!=xt_Hits.end(); xtit++){
        if ((*xtit)->tile()==(*isSS)->tile()) {
          alreadyUsed = true;
          debug() <<" Hit already used for another seed "
                <<(*xtit)->station()<<" "
                  <<(*xtit)->region()<<" "
                  <<(*xtit)->X()<<" "
                  <<(*xtit)->Y()<<" "
                  <<(*xtit)->Z()<<" alreadyUsed= "<<alreadyUsed<<endmsg;
        }
      }
      if(alreadyUsed) continue;
      
      // merge the x-talk hits if any
      debug()<<"Now merge the x-talk hits to the current seed"<<endmsg;
      
      float SSx = (float) ((*isSS)->x());
      float SSy = (float) ((*isSS)->y());
      float n = 1;
      
      // look for x-talk hits
      std::vector<MuonHit*> seedXTHits; // array of xt hits for the current seed
      
      std::vector<MuonHit*>::iterator itSSn;
      for( itSSn = isSS+1; itSSn != SSseeds.end(); itSSn++){
        
        int istam = (*isSS)->station();
        int iregm = (*isSS)->region();
        int keym = istam*4+iregm;
        
        int istan = (*itSSn)->station();
        int iregn = (*itSSn)->region();
        int keyn = istan*4+iregn;
        
        if( fabs((*isSS)->x()-(*itSSn)->x())<
            0.55*(m_pad_x[keym]+m_pad_x[keyn])&& 
            fabs((*isSS)->y()-(*itSSn)->y())<
            0.55*(m_pad_y[keym]+m_pad_y[keyn]))
        {
          SSx += (float) ((*itSSn)->x());
          SSy += (float) ((*itSSn)->y());
          n++;
          xt_Hits.push_back(*itSSn);
          seedXTHits.push_back(*itSSn); 
          candidates[SS].push_back((*itSSn));
        }
      }
      
      seedXTHits.push_back(*isSS);
      
      if ( msgLevel(MSG::DEBUG) ){
        debug()<<"Accumulated x-talk points for this seed: -->"<<endmsg;
        std::vector<MuonHit*>::iterator sxtit;
        for(sxtit = seedXTHits.begin(); sxtit!=seedXTHits.end(); 
            sxtit++){
          debug()<<(*sxtit)->station()<<" "<<(*sxtit)->region()
                 <<" "<<(*sxtit)->X()
                 <<" "<<(*sxtit)->Y()
                 <<" "<<(*sxtit)->Z()<<endmsg;
        }
        debug()<<"<--"<<endmsg;
      }
      
      // SSSeedx,SSseedy in SS are the center of mass of the various xtalk 
      // hits if any.
      // Now find x,y extrapolated to M4 (pointing to IP)
      
      SSSeedX =  SSx/n;
      SSSeedY =  SSy/n;
      debug() <<"original seed x: "<<(*isSS)->x()
              <<" average x: "<<SSSeedX<<endmsg; 
      debug() <<"original seed y: "<<(*isSS)->y()
              <<" average y: "<<SSSeedY<<endmsg;
      
      // here defines in which region lies the average x in case of xtalk cluster
      // by construction, the region is that of the hit closer to the hit cm
      double distance = 0;
      int kSS = SS*4; // pointer to SS pad size values
      double sum_padx = m_pad_x[kSS+R1]+m_pad_x[kSS+R2]+
                        m_pad_x[kSS+R3]+m_pad_x[kSS+R4];
      double sum_pady = m_pad_y[kSS+R1]+m_pad_y[kSS+R2]+
                        m_pad_y[kSS+R3]+m_pad_y[kSS+R4];
      
      double mindistance = sqrt(sum_padx*sum_padx+sum_pady*sum_pady);
      regionSS = (*isSS)->region();
      
      std::vector<MuonHit*>::iterator sxtit;
      for(sxtit = seedXTHits.begin(); sxtit<seedXTHits.end(); sxtit++){
        distance=sqrt( ( SSSeedX-(*sxtit)->x() )*( SSSeedX-(*sxtit)->x() )+
                       ( SSSeedY-(*sxtit)->y() )*( SSSeedY-(*sxtit)->y() ));
        if(distance<mindistance){
          mindistance=distance;
          regionSS=(*sxtit)->region();
        }
      }
    }
    
    debug() <<" +++++++++++++++++++ new seed "<<endmsg;          
    debug() <<"seed "<<SSSeedX<<" "<<SSSeedY<<" REGION "<< regionSS<<endmsg;
    

    std::vector<MuonHit*> bestCandidate(5); // best candidates in M1,..,M4
    std::vector<int> seedRegion(5); // seed region
    std::vector<double> bcX(5); // best candidates x
    std::vector<double> bcY(5); // best candidates y

    // special settings for SS -->
    bestCandidate[SS] = candidates[SS].at(0);

    //update the x,y coordinates of the SS seed to the cm of the hits
    bcX[SS]=SSSeedX; bcY[SS]=SSSeedY;
    seedRegion[SS]=regionSS;
    //<--

    // now extrapolate to the next station and start the matching loop
    
    double x,y;
    int nextStation = SS-1;
    if(nextStation == m_skipStation) nextStation = SS-2;
    
    x = bcX[SS] * m_zStations[nextStation]/m_zStations[SS];
    y = bcY[SS] * m_zStations[nextStation]/m_zStations[SS];

    bool matchingFound = true;
    
    for(int is = nextStation; is >= 0; is--){
      
      if(is == m_skipStation) continue; // skip a station
      
      int prevStat = is+1;
      int nextStat = is-1;
      if(prevStat == m_skipStation) prevStat = is+2;
      
      debug()<<"Search a matching in station: M"<<is+1<<" prev station: M"
             <<prevStat+1<<" next station: M"<<nextStat+1<<" skip station: M"
             <<m_skipStation+1<<endmsg;
      
      candidates[is].clear();
      
      lookInStation = is;
      lookInRegion = seedRegion[prevStat];
      scFindMatch = findMatching(x,y,
                                 lookInStation,
                                 lookInRegion,
                                 candidates[is]);
      if (scFindMatch.isFailure()) {
        err()<<"error in match finding procedure"<<endmsg;
        return StatusCode::FAILURE;
      }
      
      if(candidates[is].size()==0) {
        debug()<<"No matching for this seed. Go to next one"<<endmsg;
        matchingFound = false;
        break;
      }
      if(!m_XTalk && candidates[is].size() > 1) {
        debug()<<"Matching with multiple hit. Go to next one"<<endmsg;
        matchingFound = false;
        break;
      }
      
      bestCandidate[is] = candidates[is].at(0);
      bcX[is]=bestCandidate[is]->x();
      bcY[is]=bestCandidate[is]->y();
      
      debug() << "M"<<is+1<<" extrapolation  x = " << x << " y = "<< y
              << " BCx = "<< bestCandidate[is]->x()
              << " BCy = "<< bestCandidate[is]->y() <<endmsg;
      
      // extrapolate the best candidate to the next station.
      // extrapolation is along the previous pair of hits in x and towards
      // the IP in y

      if(nextStat == M1 && nextStat == m_skipStation) continue;
      if(nextStat == m_skipStation) {
        nextStat = is-2;
      }
      if( nextStat < 0 ) continue; // no need to extrapolate beyond M1
      
      debug()<<"Extrapolate station: M"<<is+1<<" from station: M"
             <<prevStat+1<<" to station: M"<<nextStat+1<<" skip station: M"
             <<m_skipStation+1<<endmsg;

      zslope = (m_zStations[nextStat]-m_zStations[is])/
               (m_zStations[prevStat]-m_zStations[is]);
      
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
    }

    if( !matchingFound ) continue; // no matching in at least 1 station, go to the next seed

    std::vector< std::vector<MuonHit*> >::iterator im;
    MuonTrack* muonTrack= new MuonTrack();
    for(im = candidates.begin(); im != candidates.end(); im++){
      std::vector<MuonHit*>::iterator ih;
      for(ih = im->begin(); ih != im->end(); ih++){
        int id = (*ih)->hitID();
        muonTrack->insert(id, (*ih));
      }
    }
    // store the best candidate array
    muonTrack->setBestCandidate(bestCandidate);
    m_tracks.push_back(muonTrack);

    debug()<<"++ This seed has been processed, go to the next one ++"<<endmsg;
    
  }
  
  debug()<<"number of muon Tracks "<<m_tracks.size()<<endmsg;

  return StatusCode::SUCCESS;
  
}

//========================================================================
// match hits with the track extrapolation
//========================================================================

StatusCode MuonCombRec::findMatching( double x, double y, 
                                      int station,
                                      int regionBefore,
                                      std::vector<MuonHit*> &Vec_candidate )
{
  double toll=0;
  if(regionBefore==0)toll=2;  
  if(regionBefore==1)toll=4;  
  if(regionBefore==2)toll=8;  
  if(regionBefore==3)toll=10;  
  double deltaYmin = 9999.;
  double deltaXmin = 9999.;
  int counter=0;
  MuonHit* candidate = NULL;
  
  //debug
  debug()<<"Entering hit matching routine for station M"<<station+1
         <<"; seed was in region: "<<regionBefore<<" Extrapolation is at: "
         <<x<<", "<<y<<endmsg;

  // loop over the regions of the station considered
  
  int keyBefore = station*4+regionBefore;

  std::vector<MuonHit*>::iterator itP;
  for(int region=0;region<4;region++){ //loop over regions
    int key = station*4+region;

    // loop over the hits in this region
    for(itP=m_sortedHits[key]->begin();itP!=m_sortedHits[key]->end();itP++){

      // get the distance of the hit from the extrapolation point
      double deltaX = fabs( x-(*itP)->x() );
      double deltaY = fabs( y-(*itP)->y() );
      debug()<<"this hit is at: "<<(*itP)->x()<<", "<<(*itP)->y()
             <<" dx, dy: "<<deltaX<<", "<<deltaY<<endmsg;
      
      // check if the hit is within the FOI; 
      // in any case a xFOI>1000 is not considered        
      if( deltaY < m_yFOIs[keyBefore]&&
          deltaX < m_xFOIs[keyBefore]&&
          deltaX < 1000 ){      
        
        if( deltaY < (deltaYmin - toll) ){
          deltaXmin=deltaX;
          deltaYmin=deltaY;
          candidate=*itP;          
          counter++;
        }else if( deltaY < deltaYmin + toll ){
          if( deltaX < (deltaXmin - toll) ){              
            deltaXmin=deltaX;
            deltaYmin=deltaY;
            candidate=*itP;
            counter++;
          }else if( fabs( deltaXmin - deltaX )<0.1 ){
            deltaXmin=deltaX;
            deltaYmin=deltaY;
            candidate=*itP;
            counter++;
          }
        }
      }
    } // end of loop over hits
  } // end of loop over regions
  
  //debug
  if(candidate != NULL)
    debug()<<"Best candidate is at: "<<candidate->x()<<", "
           <<candidate->y()<<endmsg;

  // now merge the x-talk hits if any
  
  if(candidate!=NULL){
    int Pcounter =0;
    Vec_candidate.push_back(candidate);
    
    for(int region=0;region<4;region++){ //loop over regions
      int key = station*4+region;

      // loop over the hits in this region
      for(itP=m_sortedHits[key]->begin();itP!=m_sortedHits[key]->end();itP++){
        
        double deltaX=fabs(candidate->x()-(*itP)->x());
        double deltaY = fabs(candidate->y()-(*itP)->y());

        if(deltaX<2&&deltaY<2) continue; // same pad...;-)

        int csta = candidate->station();
        int creg = candidate->region();
        int ckey = csta*4+creg;
        
        int ista = (*itP)->station();
        int ireg = (*itP)->region();
        int ikey = ista*4+ireg;

        if(deltaX < ( (m_pad_x[ckey]+m_pad_x[ikey])/2. )*1.1 &&
           deltaY < ( (m_pad_y[ckey]+m_pad_y[ikey])/2. )*1.1 ) 
        {
          Pcounter ++;
          Vec_candidate.push_back(*itP);
        }
      }
    }
    
  } else {
    debug()<<"No matching found for this hit !"<<endmsg;
  }
  return StatusCode::SUCCESS; 
}

//========================================================================
// tag clone tracks if M2, M3, M4 hits are shared
//========================================================================

StatusCode MuonCombRec::cloneKiller()
{

  std::vector<MuonTrack*>::iterator it1;
  std::vector<MuonTrack*>::iterator it2;
  std::vector<MuonHit*>::iterator iht1;
  std::vector<MuonHit*>::iterator iht2;

  for(it1 = m_tracks.begin(); it1 != m_tracks.end(); it1++){ // 1stloop over tracks
    bool sameM[5]={0,0,0,0,0};
    std::vector<MuonHit*> hitsT1 = (*it1)->getHits(); // get hits of 1st track

    for(it2 = it1+1; it2 != m_tracks.end(); it2++){ // 2nd loop over tracks
      std::vector<MuonHit*> hitsT2 = (*it2)->getHits(); // get hits of 2nd track
      
      for(iht1 = hitsT1.begin(); iht1 != hitsT1.end(); iht1++){ // loop over hits of 1st trk
        
        int st1 = (*iht1)->station();
        if(st1 != M2 && st1 != M3 && st1 != M4) continue;//look only in M2,M3,M4
        

        for(iht2 = hitsT2.begin(); iht2 != hitsT2.end(); iht2++){ // loop over hits of 2nd trk
        
          if((*iht1)->tile()->key() == (*iht2)->tile()->key() ) { // compare the hits
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

        double dist1=sqrt(deltaX1*deltaX1+deltaY1*deltaY1);
        double dist2=sqrt(deltaX2*deltaX2+deltaY2*deltaY2);
        
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

  std::vector<MuonTrack*>::iterator it1;
  std::vector<MuonTrack*>::iterator it2;
  std::vector<MuonHit*>::iterator iht1;
  std::vector<MuonHit*>::iterator iht2;

  for(it1 = m_tracks.begin(); it1 != m_tracks.end(); it1++){ // 1stloop over tracks
    bool sameM[5]={0,0,0,0,0};
    std::vector<MuonHit*> hitsT1 = (*it1)->getHits(); // get hits of 1st track

    for(it2 = it1+1; it2 != m_tracks.end(); it2++){ // 2nd loop over tracks
      std::vector<MuonHit*> hitsT2 = (*it2)->getHits(); // get hits of 2nd track
      
      for(iht1 = hitsT1.begin(); iht1 != hitsT1.end(); iht1++){ // loop over hits of 1st trk
        
        int st1 = (*iht1)->station();
        if(st1 != M2 && st1 != M3) continue; // look only in M2,M3

        for(iht2 = hitsT2.begin(); iht2 != hitsT2.end(); iht2++){ // loop over hits of 2nd trk
        
          if((*iht1)->tile()->key() == (*iht2)->tile()->key() ) { // compare the hits
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

        double dist1=sqrt(deltaX1*deltaX1+deltaY1*deltaY1);
        double dist2=sqrt(deltaX2*deltaX2+deltaY2*deltaY2);
        
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

StatusCode MuonCombRec::sortMuonHits()
{
  // fill the sorted hit array for later track reconstruction

  std::vector<MuonHit*>::iterator ith=m_trackhits.begin();
  for(; ith != m_trackhits.end(); ith++){
    int station = (*ith)->station();
    int region  = (*ith)->region();
    int key = station*4+region;
    m_sortedHits[key]->push_back(*ith);
  }
  
  if ( msgLevel(MSG::DEBUG) ){
    std::vector< std::vector<MuonHit*>* >::iterator ish;
    for(ish=m_sortedHits.begin() ; ish !=m_sortedHits.end() ; ish++){
      std::vector<MuonHit*>::iterator ih;
      for(ih=(*ish)->begin() ; ih !=(*ish)->end() ; ih++){
        debug()<<"sorted hit stored station: "<<(*ih)->station()
               <<" sorted hit stored  region: "<<(*ih)->region()
               <<" x,y,z: "
               <<(*ih)->X()<<" "
               <<(*ih)->Y()<<" "
               <<(*ih)->Z()<<endmsg;
      }
    }
  }
  
  return StatusCode::SUCCESS;
}


StatusCode MuonCombRec::copyToLHCbTracks()
{
  
  if (exist<LHCb::Tracks>(m_trackOutputLoc)) {
    //already called with this output location, exit
    return StatusCode::SUCCESS;
  }

  typedef std::vector< MuonTrack* > MTracks;
  
  const MTracks* mTracks = tracks();

  Tracks* tracks = new Tracks();

  for ( MTracks::const_iterator t = mTracks->begin(), tEnd = mTracks->end(); t != tEnd; ++t ) {
    /// New track
    Track* track = new Track();
    
    m_momentumTool->recMomentum( (*t), track);
    tracks->insert( track );
  }
  debug()<< " copying container to TES"<<endmsg;
  put( tracks, m_trackOutputLoc );


  return StatusCode::SUCCESS;
}


//=============================================================================
//  track fitting
//=============================================================================
 
StatusCode MuonCombRec::trackFit( )
{
  std::vector< MuonTrack* >::iterator itk;
  
  for(itk = m_tracks.begin(); itk != m_tracks.end(); itk++){
    StatusCode tsc = (*itk)->linFit();
    if(! tsc){
      err()<<"Error fitting track"<<endmsg;
      return StatusCode::FAILURE ;
    }

    if ( msgLevel(MSG::DEBUG) ){
      debug()<<"track fitted"<<endmsg;
      debug()<<"sx : "<<(*itk)->sx()<<" bx : "<<(*itk)->bx()<<endmsg;
      debug()<<"sy : "<<(*itk)->sy()<<" by : "<<(*itk)->by()<<endmsg;
      debug()<<"esx : "<<(*itk)->errsx()<<" ebx : "<< (*itk)->errbx() <<endmsg;
      debug()<<"esy : "<<(*itk)->errsy()<<" eby : "<< (*itk)->errby() <<endmsg;
    }

  }// end loop on tracks

  return StatusCode::SUCCESS;
}
