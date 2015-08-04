//-----------------------------------------------------------------------------
// Implementation file for class : VeloClusterMaker
//
// 02/02/2002 : Chris Parkes , update Tomasz Szumlak
// based on Velo TestBeam version of Mat Charles
// rewritten to use STL rather than root
//-----------------------------------------------------------------------------

// from Gaudi
#include "Kernel/VeloChannelID.h"
#include "Kernel/VeloEventFunctor.h"

// from Detector
#include "VeloDet/DeVelo.h" 

// from Event
#include "Event/MCVeloFE.h"
#include "Event/VeloDigit.h"

// local
#include "VeloClusterMaker.h"

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloClusterMaker )

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
VeloClusterMaker::VeloClusterMaker( const std::string& name,
                                    ISvcLocator* pSvcLocator) 
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_digits ( 0 ),
    m_sensor ( 0 ),
    m_clusters ( 0 ),
    m_velo ( 0 ),
    m_isDebug ( false ),
    m_isVerbose ( false )
{
  declareProperty( "InputData",  m_inputContainer  = 
		   LHCb::VeloDigitLocation::Default );
  declareProperty( "OutputData", m_outputContainer = 
		   LHCb::InternalVeloClusterLocation::Default );
  declareProperty( "MaxClusters", m_maxClusters = 10000 );
  declareProperty( "InclusionThreshold", m_inclusionThreshold = 0.1);
  declareProperty( "SignalToNoiseCut", m_signalToNoiseCut = 3.0);
  declareProperty( "ClusterSignalToNoiseCut", 
                   m_clusterSignalToNoiseCut = 4.5 );
  declareProperty( "ClusterAbsoluteADCCut", m_clusterAbsoluteADCCut = 20. );
  declareProperty( "HighThreshold", m_highThreshold= 30.0 );
}

