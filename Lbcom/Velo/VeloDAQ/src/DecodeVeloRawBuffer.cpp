// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"

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
  : Algorithm ( name , pSvcLocator ) 
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

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;

  //== Get detector element
  SmartDataPtr<DeVelo> velo( detDataService(), "/dd/Structure/LHCb/Velo" );
  if ( 0 == velo ) {
    msg << MSG::ERROR << "Unable to retrieve Velo detector element." << endreq;
    return StatusCode::FAILURE;
  }
  m_velo = velo;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode DecodeVeloRawBuffer::execute() {

  MsgStream  msg( msgSvc(), name() );

  bool verbose = msg.level() <= MSG::VERBOSE;
  bool debug   = msg.level() <= MSG::DEBUG;

  // Get the input container

  RawEvent* event;
  
  SmartDataPtr<RawEvent> myEvent ( eventSvc(), RawEventLocation::Default );
  if ( 0 == myEvent ) {
    if ( debug ) msg << MSG::DEBUG << "Access RawBuffer for decoding" 
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

  if ( debug ) msg << MSG::DEBUG << "RawEvent found" << endreq;

  // make cluster container

    m_clusters = new VeloClusters;

 //=======================================================================
 // Fill cluster with Raw Event info
 //=======================================================================

  int nbOk = 0;
  int sensor;
  int word;
  
  std::vector<RawBank> banks = event->banks( RawBuffer::Velo );
  if ( debug ) msg << MSG::DEBUG << "Number of banks : " << banks.size()
                   << endreq;

  std::vector<RawBank>::const_iterator itBank;
  
  for ( itBank = banks.begin() ; banks.end() != itBank ; itBank++ ) {
    sensor = (*itBank).bankSourceID(); // Sensor Number
    if ( verbose ) msg << MSG::VERBOSE << format( "sensor %3d", sensor ) 
                       << endreq;
    raw_int* ptData = (*itBank).data();
    int length = (*itBank).dataSize();
    while ( 0 < length ) {
      word = (*ptData++);
      length--;
      int size  = word & 0xF; // cluster size
      if ( verbose ) msg << MSG::VERBOSE << format( "cluster size %2d", size)
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
        if ( verbose ) msg << MSG::VERBOSE << format( "ADC Value %3d", ADCValue)
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

  if ( debug ) {
    msg << MSG::DEBUG << "Created " << nbOk << " clusters." << endreq;
  }

  // // initial clusters
//  //  Define an iterator to the VELO cluster vector and loop through it  
//   VeloClusters::iterator itcv;
//     for (itcv=m_clusters->begin(); itcv!=m_clusters->end(); itcv++) {

//     // printout some info. for the cluster
//     msg << MSG::DEBUG << "testVeloCluster:" 
//       	<< " sensorNumber " << (*itcv)->sensor() 
//         << " First strip in cluster " << (*itcv)->strip(0)
//         << endmsg;
//     // and the strips in the cluster
//     int size=(*itcv)->size(); // number of strips
   
//       for (int i=0;i<size;i++) {
// 	msg << MSG::DEBUG << " testVeloCluster:"
// 	    << " strip " <<   (*itcv)->strip(i) << " / " << size 
// 	    << " signal " <<  (*itcv)->adcValue(i);
//       }
//       msg << MSG::DEBUG << endmsg;
   
 
//   }

// store clusters
   StatusCode sc = eventSvc()->registerObject(m_outputContainer,m_clusters);


   if ( sc ) {
     msg << MSG::DEBUG << "Stored " << m_clusters->size() 
	 << " MCVeloClusters at " << m_outputContainer << endreq;
   } else {
     msg << MSG::ERROR << "Unable to store VeloClusters at " 
	 << m_outputContainer << endreq;
   }


  return StatusCode::SUCCESS;
};


//=============================================================================
//  Finalize
//=============================================================================
StatusCode DecodeVeloRawBuffer::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;



  return StatusCode::SUCCESS;
}

//=============================================================================
