// Include files 

// local
#include "NShared.h"


//-----------------------------------------------------------------------------
// Implementation file for class : NShared
//
// 2011-03-17 : Xabier Cid Vidal
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( NShared )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NShared::NShared( const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<INShared>(this);
  
  declareProperty("TrackLocation",
                  m_TracksPath = LHCb::TrackLocation::Default);
  
  declareProperty("useTtrack", m_useTtrack = false);
  
  // Destination of MuonPID
  declareProperty("MuonIDLocation",
                  m_MuonPIDsPath = LHCb::MuonPIDLocation::Default);  
}


//from ChargedProtoParticleAddMuonInfo
StatusCode NShared::mapMuonPIDs()
{
  // empty the map
  m_muonMap.clear();

  // Do we have any MuonPID results
  if ( !exist<LHCb::MuonPIDs>(m_MuonPIDsPath) )
  {
    Warning( "No MuonPIDs at '" + m_MuonPIDsPath +
             "' -> ProtoParticles will not be changed.", StatusCode::SUCCESS, 1 ).ignore();
    return StatusCode::FAILURE;
  }

  // yes, so load them
  const LHCb::MuonPIDs * muonpids = get<LHCb::MuonPIDs>( m_MuonPIDsPath );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully loaded " << muonpids->size()
            << " MuonPIDs from " << m_MuonPIDsPath << endmsg;
  }

  // refresh the reverse mapping
  for ( LHCb::MuonPIDs::const_iterator iM = muonpids->begin();
        iM != muonpids->end(); ++iM )
  {
    m_muonMap[ (*iM)->idTrack() ] = *iM;
  }

  return StatusCode::SUCCESS;
}



//from ChargedProtoParticleAddMuonInfo
LHCb::MuonPID* NShared::mupidFromTrack(const LHCb::Track& mother)
{
  LHCb::MuonPID * mymupid = NULL;
  
  // Does this track have a MUON PID result ?
  TrackToMuonPID::const_iterator iM = m_muonMap.find( &mother );
  if ( m_muonMap.end() == iM ) return mymupid;

  // MuonPID object is found
  mymupid = (*iM).second;
  return mymupid;
}



