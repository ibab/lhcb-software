// $Id: MuonNNetRec.cpp,v 1.3 2009-05-13 10:59:47 ggiacomo Exp $

#include <list>
#include <fstream>

#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiAlg/ISequencerTimerTool.h"

#include "MuonTrackRec/MuonLogHit.h"
#include "MuonTrackRec/MuonLogPad.h"
#include "MuonTrackRec/MuonHit.h"
#include "MuonTrackRec/IMuonHitDecode.h"
#include "MuonTrackRec/IMuonPadRec.h"
#include "MuonDet/IMuonFastPosTool.h"
#include "MuonDet/DeMuonDetector.h"
#include "Kernel/MuonTileID.h"
#include "MuonNNetRec.h"
using namespace LHCb;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : MuonNNetRec
//
// 2008-04-11 : Giovanni Passaleva 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MuonNNetRec );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonNNetRec::MuonNNetRec( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_muonDetector(NULL)
{
  declareInterface<IMuonTrackRec>(this);

  declareProperty( "PosScaleFactor"   , m_aa    = 10.0 );
  declareProperty( "NegScaleFactorB"  , m_bb    =  1.0 );
  declareProperty( "NegScaleFactorC"  , m_cc    =  1.0 );
  declareProperty( "SlamFactorB"      , m_slamb =  5.0 );
  declareProperty( "SlamFactorC"      , m_slamc =  5.0 );
  declareProperty( "ExponentXZ"       , m_xesp1 = 10.0 );
  declareProperty( "ExponentYZ"       , m_xesp2 = 10.0 );
  declareProperty( "Stimulus"         , m_dd    =  0.0 );
  declareProperty( "Temperature"      , m_temp  =  1.0 );
  declareProperty( "Convergence"      , m_dsum  =  1.0e-5 );
  declareProperty( "NeuronThreshold"  , m_scut  =  0.7 );
  declareProperty( "DoubleKillAngCut" , m_acut  =  0.1 );
  declareProperty( "TrackSpanCut"     , m_span_cut  =  2.0);
  declareProperty( "StationFiringCut" , m_firing_cut  =  2.0);
  declareProperty( "MaxNeurons"       , m_maxNeurons = 3000 );
  declareProperty( "MaxIterations"    , m_maxIterations = 100 );
  declareProperty( "SkipStation"      , m_skipStation = -1 );
  declareProperty( "AllowHoles"       , m_allowHoles = true );
  declareProperty( "PhysicsTiming"    , m_physicsTiming = false );
  declareProperty( "AssumeCosmics"    , m_assumeCosmics = true );
  declareProperty( "AssumePhysics"    , m_assumePhysics = false );
  declareProperty( "OfflineTimeAlignment", m_offlineTimeAlignment = false );
  declareProperty( "TimeResidualFile" , m_timeResidualFile = "none");
  declareProperty( "AddXTalk"         , m_XTalk = false);
  declareProperty( "DecodingTool"     , m_decToolName = "MuonHitDecode");
  declareProperty( "PadRecTool"       , m_padToolName = "MuonPadRec");
  declareProperty( "PosTool"          , m_posToolName = "MuonDetPosTool");
}
//=============================================================================
// Destructor
//=============================================================================
MuonNNetRec::~MuonNNetRec() { 
  if(MuonTrackRec::ResMap) delete MuonTrackRec::ResMap;
  clear();
} 

//=============================================================================
// clear memory
//=============================================================================

void MuonNNetRec::clear() {
  
  std::vector<MuonHit*>::iterator ih;
  std::vector<MuonTrack*>::iterator it;
  std::vector<MuonNeuron*>::iterator in;
  
  for(ih=m_trackhits.begin() ; ih !=m_trackhits.end() ; ih++)
    delete (*ih); // delete all the allocated MuonHit's
  for(it=m_tracks.begin() ; it !=m_tracks.end() ; it++)
    delete (*it); // delete all the allocated MuonTrack's
  for(in=m_allneurons.begin() ; in !=m_allneurons.end() ; in++)
    delete (*in); // delete all the allocated MuonNeuron's

  m_trackhits.clear() ; // clear the array of pointers to MuonHit's
  m_tracks.clear() ;  // clear the array of pointers to MuonTrack's
  m_allneurons.clear();  // clear the array of pointers to all MuonNeuron's
  m_useneurons.clear();  // clear the array of pointers to used MuonNeuron's
  m_recDone = m_recOK = false;
  m_tooManyHits = false;
}

