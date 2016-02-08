// Include files 

// from Gaudi
#include "GaudiKernel/IRegistry.h"

#include "Linker/LinkedTo.h"

#include "Event/MCParticle.h"
#include "Event/STCluster.h"
#include "Event/Track.h"
#include "TfKernel/RecoFuncs.h" 
#include "STDet/DeSTDetector.h"

// local
#include "PatDebugTTTruthTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatDebugTTTruthTool
//
// 2007-10-22 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatDebugTTTruthTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatDebugTTTruthTool::PatDebugTTTruthTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTupleTool ( type, name , parent )
{
  declareInterface<IPatDebugTTTool>(this);

}


StatusCode PatDebugTTTruthTool::initialize()
{
  StatusCode sc = GaudiTupleTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::TT);
  m_flags["ReconstructibleAsLong"] = false;//AD 2-1-16
  m_flags["ReconstructibleAsDown"] = false;//AD 2-1-16
  return sc;
}

//=============================================================================
// Destructor
//=============================================================================
PatDebugTTTruthTool::~PatDebugTTTruthTool() {} 
//=========================================================================
//  Print the true TT clusters associated to the specified track
//=========================================================================
void PatDebugTTTruthTool::debugTTClusterOnTrack (  const LHCb::Track* track, 
                                                   const PatTTHits::const_iterator beginCoord,
                                                   const PatTTHits::const_iterator endCoord   ) {
  //== Get the truth on this track
  std::string containerName = track->parent()->registry()->identifier();
  std::string linkerName = "Link/"+containerName;
  if ( "/Event/" == containerName.substr(0,7) ) {
    linkerName = "Link/" + containerName.substr(7);
  }
  if ( exist<LHCb::LinksByKey>( linkerName ) ) {
    LinkedTo<LHCb::MCParticle, LHCb::Track> trLink( evtSvc(), msgSvc(), containerName );
    LinkedTo<LHCb::MCParticle> itLink( evtSvc(), msgSvc(), LHCb::STClusterLocation::TTClusters );

    LHCb::MCParticle* part = trLink.first( track->key() );
    while ( 0 != part ) {
      double momentum = part->momentum().R();
      info() << format( " MC Key %4d PID %6d  P %8.3f GeV tx %9.5f ty %9.5f",
                        part->key(), part->particleID().pid(), momentum / Gaudi::Units::GeV,
                        part->momentum().x()/ part->momentum().z(),
                        part->momentum().y()/ part->momentum().z() ) << endmsg;
      for ( PatTTHits::const_iterator itH = beginCoord; endCoord != itH; ++itH ){
        const PatTTHit* hit = *itH;
        LHCb::STChannelID id = hit->hit()->lhcbID().stID();
        bool found = false;
        for( unsigned int kk = 0; hit->hit()->sthit()->cluster().pseudoSize() > kk; ++kk ) {
          LHCb::MCParticle* clusPart = 0;
          if ( id != LHCb::STChannelID(0)) clusPart = itLink.first( id );
          while ( 0 != clusPart ) {
            if ( clusPart->key() == part->key() ) found = true;
            clusPart = itLink.next();
          }
          id = m_tracker->nextRight(id);
        }
        if ( found ) {
          double xCoord = hit->hit()->x() ;
          info() << "      TT Clus " 
                 << format( "(S%1d,L%1d,R%2d,S%2d,S%3d) x%7.1f High %1d", 
                            id.station(), id.layer(), id.detRegion(), 
                            id.sector(), id.strip(), xCoord, 
                            (*itH)->hit()->sthit()->cluster().highThreshold() ) << endmsg;
        }
      }
      part = trLink.next();
    }
  }
}

//=========================================================================
//  Print the MC keys associated to this cluster
//=========================================================================
void PatDebugTTTruthTool::debugTTCluster( MsgStream& msg, const PatTTHit* hit ) {

  LinkedTo<LHCb::MCParticle> itLink( evtSvc(), msgSvc(), LHCb::STClusterLocation::TTClusters );
  std::string value = "";

  LHCb::STChannelID id = hit->hit()->lhcbID().stID();
  int lastKey = -1;
  for( unsigned int kk = 0; hit->hit()->sthit()->cluster().pseudoSize() > kk; ++kk ) {
    LHCb::MCParticle* part = 0;
    if (id != LHCb::STChannelID(0)) part = itLink.first( id );
    while ( 0 != part ) {
      if ( lastKey != part->key() ) {
        lastKey = part->key();
        msg << " " << lastKey;
      }
      part = itLink.next();
    }
    id = m_tracker->nextRight(id);
  }
}

