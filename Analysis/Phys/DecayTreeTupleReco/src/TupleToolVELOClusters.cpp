// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "TupleToolVELOClusters.h"
#include "Event/Track.h"
#include "Event/VeloCluster.h"
#include "Event/ODIN.h"

// local
#include "TupleToolVELOClusters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolVELOClusters
//
// 2009-02-11 : Patrick Koppenburg
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolVELOClusters::TupleToolVELOClusters( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : TupleToolBase ( type, name, parent )
{
  declareInterface<IEventTupleTool>(this);
}

//=============================================================================
// Destructor
//=============================================================================
TupleToolVELOClusters::~TupleToolVELOClusters() {}
//=============================================================================

//=============================================================================
StatusCode TupleToolVELOClusters::fill( Tuples::Tuple& tup )
{
  const std::string prefix = fullName();
  // Load the RecSummary object
  const LHCb::RecSummary * rS = 
    getIfExists<LHCb::RecSummary>(evtSvc(),LHCb::RecSummaryLocation::Default);
  if ( !rS ) 
  {
    rS = getIfExists<LHCb::RecSummary>(evtSvc(),LHCb::RecSummaryLocation::Default,false); 
  }
  // Fill the tuple
  bool test = true;
  std::vector<double> stations;
  std::vector<double> stationADC;

  std::vector<int> vids; vids.reserve(1000);
  LHCb::Tracks* tracksCont = get<LHCb::Tracks>( LHCb::TrackLocation::Default );
  if( tracksCont == NULL || tracksCont->size()==0 ){
    return -20000;
  }
  for (LHCb::Tracks::iterator iter = tracksCont->begin(); 
       iter != tracksCont->end(); ++iter){
    const std::vector<LHCb::LHCbID>& ids = (*iter)->lhcbIDs();
    for ( std::vector<LHCb::LHCbID>::const_iterator it = ids.begin(); it != ids.end(); ++it ){
      if (it->isVelo() == true) vids.push_back(it->veloID()); 
    } // lhcbids
  } // tracks
  
  std::sort(vids.begin(), vids.end());
  std::vector<int>::iterator u =  std::unique(vids.begin(), vids.end());
  vids.resize(u - vids.begin());

  LHCb::VeloClusters* clusters = get<LHCb::VeloClusters>( LHCb::VeloClusterLocation::Default );
  if( clusters == NULL || clusters->size()==0 ){
    return -10000;
  }
  // MDN loop over the clusters
  for ( LHCb::VeloClusters::iterator iterV = clusters->begin(); iterV != clusters->end(); ++iterV) {
    // check if used on track
    if (std::find(vids.begin(), vids.end(),(*iterV)->channelID()) == vids.end()){
        debug() << "hit is not on track " 
          << (*iterV)->isRType() <<  " "<< (*iterV)->isPhiType() <<  " " << (*iterV)->isPileUp() << std::endl; 

      int sensor = (*iterV)->channelID().sensor();
      double mysensor = sensor;
      stations.push_back(mysensor);
      double adc =(*iterV)->totalCharge();
      stationADC.push_back(adc);

    }
  }
  int unusedvelo  = clusters->size() - vids.size() ;
  //info() << " n unused =  " << unusedvelo << endmsg;
  test &= tup->column( prefix+"nUnusedVeloClusters", unusedvelo);
  test &= tup->farray( prefix+"UNUSED_VELO_STATION_ADC", stationADC.begin(), stationADC.end(), "nClusters" , 100);
  test &= tup->farray( prefix+"UNUSED_VELO_STATION_VECTOR", stations.begin(), stations.end(), "nClusters", 100);
  return StatusCode(test);


}




// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolVELOClusters )
