// Include files 

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "STTAEClusterTuple.h"
#include "Event/STCluster.h"
#include "Kernel/STDetSwitch.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STBoardMapping.h"

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

#include "Event/ODIN.h"

// from Boost
#include <boost/assign/list_of.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : STTAEClusterTuple
//
// 2009-03-10 : Mark Tobin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace ST {
  DECLARE_ALGORITHM_FACTORY( STTAEClusterTuple )
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ST::STTAEClusterTuple::STTAEClusterTuple( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : ST::TupleAlgBase ( name , pSvcLocator )
{
  // Ordered list of cluster locations
  std::vector<std::string> tmp =
    boost::assign::list_of("Event/Prev7/Raw/TT/Clusters")
    ("Event/Prev6/Raw/TT/Clusters")
    ("Event/Prev5/Raw/TT/Clusters") 
    ("Event/Prev4/Raw/TT/Clusters")
    ("Event/Prev3/Raw/TT/Clusters")
    ("Event/Prev2/Raw/TT/Clusters")
    ("Event/Prev1/Raw/TT/Clusters")
    ("Raw/TT/Clusters")
    ("Event/Next1/Raw/TT/Clusters")
    ("Event/Next2/Raw/TT/Clusters")
    ("Event/Next3/Raw/TT/Clusters")
    ("Event/Next4/Raw/TT/Clusters")
    ("Event/Next5/Raw/TT/Clusters")
    ("Event/Next6/Raw/TT/Clusters")
    ("Event/Next7/Raw/TT/Clusters");
  declareProperty( "ClusterLocations", m_clusterLocations=tmp);
  m_skipShortThick = false;

}
//=============================================================================
// Destructor
//=============================================================================
ST::STTAEClusterTuple::~STTAEClusterTuple() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ST::STTAEClusterTuple::initialize() {

  StatusCode sc = ST::TupleAlgBase::initialize(); 
  if ( sc.isFailure() ) return sc;

  m_debug = msgLevel( MSG::DEBUG );
  m_verbose = msgLevel( MSG::VERBOSE);
  if(m_verbose) m_debug = true;
  if (m_debug) debug() << "==> Initialize" << endmsg;
  if( "" == histoTopDir() ) setHistoTopDir(this->detType()+"/");
  
  m_nSamples = m_clusterLocations.size();
  // Time aligned events (Central +/- TAE)
  if(m_nSamples%2 == 0) {
    error() << "Expected odd number of Cluster Locations, got " << m_nSamples << endmsg;
    return StatusCode::FAILURE;
  }
  m_maxSample = (m_nSamples+1)/2 - 1 + 0.5;

  // Loop over input locations and fill label vector
  std::vector<std::string> tmp = boost::assign::list_of("Prev7")("Prev6")("Prev5") ("Prev4")("Prev3")("Prev2")("Prev1")
    ("Default")("Next1")("Next2")("Next3")("Next4")("Next5")("Next6")("Next7");
  m_spills = tmp;
  unsigned int diff=m_spills.size()-m_clusterLocations.size();
  m_spills.erase(m_spills.begin(),m_spills.begin()+diff/2);
  m_spills.resize(m_clusterLocations.size());

  std::vector<std::string>::iterator itCL;
  /* Loop over input locations 
     Couldn't find a good way to do this in the base class yet */
  for(itCL = m_clusterLocations.begin(); itCL != m_clusterLocations.end(); ++itCL) {
    STDetSwitch::flip(detType(), (*itCL));
    if(m_debug) debug() << "Cluster Locations: " << (*itCL) << endmsg;
  }
  
  if(this->detType() == "IT") m_skipShortThick = true;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ST::STTAEClusterTuple::execute() {

  debug() << "==> Execute" << endmsg;
  std::vector<Tuple> tuples;
  tuples.clear();
  std::vector<std::string>::const_iterator itSvcBoxes = readoutTool()->serviceBoxes().begin();
  unsigned int index = 0;
  std::map<std::string, unsigned int> tupleIndex;
  for( ; itSvcBoxes != readoutTool()->serviceBoxes().end(); ++itSvcBoxes, ++index) {
    std::string svcBox=(*itSvcBoxes);
    //    tuples[svcBox] = nTuple("TAEClusters"+svcBox);
    tuples.push_back( nTuple(svcBox) );
    tupleIndex[svcBox] = index;
  }

  counter("Number of events") += 1; 
  // Select the correct bunch id
  const LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );

  debug() << counter("Number of events") << endmsg;
  // code goes here  

  std::vector<std::string>::const_iterator itCL;
  // Loop over input locations
  unsigned int iSample=0;
  for(itCL = m_clusterLocations.begin(); itCL != m_clusterLocations.end(); ++itCL, ++iSample) {
    // Check location exists
    if(exist<LHCb::STClusters>(*itCL)){
      LHCb::STClusters* clusters = get<LHCb::STClusters>(*itCL);
      LHCb::STClusters::const_iterator itClus;
      // Loop over clusters
      if(m_debug) {
        debug() << "Number of clusters in " << (*itCL) << " is " << (clusters->size()) << endmsg;
      }
      int sample = static_cast<int>(iSample-(m_nSamples-1)/2);
      for(itClus = clusters->begin(); itClus != clusters->end(); ++itClus) {
        const LHCb::STCluster* cluster = (*itClus);

        LHCb::STChannelID chanID = cluster->channelID();
        DeSTSector* sector = findSector( chanID );
        double noise = sector->noise( chanID );
        if(noise == 0) continue;
        
        // find which ladders are short and thick.. and skip those...
        if ( m_skipShortThick && sector->thickness() > 350.*Gaudi::Units::um && sector->type() == "Short" ) continue;
        
        // Loop over the strips in this cluster and find the highest ADC
        unsigned int highestADC = 0;
        //int highestStrip = 0;
        //highestStrip = 0;

        bool foundBadChannel = false;
        LHCb::STCluster::ADCVector adcVec = cluster->stripValues();
        LHCb::STCluster::ADCVector::const_iterator iVec = adcVec.begin();
        for( ; iVec < adcVec.end(); ++iVec ) {
          if( iVec->second > highestADC ) {
            //highestStrip = iVec->first;
            highestADC = iVec->second;
            // Check for bad channels
            LHCb::STChannelID iChan = LHCb::STChannelID(int(chanID) + iVec->first);
            if( !findSector( iChan )->isOKStrip( iChan ) ) foundBadChannel = true;
          } 
        }
        // If there is any strip in this cluster that is bad: ignore cluster
        if( foundBadChannel ) {
          counter("Number of bad channels found") += 1; 
          continue;
        }

       // Determine the layer
        int layer = (cluster->isTT()) ? 
          cluster->layer()+2*(cluster->station()-1) : cluster->layer();
        
        unsigned int sourceID = cluster->sourceID();
        unsigned int tell1ID = readoutTool()->SourceIDToTELLNumber(sourceID);
        std::string svcBox = readoutTool()->serviceBox(cluster->channelID());
        unsigned int index  = tupleIndex[svcBox];
        tuples[index]->column("sourceID", sourceID );
        tuples[index]->column("tell1ID", tell1ID );
        tuples[index]->column("sectorID", chanID.uniqueSector() );
        tuples[index]->column("sectorNr", chanID.sector() );
        tuples[index]->column("thickness", sector->thickness() );
        tuples[index]->column("nSensors", (int)(sector->sensors()).size() );
        tuples[index]->column("channelID", chanID );
        tuples[index]->column("layer", layer );
        tuples[index]->column("strip", cluster->strip() );
        tuples[index]->column("clusSize", cluster->size() );
        tuples[index]->column("clusCharge", cluster->totalCharge() );
        tuples[index]->column("stn", cluster->totalCharge()/noise );        
        tuples[index]->column("maxCharge", cluster->maxADCValue() );
        tuples[index]->column("spill", sample );
        tuples[index]->column("step", odin->calibrationStep() );
        tuples[index]->column("BXID", odin->bunchId() );
        tuples[index]->write();

      }

      // fill the tuple
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ST::STTAEClusterTuple::finalize() {

  debug() << "==> Finalize" << endmsg;

  return ST::TupleAlgBase::finalize(); 
}