StatusCode NShared::getNShared(const LHCb::Track& mother, int& nshared)
{
 
  //if m_muonmap not loaded, do so
  if (!m_muonMap.size()){
    StatusCode sc=mapMuonPIDs();
    if (sc.isFailure()) return sc;
  }
  

  LHCb::MuonPID* mymupid = mupidFromTrack(mother);
  if (!mymupid) {
          if (msgLevel(MSG::DEBUG) ) debug()<< "No MuonPID found when computing NShared" << endmsg;
          return StatusCode::SUCCESS;
  }

  if (msgLevel(MSG::DEBUG) ) debug()<< "getNShared: MuonPID retrieved " << (*mymupid) << endmsg;

  // if not a muon candidate do nothing
  if (!mymupid->IsMuonLoose() ) {
    nshared=100;
    if (msgLevel(MSG::DEBUG) ) debug()<< "getNShared: Track with IsMuonLoose false" << endmsg;
    return StatusCode::SUCCESS;
  }

  const LHCb::Track* mymutrack  = mymupid->muonTrack();
  if (msgLevel(MSG::DEBUG) ) debug()<< "getNShared: MuonTrack retrieved " << (*mymutrack) << endmsg;
  nshared = 0;
  double dist1 = mymutrack->info(305,-1.);
  if (msgLevel(MSG::DEBUG) ) debug()<< "getNShared: dist1="<<dist1<< endmsg;


  // if distance badly calculated: bad track!
  if (dist1<0){
    nshared=100;
    if (msgLevel(MSG::DEBUG) ) debug()<< "getNShared: Track with wrong distance stored" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  LHCb::Tracks* trTracks = get<LHCb::Tracks>(m_TracksPath);
  LHCb::Tracks::const_iterator iTrack;
  int comp_tracks=0;

  for( iTrack = trTracks->begin() ; iTrack != trTracks->end() ; iTrack++){
    if((*iTrack)->checkFlag(LHCb::Track::Clone)) continue;
    if (mother.checkType(LHCb::Track::Long) && (!(*iTrack)->checkType(LHCb::Track::Long))) continue;
    if (mother.checkType(LHCb::Track::Ttrack) && (!(*iTrack)->checkType(LHCb::Track::Ttrack))) continue;
    if (mother.checkType(LHCb::Track::Downstream) && (!(*iTrack)->checkType(LHCb::Track::Downstream))) continue;
    
    comp_tracks++;
    
    if (msgLevel(MSG::DEBUG) ) {
      debug()<< "getNShared: comp_tracks="<<comp_tracks<<endmsg;
      debug()<< "iTrack="<<(*(*iTrack))<<endmsg;
    }

    // if not muon candidate or same candidate: nothing to compare!
    LHCb::MuonPID* mupid = mupidFromTrack(*(*iTrack));
    if (!mupid) {
      if (msgLevel(MSG::DEBUG) ) debug()<< "No MuonPID found when computing NShared" << endmsg;
      continue;
    }
    
    if (msgLevel(MSG::DEBUG) ) debug()<< "getNShared: mupid="<<(*mupid)<<endmsg;
    if (!mupid->IsMuonLoose() ) continue;
    if (mupid==mymupid) continue;
    
    if (msgLevel(MSG::DEBUG) ) debug()<< "getNShared: comparable track"<<endmsg;
    const LHCb::Track* mutrack  = mupid->muonTrack();    
    
    // see if there are shared hits between the muonIDs
    if (! compareHits( *mymutrack, *mutrack) ) continue;
    
    if (msgLevel(MSG::DEBUG) ) debug()<< "getNShared: tracks share a hit!"<<endmsg;
    double dist2 = mutrack->info(305,-1.);
    // if distance badly calculated: nothing to compare!
    if (dist2<0) continue;

    // the muonID which gets the number of shared hits is the one
    // which has the biggest dist
    if( dist2 < dist1 ) nshared+=1;
  }
  if (msgLevel(MSG::DEBUG) ) debug()  << "nShared=  " <<  nshared << endmsg;
  
  return StatusCode::SUCCESS;
}


void NShared::fillNShared()
{
 
  StatusCode sc=mapMuonPIDs();
  if (sc.isFailure()) return;

  LHCb::Tracks* trTracks = get<LHCb::Tracks>(m_TracksPath);
  LHCb::Tracks::const_iterator iTrack;

  for( iTrack = trTracks->begin() ; iTrack != trTracks->end() ; iTrack++){
    // in the clone killed output we want only
    // unique && (matched || forward || downstream)
    if(!(*iTrack)->checkFlag(LHCb::Track::Clone)  &&
       ((*iTrack)->checkType(LHCb::Track::Long) ||
        ((*iTrack)->checkType(LHCb::Track::Ttrack) && m_useTtrack) ||
        (*iTrack)->checkType(LHCb::Track::Downstream))){
      
      int nshared=-1;
      sc = getNShared((**iTrack),nshared);
      if (sc.isFailure()){
        if (msgLevel(MSG::DEBUG) ) 
          debug()<<"Error getting Nshared for track "<<(**iTrack)<<endmsg;
      }
      else{
        LHCb::MuonPID* pMuid = mupidFromTrack(*(*iTrack));
        pMuid->setNShared(nshared);
      }
      
    }
  }  
}


bool NShared::compareHits(const LHCb::Track& muTrack1,const LHCb::Track& muTrack2)
{
  bool theSame = false;
  std::vector<LHCb::LHCbID>::const_iterator iids1;
  std::vector<LHCb::LHCbID>::const_iterator iids2;
  if (msgLevel(MSG::DEBUG) ) debug()<< "comparing_hits="<<endmsg;
  
  for( iids1 = muTrack1.lhcbIDs().begin() ; iids1 != muTrack1.lhcbIDs().end() ; iids1++ ){
    if (!(*iids1).isMuon()) continue;
    if ((*iids1).muonID().station()==0) continue;
    for( iids2 = muTrack2.lhcbIDs().begin() ; iids2 != muTrack2.lhcbIDs().end() ; iids2++ ){
      if (!(*iids2).isMuon()) continue;
      if ((*iids2).muonID().station()==0) continue;
      if ((*iids1).channelID() ==  (*iids2).channelID()) {  
        theSame = true;
        return theSame;
      }
      
    }  
  }
 return theSame;
}



//=============================================================================
// Destructor
//=============================================================================
NShared::~NShared() {} 

//=============================================================================
