// $Id: PrepareVeloL1BufferFPGA.cpp,v 1.4 2004-10-26 15:10:51 dhcroft Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/RndmGenerators.h"

#include "Event/L1Buffer.h"
#include "Event/MCVeloFE.h"
#include "Event/VeloCluster.h"

// local
#include "PrepareVeloL1BufferFPGA.h"
#include "L1VeloFPGACluster.h"
#include "L1VeloFPGA.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrepareVeloL1BufferFPGA
//
// 2003-04-11 : Olivier Callot / Niels Tuning
// 2004-03-06 : modified by Chris Parkes, moved to Velo area, IT parts removed
// 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PrepareVeloL1BufferFPGA>          s_factory ;
const        IAlgFactory& PrepareVeloL1BufferFPGAFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrepareVeloL1BufferFPGA::PrepareVeloL1BufferFPGA( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ) 
  , m_hitthreshold_ston    ( 2.0 )
  , m_doslopecorrection    ( 1 )
  , m_cluthreshold_ston    ( 5.0 )
  , m_relthreshold_neig    ( 0.2 )
{
  declareProperty( "HitThreshold"         , m_hitthreshold_ston);
  declareProperty( "DoSlopeCorrection"    , m_doslopecorrection);
  declareProperty( "ClusterThreshold"     , m_cluthreshold_ston);
  declareProperty( "RelThresholdNeighbour", m_relthreshold_neig);
  declareProperty( "ChargeThreshold"      , m_chargeThreshold = 12000. );
}