//=============================================================================
// Destructor
//=============================================================================
VeloClusterMaker::~VeloClusterMaker(){}
//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode VeloClusterMaker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  //
  debug() << "==> Initialise" << endmsg;
  
  m_velo = getDet<DeVelo>( DeVeloLocation::Default );

  m_isDebug   = msgLevel( MSG::DEBUG   );
  m_isVerbose = msgLevel( MSG::VERBOSE );
  //
  return StatusCode::SUCCESS;
}
//=========================================================================
//  Main execution
//=========================================================================
StatusCode VeloClusterMaker::execute() {
  // make clusters from VeloDigits
  if(m_isDebug) debug() << "==> Execute" << endmsg;
  //
  makeClusters();
  //
  if(m_isDebug) debug() << " All clusters made" << endmsg;
  StatusCode sc=storeClusters();
  return sc;
}
//==========================================================================
void VeloClusterMaker::makeClusters(){
  //
  if(m_isDebug) debug()<< " ==> VeloClusterMaker::makeClusters() " <<endmsg;
  // Creates a new set of clusters for the event.
  if(m_isDebug) debug() << "====> making clusters of event" << endmsg;
  // make a container that will get stored on the TDS of the clusters
  // even if zero clusters made - the empty container must 
  // still be made and stored
  m_clusters = new LHCb::InternalVeloClusters;
  // get all hits from which the clusters will be made
  m_digits = get<LHCb::VeloDigits>(m_inputContainer);
  if(m_isDebug) debug()<< " ==> size of digits container " 
		       << m_digits->size() <<endmsg;
  std::vector< DeVeloSensor * >::const_iterator iSens;
  unsigned int detIndex = 0;
  for( iSens = m_velo->sensorsBegin() ; iSens != m_velo->sensorsEnd() ; 
       ++iSens, ++detIndex ){
    // set all channels not used
    m_sensor=(*iSens)->sensorNumber();;
    if(m_isDebug) debug() << "makeClusters:DetectorNumber " << detIndex 
                          << " Sensor " << m_sensor<< endmsg;
    m_channelUsed.clear();
    m_channelUsed.insert( m_channelUsed.begin(), 
                          (*iSens)->numberOfStrips(), false); 
    // retrieve hits of detector
    std::pair<LHCb::VeloDigits::iterator,LHCb::VeloDigits::iterator> 
      range=getVeloDigitsOfSensor(m_sensor);
    if(m_isDebug) debug()<< "makeClusters:retrieved digits for det "
                         << detIndex << " size " 
                         << range.second - range.first <<endmsg;    
    // sort by increasing ADC value          
    std::stable_sort(range.first, range.second,
		     VeloEventFunctor::Less_by_adcValue<const LHCb::VeloDigit*>());
    // swap to decreasing ADC
    std::reverse(range.first, range.second);
    // iterate over hits
    for(LHCb::VeloDigits::iterator digIt = range.first; 
        digIt!=range.second; digIt++) { 
      // try to make cluster from this hit
      double currentClusterSTN=0;
      LHCb::VeloDigit* currentDigit=*digIt;    
      if(m_isVerbose) verbose()<< "makeClusters: sorted by ADC "
                               << currentDigit->adcValue() << " for sensor "
                               << currentDigit->sensor() <<endmsg;
      LHCb::InternalVeloCluster* currentCluster=
        makeClusterFromDigit(currentDigit,currentClusterSTN);
      if(!currentCluster) continue; // did not make a cluster
      if(m_isVerbose) verbose()<< " made cluster from digit S/N = " 
			       << currentClusterSTN
			       << endmsg;
      double adcTotal = 0.;
      for ( int iS = 0 ; iS < currentCluster->size() ; ++iS ){
	adcTotal += currentCluster->adcValue(iS);
      }
      // global cluster S/N cut:
      if( currentClusterSTN > m_clusterSignalToNoiseCut &&
	 adcTotal > m_clusterAbsoluteADCCut ){
	// Cluster is OK! 
	// set Key and Add it to the event.
	LHCb::VeloChannelID channelID( currentCluster->sensor(), 
				       currentCluster->strip(0) );          
	m_clusters->insert(currentCluster,channelID);
	if(m_isVerbose) 
	  verbose()<< "makeClusters:Cluster added S/N " 
		   << currentClusterSTN << " size " 
		   << currentCluster->size() 
		   << " total clusters " << m_clusters->size() <<endmsg;
	if(m_isVerbose) 
	  verbose()<< "currentCluster key " 
		   << currentCluster->key() << " size " 
		   << currentCluster->size() 
		   << " first strip " << currentCluster->strip(0) 
		   << " first ADC " << currentCluster->adcValue(0)
		   <<endmsg;
      }else{
	// Cluster failed its global S/N cut: unmark channels as used
	unmarkCluster(currentCluster);
      }
      if( (int)m_clusters->size()>= m_maxClusters ) {
	// Too many clusters to add more after this one.
	Warning("Too many clusters in this event processing incomplete").ignore();
	info() << "Processing limit is" <<  m_maxClusters 
	       << " clusters. VeloClusterMaker::MakeClusters "
	       << "is stopping clusterization while processing "
	       << "sensor " << m_sensor
	       << " for this event." <<endmsg;
	return; // **alternate exit point**
      }
    } // loop over Digits
  } // loop over detectors
  //  
  return;
}  
//==========================================================================
LHCb::InternalVeloCluster* 
VeloClusterMaker::makeClusterFromDigit(LHCb::VeloDigit* currentDigit,
				       double& currentClusterSTN){
  //
  if(m_isDebug) debug()<< " ==> VeloClusterMaker::makeClusterFromDigit " 
		       <<endmsg;
  // make new cluster 
  // - if we decide we don't want it later then it will be deleted
  LHCb::InternalVeloCluster* currentCluster = new LHCb::InternalVeloCluster();
  // Add the cluster centre:
  bool OK = TryToAddCentralChannel(currentCluster, currentClusterSTN, 
                                   currentDigit);
  if(m_isDebug) debug()<< "cluster central channel" 
		       << (OK ? " added ":" rejected") <<endmsg;
  if( !OK ){ // Failed to make a cluster centre	
    delete currentCluster; 
    return 0; 
  }  
  // consider nearby strips, if they meet the criteria, add them to the cluster
  OK = TryToAddChannel(currentCluster, currentClusterSTN, currentDigit,-1);
  // Strip:  | |1| | | |  (offset -1)
  if(m_isDebug) debug()<< "cluster -1 channel"
		       << (OK ? " added ":" rejected") <<endmsg;
  if(OK){
    //We only add channels two strips away if the channels 1 strip away
    // was also included:
    OK = TryToAddChannel(currentCluster, currentClusterSTN, currentDigit,-2); 
    // Strip:  |0| | | | |  (offset -2)
    if(m_isDebug) debug()<< "cluster -2 channel"
			 << (OK ? " added ":" rejected") <<endmsg;
  }
  //
  OK = TryToAddChannel(currentCluster, currentClusterSTN, currentDigit,+1); 
  // Strip:  | | | |3| |  (offset +1)
  if(m_isDebug) debug()<< "cluster +1 channel"
		       << (OK ? " added ":" rejected") <<endmsg;
  if(OK){
    //We only add channels two strips away if the channels 1 strip away
    // was also included:
    OK = TryToAddChannel(currentCluster, currentClusterSTN, currentDigit,+2); 
    // Strip:  | | | | |4|  (offset +2)
    if(m_isDebug) debug()<< "cluster +2 channel" 
			 << (OK ? " added ":" rejected") << endmsg;
  }
  //	
  currentCluster->setSensorID(LHCb::VeloChannelID(m_sensor,0));
  // check if cluster total ADC values are above the non-spillover threshold
  double totalADC = 0;
  for( int i = 0 ; i < currentCluster->size() ; ++i ){
    totalADC += currentCluster->adcValue(i);
  }
  if( m_highThreshold <= totalADC ){
    currentCluster->setHasHighThreshold(true);
  }else{
    currentCluster->setHasHighThreshold(false); 
  }
  if(m_isVerbose) verbose() << " cluster: sensor " 
			    <<  currentCluster->sensor() 
			    << " first strip " << currentCluster->strip(0) 
			    << " ADC " <<  currentCluster->adcValue(0) 
			    << " channelID " << currentCluster->channelID(0)
			    << " has high threshold " 
			    << (currentCluster->hasHighThreshold()? 'T' : 'F')
			    <<endmsg;
  //  
  return (currentCluster);
}  
//==========================================================================
bool VeloClusterMaker::TryToAddCentralChannel(LHCb::InternalVeloCluster * currentCluster,
					      double& currentClusterSTN,
					      LHCb::VeloDigit*  currentDigit){
  //
  if(m_isDebug) debug()<< " ==> VeloClusterMaker::TryToAddCentralChannel " 
		       <<endmsg;
  // is this a suitable start for a cluster
  // check if channel already used
  if ( m_channelUsed[currentDigit->strip()] ) return false;  
  // check if over required single channel SN
  if ( signalToNoise(currentDigit) < m_signalToNoiseCut ) return false;
  // Mark used
  m_channelUsed[currentDigit->strip()] = true;
  // If we reach here, it's OK to use this channel. Add it to the cluster:
  addDigit(currentCluster,currentClusterSTN,currentDigit,0);
  //
  if(m_isDebug) debug()<< " strip " << currentDigit->strip()
		       << " ADC " << currentDigit->adcValue() 
		       << " STN " << currentClusterSTN << endmsg;
  // All done OK
  return (true);
}
//==========================================================================
bool VeloClusterMaker::TryToAddChannel(LHCb::InternalVeloCluster * currentCluster,
                                       double& currentClusterSTN, 
                                       LHCb::VeloDigit*  currentDigit, 
                                       int offset){
  //
  if(m_isDebug) debug()<< " ==> VeloClusterMaker::TryToAddChannel " <<endmsg;
  // Tries to add a channel (offset) strips away from currentDigit.
  // If successful, it updates the cluster
  LHCb::VeloChannelID nearbyStripId;
  const DeVeloSensor* sens=m_velo->sensor(currentDigit->key().sensor());
  StatusCode sc = sens->neighbour(currentDigit->key(), offset, nearbyStripId);
  if (!sc) return false; // no available neighbour
  // Channel already used ?
  if ( m_channelUsed[nearbyStripId.strip()] ) return false;  
  LHCb::VeloDigit *nearbyDigit =m_digits->object(nearbyStripId);
  if(!nearbyDigit) return false; // no hit on nearby strip 
  // if (A or B) {
  //   [mark channel used]
  //   if (A) {
  //     [add hit]
  //   }
  // }
  // where A is comparisonHitPlusOne->SignalToNoise() > 
  //            centralHit->SignalToNoise()*InclusionThreshold
  //       B is comparisonHitPlusOne->SignalToNoise() > 
  //            SignalToNoiseCut
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
  // where A is comparisonHitMinusTwo->SignalToNoise() > 
  //            centralHit->SignalToNoise()*InclusionThreshold
  //       B is comparisonHitMinusTwo->SignalToNoise() > 
  //            SignalToNoiseCut(detindex)
  //       C is comparisonHitMinusTwo->SignalToNoise() < 
  //            comparisonHitMinusOne->SignalToNoise()
  //
  // For now I've made things mimic the original code so I
  // can reproduce its results, but once everything's sorted
  // out these should probably be updated.

  // This bit is check C above and is only used for strips outside the 
  // central three:
  if(abs(offset)>1){
    // Strip is outside the central three - make an extra cut:
    signed int inner_offset; // One strip closer to the centre than (offset)
    inner_offset = (offset<0) ? offset + 1 : offset - 1;
    // One strip closer to centre than nearbyChan
    LHCb::VeloChannelID innerStripId;
    StatusCode sc = sens->neighbour(currentDigit->key(),inner_offset,
                                    innerStripId);
    if(sc.isFailure()) return false; // Invalid channel somehow

    // Hit object corresponding to that strip
    LHCb::VeloDigit* innerDigit=m_digits->object(innerStripId); 
    if( !innerDigit ){
      Warning("Asking for a +2 VeloDigit where +1 VeloDigit does not exist!").ignore();
      if(m_isDebug) debug() << "innerStripID = " << innerStripId << endmsg;
      return (false);
    }
    // check strips monotonically decreasing in S/N 
    if( signalToNoise(innerDigit)< signalToNoise(nearbyDigit) ) return false;
  }
  //
  // The following two conditions are A and B respectively:
  bool passesInclusionThreshold=( signalToNoise(nearbyDigit)  >
				  signalToNoise(currentDigit)*
				  m_inclusionThreshold);
  bool passesDetectorSTNCut=( signalToNoise(nearbyDigit)>
			      m_signalToNoiseCut); 

  // Fails (A or B) cut ?
  if((!passesInclusionThreshold)&&(!passesDetectorSTNCut)) return false;

  // Mark used
  m_channelUsed[nearbyStripId.strip()] = true;
  // Fails (A) cut
  if ( !passesInclusionThreshold ) return (false);
  
  // If we reach here, it's OK to use the nearby channel. Add it to the cluster
  addDigit(currentCluster,currentClusterSTN,nearbyDigit,offset); 
  if(m_isVerbose) verbose() << " stripID" << nearbyStripId.strip() << " ADC " 
			    << nearbyDigit->adcValue()
			    << " STN " << currentClusterSTN << endmsg;
  // All done OK
  return true;
}
//==========================================================================
void VeloClusterMaker::addDigit(LHCb::InternalVeloCluster* currentCluster,
                                double& currentClusterSTN,
                                LHCb::VeloDigit* nearbyDigit,
                                signed int offset) {
  //
  if(m_isDebug) debug()<< " ==> VeloClusterMaker::addDigit " <<endmsg;
  // add ADC Value to end of cluster
  std::vector<std::pair<long,double> > signals=currentCluster->stripSignals();
  std::pair<long,double> newpair(nearbyDigit->strip(),
                                 nearbyDigit->adcValue()); 
  if(offset>0){ 
    signals.push_back(newpair);
  }else{
    // add ADC value and this becomes first hit in cluster
    std::vector<std::pair<long,double> >::iterator sigFirst=signals.begin(); 
    signals.insert(sigFirst,newpair); 
    // update S/N
    currentClusterSTN+=signalToNoise(nearbyDigit);
  }
  currentCluster->setStripSignals(signals);
}  
//==========================================================================
void VeloClusterMaker::unmarkCluster(LHCb::InternalVeloCluster* currentCluster){
  //
  if(m_isDebug) debug()<< " ==> VeloClusterMaker::unmarkCluster() " <<endmsg;
  // This cluster has been rejected.
  // unmark its strips that were tagged as being used.
  std::vector<std::pair<long,double> > signals = currentCluster->stripSignals();
  std::vector<std::pair<long,double> >::iterator stripIt;
  for ( stripIt= signals.begin() ;  signals.end() != stripIt ; stripIt++ ) {
    long stripIndex=stripIt->first;
    m_channelUsed[stripIndex] = false;
  }
  // delete cluster
  delete (currentCluster);
}
//=========================================================================
StatusCode VeloClusterMaker::storeClusters(){
  //
  if(m_isDebug) debug()<< " ==> VeloClusterMaker::storeClusters() " <<endmsg;
  // write clusters to TDS
  // sort VeloClusters into order of ascending sensor + strip
  std::stable_sort(m_clusters->begin(),m_clusters->end(),
		   VeloEventFunctor::Less_by_key<const LHCb::InternalVeloCluster*>());
  if(m_isDebug) debug()<< " ==> size of clusters container: " 
		       << m_clusters->size() <<endmsg;  
  put(m_clusters, m_outputContainer);  
  if(m_isDebug) debug()<< "Stored " << m_clusters->size() 
                       << " MCInternalVeloClusters at " 
                       << m_outputContainer <<endmsg;
  return StatusCode::SUCCESS;
}  
//=========================================================================
std::pair<LHCb::VeloDigits::iterator, LHCb::VeloDigits::iterator> 
VeloClusterMaker::getVeloDigitsOfSensor(int sensorId) {
  //
  if(m_isDebug) debug()<< " ==> VeloClusterMaker::getVeloDigitsOfSensor() " <<endmsg;
  // get position of velo digits in vector for the specified sensor
  // this makes use of fact that Digits from each sensor have already been
  // sorted so that they are adjacent
  int idum=0; //dummy strip number
  LHCb::VeloDigit selObj(LHCb::VeloChannelID(sensorId,idum,LHCb::VeloChannelID::Null));
  // ensure sorted   
  std::stable_sort(m_digits->begin(),m_digits->end(),
		   VeloEventFunctor::Less_by_sensor<const LHCb::VeloDigit*>());
  // get range with required sensor no.
  std::pair<LHCb::VeloDigits::iterator, LHCb::VeloDigits::iterator>
    range = std::equal_range(m_digits->begin(),m_digits->end(),&selObj,
			     VeloEventFunctor::Less_by_sensor<const LHCb::VeloDigit*>());
  //
  return (range);
}
//=========================================================================
double VeloClusterMaker::signalToNoise(LHCb::VeloDigit* digit){
  // get the noise in ADC counts from the Detector Element
  double noise = m_velo->sensor(digit->channelID())->stripNoise(digit->channelID().strip());
  if(fabs(noise) < 0.00001) {
    Warning("Strip with |noise| < 0.00001, S/N not valid here").ignore();
    noise = 0.00001; // sanity check for zero noise...
  }
  // calc the S/N for this digit  
  return static_cast<double>(digit->adcValue())/noise;
}