//=============================================================================
// Initialization
//=============================================================================


StatusCode MuonNNetRec::initialize ()
{
  StatusCode sc = GaudiTool::initialize() ;
  
  if (!sc) return sc;
  m_muonDetector = getDet<DeMuonDetector>(DeMuonLocation::Default);
  if(!m_muonDetector){
    err()<<"error retrieving the Muon detector element "<<endreq;
    return StatusCode::FAILURE;
  }
  
  m_decTool = tool<IMuonHitDecode>(m_decToolName);
  if(!m_decTool){
    error()<<"error retrieving the muon decoding tool "<<endreq;
    return StatusCode::FAILURE;
  }

  m_padTool = tool<IMuonPadRec>(m_padToolName);
  if(!m_padTool){
    error()<<"error retrieving the muon pad rec. tool "<<endreq;
    return StatusCode::FAILURE;
  }

  m_posTool = tool<IMuonFastPosTool>(m_posToolName);
  if(!m_posTool) {
    error()<<"error retrieving the muon position tool "<<endreq;
    return StatusCode::FAILURE;
  }

  incSvc()->addListener( this, IncidentType::EndEvent );

  // clear all object containers
  clear();

  // timing
  m_timer = tool<ISequencerTimerTool>( "SequencerTimerTool" );
  m_timer->increaseIndent();
  m_timeinitNet = m_timer->addTimer( "Init network" );
  m_timeconvNet = m_timer->addTimer( "Network convergence" );
  m_timefitTrack= m_timer->addTimer( "Track fitting" );
  m_timer->decreaseIndent();
  // timing

  // set tool options
  setPhysicsTiming(m_physicsTiming);
  setAssumeCosmics(m_assumeCosmics);
  setAssumePhysics(m_assumePhysics);
  if(m_offlineTimeAlignment && m_timeResidualFile != "none"){
    if(!loadTimeRes()){
      err()<<"Time corrections file "<<m_timeResidualFile<<" not found"<<endmsg;
      return StatusCode::FAILURE;
    }
    m_offlineTimeAlignment = true;
  } else {
    m_offlineTimeAlignment = false;
  }
  setOfflineTimeAlignment(m_offlineTimeAlignment);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Incident handling: EndEvent
//=============================================================================

void MuonNNetRec::handle ( const Incident& incident )
{ 
  if ( IncidentType::EndEvent == incident.type() ) {
    verbose() << "End Event: clear everything"<<endmsg;
    clear() ;
  }
}

//=============================================================================
// Finalization
//=============================================================================

StatusCode MuonNNetRec::finalize ()

{
  debug()<<" MuonNNetRec::finalize"<<endmsg;
  return   GaudiTool::finalize() ;
}

//=============================================================================
// Main reconstruction routine
//=============================================================================

StatusCode MuonNNetRec::muonNNetMon(){  

  // call decoding and pad reconstruction
  const std::vector<MuonLogHit*> *myhits = m_decTool->hits();
  if(myhits) {
    StatusCode sc = m_padTool->buildLogicalPads(myhits);
    if (!sc) return sc;
  }
  const std::vector<MuonLogPad*>* coords =m_padTool->pads(); 

  if( m_recDone == true ) {
    return StatusCode::SUCCESS;
  }
  
  // timing
  m_timer->start( m_timeinitNet );
  
  debug()<<"\n running MuonNNetMon"<<endmsg;
  clear();
  m_recDone = true;
  
  // check that of hits is reasonable
  std::vector<MuonLogPad*>::const_iterator iCoord;
  int hit_per_station[5]= {0,0,0,0,0};
  // check on maximum number of hit combinations giving a neuron
  for (iCoord = coords->begin(); iCoord != coords->end();iCoord++ ){
    if ((*iCoord)->truepad()) hit_per_station[(*iCoord)->tile()->station()]++;
  }
  int ncomb=0;
  for (unsigned int i=0; i<5 ;i++) {
    for (unsigned int j=i+1; j<5 && j<(i+3) ;j++) {
      ncomb +=  hit_per_station[i]*hit_per_station[j];
    }
  }
  if(ncomb > m_maxNeurons) {
    m_tooManyHits=true;
    info() << "Too many hits to proceed with cosmic track finding" << endmsg;
    return StatusCode::SUCCESS;
  }
  int nhits=0; // hits per event

  
  // loop over the coordinates and fill the MuonHits  
  for (iCoord = coords->begin(); iCoord != coords->end();iCoord++ ){
    if(! (*iCoord)->truepad() ) continue;
    const LHCb::MuonTileID* tile= (*iCoord)->tile();
    //float m_hitTime = (*iCoord)->time();
    //float m_hitDeltaTime = (*iCoord)->dtime() ;
    
    double x,dx,y,dy,z,dz;

    if ( (m_posTool->calcTilePos(*tile,x,dx,y,dy,z,dz)).isFailure() )
      return StatusCode::FAILURE;

    // counters
    nhits++;
    
    // fill the hit array
    // create a MuonHit object
    MuonHit* muon_Hit = new MuonHit(m_muonDetector, *iCoord, 
                                    x,y,z,m_posTool );
    
    // store a progressive hit number for debugging purposes
    StatusCode sc = muon_Hit->setHitID(nhits);
    if ( sc.isFailure() )  {
      err()<<"Hit ID = 0 is not valid"<<endmsg;
      delete muon_Hit;
      return StatusCode::FAILURE;
    }
    // fill the vector of MuonHits
    m_trackhits.push_back(muon_Hit);
  } // end of loop over coords


  // starts the NNet reconstruction

  // here starts the double loop over hits to build the neurons
  // Neurons are oriented segments with a tail (close to the IP) and a head
  // (far from the IP). Neurons can be connected head-head (tail-tail) 
  //if they enter (exit) in (from) the same hit 
  // 
  // if holes are allowed (default) max neuron length is 2 else 1
  int neuronLength = m_allowHoles ? 2 : 1;
  debug()<<"MAX NEURON LENGTH FOR THIS JOB IS: "<<neuronLength<<endmsg;
  
  std::list< MuonNeuron* > neurons;
  int Nneurons = 0;
  std::vector<MuonHit*>::iterator ihT,ihH; 
  for(ihT = m_trackhits.begin(); ihT != m_trackhits.end() ; ihT++){
    
    // skip a station for efficiency studies
    int stT = (*ihT)->station();
    if (stT == m_skipStation) continue;
    
    int tID = (*ihT)->hitID(); // tail ID
    for(ihH = ihT+1 ;ihH != m_trackhits.end() ; ihH++){
      
      // skip a station for efficiency studies
      int stH = (*ihH)->station();
      if (stH == m_skipStation) continue;
      
      // cut on neuron length in terms of crossed stations
      
      if(((*ihH)->station() - (*ihT)->station()) > neuronLength || 
         ((*ihH)->station() - (*ihT)->station()) == 0) continue;
      
      int sta = (*ihT)->station() + 1;
      int reg = (*ihT)->region()  + 1;
      int hID = (*ihH)->hitID(); // head ID
      
      // here if everything OK. Now build the neurons.
      
      Nneurons++;
      // create the MuonNeuron
      MuonNeuron* Neuron = new MuonNeuron(*(*ihH),*(*ihT),hID,tID,sta,reg);
      // store progressive neuron number for debugging
      Neuron->setNeuronID(Nneurons);
      // container to retrieve neurons from tool
      m_allneurons.push_back(Neuron); // std::vector< MuonNeuron* >
      // fill a neuron list to mainipulate them in the program
      neurons.push_back(Neuron); // this is a std::list<MuonNeuron*>
    }
  }
  
  // the manipulation of neurons is made through the std::list neurons
  
  debug()<< "Created neurons: "<<neurons.size()<<endmsg;
  
  std::list< MuonNeuron* >::iterator nl1 = neurons.begin();
  
  // now calculate the weights
  
  for(; nl1 != neurons.end(); nl1++){
    
    std::list< MuonNeuron* >::iterator nl2 = nl1;
    nl2++;
    
    for(; nl2 != neurons.end(); nl2++){
      
      double ww=0;
      
      // neurons 1 and 2 are tail-head or head-tail
      if((*nl1)->tailHead(*(*nl2)) || (*nl1)->headTail(*(*nl2))) {
        double thxz = (*nl1)->angleXZ(*(*nl2));
        double thyz = (*nl1)->angleYZ(*(*nl2));
        ww = m_aa*(pow((1-sin(thxz)),m_xesp1)*pow((1-sin(thyz)),m_xesp2));
        
        // store neuron 2 pointer in neuron 1 and its weight        
        (*nl1)->setWeight(&(*(*nl2)),ww);
        // store neuron 1 pointer in neuron 2 and its weight        
        (*nl2)->setWeight(&(*(*nl1)),ww);
      }
      // neurons 1 and 2 are head-head
      if((*nl1)->headHead(*(*nl2))) {
        
        ww = -m_slamb*m_bb;
        // store neuron 2 pointer in neuron 1 and its weight        
        (*nl1)->setWeight(&(*(*nl2)),ww);
        // store neuron 1 pointer in neuron 2 and its weight        
        (*nl2)->setWeight(&(*(*nl1)),ww);
      }
      // neurons 1 and 2 are tail-tail
      if((*nl1)->tailTail(*(*nl2))) {
        
        ww = -m_slamc*m_cc;
        // store neuron 2 pointer in neuron 1 and its weight        
        (*nl1)->setWeight(&(*(*nl2)),ww);
        // store neuron 1 pointer in neuron 1 and its weight        
        (*nl2)->setWeight(&(*(*nl1)),ww);
      }
    }
  }
  
  if(m_allowHoles){
    // kill double length neurons if there is a unit length one 
    nl1 = neurons.begin();
    for(; nl1 != neurons.end(); nl1++){
      (*nl1)->killDoubleLength( m_acut );
    }
  }
  
  // clean up weights. only the tail-head and the head-tail with the best 
  // weight are kept
  nl1 = neurons.begin();
  for(; nl1 != neurons.end(); nl1++){
    (*nl1)->cleanupWeights();
  }
  
  // timing
  m_timer->stop( m_timeinitNet );
  // timing
  
  
  //
  // now starts network evolution
  //
  
  // timing
  m_timer->start( m_timeconvNet );
  // timing
  
  int iterations = 0;
  double ssum = 2*m_dsum;
  while( ssum > m_dsum ) {
    iterations++;
    ssum = 0.0;
    /// update  neuron status
    nl1 = neurons.begin();
    for(; nl1 != neurons.end(); nl1++){
      
      std::list< std::pair< MuonNeuron*,double > > wl = (*nl1)->getWeights();
      std::list< std::pair< MuonNeuron*,double > >::iterator iw=wl.begin();
      double sum = 0.0;
      for( ; iw != wl.end(); iw++){
        sum += iw->second * iw->first->status();
      }
      double s = 0.5 * (1.0 + tanh(m_dd/m_temp+sum/m_temp)); // actual status
      double sp = (*nl1)->status(); // previous status
      if(sum == 0) s = 0;
      ssum += fabs(s - sp)/neurons.size();
      (*nl1)->setStatus(s); // update neuron status
    }
    if (iterations == m_maxIterations) break;
  }
  
  if(iterations == m_maxIterations)
    warning() << "maximum number of iterations "<<m_maxIterations
              <<" reached"<<endmsg;
  else {
    debug() << "network convergence after "<<iterations<<" iterations"<<endmsg;
    m_recOK = true;
  }

  // drop OFF neurons
  
  nl1 = neurons.begin();
  int pip=0;
  while( nl1 != neurons.end() ){
    if((*nl1)->status() <= m_scut) {
      nl1 = neurons.erase(nl1);
    } else {
      pip++;
      // store progressive neuron number for debugging
      (*nl1)->setNeuronID(pip);
      // used neurons are stored in this container.
      m_useneurons.push_back(*nl1); //std::vector< MuonNeuron* > 
      nl1++;
    }
  }
  debug()<<"ON neurons after convergence: "<<neurons.size()<<endmsg;
  
  // timing
  m_timer->stop( m_timeconvNet );
  // timing
  
  
  //
  // create the tracks
  //
  
  // timing
  m_timer->start( m_timefitTrack );
  // timing
  
  
  std::list< MuonNeuron* > tmpList;
  nl1 = neurons.begin();
  while( (nl1 != neurons.end()) && ( ! neurons.empty()) ){
    MuonTrack* muonTrack= new MuonTrack();
    std::list< MuonNeuron* > track; // create a new track
    //1st neuron copied in temp list and deleted from primary
    tmpList.push_back(*nl1);
    MuonNeuron* firstNeuron =(*nl1);
    neurons.pop_front();
    // now loop over all other neurons and copy in the tmp list those in contact with 1st
    // also these neurons are removed from the primary list
    
    while( ! tmpList.empty() ){
      
      std::list< MuonNeuron* >::iterator nl2 = neurons.begin();
      
      while( (nl2  != neurons.end()) && ( ! neurons.empty()) ){
        
        if((*nl2)->connectedTo(*firstNeuron)) {
          tmpList.push_back(*nl2);
          nl2 = neurons.erase(nl2);
        } else {
          nl2++;
        }
        
      }
      // the first neuron of tmp is copied into the track and removed from 
      // the tmp list; then we proceed with the 1st element of the tmp list 
      // and look for connected neurons that are in turn added to the tmp list
      
      int hID = ((tmpList.front())->headTailID()).first;
      int tID = ((tmpList.front())->headTailID()).second;
      const MuonHit* head = (tmpList.front())->head(); 
      const MuonHit* tail = (tmpList.front())->tail(); 
      
      
      muonTrack->insert( hID, *head ); // insert the head point
      muonTrack->insert( tID, *tail ); // insert the tail point
      
      track.push_back(tmpList.front());
      tmpList.pop_front();
      if (!tmpList.empty()) firstNeuron = tmpList.front();
    }
    
    // here the track should be filled and we can proceed with the next 
    // remaining neuron
    
    if ( msgLevel(MSG::DEBUG) ){
      debug()<<"the track"<<endmsg;
      std::list< MuonNeuron* >::iterator itk = track.begin();
      for(; itk != track.end(); itk++){
        debug()<<(*itk)->neuronID()<<endmsg;
      }
      
      
      std::vector< MuonHit* > t_hits = muonTrack->getHits();
      std::vector< MuonHit* >::iterator tk = t_hits.begin();
      
      debug()<<"track hits "<<t_hits.size()<<endmsg;
      
      for(; tk != t_hits.end(); tk++){
        debug() <<(*tk)->X()<<" "
                <<(*tk)->Y()<<" "
                <<(*tk)->Z()<<endmsg;
      }
    }
    
    int span = muonTrack->getSpan();
    if ( msgLevel(MSG::DEBUG) )
      debug()<<"track span "<<span<<endmsg;
    
    int firstat = muonTrack->getFiringStations();
    if ( msgLevel(MSG::DEBUG) )
      debug()<<"firing stations "<<firstat<<endmsg;
    
    // fill the track vector only for those tracks where at least 3 stations
    // are firing
    if( span >= m_span_cut && firstat > m_firing_cut) {
      if(m_XTalk) {
	//	info()<< " before "<<muonTrack->getHits().size() <<" hits to the track"<<endmsg;
	
	StatusCode sct = muonTrack->AddXTalk( m_trackhits);
	
	//	info()<< " After "<<muonTrack->getHits().size() <<" hits to the track"<<endmsg;
	
	if(!sct)  warning()<<"WARNING: problem adding XTalk pads"<<endmsg;
      }
      
      m_tracks.push_back(muonTrack);
            
    } else 
      delete muonTrack;
    
    nl1 = neurons.begin();
  }
  
  // here I have the list of good tracks selected by span cut.
  
  debug()<<"selected tracks: "<< m_tracks.size()<<endmsg;
  
  
  // start track fitting
  
  StatusCode scf = trackFit();
  if(!scf) {
    warning()<<"WARNING: problem in track fitting"<<endmsg;
  }
  
  // timing
  m_timer->stop( m_timefitTrack );
  // timing
  
  //  clearNeurons();
  return StatusCode::SUCCESS;
}

//=============================================================================
//  track fitting
//=============================================================================
 
StatusCode MuonNNetRec::trackFit( )
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

//=============================================================================
// load map of time residuals for offline correction
//=============================================================================

bool MuonNNetRec::loadTimeRes() {
  char inbuf[100];
  bool out = true;
  int s=0,r=0,q=0,nx=0,ny=0,v=0;
  float res=0.;
  std::ifstream resList;
  resList.open(m_timeResidualFile.c_str());
  if (resList.good() ) {
    MuonTrackRec::ResMap = new std::map<long int, float>;
    while (! resList.eof()) {
      resList.getline(inbuf,sizeof(inbuf));
      sscanf(inbuf,"M%dR%dQ%d %d %d v%d %f",&s,&r,&q,&nx,&ny,&v,&res);
      long int key = MuonTrackRec::logicalPadKey(q-1, s-1, r-1, nx, ny, v);
      (*MuonTrackRec::ResMap)[key] = res /25. * 16. ; // convert from ns to TDC units
    }
  }
  else {
    out = false;
  }
  return out;
}

