// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "Event/RawEvent.h"

// local
#include "DecodeVeloRawBuffer.h"


//-----------------------------------------------------------------------------
// Implementation file for class : DecodeVeloRawBuffer
//
// 2004-02-02 : Chris Parkes
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<DecodeVeloRawBuffer>          Factory ;
const        IAlgFactory& DecodeVeloRawBufferFactory = Factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DecodeVeloRawBuffer::DecodeVeloRawBuffer( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
 , m_outputContainer                ( VeloClusterLocation::Default )
{
   declareProperty( "OutputData"          , m_outputContainer );
}


//=============================================================================
// Destructor
//=============================================================================
DecodeVeloRawBuffer::~DecodeVeloRawBuffer() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode DecodeVeloRawBuffer::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug () << "==> Initialise" << endreq;

  //== Get detector element
  m_velo = getDet<DeVelo>( "/dd/Structure/LHCb/Velo" );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode DecodeVeloRawBuffer::execute() {

  MsgStream  msg( msgSvc(), name() );

  bool isVerbose = msgLevel( MSG::VERBOSE );
  bool isDebug   = msgLevel( MSG::DEBUG );

  // Get the input container

  RawEvent* event;
  
  SmartDataPtr<RawEvent> myEvent ( eventSvc(), RawEventLocation::Default );
  if ( 0 == myEvent ) {
    if ( isDebug ) debug() << "Access RawBuffer for decoding" 
			   << endreq;
    
    SmartDataPtr<RawBuffer> buffer ( eventSvc(), RawBufferLocation::Default );
    if ( 0 == buffer ) {
      msg << MSG::ERROR << "== Unable to find RawBuffer. Abort." << endreq;
      return StatusCode::FAILURE;
    }
    event = new RawEvent( buffer );
    eventSvc()->registerObject( RawEventLocation::Default, event );
  } else {
    event = myEvent;
  }

  if ( isDebug ) debug() << "RawEvent found" << endreq;

  // make cluster container

    m_clusters = new VeloClusters;

 //=======================================================================
 // Fill cluster with Raw Event info
 //=======================================================================

  int nbOk = 0;
  int sensor;
  int word;
  
  std::vector<RawBank> banks = event->banks( RawBuffer::Velo );
  if ( isDebug ) debug() << "Number of banks : " << banks.size()
			 << endreq;

  std::vector<RawBank>::const_iterator itBank;
  
  for ( itBank = banks.begin() ; banks.end() != itBank ; itBank++ ) {
    sensor = (*itBank).bankSourceID(); // Sensor Number
    if ( isVerbose ) verbose() << format( "sensor %3d", sensor ) 
                       << endreq;
    raw_int* ptData = (*itBank).data();
    int length = (*itBank).dataSize();
    while ( 0 < length ) {
      word = (*ptData++);
      length--;
      int size  = word & 0xF; // cluster size
      if ( isVerbose ) verbose() << format( "cluster size %2d", size)
                         << endreq;
      int strip = ( word >> 12 ) & 0x7FF;
      VeloChannelID channelID( sensor, strip );
      // new cluster with key	
       VeloCluster* currentCluster = new VeloCluster(channelID); 
       currentCluster->setSensor(sensor);
      //== ADC values for this cluster
      int    shift = 24;
      int    channel = 0;
      while ( channel < size ) {
        if ( 32 == shift ) {
          shift = 0;
          word  = (*ptData++);
          length--;
        }
        int ADCValue = int( (word >> shift) & 0xFF );
        if ( isVerbose ) verbose() << format( "ADC Value %3d", ADCValue)
                           << endreq;

        // fill cluster with adc values 
        std::vector<std::pair<long,double> > signals =
                                                 currentCluster->stripSignals();
        std::pair<long,double> newpair(strip+channel,ADCValue); 
        if (channel>0){ signals.push_back(newpair);}
        else { // new cluster
          std::vector<std::pair<long,double> >::iterator sigFirst = 
                                                                signals.begin();
          signals.insert(sigFirst,newpair); 
        }
        currentCluster->setStripSignals(signals);

        shift += 8;
        channel   ++;
        // store cluster
        if (channel==size) m_clusters->insert(currentCluster,channelID);
      }

      nbOk++;
    }
  }

  //== End

  if ( isDebug ) {
    debug() << "Created " << nbOk << " clusters." << endreq;
  }

  // // initial clusters
//  //  Define an iterator to the VELO cluster vector and loop through it  
//   VeloClusters::iterator itcv;
//     for (itcv=m_clusters->begin(); itcv!=m_clusters->end(); itcv++) {

//     // printout some info. for the cluster
//    debug() << "testVeloCluster:" 
//       	<< " sensorNumber " << (*itcv)->sensor() 
//         << " First strip in cluster " << (*itcv)->strip(0)
//         << endmsg;
//     // and the strips in the cluster
//     int size=(*itcv)->size(); // number of strips
   
//       for (int i=0;i<size;i++) {
// 	debug() << " testVeloCluster:"
// 	    << " strip " <<   (*itcv)->strip(i) << " / " << size 
// 	    << " signal " <<  (*itcv)->adcValue(i);
//       }
//      debug()<< endmsg;
   
 
//   }

// store clusters
   StatusCode sc = eventSvc()->registerObject(m_outputContainer,m_clusters);


   if ( sc ) {
    debug()<< "Stored " << m_clusters->size() 
	 << " MCVeloClusters at " << m_outputContainer << endreq;
   } else {
     msg << MSG::ERROR << "Unable to store VeloClusters at " 
	 << m_outputContainer << endreq;
   }


  return StatusCode::SUCCESS;
};


