// $Id: PrepareVeloRawBuffer.cpp,v 1.8 2006-02-15 18:08:39 krinnert Exp $

#include <vector>
#include <algorithm>

#include "GaudiKernel/AlgFactory.h"

#include "Event/RawEvent.h"

#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiADCWord.h"

#include "VeloDAQ/VeloClusterWord.h"
#include "VeloDAQ/VeloRawBufferWord.h"

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
  m_bankVersion(2)
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
  std::vector<const LHCb::InternalVeloCluster*> sortedClusters(clusters->size());
  std::copy(clusters->begin(),clusters->end(),sortedClusters.begin());

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
    rawEvent->addBank(currentSensorNumber, LHCb::RawBank::Velo, m_bankVersion, m_rawData);

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

  // pcn is next 8 bits: set to zero here (never any errors in sim)
  SiHeaderWord hw(numClu,0,0); 
  buffer_word pcnAndNumClu = hw.value();

  // clear temporary buffers
  m_clusterPosBuffer.clear();
  m_clusterADCBuffer.clear();

  // for storing 32 bit 'rows' of adc values and cluster positions
  buffer_word rowData   = 0x0; 
  buffer_word cluRowData = 0x0;

  // loop over clusters in range defined by iterator 
  unsigned int nClu = 0;  // cluster counter
  unsigned int nAdc = 0 ; 
  std::vector<const LHCb::InternalVeloCluster*>::const_iterator iC;

  if(isVerbose) 
  {
    verbose() <<"Number of clusters:" << numClu  <<endmsg;
  }  

  for ( iC = begin ; iC != end ; ++iC) // 
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

    buffer_word packedCluster;

    // case of one strip cluster is handled here
    // 
    if( numStrips == 1 )  
    {
      // single strip cluster
      VeloClusterWord vcw(clu->strip(0),0.0,1, false);
      packedCluster = static_cast<buffer_word>(vcw.value());
      if ( isVerbose )
	    {
	      verbose() <<"STRIP: " << clu->strip(0)
                  << ",ADC: " << clu->adcValue( 0) << endmsg;
	    }

      SiADCWord aw(clu->adcValue(0),true);
      rowData |= (aw.value() << ((nAdc % 4) << 3));

      ++nAdc;
      if (nAdc % 4 == 0)
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
      unsigned int i;
      for ( i=0 ; i<stripSignals.size(); ++i) 
	    {
        unsigned int stripNumber = stripSignals[i].first;
        double adcCount = stripSignals[i].second;
     
        // ...
        adcCount = (adcCount>127 ? 127 : adcCount);
	   
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

        rowData |= (aw.value() << ((nAdc % 4) << 3));

        ++nAdc;
        if (nAdc % 4 == 0)
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
      VeloClusterWord vcw(channelPos, interStripPos, numStrips, false);
      packedCluster = static_cast<buffer_word>(vcw.value());

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
    cluRowData |= (packedCluster << ((nClu % 2) << 4));
    ++nClu;
    if ( nClu % 2 == 0 ) 
    {
      m_clusterPosBuffer.push_back( cluRowData );
      // clear the cluster buffer 
      cluRowData = 0x0 ; 
    }
    
  }// end for 

  // finished loooping over clusters 

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
  std::vector<buffer_word>::iterator tmpCp;
  for ( tmpCp = m_clusterPosBuffer.begin() ; tmpCp != m_clusterPosBuffer.end() ; ++tmpCp )
  {      
    if ( isVerbose  ) 
    {

      verbose()<< "Cluster position[" << format( "%10X" , *tmpCp ) << "]"<<endmsg;
    }

    m_rawData.push_back ( *tmpCp ) ;
  }
  
  // add adc values 
  std::vector<buffer_word>::iterator tmpAdc;
  for ( tmpAdc = m_clusterADCBuffer.begin() ; tmpAdc != m_clusterADCBuffer.end() ; ++tmpAdc )
  {
    if ( isVerbose ) 
    {

      verbose()<< "Adc row[" << format( "%10X" , *tmpAdc ) << "]"<<endmsg;
    }

    m_rawData.push_back ( *tmpAdc ) ;
  }

  if ( isVerbose) 
  {
    verbose()<< "Raw  output of cluster end" <<endmsg;
  }

  return nClu;
}

