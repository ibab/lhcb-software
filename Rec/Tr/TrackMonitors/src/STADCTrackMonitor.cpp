// Include files 
#include "STADCTrackMonitor.h"

//event
#include "Event/Track.h"
#include "Event/TrackFitResult.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "Event/ODIN.h"
#include "Event/VeloCluster.h"
#include "Event/VeloPhiMeasurement.h"
#include "Event/VeloRMeasurement.h"
#include "Event/KalmanFitResult.h"
#include "Kernel/HitPattern.h"
#include "TrackInterfaces/IHitExpectation.h"
#include "TrackInterfaces/IVeloExpectation.h"

// Det
#include "STDet/DeSTDetector.h"
#include "STDet/DeITDetector.h"
#include "VeloDet/DeVelo.h"
#include "Event/FitNode.h"
#include "Event/RecVertex.h"


// ST DAQ
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/LHCbConstants.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STDAQDefinitions.h"

// STTELL1Event
#include "Event/STTELL1Data.h"
#include "Kernel/STTell1Board.h"

// gsl
#include "gsl/gsl_math.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToStream.h"

#include <map>
#include <stdlib.h>
#include <time.h>
#include "Map.h"
#include <sstream>
#include <iostream>

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::lambda;
using namespace LHCb;
using namespace Gaudi;

//-----------------------------------------------------------------------------
// Implementation file for class : STADCTrackMonitor
//
// 2013-06-17 : Christian Elsasser
// 2015-11-18 : Elena Graverini
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( STADCTrackMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STADCTrackMonitor::STADCTrackMonitor(const std::string& name,
                           ISvcLocator* pSvcLocator ) :
  TrackMonitorTupleBase( name , pSvcLocator )
{
  declareProperty("DetType",m_detType,"TT");
  declareProperty("Layer",m_layer,"TTaX");
  declareProperty("RawDataCent",m_rawDataLocCent,"/Event/Raw/TT/LCMSADCs");
  
  declareProperty("ODINData",m_odinLoc,"DAQ/ODIN");

  declareProperty("MaxNeighbours",m_maxNeighbours=3);
  

  // TT layer mapping
  m_layerMap.insert(std::pair<std::string, unsigned int>("TTaX",9));
  m_layerMap.insert(std::pair<std::string, unsigned int>("TTaU",10));
  m_layerMap.insert(std::pair<std::string, unsigned int>("TTbV",17));
  m_layerMap.insert(std::pair<std::string, unsigned int>("TTbX",18));

  // IT layer mapping
  m_layerMap.insert(std::pair<std::string, unsigned int>("T1X1",9));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T1U", 10));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T1V", 11));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T1X2",12));

  m_layerMap.insert(std::pair<std::string, unsigned int>("T2X1",17));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T2U", 18));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T2V", 19));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T2X2",20));

  m_layerMap.insert(std::pair<std::string, unsigned int>("T3X1",25));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T3U", 26));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T3V", 27));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T3X2",28));
}

//=============================================================================
// Destructor
//=============================================================================
STADCTrackMonitor::~STADCTrackMonitor() {}

