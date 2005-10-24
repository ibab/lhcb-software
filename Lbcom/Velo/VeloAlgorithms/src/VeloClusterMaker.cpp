//-----------------------------------------------------------------------------
// Implementation file for class : VeloClusterMaker
//
// 02/02/2002 : Chris Parkes 
// based on Velo TestBeam version of Mat Charles
// rewritten to use STL rather than root
//-----------------------------------------------------------------------------

// Method for making clusters :
//
//
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from Detector
#include "VeloDet/DeVelo.h" 
// from Event
#include "Event/MCVeloFE.h"
#include "Event/VeloCluster.h"
#include "Event/VeloFullFPGADigit.h"
#include "Kernel/VeloChannelID.h"
// local
#include "VeloClusterMaker.h"
#include "VeloAlgorithms/VeloEventFunctor.h"

// Declaration of the Algorithm Factory
static const AlgFactory<VeloClusterMaker>          Factory ;
const        IAlgFactory& VeloClusterMakerFactory = Factory ; 

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
VeloClusterMaker::VeloClusterMaker( const std::string& name,
                                    ISvcLocator* pSvcLocator) 
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_inputContainer                 ( VeloFullFPGADigitLocation::Default )
  , m_outputContainer                ( VeloClusterLocation::Default )
  , m_defaultSignalToNoiseCut        (3.0F)
  , m_defaultClusterSignalToNoiseCut (3.0F)
  , m_maxClusters                    (10000)   
  , m_inclusionThreshold             (0.1F)
{
  declareProperty( "InputData"           , m_inputContainer  );
  declareProperty( "OutputData"          , m_outputContainer );
  declareProperty( "MaxClusters"         , m_maxClusters );
  declareProperty( "InclusionThreshold"  , m_inclusionThreshold );
  declareProperty( "DefaultSignalToNoiseCut", m_defaultSignalToNoiseCut );
  declareProperty( "DefaultClusterSignalToNoiseCut", 
                   m_defaultClusterSignalToNoiseCut );

}

//=============================================================================
// Destructor
//=============================================================================
VeloClusterMaker::~VeloClusterMaker(){}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode VeloClusterMaker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialise" << endreq;

  // set default signal to noise cuts:
  for (int idet=0; idet<maxVeloSensors; idet++) {
    m_signalToNoiseCut[idet]=m_defaultSignalToNoiseCut;
    m_clusterSignalToNoiseCut[idet]=m_defaultClusterSignalToNoiseCut;
  }

  // get DetectorElement
  m_velo = getDet<DeVelo>( "/dd/Structure/LHCb/Velo" );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Main execution
//=========================================================================
StatusCode VeloClusterMaker::execute() {
  // make clusters from VeloFullFPGADigits
 
  debug() << "==> Execute" << endreq;
  makeClusters();
  debug() << " All clusters made" << endreq;
  StatusCode sc=storeClusters();
  return sc;
}


////////////////////////////////////////////////////////////////////////////////
// Accessors and other simple functions

float VeloClusterMaker::signalToNoiseCut(int detID) const
{
  // get Signal to Noise cut of this detector
  int detIndex=m_velo->sensorIndex(detID);
  return detIndex>=0 ? m_signalToNoiseCut[detIndex] : -999;
}

//  void VeloClusterMaker::setSignalToNoiseCut(int detID, float newSN)
//  {
//    // set Signal to Noise cut of this detector
//    int detIndex=m_velo->sensorArrayIndex(detID);
//    if (detIndex>=0) {m_signalToNoiseCut[detIndex] = newSN;}
//  }

float VeloClusterMaker::clusterSignalToNoiseCut(int detID) const
{
  // set Signal to Noise cut of this detector
  int detIndex=m_velo->sensorIndex(detID);
  return detIndex>=0 ? m_clusterSignalToNoiseCut[detIndex] : -999;
}


