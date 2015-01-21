// Include files 
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <boost/any.hpp>

 // from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Particle.h"
#include "Event/ODIN.h"
#include "Event/RecVertex.h"
#include "Event/MuonCoord.h"
// local
#include "JsonConverter.h"
#include "JSONStream.h"
#include "MuonDet/DeMuonDetector.h"

using namespace JSON;
using boost::any_cast;
typedef std::map<std::string, double> msd;


//-----------------------------------------------------------------------------
// Implementation file for class : JsonConverter
//
// 2014-12-03 : Ben Couturier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( JsonConverter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
JsonConverter::JsonConverter( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : DaVinciAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
JsonConverter::~JsonConverter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode JsonConverter::initialize() {
  StatusCode sc = DaVinciAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode JsonConverter::execute() {

  // Map containing the various data entries
  // HAs to use boost::any unfortunately...
  Stream jsonStream(Container::MAP);

  //std::map<std::string, boost::any> eventData;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  std::cout << "=====> IN JSON CONVERTER" << std::endl;


  std::cout << "=====> Extracting ODIN " << std::endl;
  LHCb::ODIN *odin = (LHCb::ODIN *)get<LHCb::ODIN>("/Event/DAQ/ODIN");
  std::cout << "ODIN RunNumber    : " << odin->runNumber() << std::endl;
  std::cout << "ODIN EventNumber  : " << odin->eventNumber() << std::endl;
  std::cout << "ODIN GPS Time     : " << odin->gpsTime() << std::endl;
  std::cout << "ODIN Event Type   : " << odin->eventType() << std::endl;

  jsonStream << std::make_pair("runNumber", odin->runNumber());
  jsonStream << std::make_pair("eventNumber", odin->eventNumber());
  jsonStream << std::make_pair("gpsTime", odin->gpsTime());

  std::time_t result = std::time(NULL);
  char mbstr[100];
  if (std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(&result))) {
    jsonStream << std::make_pair("time", std::string(mbstr));
  }

  
  std::cout << "=====> Extracting PVs " << std::endl;
  LHCb::RecVertices* vertices = get<LHCb::RecVertices>("/Event/Rec/Vertex/Primary");
  int vertexCount = 0;
  Stream vlistJson(Container::LIST);
  for_each (vertices->begin(), 
              vertices->end(),
            [&vertexCount, &vlistJson] (LHCb::RecVertex *r)
              {
                Stream vertJson(Container::MAP);
                vertJson << std::make_pair("pv_x", r->position().x());
                vertJson << std::make_pair("pv_y", r->position().y());
                vertJson << std::make_pair("pv_z", r->position().z());
                vlistJson << vertJson;
                vertexCount++;
              });
  // Now adding the entries to the main map
  jsonStream << std::make_pair("PVS", vlistJson);


  const LHCb::MuonCoords* coords = getIfExists<LHCb::MuonCoords>("Raw/Muon/Coords");
  if( NULL == coords ) {
    if(msgLevel(MSG::DEBUG)) debug()<<" Container Raw/Muon/Coords doesn't exist"<<endmsg;    
  }
  Stream muonJson(Container::LIST);
  for_each(coords->begin(),
           coords->end(),
           [this, &muonJson] (LHCb::MuonCoord *c) 
           {
             DeMuonDetector* muonDetector=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");             
             std::vector<LHCb::MuonTileID> digitTiles = c->digitTile();
             for (auto dt: digitTiles) 
             {
               
               double x, dx, y, dy, z, dz;
               muonDetector-> Tile2XYZ(dt, x, dx, y, dy, z, dz);
               Stream hitJson(Container::LIST);
               hitJson << int(x) << int(dx) << int(y) << int(dy) 
                       << int(z) << int(dz);
               muonJson << hitJson;
             }
           });
  
  jsonStream << std::make_pair("MUON", muonJson);


// LHCb::Tracks *inputTracks = get<LHCb::Tracks>(TracksTESLocation);
  // int trackCount = 0;
  // for_each (inputTracks->begin(), 
  //           inputTracks->end(),
  //           [&trackCount] (LHCb::Track *t)
  //           {
  //             trackCount++;
  //             auto p = t->phi();
  //           });
  
  // std::cout << "Track count " << trackCount << std::endl;


  // const std::string PartsTESLocation("/Event/Phys/StdNoPIDsPions/Particles");  
  // LHCb::Particles *inputParts = get<LHCb::Particles>(PartsTESLocation);
  // int partCount = 0;
  // for_each (inputParts->begin(), 
  //           inputParts->end(),
  //           [&partCount] (LHCb::Particle *p)
  //           {
  //             partCount++;
  //             std::string pname = "";
  //             if (p->charge() >0) 
  //             {
                
  //             }
              

  //           });

  //std::cout << "Part count " << partCount << std::endl;

  // Now printing the result
  std::cout << "=====================> JSON Result" << std::endl;
  std::cout << jsonStream.str() << std::endl;
  std::cout << "=====================> JSON Result" << std::endl;
  
  std::ofstream jsonOutput;
  std::stringstream buf;
  buf << odin->runNumber()
      << "_"
      << odin->eventNumber()
      << ".json";
  
  jsonOutput.open(buf.str());
  jsonOutput << jsonStream.str() << std::endl;
  jsonOutput.close();
  

  setFilterPassed(true);  // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode JsonConverter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DaVinciAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
