// $Id: PrepareVeloRawBuffer.cpp,v 1.2 2004-11-29 14:16:06 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// From event
#include "Event/RawBuffer.h"

// local
#include "PrepareVeloRawBuffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrepareVeloRawBuffer
//
// 2003-04-11 : Olivier Callot
// 2004-04-13 : modified and moved to Velo area Chris Parkes
// 2004-11-03 : modified to GaudiAlg dependance
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PrepareVeloRawBuffer>          Factory ;
const        IAlgFactory& PrepareVeloRawBufferFactory = Factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrepareVeloRawBuffer::PrepareVeloRawBuffer( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
{
 
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

 //== Get detector element
  SmartDataPtr<DeVelo> velo( detDataService(), "/dd/Structure/LHCb/Velo" );
  if ( 0 == velo ) {
    error() << "Unable to retrieve Velo detector element." << endreq;
    return StatusCode::FAILURE;
  }
  m_velo = velo;
 
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrepareVeloRawBuffer::execute() {

  debug() << "==> Execute" << endreq;

  bool isVerbose = msgLevel( MSG::VERBOSE );
  bool isDebug   = msgLevel( MSG::DEBUG );

  // Get the input container
  SmartDataPtr< VeloClusters > clusters ( eventSvc(), 
                                          VeloClusterLocation::Default );
  if( 0 == clusters ) {
    error() << "-- Unable to retrieve VeloClusters --" << endreq;
    return StatusCode::FAILURE;
  }
  debug() << "retrieved " <<   clusters->size() 
	  << " VeloClusters" << endreq;
  //VeloDaqBuffer* outBuf = new VeloDaqBuffer();
  //eventSvc()->registerObject( "Event/DAQ/Velo", outBuf );

  // Get the RAW output buffer. Create it if needed
  RawBuffer* buffer;
  
  SmartDataPtr< RawBuffer > theRawBuffer ( eventSvc(), 
                                           RawBufferLocation::Default );
  if ( 0 == theRawBuffer ) {
    buffer = new RawBuffer();
    eventSvc()->registerObject( RawBufferLocation::Default, buffer );
  } else {
    buffer = theRawBuffer;
  }
  
  std::vector<VeloCluster*> myClus;
  myClus.reserve( clusters->size() );

  std::vector< std::pair<long,double> >::const_iterator strIt;

  //== First, apply a threshold on the cluster charge

  VeloClusters::const_iterator clIt;
  for ( clIt = clusters->begin(); clusters->end() != clIt; clIt++ ) {
    VeloCluster* clu = *clIt;

    double sum   = 0.;
    for ( strIt = clu->stripSignals().begin() ; 
          clu->stripSignals().end() != strIt ; strIt++ ) {
      double adc = (*strIt).second; 
      sum   += adc;
    }
    myClus.push_back( clu );
  }

  //== Then sort the clusters by position
  
  std::sort( myClus.begin(), myClus.end(), 
             PrepareVeloRawBuffer::sortClustersByID() );
  
  //== Then build the Raw Event

  int nbOk = 0;

  int lastSensor  = -1;
  int nClusterInSensor = 0;
  int bankType = RawBuffer::Velo;
  std::vector<raw_int> data;

  for ( clIt = myClus.begin(); myClus.end() != clIt; clIt++ ) {
    VeloCluster* clu = *clIt;
    int sensor   = clu->sensor();
    if ( lastSensor != sensor ) {
      if ( 0 <= lastSensor ) {
        if ( isDebug ) {
          debug() 
              << format( "Sensor %3d nClusters %4d words %5d", 
                         lastSensor, nClusterInSensor, data.size() ) 
              << endreq;
        }
        buffer->addBank( lastSensor, bankType, data );
      }
      data.clear();
      while ( ++lastSensor != sensor ) {

	// sensor numbers are discontinuous, skip 40->63 
	// (gap between R and phi) numbers
	if( 39 < lastSensor && lastSensor < 64 ) { continue;}
	// this is an ugly hack: please fix properly

        if ( isDebug ) {
          debug() 
              << format( "Sensor %3d no clusters words %5d", 
                         lastSensor, nClusterInSensor, data.size() ) 
              << endreq;
        }
        buffer->addBank( lastSensor, bankType, data );
      }
      nClusterInSensor = 0;
    }
    nClusterInSensor ++;

    if ( isVerbose ) verbose() << format(" Cluster ");
    int size       = clu->size();
    int firstStrip = clu->strip(0);
    raw_int word   = size + (firstStrip << 12);
    int nInWord    = 24;
    if ( isVerbose ) verbose() 
                       << format(" Cluster Size %2d First Strip %4d",
				 size, firstStrip);
    for ( strIt = clu->stripSignals().begin() ; 
          clu->stripSignals().end() != strIt ; strIt++ ) {
      if ( isVerbose ) verbose() <<  " ADC " << (*strIt).second;
      word    += ( int( (*strIt).second ) << nInWord );
      nInWord += 8;
      if ( 32 == nInWord ) {
        data.push_back( word );
        if ( isVerbose ) verbose() << format( " word %8x ", word );
        nInWord = 0;
        word = 0;
      }
    }
    if ( 0 != nInWord ) {
      data.push_back( word );
      if ( isVerbose ) verbose() << format( " word %8x ", word );
    }

    if ( isVerbose ) verbose() << endreq;
    nbOk++;
  }

  if ( isDebug ) {
    debug() 
        << format( "Sensor %3d size %4d words %5d", 
                   lastSensor, nClusterInSensor, data.size() ) 
        << endreq;
  }
  buffer->addBank( lastSensor, bankType, data );
  data.clear();
  
  while ( m_velo->nbSensor() > ++lastSensor ) {
    if ( isDebug ) {
      debug() 
	<< format( "Sensor %3d no clusters words %5d", 
		   lastSensor, nClusterInSensor, data.size() ) 
	<< endreq;
        }
      buffer->addBank( lastSensor, bankType, data );
  }  

  debug() << "--- NbOK = " << nbOk << endreq;
  
  return StatusCode::SUCCESS;
};