//  void VeloClusterMaker::setClusterSignalToNoiseCut(int detID, float newSN)
//  {
//    // Accessor for cluster S/N cut (set)
//    int detIndex=m_velo->sensorArrayIndex(detID);
//    if (detIndex>=0) {m_signalToNoiseCut[detIndex] = newSN;}
//  }
//=========================================================================
//  
//=========================================================================
void VeloClusterMaker::makeClusters(){
  // Creates a new set of clusters for the event.

  bool isDebug   = msgLevel( MSG::DEBUG   );
  bool isVerbose = msgLevel( MSG::VERBOSE );

  if(isDebug) debug() << "====> making clusters of event" << endreq;

  // make a container that will get stored on the TDS of the clusters
  // even if zero clusters made - the empty container must 
  // still be made and stored
  m_clusters = new VeloClusters;
  // get all hits from which the clusters will be made
  m_digits = get<VeloFullFPGADigits>(m_inputContainer);

  std::vector< DeVeloSensor * >::iterator iSens;
  std::vector< DeVeloSensor * > sensors = m_velo->vpSensors();
  for( iSens = sensors.begin() ; iSens != sensors.end() ; iSens++ ){

    // ignore the 4 pileup sensors
    if( (*iSens)->isPileUp() ) { continue; }

    unsigned int detIndex = iSens-sensors.begin();
    // set all channels not used
    m_sensor=(*iSens)->sensorNumber();;
    if(isDebug) debug() << "makeClusters:DetectorNumber " << detIndex 
			<< " Sensor " << m_sensor<< endreq;
    m_channelUsed.clear();
    m_channelUsed.insert( m_channelUsed.begin(), 
                          m_velo->numberStrips(m_sensor), false); 
    // retrieve hits of detector 
    std::pair<VeloFullFPGADigits::iterator,VeloFullFPGADigits::iterator> 
      range=getVeloFullFPGADigitsOfSensor(m_sensor);
    if(isDebug) debug() << "makeClusters:retrieved digits for det " 
			<< detIndex << " size " 
			<< range.second - range.first << endreq;
    
    // sort by increasing ADC value          
    std::sort(range.first, range.second, 
	      VeloEventFunctor::Less_by_adcValue<const VeloFullFPGADigit*>());
    // swap to decreasing ADC
    std::reverse(range.first, range.second);

    // iterate over hits
    
    for (VeloFullFPGADigits::iterator digIt = range.first; 
         digIt!=range.second; digIt++) { 
      // try to make cluster from this hit
      float currentClusterSTN=0;
      VeloFullFPGADigit* currentDigit=*digIt;    
      if(isVerbose) verbose()  <<"makeClusters: sorted by ADC  " 
			       << currentDigit->adcValue(VeloFullFPGADigit::RAW) << " for sensor" 
			       <<  currentDigit->sensor() << endreq;  
      VeloCluster* currentCluster=
	makeClusterFromDigit(currentDigit,currentClusterSTN);
      if(isVerbose) verbose()  << " made cluster from digit " << endreq;
    
      if (NULL!=currentCluster){
	// global cluster S/N cut:
        if (checkCluster(currentCluster,currentClusterSTN)){
          // Cluster is OK! 
          // set Key and Add it to the event.
          VeloChannelID channelID( currentCluster->sensor(), 
                                   currentCluster->strip(0) );
          m_clusters->insert(currentCluster,channelID);  
          if(isVerbose) {
	    verbose()  << "makeClusters:Cluster added S/N " 
		       << currentClusterSTN << " size " 
		       << currentCluster->size() 
		       << " total clusters " << m_clusters->size() << endreq;
	    verbose()  << "currentCluster key " 
		       << currentCluster->key() << " size " 
		       << currentCluster->size() 
		       << " first strip " << currentCluster->strip(0) 
		       << " first ADC " << currentCluster->adcValue(0)
		       << endreq;
	  }
        } else {
	  // Cluster failed its global S/N cut, so unmark channels as being used.
          unmarkCluster(currentCluster);
        }
        if ( m_clusters->size()>= m_maxClusters ) {
          // Too many clusters to add more after this one.
          info() << "Warning: Already have " <<m_maxClusters 
		 << " clusters.  VeloClusterMaker::MakeClusters "
		 << "is stopping clusterization while processing "
		 << "sensor " << m_sensor
		 << " for this event." << endreq;
          return; // **alternate exit point**
        }
      }
    } // loop over Digits
  } // loop over detectors
  
  return;
}