//=============================================================================
// Destructor
//=============================================================================
PrepareVeloL1BufferFPGA::~PrepareVeloL1BufferFPGA() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PrepareVeloL1BufferFPGA::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;

 //== Get detector element
  SmartDataPtr<DeVelo> velo( detDataService(), "/dd/Structure/LHCb/Velo" );
  if ( 0 == velo ) {
    msg << MSG::ERROR << "Unable to retrieve Velo detector element." << endreq;
    return StatusCode::FAILURE;
  }
  m_velo = velo;

  // random number initialisation
  
  StatusCode sc=m_gaussDist.initialize( randSvc(), Rndm::Gauss(0.,1.0));
  if ( !(sc) ) {
    msg << MSG::ERROR << "Random number init failure" << endreq;
    return StatusCode::FAILURE;
  }
 
  msg << MSG::INFO
      << "HitThreshold          = " << m_hitthreshold_ston << endreq
      << "DoSlopeCorrection     = " << m_doslopecorrection << endreq
      << "ClusterThreshold      = " << m_cluthreshold_ston << endreq
      << "RelThresholdNeighbour = " << m_relthreshold_neig << endreq
      << "ChargeThreshold       = " << m_chargeThreshold   << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrepareVeloL1BufferFPGA::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;

  bool verbose = msg.level() <= MSG::VERBOSE;
  bool debug   = msg.level() <= MSG::DEBUG;

  // Get the input container
  SmartDataPtr<MCVeloFEs> fe ( eventSvc() , MCVeloFELocation::Default );
  if( 0 == fe ) {
    msg << MSG::ERROR
        << "Unable to retrieve input data container="
        << " MCVeloFELocation::Default "  << endreq;
    return StatusCode::FAILURE;
  }

  SmartDataPtr< VeloClusters > clusters ( eventSvc(), 
                                          VeloClusterLocation::Default );
  if( 0 == clusters ) {
    msg << MSG::ERROR << "-- Unable to retrieve VeloClusters --" << endreq;
    return StatusCode::FAILURE;
  }

  // Get the L1 output buffer. Create it if needed
  L1Buffer* buffer;
  
  SmartDataPtr< L1Buffer > theL1Buffer ( eventSvc(), 
                                         L1BufferLocation::Default );
  if ( 0 == theL1Buffer ) {
    buffer = new L1Buffer();
    eventSvc()->registerObject( L1BufferLocation::Default, buffer );
  } else {
    buffer = theL1Buffer;
  }
  
  //== First, do the clustering
  int    sensor;
  int    strip;
  double charge;
  
  // NT 19/11/02: Divide strips on sensor in to gorups of 32 strips.
  // (The common mode correction inside the FPGAs is performed on groups of 32.)
  // A group of 32 strips is called a FPGA.
  //---------------------------------------------------------------------------
  int nStripPerFPGA    = 32;
  int nFPGAperSensor   = 2048/nStripPerFPGA ;
  int idFPGA           = -999;
  int countStrips      = 0;
  int previousFPGA     = -999;
  int previousSensor   = -999;
  int previousStrip    = -999;
  int nStripshit       = 0;
  int nFPGAs           = 0;
  int firstStripOfFPGA = -999;
  int nClusInSensor    = 0;
  
  std::vector<MCVeloFE*>  groupOfMCVeloFE;
  groupOfMCVeloFE.clear();

  std::vector<L1VeloFPGACluster*> myClus;
  myClus.clear();

  // Loop over MCVeloFEs
  // NB: Strips should be ordered according to sensor and strip number!!
  // -------------------------------------------------------------------
  for ( MCVeloFEs::const_iterator feIt = fe->begin() ; 
        fe->end() >= feIt ; feIt++ ) {

    if ( fe->end() == feIt ) {  //== Extra iteration to flush the FPGA
      strip = 999;
      sensor = 999;
    } else {
      countStrips++;
      charge = (*feIt)->charge();
      strip  = (*feIt)->strip();
      sensor = (*feIt)->sensor();
      if ( verbose ) msg << MSG::VERBOSE
                       << " NT Loop over MCVeloFEs: " << sensor 
                       << " " << strip << " " << countStrips 
                       << " " << charge << endreq;    
    }
    if (sensor != previousSensor && countStrips > 1) {
      msg << MSG::DEBUG
          << " NT Sensor "  <<  previousSensor
          << " has " << nClusInSensor << " clusters."
          << endreq;
      nClusInSensor = 0;
    }
    // Identifier for the group of 32: 
    // (64 groups of 32 strips per sensor of 2048 strips)
    // (21 stations -> 84 sensors -> 5376 groups)
    // --------------------------------------------------
    idFPGA = nFPGAperSensor*sensor + int(strip/nStripPerFPGA);
    
    // Initialise:
    if ( 1 == countStrips){
      nStripshit    = 1;
      previousFPGA  = idFPGA;
      previousStrip = strip;
      previousSensor= sensor;
    }
    // New FPGA:
    if (idFPGA != previousFPGA && countStrips > 1){
      // First strip of the group (ie. multiple of 32):
      firstStripOfFPGA = nStripPerFPGA*int(previousStrip/nStripPerFPGA);

      // Create FPGA object, ie. group of 32 strips:
      // --------------------------------------------
      msg << MSG::VERBOSE
          << " ---- NT make FPGA object " << nFPGAs << " " << firstStripOfFPGA 
          << " " << groupOfMCVeloFE.size()
          <<  endreq;      
      L1VeloFPGA myFPGA(firstStripOfFPGA, groupOfMCVeloFE);

      // Add random noise in surrounding strips:
      // --------------------------------------------
      msg << MSG::VERBOSE << " NT setNoise           " << nFPGAs <<  endreq;
      myFPGA.setNoise( m_gaussDist );

      // Apply Linear Common Mode Suppression (See LHCB Note 2001-034):
      // --------------------------------------------------------------
      msg << MSG::VERBOSE << " NT cmCorrection       " << nFPGAs <<  endreq;
      myFPGA.cmCorrection(m_hitthreshold_ston, m_doslopecorrection);

      // Make clusters per 32 channels in FPGA:
      // --------------------------------------
     
      msg << MSG::VERBOSE << " NT clustering         " << nFPGAs << endreq;
      myFPGA.clustering(m_cluthreshold_ston, m_relthreshold_neig);

      //loop over found clusters:
      int nClustersPerFPGA = (myFPGA.FPGAClusters()).size();
      nClusInSensor       += nClustersPerFPGA;
      msg << MSG::VERBOSE << " NT loop over clusters " << nFPGAs 
          << " " << nClustersPerFPGA <<  endreq;      
      for ( int iclus = 0 ; iclus < nClustersPerFPGA ; iclus++ ) {
        //L1VeloFPGACluster myclus = myFPGA.FPGAClusters()[iclus];
        L1VeloFPGACluster tmpclus = myFPGA.FPGAClusters()[iclus];
        L1VeloFPGACluster* myclus = new L1VeloFPGACluster( tmpclus );
        myClus.push_back(myclus);
        msg << MSG::VERBOSE
            << " PrepareVeloL1BufferFPGA: " << sensor 
            << " " << firstStripOfFPGA 
            << " " << iclus << " " << myclus->strip() << " " << tmpclus.strip()
            << endreq;
        //delete myclus;        
      } // end loop over clusters in FPGA 
      msg << MSG::VERBOSE
          << " NT Group "  << idFPGA 
          << " has found " << nClustersPerFPGA << " clusters."
          << endreq;
      groupOfMCVeloFE.clear();
      nFPGAs++;
      previousFPGA  = idFPGA;
      previousStrip = strip;
      nStripshit    = 0; 
    } // end if new FPGA (i.e. new group of 32 channels) 

    if ( fe->end() != feIt ) {  //== Extra iteration to flush the FPGA
      nStripshit++;
      groupOfMCVeloFE.push_back(*feIt);
      msg << MSG::VERBOSE << "      NT groupOfMCVeloFE.push_back " 
	  << " " << countStrips << " " << nStripshit 
	  << " " << groupOfMCVeloFE.size() <<   endreq;
    
      previousSensor = sensor; // in case a sensor nr is required in ntuple
    }
  } // end loop over MCVeloFEs
  //--------------------------
    
  msg << MSG::DEBUG
      << " Nr FPGAs (ie groups of 32 with a hit): " << nFPGAs
      << " All L1 clusters: " <<  myClus.size()
      << endreq;



  //== Then sort the clusters by z position and strip number
  
  std::sort( myClus.begin(), myClus.end(), 
             PrepareVeloL1BufferFPGA::sortClustersByPosition(m_velo) );
  
  //== Then build the L1 Event

  int nbOk = 0;

  int lastSensor  = 0; // start with Sensor 0, first non-pile-up sensor 
  int sourceID = 0;
  int numberOfR = 0;
  int numberOfPhi = 0;
  
  int nClusterInSensor = 0;
  int bankType = 0;
  std::vector<l1_int> data;
  
  std::vector<DeVeloSensor*> vpSensors = m_velo->vpSensors();
  

  // setup bank parameters for first sensor
  if ( m_velo->isRSensor(lastSensor)) {
    sourceID = numberOfR++;
    bankType = L1Buffer::VeloR;
  } else {
    msg << MSG::ERROR << "First Sensor is not an R sensor " << endreq; 
  }

  for( unsigned int iclus = 0 ; iclus < myClus.size()  ; iclus++ ) {
    // loop over clusters
    L1VeloFPGACluster* clu = myClus[iclus];    
    int sensor   = clu->sensor(); // get sensor number
    msg << MSG::DEBUG 
        << " NT L1VeloFPGACluster " << iclus 
        << " " <<  sensor << " " << clu->strip() << endreq;    
    if ( lastSensor != sensor ) {
      // cluster is on a different sensor so store bank
      msg << MSG::DEBUG 
          << format( "Sensor %3d size %4d words %5d ID %2d type %2d", 
                     lastSensor, nClusterInSensor, 
                     data.size(), sourceID, bankType ) 
          << endreq;
      buffer->addBank( sourceID, bankType, data );
      msg << MSG::VERBOSE<<"added Bank for sensor " << lastSensor 
	  << " of type " << bankType << endreq;
      data.clear();

      // store empty banks for any sensors  between last one and 
      // new cluster's sensor
      int lastSensorIndex=m_velo->sensorIndex(lastSensor);
      int sensorIndex=m_velo->sensorIndex(sensor);
      msg << MSG::DEBUG << "lastsensor " << lastSensor << " lastSensorIndex " 
	  << lastSensorIndex << " sensor " << sensor 
	  << " sensor index " << sensorIndex << endreq; 
      while (  ++lastSensorIndex != sensorIndex ) {
	 lastSensor=vpSensors[lastSensorIndex]->sensorNumber();
         if ( m_velo->isRSensor( lastSensor )  ){
	  sourceID = numberOfR++;
          bankType = L1Buffer::VeloR;
	  buffer->addBank( sourceID, bankType, data );
	  msg << MSG::VERBOSE<<"added empty Bank for sensor " << lastSensor 
	      << " of type " << bankType << endreq;
        } else {
         if ( m_velo->isPhiSensor( lastSensor )  ){
          sourceID = numberOfPhi++;
          bankType = L1Buffer::VeloPhi;
	  buffer->addBank( sourceID, bankType, data );
	  msg << MSG::VERBOSE<<"added empty Bank for sensor " << lastSensor 
	      << " of type " << bankType << endreq;
	 }
	 // Unused sensor number. Do not store bank.
        }              
      }
      lastSensor=vpSensors[lastSensorIndex]->sensorNumber();

      // setup bank parmaeters for new cluster's sensor
      nClusterInSensor = 0;
      if ( m_velo->isRSensor(lastSensor) ) {
        sourceID = numberOfR++;
        bankType = L1Buffer::VeloR;
      } else {
        sourceID = numberOfPhi++;
        bankType = L1Buffer::VeloPhi;
      }
    }
    // max number of clusters in a bank
    if ( 126 <= nClusterInSensor ) continue;

    // max cluster size
    int size       = clu->nStrips();
    if ( 3 < size ) continue;
    
    // multiple strips in cluster 
    l1_int word    = clu->strip();
    if ( clu->nStrips() > 1 ) { 
      word += 1 << 14;
    }

    // High signal threshold
    if ( m_chargeThreshold < clu->charge() ) {
      word += 1 << 13; 
    }

    nClusterInSensor ++;
    data.push_back( word );
    msg << MSG::VERBOSE << format( "Coded : %4x ", word ) << endreq;
    nbOk++;
  } // end loop over L1VeloFPGAClusters 

  if ( debug ) {
    msg << MSG::DEBUG 
        << format( "Sensor %3d size %4d words %5d ID %2d type %1d", 
                   lastSensor, nClusterInSensor, data.size(), 
                   sourceID, bankType ) 
        << endreq;
  }
  // add last cluster bank
  buffer->addBank( sourceID, bankType, data );
  msg << MSG::VERBOSE<<"added Bank for last sensor with data " << lastSensor 
      << " of type " << bankType << endreq;

  // fill empty banks for any remaining sensors 
  data.clear(); 
  int lastSensorIndex = m_velo->sensorIndex(lastSensor);
  int finalSensorIndex = vpSensors.size(); finalSensorIndex--;
  if (finalSensorIndex!=lastSensorIndex){
    while (  ++lastSensorIndex != finalSensorIndex ) {
      lastSensor=vpSensors[lastSensorIndex]->sensorNumber();
      if ( m_velo->isRSensor( lastSensor )  ){
	sourceID = numberOfR++;
	bankType = L1Buffer::VeloR;
	buffer->addBank( sourceID, bankType, data );
	msg << MSG::VERBOSE<<"added empty Bank for remaining sensor " 
	    << lastSensor << " of type " << bankType << endreq;
      } else {
	if ( m_velo->isPhiSensor( lastSensor )  ){
	  sourceID =  numberOfPhi++;
	  bankType = L1Buffer::VeloPhi;
	  buffer->addBank( sourceID, bankType, data );
	  msg << MSG::VERBOSE<<"added empty Bank for remaining sensor " 
	      << lastSensor << " of type " << bankType << endreq;
	}
	// Unused sensor number. Do not store bank.
      }
    }           
  }

  msg << MSG::DEBUG << "--- NbOK = " << nbOk << endreq;
  
  // ---------------------
  // --- Clear clusters:
  // ---------------------
  for( unsigned int iclus = 0 ; iclus < myClus.size()  ; iclus++ ) {
    delete myClus[iclus];    
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrepareVeloL1BufferFPGA::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
