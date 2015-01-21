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
#include "Event/OTTime.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"
#include "Event/CaloDigit.h"
// local
#include "JsonConverter.h"
#include "JSONStream.h"

#include "MuonDet/DeMuonDetector.h"
#include "OTDet/DeOTDetector.h"
#include "STDet/DeITDetector.h"
#include "STDet/DeTTDetector.h"
#include "VeloDet/DeVelo.h"
#include "VeloDet/DeVeloSensor.h"
#include "CaloDet/DeCalorimeter.h"

#include "Kernel/OTChannelID.h"           
#include "Kernel/LHCbID.h"           

using namespace JSON;

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

  std::cout << "=====> Extracting MUON Hits " << std::endl;
  const LHCb::MuonCoords* coords = getIfExists<LHCb::MuonCoords>("Raw/Muon/Coords");
  if( NULL == coords ) {
    if(msgLevel(MSG::INFO)) info()<<" Container Raw/Muon/Coords doesn't exist"<<endmsg;    
  } else 
  {
    
    Stream muonJson(Container::LIST);
    DeMuonDetector* muonDetector=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");             
    for_each(coords->begin(),
             coords->end(),
             [this, &muonJson, &muonDetector] (LHCb::MuonCoord *c) 
             {
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
  }
  
  std::cout << "=====> Extracting OT Hits " << std::endl;
  const LHCb::OTTimes* otTimes = getIfExists<LHCb::OTTimes>("Raw/OT/Times");
  if( NULL == otTimes ) {
    if(msgLevel(MSG::INFO)) info()<<" Container Raw/OT/Times doesn't exist"<<endmsg;    
  } else 
  {    
    Stream otJson(Container::LIST);
    DeOTDetector* otDetector=getDet<DeOTDetector>("/dd/Structure/LHCb/AfterMagnetRegion/T/OT");
    
    for_each(otTimes->begin(),
             otTimes->end(),
             [this, &otJson, &otDetector] (LHCb::OTTime *c) 
             {
               LHCb::OTChannelID cid = c->channel();
               LHCb::LHCbID id(cid);
               std::auto_ptr<LHCb::Trajectory> t = otDetector->trajectory(id, 0.0);
               Stream othit(Container::LIST);
               othit << t->beginPoint().x() << t->beginPoint().y() << t->beginPoint().z()
                     << t->endPoint().x() << t->endPoint().y() << t->endPoint().z();
               otJson << othit;
             });
    jsonStream << std::make_pair("OT", otJson);
  }
  
  std::cout << "=====> Extracting IT Hits " << std::endl;
  const LHCb::STClusters* itClusters = getIfExists<LHCb::STClusters>("/Event/Raw/IT/Clusters");
  if( NULL == itClusters ) {
    if(msgLevel(MSG::INFO)) info()<<" Container Raw/IT/Clusters doesn't exist"<<endmsg;    
  } else 
  {
    
    Stream itJson(Container::LIST);
    DeITDetector* itDetector=getDet<DeITDetector>("/dd/Structure/LHCb/AfterMagnetRegion/T/IT");
    for_each(itClusters->begin(),
             itClusters->end(),
             [this, &itJson, &itDetector] (LHCb::STCluster *c) 
             {
               LHCb::STChannelID cid = c->channelID();
               LHCb::LHCbID id(cid);
               std::auto_ptr<LHCb::Trajectory> t = itDetector->trajectory(id, 0.0);
               Stream ithit(Container::LIST);
               ithit << t->beginPoint().x() << t->beginPoint().y() << t->beginPoint().z()
                     << t->endPoint().x() << t->endPoint().y() << t->endPoint().z();
               itJson << ithit;
             });
    jsonStream << std::make_pair("IT", itJson);
  }
  
  std::cout << "=====> Extracting TT Hits " << std::endl;
  const LHCb::STClusters* ttClusters = getIfExists<LHCb::STClusters>("/Event/Raw/TT/Clusters");
  if( NULL == ttClusters ) {
    if(msgLevel(MSG::INFO)) info()<<" Container Raw/TT/Clusters doesn't exist"<<endmsg;    
  } else 
  {
    
    Stream ttJson(Container::LIST);
    DeTTDetector* ttDetector=getDet<DeTTDetector>("/dd/Structure/LHCb/BeforeMagnetRegion/TT");
    for_each(ttClusters->begin(),
             ttClusters->end(),
             [this, &ttJson, &ttDetector] (LHCb::STCluster *c) 
             {
               LHCb::STChannelID cid = c->channelID();
               LHCb::LHCbID id(cid);
               std::auto_ptr<LHCb::Trajectory> t = ttDetector->trajectory(id, 0.0);
               Stream tthit(Container::LIST);
               tthit << t->beginPoint().x() << t->beginPoint().y() << t->beginPoint().z()
                     << t->endPoint().x() << t->endPoint().y() << t->endPoint().z();
               ttJson << tthit;
             });
    jsonStream << std::make_pair("TT", ttJson);
  }

  std::cout << "=====> Extracting HCAL Hits " << std::endl;
  const LHCb::CaloDigits* caloDigits = getIfExists<LHCb::CaloDigits>("/Event/Raw/Hcal/Digits");
  if( NULL == caloDigits ) {
    if(msgLevel(MSG::INFO)) info()<<" Container Raw/Hcal/Digits doesn't exist"<<endmsg;    
  } else 
  {
    
    Stream hcalJson(Container::LIST);
    DeCalorimeter* hcalDetector=getDet<DeCalorimeter>("/dd/Structure/LHCb/DownstreamRegion/Hcal");
    for_each(caloDigits->begin(),
             caloDigits->end(),
             [this, &hcalJson, &hcalDetector] (LHCb::CaloDigit *c) 
             {
               if (c->e() > 150) 
               {
                 const LHCb::CaloCellID cid = c->cellID();
                 double x = hcalDetector->cellX(cid);
                 double y = hcalDetector->cellY(cid);
                 double z = hcalDetector->cellZ(cid);
                 double s = hcalDetector->cellSize(cid);
                 Stream hcalhit(Container::LIST);
                 hcalhit << int(c->e()) << int(x) << int(y) << int(z)
                         << int(s);
                 hcalJson << hcalhit;
               }
             });
    jsonStream << std::make_pair("HCAL", hcalJson);
  }

  std::cout << "=====> Extracting ECAL Hits " << std::endl;
  const LHCb::CaloDigits* ecalDigits = getIfExists<LHCb::CaloDigits>("/Event/Raw/Ecal/Digits");
  if( NULL == ecalDigits ) {
    if(msgLevel(MSG::INFO)) info()<<" Container Raw/Ecal/Digits doesn't exist"<<endmsg;    
  } else 
  {
    
    Stream ecalJson(Container::LIST);
    DeCalorimeter* ecalDetector=getDet<DeCalorimeter>("/dd/Structure/LHCb/DownstreamRegion/Ecal");
    for_each(ecalDigits->begin(),
             ecalDigits->end(),
             [this, &ecalJson, &ecalDetector] (LHCb::CaloDigit *c) 
             {
               if (c->e() > 150) 
               {
                 const LHCb::CaloCellID cid = c->cellID();
                 double x = ecalDetector->cellX(cid);
                 double y = ecalDetector->cellY(cid);
                 double z = ecalDetector->cellZ(cid);
                 double s = ecalDetector->cellSize(cid);
                 Stream ecalhit(Container::LIST);
                 ecalhit << int(c->e()) << int(x) << int(y) << int(z)
                         << int(s);
                 ecalJson << ecalhit;
               }
             });
    jsonStream << std::make_pair("ECAL", ecalJson);
  }


  std::cout << "=====> Extracting VELO Hits " << std::endl;
  const LHCb::VeloClusters* veloClusters = getIfExists<LHCb::VeloClusters>("/Event/Raw/Velo/Clusters");
  if( NULL == veloClusters ) {
    if(msgLevel(MSG::INFO)) info()<<" Container Raw/Velo/Clusters doesn't exist"<<endmsg;    
  } else 
  {
    
    Stream veloRJson(Container::LIST);
    Stream veloPhiJson(Container::LIST);
    DeVelo* veloDetector=getDet<DeVelo>("/dd/Structure/LHCb/BeforeMagnetRegion/Velo");
    for_each(veloClusters->begin(),
             veloClusters->end(),
             [this, &veloRJson, &veloPhiJson, &veloDetector] (LHCb::VeloCluster *c) 
             {
               LHCb::VeloChannelID cid = c->channelID();
               const DeVeloSensor *sensor = veloDetector->sensor(cid);
               const LHCb::VeloCluster::ADCVector values = c->stripValues();
               for (auto p: values) 
               {
                 auto striplimits = sensor->globalStripLimits(p.first);
                 auto f = striplimits.first;
                 auto s = striplimits.second;
                 Stream hit(Container::LIST);
                 hit << int(f.x()) << int(f.y()) << int(f.z())
                     << int(s.x()) << int(s.y()) << int(s.z());

                 if (c->isRType() || c->isPileUp()) 
                 {
                   veloRJson << hit;  
                 } else 
                 {
                   veloPhiJson << hit;  
                 }
               }
             });
    jsonStream << std::make_pair("VELOPHI", veloPhiJson);
    jsonStream << std::make_pair("VELOR", veloRJson);
  }


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
  //std::cout << "=====================> JSON Result" << std::endl;
  //std::cout << jsonStream.str() << std::endl;
  //
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