//=========================================================================
//  
//=========================================================================
VeloCluster* 
VeloClusterMaker::makeClusterFromDigit(VeloFullFPGADigit* currentDigit , 
				       float& currentClusterSTN){

  bool isVerbose = msgLevel( MSG::VERBOSE );

  // make new cluster 
  // - if we decide we don't want it later then it will be deleted
  VeloCluster* currentCluster = new VeloCluster();
  // Add the cluster centre:
  bool OK = TryToAddCentralChannel(currentCluster, currentClusterSTN, 
                                   currentDigit);
  if(isVerbose) verbose()  << "cluster central channel" 
			   << (OK ? " added ":" rejected") << endreq;
  if ( !OK ) { 
    delete currentCluster; 
    return NULL; 
  }  // Failed to make a cluster centre
	
  //now consider nearby ones. If they
  // meet the criteria, add them to the cluster
  OK = TryToAddChannel(currentCluster, currentClusterSTN, currentDigit,-1);
  // Strip:  | |1| | | |  (offset -1)
  if(isVerbose) verbose()  << "cluster -1 channel"
			   << (OK ? " added ":" rejected") << endreq;
  if (OK) {
    //We only add channels two strips away if the channels 1 strip away
    // was also included:
    OK = TryToAddChannel(currentCluster, currentClusterSTN, currentDigit,-2); 
    // Strip:  |0| | | | |  (offset -2)
    if(isVerbose) verbose()  << "cluster -2 channel"
			     << (OK ? " added ":" rejected") << endreq;
  }

  //
  OK = TryToAddChannel(currentCluster, currentClusterSTN, currentDigit,+1); 
  // Strip:  | | | |3| |  (offset +1)
  if(isVerbose) verbose()  << "cluster +1 channel"
			   << (OK ? " added ":" rejected") << endreq;
  if (OK) {
    //We only add channels two strips away if the channels 1 strip away
    // was also included:
    OK = TryToAddChannel(currentCluster, currentClusterSTN, currentDigit,+2); 
    // Strip:  | | | | |4|  (offset +2)
    if(isVerbose) verbose()  << "cluster +2 channel" 
			     << (OK ? " added ":" rejected") << endreq;
  }
	
  currentCluster->setSensorID(VeloChannelID(m_sensor,0));
  if(isVerbose) verbose()  << " cluster: sensor " <<  currentCluster->sensor() 
			   << " first strip " << currentCluster->strip(0) 
			   << " ADC " <<  currentCluster->adcValue(0) 
			   << " channelID " << currentCluster->channelID(0) 
			   << endreq;
  
  return currentCluster;
}

//=========================================================================
//  
//=========================================================================
bool VeloClusterMaker::TryToAddCentralChannel(VeloCluster * currentCluster,
                                              float& currentClusterSTN,
                                              VeloFullFPGADigit*  currentDigit){
  bool isVerbose = msgLevel( MSG::VERBOSE );
  // is this a suitable start for a cluster

  int stripId = currentDigit->strip();
  int stripIndex = stripId;//m_velo->stripArrayIndex(m_sensor, stripId);

  if ( m_channelUsed[stripIndex] ) return false;   // Channel already used

  int signAdjust=(currentDigit->adcValue(VeloFullFPGADigit::RAW)<=0.0) ? -1 : +1;
  bool passesDetectorSTNCut = ( signAdjust*currentDigit->signalToNoise() > 
				signalToNoiseCut(m_sensor) ); 
  if ( !passesDetectorSTNCut) {
    // Fails cut
    return false;
  }
  // Mark used
  m_channelUsed[stripIndex] = true;

  // If we reach here, it's OK to use this channel. Add it to the cluster:
  addDigit(currentCluster,currentClusterSTN,currentDigit,0);

  if(isVerbose) verbose()  << " stripID" << stripId 
			   << " ADC " << currentDigit->adcValue(VeloFullFPGADigit::RAW) 
			   << " STN " << currentClusterSTN << endmsg;
  // All done OK
  return true;
}