//=============================================================================
//  Does this hit belong to the MCParticle matched to the track?
//=========================================================================
bool PatDebugTTTruthTool::isTrueHit( const LHCb::Track* track, const PatTTHit* hit){
  
  LinkedTo<LHCb::MCParticle, LHCb::Track> mySeedLink ( evtSvc(), msgSvc(),LHCb::TrackLocation::Seed);
  LinkedTo<LHCb::MCParticle, LHCb::STCluster> myClusterLink ( evtSvc(), msgSvc(), LHCb::STClusterLocation::TTClusters );

  const LHCb::MCParticle* mcSeedPart = mySeedLink.first( track->key() );
  
  if( mcSeedPart == nullptr ) return false;
  
  bool found = false;
  LHCb::MCParticle* mcPart = myClusterLink.first( hit->hit()->lhcbID().stID() );
  while( mcPart != nullptr ){
    if( mcPart == mcSeedPart) found = true;
    mcPart = myClusterLink.next();
  }
      
  return found;
  
}
//=============================================================================
//  Fraction of 'good' (ie matched) hits on this track
//=============================================================================
double PatDebugTTTruthTool::fracGoodHits( const LHCb::Track* track, const PatTTHits& hits){
  
  LinkedTo<LHCb::MCParticle, LHCb::Track> mySeedLink ( evtSvc(), msgSvc(),LHCb::TrackLocation::Seed);
  LinkedTo<LHCb::MCParticle, LHCb::STCluster> myClusterLink ( evtSvc(), msgSvc(), LHCb::STClusterLocation::TTClusters );

  const LHCb::MCParticle* mcSeedPart = mySeedLink.first( track->key() );

  if( mcSeedPart == nullptr ) return 0.0;

  unsigned int nTrueHits = 0;
  unsigned int nBadHits = 0;
  for( PatTTHit* hit : hits ){
    
    bool found = false;
    
    LHCb::MCParticle* mcPart = myClusterLink.first( hit->hit()->lhcbID().stID() );
    while( mcPart != nullptr){
      if( mcPart == mcSeedPart ) found = true;
      mcPart = myClusterLink.next();
    }
    
    if(found){
      ++nTrueHits;
    }else{
      ++nBadHits;
    }
      
  }
  
  info () << "good Hits: " << nTrueHits << " badHits " << nBadHits << " fraction: " 
          <<  (double)nTrueHits / ( (double)nTrueHits + (double)nBadHits ) << endmsg;
  
  return (double)nTrueHits / ( (double)nTrueHits + (double)nBadHits );
  
}
//=============================================================================
//  Is this a 'good' track (according to the TrackAssociator) definition?
//=============================================================================
bool PatDebugTTTruthTool::isTrueTrack( const LHCb::Track* track, const PatTTHits& hits){
  
  LinkedTo<LHCb::MCParticle, LHCb::Track> mySeedLink ( evtSvc(), msgSvc(),LHCb::TrackLocation::Seed);
  LinkedTo<LHCb::MCParticle, LHCb::STCluster> myClusterLink ( evtSvc(), msgSvc(), LHCb::STClusterLocation::TTClusters );

  const LHCb::MCParticle* mcSeedPart = mySeedLink.first( track->key() );

  if( mcSeedPart == nullptr ) return false;

  unsigned int nTrueHits = 0;
  unsigned int nBadHits = 0;
  for(PatTTHit* hit : hits){
    
    bool found = isTrueHit( track, hit);
    
    if(found){
      ++nTrueHits;
    }else{
      ++nBadHits;
    }
  }
  
  const unsigned int nTotHits = nTrueHits + nBadHits;

  return nTrueHits > nTotHits - 2;
  
}
//=============================================================================
//Overload Is this a good track
//=============================================================================
bool PatDebugTTTruthTool::isTrueTrack( const LHCb::Track* track, const Tf::TTStationHitManager<PatTTHit>::HitRange& hits){
  PatTTHits m_hits;
  for(auto hit: hits) m_hits.push_back(hit);
  return isTrueTrack(track,m_hits);
  
}

//=============================================================================
//  Put chi2, number of hits and momentum in a tuple
//=========================================================================
void PatDebugTTTruthTool::chi2Tuple( const double p, const double chi2, const unsigned int nHits){

  Tuple tuple = nTuple( "chi2Tuple", "chi2Tuple" );
  tuple->column("p", p );
  tuple->column("nHits", nHits );
  tuple->column("chi2", chi2 );
  //add the eff vs step from this tool.
  for(auto flag : m_flags){//AD
    tuple->column(flag.first, flag.second);//AD
  }//AD
  tuple->write();
  
}

//AD 2-1-16

void PatDebugTTTruthTool::initializeSteps(std::vector<std::string> steps){
  for(auto step : steps){//AD, loop over steps to do, and initialize to false. This adds to the map m_flags.
    m_flags[step] = false;
  }
  return;
}

void PatDebugTTTruthTool::recordStepInProcess(std::string step,bool result){
  if(!m_flags[step]){
    m_flags[step]|=result;
  }
  //don't change things if we already have the right answer.
  debug()<<"Recorded for step "<<step<<" result"<<result<<endmsg;
}

void PatDebugTTTruthTool::resetflags(){
  for(auto flag : m_flags){//note, this resets also the Reconstructible flags.
    flag.second=false;//I think this is right
  }
  return;
}

void PatDebugTTTruthTool::ForceMCHits(PatTTHits& hits, LHCb::Track* track){
  //modify the input containter to return only the hits which are MC tached
  PatTTHits tmp;
  for(auto hit: hits){
    if(!isTrueHit( track, hit))continue;
    tmp.push_back(hit);
  }
  hits.clear();
  for(auto hit: tmp){hits.push_back(hit);}
  return;
}