StatusCode STADCTrackMonitor::initialize()
{
  // Mandatory initialization of GaudiAlgorith
  StatusCode sc = TrackMonitorTupleBase::initialize();

  
  

  if ( sc.isFailure() ) { return sc; }
  m_veloDet = getDet<DeVelo>(  DeVeloLocation::Default ) ;


  // Definition of readout tool
  if ( m_detType=="IT" ){
    m_readoutTool = tool<ISTReadoutTool>("ITReadoutTool");
  }else{
    m_readoutTool = tool<ISTReadoutTool>("TTReadoutTool");
  }
  
     
  m_veloExpectation = tool<IVeloExpectation>("VeloExpectation");
  m_ttExpectation   = tool<IHitExpectation>("TTHitExpectation");
  m_itExpectation   = tool<IHitExpectation>("ITHitExpectation");
  m_otExpectation   = tool<IHitExpectation>("OTHitExpectation");

  m_extrapolator    = tool<ITrackExtrapolator>("TrackParabolicExtrapolator");
  m_STDet           = getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));
  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode STADCTrackMonitor::execute()
{
  

  // Create Tuple
  Tuples::Tuple m_tuple_hit = GaudiTupleAlg::nTuple("HitInfo/"+m_layer,   "HitInfo",   CLID_ColumnWiseTuple );
  Tuples::Tuple m_tuple_noi = GaudiTupleAlg::nTuple("NoiseInfo/"+m_layer, "NoiseInfo", CLID_ColumnWiseTuple );

  // Check if ODIN is available, otherwise skip event
  bool isEverythingHere = true;

  if(!exist<LHCb::ODIN>(m_odinLoc)){ 
    warning() << "ODIN data are not available!" << endmsg;
    isEverythingHere = false;
  }

  if(!isEverythingHere){ 
    warning() << "Skip event!" << endmsg;
    return StatusCode::SUCCESS;
  }

  // get ODIN
  const LHCb::ODIN* odin = get<LHCb::ODIN>(m_odinLoc);
  int calibStepValue = odin->calibrationStep();
  debug() << "Value of ODIN calibration step is " << calibStepValue << "." << endmsg;

  // Check if track container is available,...
  if(!exist<LHCb::Track::Range>(inputContainer())){
    warning() << "Tracks are not available!" << endmsg;
    isEverythingHere = false;
  }

  // Check if data of this bunch crossing is available,...
  if(!exist<LHCb::STTELL1Datas>(m_rawDataLocCent)){
    warning() << "Central raw data are not available!" << endmsg;
    isEverythingHere = false;
  }

  // ... otherwise skip event, unless next/previous bunch crossing is excluded
  if(!isEverythingHere){ 
    warning() << "Skip event!" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  

  // get the input data
  LHCb::Track::Range tracks = get<LHCb::Track::Range>(inputContainer());

  // define data prefix
  std::vector<std::string> dataPrefix;

  // get raw data
  std::vector<LHCb::STTELL1Datas*> rawData;

  // get raw data locations
  rawData.push_back(get<LHCb::STTELL1Datas>(m_rawDataLocCent));
  dataPrefix.push_back("C");


  // Cast calibration step to string
  std::string calibStep = boost::lexical_cast<std::string>( calibStepValue );
  
  //for(MultiplicityMap::iterator it = m_multiplicityMap.begin() ;
  //    it != m_multiplicityMap.end(); ++it) it->second = 0 ;
  
 
  // Extract further event info
  int    runN     = odin->runNumber();
  int    evtN     = odin->eventNumber();
  int    BCID     = odin->bunchId();
  int    BCType   = odin->bunchCrossingType();

  
  // Vectors to store boards and strip assigned to tracks
  std::vector<unsigned int> boardVec;
  std::vector<unsigned int> stripVec;
  
  
  // Loop over tracks in event  
  BOOST_FOREACH( const LHCb::Track* track, tracks) {
    std::string type = histoDirName(*track) ;
    m_multiplicityMap[type] += 1;
    
    // Extract track momentum
    double trackMom = track->p();
    

    debug() << "Track type is " << track->type() << " with momentum " << trackMom << " MeV/c." << endmsg;

    // Check if track is long (type()=3) TO DO: change to Track::Long
    if(track->type()==Track::Long){
      debug() << "It's a long track!" << endmsg;

      // Extract track info
      double px       = track->momentum().x();
      double py       = track->momentum().y();
      double pz       = track->momentum().z();
      double pt       = track->pt();
      double trChi2   = track->chi2();
      int    ndof     = track->nDoF();
      double ghostP   = track->ghostProbability();
      int    nMeas    = track->nMeasurements();
      int    nIDs     = track->nLHCbIDs();

      

      
      // Get extrapolated hit position
      StateVector* hit = 0;
      if(m_detType=="TT"){
        hit = GetTTHit(track);
      }else{
        hit = GetITHit(track);
      }

      if(!hit){
        warning() << "Extrapolated hit not found! Skip event!" << endmsg;
        return StatusCode::SUCCESS;
      }

      // Extract hit 
      double xHit = hit->x();
      double yHit = hit->y();
      double zHit = hit->z();
      double dxdz = hit->tx();
      double dydz = hit->ty();
      double pHit = hit->p();
      
      // Get hits extrapolated from track
      std::vector<LHCb::LHCbID> ids(0);
      if(m_detType=="TT"){
        m_ttExpectation->collect(*track,ids);
      }else{
        m_itExpectation->collect(*track,ids);
      }
      
      delete hit;
      
      std::vector< LHCb::STChannelID > 	flt_ids;
      
      // Check if hit is in right detector and layer
      BOOST_FOREACH( LHCb::LHCbID chan, ids) {
        if (chan.isST() == true){
          if (m_detType == "IT" ){
            if (chan.isIT() == true && m_layerMap[m_layer]==chan.stID().uniqueLayer()) flt_ids.push_back(chan.stID()); 
            debug() << "Filtered id is:   " << chan.stID() << endmsg;
            debug() << "Unique layer is:  " << chan.stID().uniqueLayer() << endmsg;
            debug() << "Unique sector is: " << chan.stID().uniqueSector() << endmsg;
            
          }
          if (m_detType == "TT"){
            if (chan.isTT() == true && m_layerMap[m_layer]==chan.stID().uniqueLayer()) flt_ids.push_back(chan.stID());
            debug() << "Filtered id is:   " << chan.stID() << endmsg;
            debug() << "Unique layer is:  " << chan.stID().uniqueLayer() << endmsg;
            debug() << "Unique sector is: " << chan.stID().uniqueSector() << endmsg; 
          }
        }
      }

      // Loop over hits
      BOOST_FOREACH( LHCb::STChannelID chan, flt_ids){

        int sector = chan.uniqueSector();
        
        // This should be the interstrip fraction
        double isf = 0;
        
        STDAQ::chanPair aPair = m_readoutTool->offlineChanToDAQ(chan,isf);
        debug() << "Board " << aPair.first <<", Channel " << aPair.second << endmsg;
        unsigned int stripID = aPair.second;
        unsigned int boardID = aPair.first.id();
        
        if(stripID > 3071){
          warning() << "Strip ID is too high (>3071) or too low (<0)" << endmsg;
          continue;
        }
        
        // Loop over bunch crossings
        std::vector<std::string>::iterator prefixIter = dataPrefix.begin();
        BOOST_FOREACH( LHCb::STTELL1Datas* dataLoop,rawData){
          // Loop over TELL1 boards
          LHCb::STTELL1Datas::const_iterator iterBoard = dataLoop->begin();
          for(;iterBoard != dataLoop->end(); ++iterBoard){
            int nStripsBeetle = LHCbConstants::nStripsInBeetle;
            
            // Check if hit is associated to TELL1 board
            if ((*iterBoard)->TELL1ID() == boardID){
              // Register TELL1 board as used
              boardVec.push_back(boardID);
              
              
              std::string secName = boost::lexical_cast<std::string>( sector );
              
              const int stripInHybrid = stripID%(4*nStripsBeetle);

              // Filling track info
              debug() << "Filling track info" << endmsg;
              m_tuple_hit->column("p",        trackMom);
              m_tuple_hit->column("pt",       pt);
              m_tuple_hit->column("px",       px);
              m_tuple_hit->column("py",       py);
              m_tuple_hit->column("pz",       pz);
              m_tuple_hit->column("TrChi2",   trChi2);
              m_tuple_hit->column("TrNDoF",   ndof);
              m_tuple_hit->column("GhostP",   ghostP);
              m_tuple_hit->column("nMeas",    nMeas);
              m_tuple_hit->column("nLHCbIDs", nIDs);

              // Filling hit info
              debug() << "Filling hit info" << endmsg;
              m_tuple_hit->column("xHit",     xHit);
              m_tuple_hit->column("yHit",     yHit);
              m_tuple_hit->column("zHit",     zHit);
              m_tuple_hit->column("tx",       dxdz);
              m_tuple_hit->column("ty",       dydz);
              m_tuple_hit->column("pHit",     pHit);

              // Filling ODIN info
              debug() << "Filling ODIN info" << endmsg;
              m_tuple_hit->column("odinStep", calibStepValue);
              m_tuple_hit->column("runNumber",runN);
              m_tuple_hit->column("evtNumber",evtN);
              m_tuple_hit->column("bunchID",  BCID);
              m_tuple_hit->column("BCType",   BCType);
              
              
              // Filling sector Number
              debug() << "Filling sector number" << endmsg;
              m_tuple_hit->column("sector",   sector);
              
              
              int summedADCValue  = 0;
              // Loop over cluster sizes
              for(int nNeigh = 0; nNeigh<=m_maxNeighbours; nNeigh++){
                std::string nStrips = boost::lexical_cast<std::string>( nNeigh*2+1 );
                if (stripInHybrid > nNeigh-1 && stripInHybrid < (4*nStripsBeetle)-nNeigh){
                  debug() << "Fill ADC value for " << nStrips << " strips" << endmsg;
                  summedADCValue= GetADCValue((*iterBoard),stripID,nStripsBeetle,
                                        summedADCValue,nNeigh);
                  m_tuple_hit->column("val"+nStrips,summedADCValue);
                }else{
                  debug() <<  "Too close to boarder for next higher neighbour number!" << endmsg;
                  m_tuple_hit->column("val"+nStrips,-1000);
                }
              }

              // Write
              debug() << "Write signal tuple" << endmsg;
              StatusCode status = m_tuple_hit->write();
               if(status.isFailure()){
               return Error("Cannot fill signal ntuple");
              }
            }
          }
          ++prefixIter;
        }     
      }
    }
  }
  
  

  
  // Retrieve noise
  

  // Take random channel in TELL1 board
  std::vector<std::string>::iterator prefixIter = dataPrefix.begin();
  srand(time(NULL));
  unsigned int stripID = rand()%3072;
  // Loop over bunch crossings
  BOOST_FOREACH( LHCb::STTELL1Datas* dataLoop,rawData){
    // Loop over TELL1 boards
    LHCb::STTELL1Datas::const_iterator iterBoard = dataLoop->begin();
    for(;iterBoard != dataLoop->end(); ++iterBoard){
      // Check if board is not registered to have hit
      if (!(*iterBoard)) {
        error() << "Board has not been found!" << endmsg;
        continue;
      }
      
      if(boardVec.end()==(find(boardVec.begin(),boardVec.end(),(*iterBoard)->TELL1ID()))){
        debug() << "TELL1 Board " << (*iterBoard)->TELL1ID() << " not used" << endmsg;
        int nStripsBeetle = LHCbConstants::nStripsInBeetle;
        
        debug() << "Channel " << stripID << endmsg;
        if(stripID > 3071){
          warning() << "Strip ID is too high (>3071) or too low (<0)" << endmsg;
          continue;
        }
        
        

        // Check if TELL1 board belongs to considered layer
        debug() << "Check if TELL1 board belongs to considered layer" << endmsg;
        debug() << stripID << endmsg;
        std::vector<LHCb::STChannelID> sectorVec = m_readoutTool->sectorIDs(STTell1ID((*iterBoard)->TELL1ID()));
        if (stripID/(4*nStripsBeetle)>= sectorVec.size() || m_layerMap[m_layer]!=sectorVec[stripID/(4*nStripsBeetle)].uniqueLayer() ||
            ((m_detType=="TT" && sectorVec[stripID/(4*nStripsBeetle)].isIT()) || 
             (m_detType=="IT" && sectorVec[stripID/(4*nStripsBeetle)].isTT()))){
              debug() << "Channel is not available!" << endmsg;
              debug() << stripID/(4*nStripsBeetle) << ":" << sectorVec.size() << endmsg;
          continue;
        }


        // Filling ODIN info
        debug() << "Filling ODIN info" << endmsg;
        m_tuple_noi->column("odinStep", calibStepValue);
        m_tuple_noi->column("runNumber",runN);
        m_tuple_noi->column("evtNumber",evtN);
        m_tuple_noi->column("bunchID",  BCID);
        m_tuple_noi->column("BCType",   BCType);

        // Filling sector Number
        debug() << "Filling sector number" << endmsg;
        double sector = sectorVec[stripID/(4*nStripsBeetle)].uniqueSector();
        m_tuple_noi->column("sector",   sector);

        std::string secName = boost::lexical_cast<std::string>( sectorVec[stripID/(4*nStripsBeetle)].uniqueSector() );
        debug() << "Sector "<< secName << " used for noise!" << endmsg;
        const int stripInHybrid = stripID%(4*nStripsBeetle);

        int summedADCValue = 0;

        for(int nNeigh = 0; nNeigh<=m_maxNeighbours; nNeigh++){
          std::string nStrips = boost::lexical_cast<std::string>( nNeigh*2+1 );
          if (stripInHybrid > nNeigh-1 && stripInHybrid < (4*nStripsBeetle)-nNeigh){
            debug() << "Fill ADC value for " << nStrips << " strips" << endmsg;
            summedADCValue= GetADCValue((*iterBoard),stripID,nStripsBeetle,summedADCValue,nNeigh);
            m_tuple_noi->column("val"+nStrips,summedADCValue);
          }else{
            debug() <<  "Too close to boarder for next higher neighbour number!" << endmsg;
            m_tuple_noi->column("val"+nStrips,-1000);
          }
        }
        // Write
        debug() << "Write noise tuple" << endmsg;
        StatusCode status = m_tuple_noi->write();
         if(status.isFailure()){
         return Error("Cannot fill noise ntuple");
        }
        
        
      }
    }
    ++prefixIter;
  }
                                          



  


  
  debug() << "Increment counters" << endmsg;

  // fill counters....
  counter("#Tracks") += tracks.size();
  for(MultiplicityMap::const_iterator it = m_multiplicityMap.begin() ;
      it != m_multiplicityMap.end(); ++it) {
    counter("#"+it->first) += it->second;    
  } 

  
  
  
  debug() << "End of execute!" << endmsg;
  return StatusCode::SUCCESS;
}

void STADCTrackMonitor::findRefStates(const LHCb::Track& track,
                                 const LHCb::State*& firstMeasurementState,
                                 const LHCb::State*& lastMeasurementState) const
{
  firstMeasurementState = lastMeasurementState = 0  ;
  LHCb::Track::ConstNodeRange nodes = track.nodes() ;
  for( LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin() ;
       inode != nodes.end() ; ++inode) 
    if( (*inode)->type()==LHCb::Node::HitOnTrack ) {
      if( firstMeasurementState==0 ||
          (*inode)->z() < firstMeasurementState->z() )
        firstMeasurementState = &((*inode)->state()) ;
      if( lastMeasurementState==0 ||
          (*inode)->z() > lastMeasurementState->z() )
        lastMeasurementState = &((*inode)->state()) ;    }
}

 int STADCTrackMonitor::GetADCValue(LHCb::STTELL1Data* board,
                                    unsigned int stripID,
                                    int nStripsBeetle,
                                    int summedADCValue,
                                    int nNeigh
                                    )
 {
   if(nNeigh==0){
     summedADCValue += board->data()[(stripID)/nStripsBeetle][(stripID)%nStripsBeetle];
   }else{
     summedADCValue += board->data()[(stripID-nNeigh)/nStripsBeetle][(stripID-nNeigh)%nStripsBeetle];
     summedADCValue += board->data()[(stripID+nNeigh)/nStripsBeetle][(stripID+nNeigh)%nStripsBeetle];
   }
   return summedADCValue;
 }



StateVector* STADCTrackMonitor::GetTTHit(const LHCb::Track* track)
{

  const DeSTDetector::Layers& layers = m_STDet->layers();

  int layerIndex = m_layerMap[m_layer]-9;
  
  if(layerIndex>3 || layerIndex<0)
  {
    error() << "Layer index for IT out of bound: " << layerIndex << endmsg;
    return 0;
  }

  double zTT   =  layers[layerIndex]->globalCentre().z();

  const State& TTState = track->closestState(zTT);

  
  StateVector* stateVectorTT = new StateVector(TTState.position(), TTState.slopes());

  // loop over the sectors
  const DeSTDetector::Sectors& sectorVector = m_STDet->sectors();
  DeSTDetector::Sectors::const_iterator iterS = sectorVector.begin();
  for (; iterS != sectorVector.end(); ++iterS){
    if((*iterS)->elementID().uniqueLayer()==m_layerMap[m_layer]){
      m_extrapolator->propagate((*stateVectorTT),(*iterS)->globalCentre().z());
      return stateVectorTT;
    }
  }
  return 0;
}

StateVector* STADCTrackMonitor::GetITHit(const LHCb::Track* track)
{
  const DeSTDetector::Layers& layers = m_STDet->layers();

  int layerIndex = (m_layerMap[m_layer]-1)%4+(m_layerMap[m_layer]-9)/8*4;
  
  if(layerIndex>11 || layerIndex<0)
  {
    error() << "Layer index for IT out of bound: " << layerIndex << endmsg;
    return 0;
  }

  double zIT   =  layers[layerIndex]->globalCentre().z();

  const State& ITState = track->closestState(zIT);
  StateVector* stateVectorIT = new StateVector(ITState.position(), ITState.slopes());

  // loop over the sectors
  const DeSTDetector::Sectors& sectorVector = m_STDet->sectors();
  DeSTDetector::Sectors::const_iterator iterS = sectorVector.begin();
  for (; iterS != sectorVector.end(); ++iterS){
    if((*iterS)->elementID().uniqueLayer()==m_layerMap[m_layer]){
      m_extrapolator->propagate((*stateVectorIT),(*iterS)->globalCentre().z());
      return stateVectorIT;
    }
  }
  return 0;
}


 