//=========================================================================
//  
//=========================================================================
bool VeloClusterMaker::TryToAddChannel(VeloCluster * currentCluster,
                                       float& currentClusterSTN, 
                                       VeloFullFPGADigit*  currentDigit, 
                                       int offset){

  bool isVerbose = msgLevel( MSG::VERBOSE );

  // Tries to add a channel (offset) strips away from currentDigit.
  // If successful, it updates the cluster

  VeloChannelID nearbyStripId;
  StatusCode sc = m_velo->neighbour(currentDigit->key(), offset, nearbyStripId);
  if (!sc) return false; // no available neighbour
  int nearbyStripIndex = nearbyStripId.strip();
  if ( m_channelUsed[nearbyStripIndex] ) return false;  // Channel already used
  VeloFullFPGADigit *nearbyDigit =m_digits->object(nearbyStripId);
  if ( NULL==nearbyDigit) return false; // no hit on nearby strip
 
  // if (A or B) {
  //   [mark channel used]
  //   if (A) {
  //     [add hit]
  //   }
  // }
  // where A is signAdjust*comparisonHitPlusOne->SignalToNoise() > 
  //            signAdjust*centralHit->SignalToNoise()*InclusionThreshold
  //       B is signAdjust*comparisonHitPlusOne->SignalToNoise() > 
  //            SignalToNoiseCut(detindex))
  //
  // B is nearly redundant!
  //
  // For the outer two strips, we have an extra check:
  //
  // if (C and (A or B)) {
  //   [mark channel used]
  //   if (A) {
  //     [add hit]
  //   }
  // }
  //
  // where A is signAdjust*comparisonHitMinusTwo->SignalToNoise() > 
  //            signAdjust*centralHit->SignalToNoise()*InclusionThreshold
  //       B is signAdjust*comparisonHitMinusTwo->SignalToNoise() > 
  //            SignalToNoiseCut(detindex)
  //       C is signAdjust*comparisonHitMinusTwo->SignalToNoise() < 
  //            signAdjust*comparisonHitMinusOne->SignalToNoise()
  //
  // For now I've made things mimic the original code so I
  // can reproduce its results, but once everything's sorted
  // out these should probably be updated.

  // This bit is check C above and is only used for strips outside the 
  // central three:
  int signAdjust=(currentDigit->adcValue(VeloFullFPGADigit::RAW)<=0.0) ? -1 : +1;

  if ( abs(offset)>1 ) {
    // Strip is outside the central three - make an extra cut:
    signed int inner_offset; // One strip closer to the centre than (offset)
    inner_offset = (offset<0) ? offset + 1 : offset - 1;
    // One strip closer to centre than nearbyChan
    VeloChannelID innerStripId;
    StatusCode sc = m_velo->neighbour(currentDigit->key(),inner_offset,
                                      innerStripId);
    if ( ! sc ) {
      // Invalid channel somehow, though this shouldn't happen.
      return false;
    }
    // Hit object corresponding to that strip
    VeloFullFPGADigit* innerDigit = m_digits->object(innerStripId); 
    if ( NULL==innerDigit ) {
      // Invalid channel somehow, though this shouldn't happen.
      return false;
    }
    bool passesMonotonicDecreasingCut = 
      ( signAdjust*innerDigit->signalToNoise() >= 
	signAdjust*nearbyDigit->signalToNoise() );

    if ( !passesMonotonicDecreasingCut ) {
      return false;
    }
  }

  //
  // The following two conditions are A and B respectively:
  bool passesInclusionThreshold = ( signAdjust*nearbyDigit->signalToNoise() > 
                                    signAdjust*currentDigit->signalToNoise()*
                                    m_inclusionThreshold );
  bool passesDetectorSTNCut     = ( signAdjust*nearbyDigit->signalToNoise() > 
                                    signalToNoiseCut(m_sensor) ); 
  if ( !passesInclusionThreshold && !passesDetectorSTNCut) {
    // Fails (A or B) cut
    return false;
  }
  // Mark used
  m_channelUsed[nearbyStripIndex] = true;

  if ( !passesInclusionThreshold ) {
    // Fails (A) cut
    return false;
  }

  // If we reach here, it's OK to use the nearby channel. Add it to the cluster:
  addDigit(currentCluster,currentClusterSTN,nearbyDigit,offset);
 
  if(isVerbose) verbose()  << " stripID" << nearbyStripId.strip() << " ADC " 
			   << nearbyDigit->adcValue(VeloFullFPGADigit::RAW)
         << " STN " << currentClusterSTN << endreq;

  // All done OK
  return true;
}


//=========================================================================
//  
//=========================================================================
void VeloClusterMaker::addDigit(VeloCluster* currentCluster,
                                float& currentClusterSTN,
                                VeloFullFPGADigit* nearbyDigit,
                                signed int offset) {
  // add ADC Value to end of cluster
  std::vector<std::pair<long,double> > signals = currentCluster->stripSignals();
  std::pair<long,double> newpair(nearbyDigit->strip(),
                                 nearbyDigit->adcValue(VeloFullFPGADigit::RAW)); 
  if (offset>0) signals.push_back(newpair);
  else{
    // add ADC value and this becomes first hit in cluster
    std::vector<std::pair<long,double> >::iterator sigFirst = signals.begin(); 
    signals.insert(sigFirst,newpair); 
    // update S/N
    currentClusterSTN += float(nearbyDigit->signalToNoise());
  }
  currentCluster->setStripSignals(signals);
}


//=========================================================================
//  
//=========================================================================
bool VeloClusterMaker::checkCluster( VeloCluster* currentCluster, 
                                     float& currentClusterSTN) {
  // Now, last of all, we check if the global cluster S/N is above the cut:

  return 
    fabs(currentClusterSTN)>clusterSignalToNoiseCut(currentCluster->sensor());
}

//=========================================================================
//  
//=========================================================================
void VeloClusterMaker::unmarkCluster(VeloCluster* currentCluster) {
  // This cluster has been rejected.
  // unmark its strips that were tagged as being used.
  std::vector<std::pair<long,double> > signals = currentCluster->stripSignals();
  std::vector<std::pair<long,double> >::iterator stripIt;
  for ( stripIt= signals.begin() ;  signals.end() != stripIt ; stripIt++ ) {
    long stripIndex=stripIt->first;
    m_channelUsed[stripIndex] = false;
  }
  // delete cluster
  delete currentCluster;
}



//=========================================================================
//  
//=========================================================================
StatusCode VeloClusterMaker::storeClusters(){
  // write clusters to TDS

  bool isDebug   = msgLevel( MSG::DEBUG   );

  // sort VeloClusters into order of ascending sensor + strip
  std::stable_sort(m_clusters->begin(),m_clusters->end(),
		   VeloEventFunctor::Less_by_key<const VeloCluster*>());

  StatusCode sc = eventSvc()->registerObject(m_outputContainer,m_clusters);
  
  if ( sc ) {
    if(isDebug) debug() << "Stored " << m_clusters->size() 
			<< " MCVeloClusters at " 
			<< m_outputContainer << endreq;
  } else {
    error() << "Unable to store VeloClusters at " 
	    << m_outputContainer << endreq;
  }

  return sc;

}


//=========================================================================
//  
//=========================================================================
std::pair<VeloFullFPGADigits::iterator, VeloFullFPGADigits::iterator> 
VeloClusterMaker::getVeloFullFPGADigitsOfSensor(int sensorId) {
  // get position of velo digits in vector for the specified sensor
  // this makes use of fact that Digits from each sensor have already been
  // sorted so that they are adjacent

  int idum=0; //dummy strip number
  VeloFullFPGADigit selObj(VeloChannelID(sensorId,idum,VeloChannelID::Null));
  // ensure sorted   
  std::stable_sort(m_digits->begin(),m_digits->end(),
		   VeloEventFunctor::Less_by_sensor<const VeloFullFPGADigit*>());

  // get range with required sensor no.
  std::pair<VeloFullFPGADigits::iterator, VeloFullFPGADigits::iterator>
    range = std::equal_range(m_digits->begin(),m_digits->end(),&selObj,
			     VeloEventFunctor::Less_by_sensor<const VeloFullFPGADigit*>());

  return range;

}




