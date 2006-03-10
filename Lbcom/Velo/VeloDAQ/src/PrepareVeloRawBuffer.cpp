// $Id: PrepareVeloRawBuffer.cpp,v 1.15 2006-03-10 15:55:04 dhcroft Exp $

#include <vector>
#include <algorithm>

#include "GaudiKernel/AlgFactory.h"

#include "Event/RawEvent.h"

#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiADCWord.h"
#include "SiDAQ/SiRawBufferWord.h"

#include "VeloClusterWord.h"
#include "VeloRawBankVersions.h"
#include "VeloRawWordSizes.h"

#include "PrepareVeloRawBuffer.h"


//-----------------------------------------------------------------------------
// Implementation file for class : PrepareVeloRawBuffer
//
// 2003-04-11 : Olivier Callot
// 2004-04-13 : modified and moved to Velo area Chris Parkes
// 2004-11-03 : modified to GaudiAlg dependance
// 2006-02-15 : David Jones, Kurt Rinnert: complete rewrite for 1MHz raw buffer
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PrepareVeloRawBuffer>          Factory ;
const        IAlgFactory& PrepareVeloRawBufferFactory = Factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrepareVeloRawBuffer::PrepareVeloRawBuffer( const std::string& name,
                                          ISvcLocator* pSvcLocator) 
  : 
  GaudiAlgorithm (name , pSvcLocator),
  m_clusterLoc(LHCb::InternalVeloClusterLocation::Default),
  m_rawEventLoc(LHCb::RawEventLocation::Default),
  m_bankVersion(VeloDAQ::v2)
{
  declareProperty("InternalVeloClusterLocation",m_clusterLoc=LHCb::InternalVeloClusterLocation::Default);
  declareProperty("RawEventLocation",m_rawEventLoc=LHCb::RawEventLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
PrepareVeloRawBuffer::~PrepareVeloRawBuffer() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PrepareVeloRawBuffer::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialise" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrepareVeloRawBuffer::execute() {

  bool isDebug   = msgLevel( MSG::DEBUG );

 
  if (isDebug) debug() << "==> Execute" << endreq;
  
  // Get the input container
  // Get the InternalVeloClusters from their default location 
  const LHCb::InternalVeloClusters* clusters = 0;
  if(!exist<LHCb::InternalVeloClusters>(m_clusterLoc)){
    error()<< " ==> There are no VeloClusters in TES! " <<endmsg;
    return (StatusCode::FAILURE);
  }else{
    clusters=get<LHCb::InternalVeloClusters>(m_clusterLoc);
  }

  m_sortedClusters.clear();
  m_sortedClusters.resize(clusters->size());
  std::copy(clusters->begin(),clusters->end(),m_sortedClusters.begin());

  // Then sort the clusters by sensor number and local coordinate
  std::sort( m_sortedClusters.begin(), m_sortedClusters.end(), 
             PrepareVeloRawBuffer::sortClustersBySensorAndStrip() );

  // define the pointer  
  LHCb::RawEvent* rawEvent;
  // see whether the raw event exits
  if(exist<LHCb::RawEvent>(m_rawEventLoc))
  {
    rawEvent = get<LHCb::RawEvent>(m_rawEventLoc);
  }
  else
  {
    // raw rawEvent doesn't exist. We need to create it 
    rawEvent = new LHCb::RawEvent();
    eventSvc()->registerObject(m_rawEventLoc, rawEvent);
  } 
 
 // loop over all clusters and write one bank per sensor
  std::vector<const LHCb::InternalVeloCluster*>::const_iterator firstOnSensor, lastOnSensor;
  int currentSensorNumber;

  lastOnSensor = firstOnSensor =  m_sortedClusters.begin();
  while (firstOnSensor != m_sortedClusters.end()) {

    currentSensorNumber = (*firstOnSensor)->sensor();

    while (lastOnSensor != m_sortedClusters.end() && 
           (*lastOnSensor)->sensor() == currentSensorNumber) {
      ++lastOnSensor;
    }

    // create new raw buffer in raw data cache, old one is cleared
    makeBank(firstOnSensor, lastOnSensor);

    LHCb::RawBank* newBank = rawEvent->createBank(static_cast<SiDAQ::buffer_word>(currentSensorNumber),
                                                  LHCb::RawBank::Velo,
                                                  m_bankVersion,
                                                  m_bankSizeInBytes, 
                                                  &(m_rawData[0]));

    // add new bank and pass memory ownership to raw event
    rawEvent->adoptBank(newBank,true);

    firstOnSensor = lastOnSensor;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Whatever needs to be done at the end
//=============================================================================
StatusCode PrepareVeloRawBuffer::finalize() {

  debug() << "==> Finalise" << endreq;

  return StatusCode::SUCCESS;
};

unsigned int
PrepareVeloRawBuffer::makeBank (std::vector<const LHCb::InternalVeloCluster*>::const_iterator begin, 
                                std::vector<const LHCb::InternalVeloCluster*>::const_iterator end) 
{
  bool isVerbose = msgLevel( MSG::VERBOSE );

  // clear bank
  m_rawData.clear();
  
  // work out number of clusters for this sensor uses 1st 16 bits
  int numClu = end - begin;

  if(isVerbose) 
  {
    verbose() <<"Number of clusters:" << numClu  <<endmsg;
  }  

  // pcn is next 8 bits: set to zero here (never any errors in sim)
  SiHeaderWord hw(numClu,0,0); 
  SiDAQ::buffer_word pcnAndNumClu = hw.value();

  // clear temporary buffers
  m_clusterPosBuffer.clear();
  m_clusterADCBuffer.clear();

  // for storing 32 bit 'rows' of adc values and cluster positions
  SiDAQ::buffer_word rowData   = 0x0; 
  SiDAQ::buffer_word cluRowData = 0x0;

  // loop over clusters in range defined by iterator 
  unsigned int nClu = 0;  // cluster counter
  unsigned int nAdc = 0;  // ADC word counter, used to to determine
                          // number of padding bytes at end of raw bank 

  std::vector<const LHCb::InternalVeloCluster*>::const_iterator iC = begin;
  for ( ; iC != end ; ++iC) // 
  {
    // get a cluster 
    const LHCb::InternalVeloCluster* clu = *iC;
    // get num strips 
    unsigned int numStrips = clu->size();
      
    if(isVerbose) 
    {
      verbose() <<"Reading cluster from sensor "
                << clu->sensor() << " with " <<
        numStrips << " strips." <<endmsg;
    }

    SiDAQ::buffer_word packedCluster;

    // case of one strip cluster is handled here
    // 
    if( numStrips == 1 )  
    {
      // single strip cluster

      // currently we have no way of retrieving the high theshold
      // information since the VeloInternalCluster does not
      // provide an interface.
      bool highThresh = clu->hasHighThreshold();

      double adcCount = clu->adcValue(0);

      if (adcCount > 127)
      {
        adcCount = 127;
      }
      VeloClusterWord vcw(clu->strip(0),0.0,1, highThresh);
      packedCluster = static_cast<SiDAQ::buffer_word>(vcw.value());
      if ( isVerbose )
	    {
	      verbose() <<"STRIP: " << clu->strip(0)
                  << ",ADC: " << clu->adcValue( 0) << endmsg;
	    }

      SiADCWord aw(adcCount,true);
      rowData |= (aw.value() << ((nAdc % VeloDAQ::adc_per_buffer) << VeloDAQ::adc_shift));

      ++nAdc;
      if (nAdc % VeloDAQ::adc_per_buffer == 0)
	    {
	      m_clusterADCBuffer.push_back ( rowData );
	      //reset rowData
	      rowData = 0x0;
	    }

      if ( isVerbose )
	    {
	      verbose() <<"strip " << clu->strip(0)
                  << ",adc= " + aw.value() << endmsg;
	    }
    }
    else 
    {
      // multiple strip cluster
      // calculate weighted average position of cluster 
      const std::vector< std::pair<long,double> >& stripSignals = clu->stripSignals();
      double sumADC = 0.;
      double sumADCWeightedeStrip = 0.;

      // loop over all strip signals 

      // currently we have no way of retrieving the high theshold
      // information since the VeloInternalCluster does not
      // provide an interface.
      bool highThresh = false; 

      unsigned int i = 0;
      for (; i<stripSignals.size(); ++i) 
	    {
        unsigned int stripNumber = stripSignals[i].first;
        double adcCount = stripSignals[i].second;
        if (adcCount > 127)
        {
          adcCount = 127;
        }
	   
	      if(isVerbose)
        {
          verbose() << "ADC COUNT:" <<  adcCount << " STRIP:" << stripNumber
                    << endmsg; 
        }
	      // sum adc values and adc weighted strip numbers
	      sumADC += adcCount;
	      sumADCWeightedeStrip += static_cast<double>(stripNumber) * adcCount;

        // create new adc word
        bool endOfCluster = (stripSignals.size() == i+1);
        SiADCWord aw(adcCount,endOfCluster);

        rowData |= (aw.value() << ((nAdc % VeloDAQ::adc_per_buffer) << VeloDAQ::adc_shift));

        ++nAdc;
        if (nAdc % VeloDAQ::adc_per_buffer == 0)
        {
          m_clusterADCBuffer.push_back ( rowData );
          //reset rowData
          rowData = 0x0;
        }

	    }// end for  
      if(isVerbose)
	    {
	      verbose() <<endmsg;
	    }

      double cPos = (sumADCWeightedeStrip/sumADC) ; // get weighted mean
      unsigned int channelPos = static_cast<unsigned int>(cPos); // without fractional part
      double interStripPos = cPos - channelPos; // fractional part
      VeloClusterWord vcw(channelPos, interStripPos, numStrips, highThresh);
      packedCluster = static_cast<SiDAQ::buffer_word>(vcw.value());

      if(isVerbose)
	    {
	      verbose() << "CPOS: " << cPos
                  << " CHPOS: " << channelPos
                  << " ISPOS: " << interStripPos
                  << " WORD: " << format( "%8X" , packedCluster )
                  <<endmsg;
	    }

    }// end else 
   
    // store the cluster position
    cluRowData |= (packedCluster << ((nClu % VeloDAQ::clu_per_buffer) << VeloDAQ::clu_shift));
    ++nClu;
    if ( nClu % VeloDAQ::clu_per_buffer == 0 ) 
    {
      m_clusterPosBuffer.push_back( cluRowData );
      // clear the cluster buffer 
      cluRowData = 0x0 ; 
    }
    
  }// end for 

  // if the cluster row isn't empty add the contents to the vector 
  if ( cluRowData != 0x0 )
  {
    m_clusterPosBuffer.push_back ( cluRowData  );
  }
  // check row data is not empty, store remaining adcs
  if ( rowData != 0x0 )
  {
    m_clusterADCBuffer.push_back ( rowData );
  }
  
  // write to buffer or return ? 

  if ( isVerbose ) 
  {
    verbose()<< "Raw  output of cluster start" <<endmsg;
    verbose()<< "PCN error flag and number of clusters[" << format( "%10X" , pcnAndNumClu ) 
             << "]"<<endmsg;
  }

  // add PCN and R
  m_rawData.push_back ( pcnAndNumClu  );

  // add clusters positions  
  std::vector<SiDAQ::buffer_word>::iterator tmpCp = m_clusterPosBuffer.begin();
  for ( ; tmpCp != m_clusterPosBuffer.end() ; ++tmpCp )
  {      
    if ( isVerbose  ) 
    {

      verbose()<< "Cluster position[" << format( "%10X" , *tmpCp ) << "]"<<endmsg;
    }

    m_rawData.push_back ( *tmpCp ) ;
  }
  
  // add adc values 
  std::vector<SiDAQ::buffer_word>::iterator tmpAdc = m_clusterADCBuffer.begin();
  for ( ; tmpAdc != m_clusterADCBuffer.end() ; ++tmpAdc )
  {
    if ( isVerbose ) 
    {

      verbose()<< "Adc row[" << format( "%10X" , *tmpAdc ) << "]"<<endmsg;
    }

    m_rawData.push_back ( *tmpAdc ) ;
  }

  // Find number of padding bytes at the end of bank an compute
  // raw bank size in bytes, including the 4 byte header but
  // *without* the padding bytes at the end. 
  // The number of padding bytes is completely determined by
  // the number of ADC words in the raw bank.
  m_bankSizeInBytes = sizeof(SiDAQ::buffer_word)*m_rawData.size() - (nAdc%VeloDAQ::adc_per_buffer);

  if ( isVerbose) 
  {
    verbose()<< "Raw  output of cluster end" <<endmsg;
  }

  return nClu;
}

