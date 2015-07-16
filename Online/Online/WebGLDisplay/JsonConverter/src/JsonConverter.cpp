// Include files 
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>
#include <map>
#include <boost/any.hpp>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Particle.h"
#include "Event/ODIN.h"
#include "Event/RecVertex.h"
#include "Event/MuonCoord.h"
//#include "Event/OTTime.h"
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

#include "TfKernel/OTHit.h"
#include "TfKernel/IOTHitCreator.h"


#include "LHCbMath/EigenSystem.h"
//#include "Kernel/OTChannelID.h"           
#include "Kernel/LHCbID.h"           

#include "TrackInterfaces/ITrackExtrapolator.h"

using namespace JSON;

//-----------------------------------------------------------------------------
// Implementation file for class : JsonConverter
//
// 2014-12-03 : Ben Couturier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( JsonConverter )


// Advance declarations
/**
 * Process a proto particle, dumping the track to JSON
 */
void processTrack(ITrackExtrapolator *extrapolator,
                  const LHCb::Track* track,
                  LHCb::ParticleID pid,
                  double zmin,
                  double zmax,
                  Stream& json);



//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
JsonConverter::JsonConverter( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : DaVinciAlgorithm ( name , pSvcLocator )
{
  declareProperty("OutputDirectory", m_outputDir = "");
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
  m_trackExtrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator", this);
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
  if ( msgLevel(MSG::DEBUG) ) debug() << "=====> IN JSON CONVERTER" << std::endl;


  if ( msgLevel(MSG::DEBUG) ) debug() << "=====> Extracting ODIN " << endmsg;
  LHCb::ODIN *odin = (LHCb::ODIN *)get<LHCb::ODIN>("/Event/DAQ/ODIN");
  if ( msgLevel(MSG::DEBUG) ) debug() << "ODIN RunNumber    : " << odin->runNumber() << endmsg;
  if ( msgLevel(MSG::DEBUG) ) debug() << "ODIN EventNumber  : " << odin->eventNumber() << endmsg;
  if ( msgLevel(MSG::DEBUG) ) debug() << "ODIN GPS Time     : " << odin->gpsTime() << endmsg;
  if ( msgLevel(MSG::DEBUG) ) debug() << "ODIN Event Type   : " << odin->eventType() << endmsg;

  jsonStream << std::make_pair("runNumber", odin->runNumber());
  jsonStream << std::make_pair("eventNumber", odin->eventNumber());
  jsonStream << std::make_pair("gpsTime", odin->gpsTime());

  std::time_t result = odin->gpsTime()/1000000;
  char mbstr[100];
  if (std::strftime(mbstr, sizeof(mbstr), "%a, %d %b %Y %H:%M:%S", std::localtime(&result))) {
    jsonStream << std::make_pair("time", std::string(mbstr));
  }

  
  if ( msgLevel(MSG::DEBUG) ) debug() << "=====> Extracting PVs " << endmsg;
  LHCb::RecVertices* vertices = getIfExists<LHCb::RecVertices>("/Event/Rec/Vertex/Primary");
  if (NULL == vertices)   {
    if(msgLevel(MSG::INFO)) info()<<" Container Rec/Vertex/Primary doesn't exist"<<endmsg;    
  } else {
    int vertexCount = 0;
    Stream vlistJson(Container::LIST);
    for_each (vertices->begin(), 
              vertices->end(),
              [&vertexCount, &vlistJson] (LHCb::RecVertex *r)
              {
                
                if (nullptr == r)
                  return;
		//getting the covariance matrix
		Gaudi::SymMatrix3x3 cov = r->covMatrix();
		Gaudi::Math::GSL::EigenSystem esystem;
		Gaudi::Vector3 evalues;
		Gaudi::Matrix3x3 evectors;
		esystem.eigenVectors ( cov, evalues, evectors, false );

                Stream vertJson(Container::MAP);
                vertJson << std::make_pair("pv_x", r->position().x());
                vertJson << std::make_pair("pv_y", r->position().y());
                vertJson << std::make_pair("pv_z", r->position().z());

                vertJson << std::make_pair("eval_0", evalues(0));
                vertJson << std::make_pair("eval_1", evalues(1));
                vertJson << std::make_pair("eval_2", evalues(2));
		double mag0 = sqrt(evectors(0,0)*evectors(0,0)+evectors(1,0)*evectors(1,0)+evectors(2,0)*evectors(2,0));
		double mag1 = sqrt(evectors(0,1)*evectors(0,1)+evectors(1,1)*evectors(1,1)+evectors(2,1)*evectors(2,1));
		double mag2 = sqrt(evectors(0,2)*evectors(0,2)+evectors(1,2)*evectors(1,2)+evectors(2,2)*evectors(2,2));
                vertJson << std::make_pair("evec_0x", evectors(0,0)/mag0);
                vertJson << std::make_pair("evec_0y", evectors(1,0)/mag0);
                vertJson << std::make_pair("evec_0z", evectors(2,0)/mag0);
                vertJson << std::make_pair("evec_1x", evectors(0,1)/mag1);
                vertJson << std::make_pair("evec_1y", evectors(1,1)/mag1);
                vertJson << std::make_pair("evec_1z", evectors(2,1)/mag1);
                vertJson << std::make_pair("evec_2x", evectors(0,2)/mag2);
                vertJson << std::make_pair("evec_2y", evectors(1,2)/mag2);
                vertJson << std::make_pair("evec_2z", evectors(2,2)/mag2);

                vlistJson << vertJson;
                vertexCount++;
              });
    // Now adding the entries to the main map
    jsonStream << std::make_pair("PVS", vlistJson);
  }

  if ( msgLevel(MSG::DEBUG) ) debug() << "=====> Extracting MUON Hits " << endmsg;
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

              if (nullptr == c)
                return;
              
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
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "=====> Extracting OT Hits " << endmsg;
  // Using the Hit creator as recommended y W.Hulsbergen
  auto hitcreator = tool<Tf::IOTHitCreator>("Tf::OTHitCreator/OTHitCreator") ;
  if (hitcreator != NULL) 
  {
    Tf::OTHitRange othits = hitcreator->hits();
    Stream otJson(Container::LIST);
    for( const auto& othit : othits ) {
      Stream sothit(Container::LIST);
      auto beginPoint = othit->beginPoint();
      auto endPoint =  othit->position( othit->yEnd());    
      sothit << beginPoint.x() << beginPoint.y() << beginPoint.z()
             << endPoint.x() << endPoint.y() << endPoint.z();
      otJson << sothit;
    }
    jsonStream << std::make_pair("OT", otJson);
  }
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "=====> Extracting IT Hits " << endmsg;
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

               if (nullptr == c)
                 return;

               LHCb::STChannelID cid = c->channelID();
               LHCb::LHCbID id(cid);
               std::auto_ptr<LHCb::Trajectory> t = itDetector->trajectory(id, 0.0);
	       if ( t.get() )  {
		 Stream ithit(Container::LIST);
		 ithit << t->beginPoint().x() << t->beginPoint().y() << t->beginPoint().z()
		       << t->endPoint().x() << t->endPoint().y() << t->endPoint().z();
		 itJson << ithit;
	       }
             });
    jsonStream << std::make_pair("IT", itJson);
  }
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "=====> Extracting TT Hits " << endmsg;
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

               if (nullptr == c)
                 return;

               LHCb::STChannelID cid = c->channelID();
               LHCb::LHCbID id(cid);
               std::auto_ptr<LHCb::Trajectory> t = ttDetector->trajectory(id, 0.0);
	       if ( t.get() )  {
		 Stream tthit(Container::LIST);
		 tthit << t->beginPoint().x() << t->beginPoint().y() << t->beginPoint().z()
		       << t->endPoint().x() << t->endPoint().y() << t->endPoint().z();
		 ttJson << tthit;
	       }
             });
    jsonStream << std::make_pair("TT", ttJson);
  }

  if ( msgLevel(MSG::DEBUG) ) debug() << "=====> Extracting HCAL Hits " << endmsg;
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

               if (nullptr == c)
                 return;

               if (c->e() > 100.0)//150 
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

  if ( msgLevel(MSG::DEBUG) ) debug() << "=====> Extracting ECAL Hits " << endmsg;
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
               if (c->e() > 50.0) //150.0
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


  if ( msgLevel(MSG::DEBUG) ) debug() << "=====> Extracting VELO Hits " << endmsg;
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
               if (nullptr == c)
                 return;

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


  if ( msgLevel(MSG::DEBUG) ) debug() << "=====> Extracting Velo tracks " << endmsg;
  LHCb::Tracks *vtracks = getIfExists<LHCb::Tracks>("/Event/Rec/Track/Best");
  Stream veloTracksJson(Container::LIST);
  int vtracksCount = 0;
  if( NULL == vtracks ) {
    if(msgLevel(MSG::INFO)) info()<<" Container /Event/Rec/Track/Best doesn't exist"<<endmsg;    
  } else 
  {
    for_each (vtracks->begin(), 
              vtracks->end(),
              [this, &vtracksCount, &veloTracksJson, &vertices] (LHCb::Track *t)
	      {
		if (t->checkType(LHCb::Track::Types::Velo))//only look at velo tracks
		  {
		    //LHCb::RecVertex* bestpv = NULL;
		    Gaudi::XYZPoint pos = t->position();
		    Gaudi::XYZVector mom = t->momentum();
		    Gaudi::XYZVector momnormalized = mom/sqrt(mom.Mag2());
		    double zstart = 0.0;
		    double bestip = 1.0e+6;
		    Gaudi::XYZPoint closest(0.0, 0.0, 0.0);

		    for_each (vertices->begin(), 
			      vertices->end(),
			      [&t, &pos, &bestip, &closest, &zstart, &momnormalized] (LHCb::RecVertex *pv)
			      {
				Gaudi::XYZPoint pvpos = pv->position();
				Gaudi::XYZPoint pvminuspos(pvpos.x()-pos.x(), pvpos.y()-pos.y(), pvpos.z()-pos.z());
				double currentip = sqrt(  (pvminuspos - pvminuspos.Dot(momnormalized)*momnormalized).Mag2() );
				if (currentip < bestip)
				  {
				    closest = pos + pvminuspos.Dot(momnormalized)*momnormalized;
				    zstart = closest.z();
				    bestip = currentip;
				  }
			      });

		    double zfinal = (closest+momnormalized*500.0).z();
		    if (t->checkFlag(LHCb::Track::Flags::Backward)) //backwards track
		      zfinal = (closest-momnormalized*500.0).z();
		    Stream trackJson(Container::LIST);
		    const unsigned int npoints = 5;
		    LHCb::State s;
		    for(unsigned int i=0; i < npoints; i++) 
		      {
			double z = zstart + i*(zfinal-zstart)/npoints;
			m_trackExtrapolator->propagate(*t, z, s);
			Stream p(Container::LIST);
			p << s.x() << s.y() << s.z();
			trackJson << p;
		      }
		    veloTracksJson << trackJson;
		  }
	      });
    if ( msgLevel(MSG::DEBUG) ) debug() << "Vtracks count " << vtracksCount << endmsg;  
  }
  jsonStream << std::make_pair("VTRACKS", veloTracksJson);


  if ( msgLevel(MSG::DEBUG) ) debug() << "=====> Extracting Particles " << endmsg;
  const std::string PartsTESLocation("/Event/Phys/StdAllNoPIDsPions/Particles");  
  LHCb::Particles *inputParts = getIfExists<LHCb::Particles>(PartsTESLocation);
  Stream allParticlesJson(Container::LIST);
  int partCount = 0;
  if( NULL == inputParts ) {
    if(msgLevel(MSG::INFO)) info()<<" Container /Event/Phys/StdAllNoPIDsPions/Particles doesn't exist"<<endmsg;    
  } else 
  {
    for_each (inputParts->begin(), 
              inputParts->end(),
              [this, &partCount, &allParticlesJson] (LHCb::Particle *p)
              {
                Stream pJson(Container::MAP);
                partCount++;
                 
		 if (p == nullptr)
		    return;

                 std::string partName =  getParticleName(p);
                 pJson <<  std::make_pair("name", partName);
                 pJson <<  std::make_pair("m", p->measuredMass());
                 pJson <<  std::make_pair("E", p->momentum().E());
                 pJson <<  std::make_pair("px", p->momentum().px());
                 pJson <<  std::make_pair("py", p->momentum().py());
                 pJson <<  std::make_pair("pz", p->momentum().pz());
                 pJson <<  std::make_pair("q", p->charge());

                 // XXX Missing Impact parameter info
                 // Finding boundaries for the track
                 auto bestVertex = findBestVertex(p);
                 double zmin = 0;
                 if (bestVertex != 0)
                 {  
                   zmin = bestVertex->position().z();
                 }
                
	        double zmax = HCAL_Z;
                if (partName == "mu+" || partName == "mu-") 
                  zmax = MUON_Z;
                 
                // Now propagating the track!
                Stream trackJson(Container::LIST);
                if (p->proto() != nullptr) 
                {
                  processTrack(m_trackExtrapolator,
                               p->proto()->track(), 
                               p->particleID(),
                               zmin,
                               zmax,
                               trackJson);
                  pJson <<  std::make_pair("track", trackJson); 
                }
                 
                // Adding this particle to the list
                allParticlesJson << pJson;

              });
   
    if ( msgLevel(MSG::DEBUG) ) debug() << "Part count " << partCount << endmsg;  
  }
  jsonStream << std::make_pair("PARTICLES", allParticlesJson);


  //SVs
  /*
  const std::string PartsTESLocation("/Event/Phys/StdAllNoPIDsPions/Particles");  
  LHCb::Particles *inputParts = getIfExists<LHCb::Particles>(PartsTESLocation);
  Stream allParticlesJson(Container::LIST);
  int partCount = 0;
  if( NULL == inputParts ) {
    if(msgLevel(MSG::INFO)) info()<<" Container /Event/Phys/StdAllNoPIDsPions/Particles doesn't exist"<<endmsg;    
  } else 
  {
    for_each (inputParts->begin(), 
              inputParts->end(),
              [this, &partCount, &allParticlesJson] (LHCb::Particle *p)
              {
*/



  // Now printing the result
  // if ( msgLevel(MSG::DEBUG) ) debug() << "=====================> JSON Result" << endmsg;
  // if ( msgLevel(MSG::DEBUG) ) debug() << jsonStream.str() << endmsg;
  // if ( msgLevel(MSG::DEBUG) ) debug() << "=====================> JSON Result" << endmsg;
  
  std::ofstream jsonOutput;
  char fname[PATH_MAX];
  ::snprintf(fname,sizeof(fname),"%s/%08ld_%010ld.json",
	     m_outputDir.empty() ? "." : m_outputDir.c_str(),
	     long(odin->runNumber()), long(odin->eventNumber()));
  jsonOutput.open(fname);
  jsonOutput << jsonStream.str() << std::endl;
  jsonOutput.close();

  if(msgLevel(MSG::INFO)) info()<< "Written JSON file: "  << fname << endmsg;    
  

  setFilterPassed(true);  // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}


std::string getParticleName(LHCb::Particle *p) 
{

  int  ProbNNe = 700;  //# The ANN probability for the electron hypothesis
  int  ProbNNmu = 701; //# The ANN probability for the muon hypothesis
  int  ProbNNpi = 702; //# The ANN probability for the pion hypothesis
  int  ProbNNk = 703; //  # The ANN probability for the kaon hypothesis
  int  ProbNNp = 704; //  # The ANN probability for the proton hypothesis
  int  ProbNNghost = 705; //# The ANN probability for the ghost hypothesis

  // A few checks in case
  if (p == nullptr)
    return "NullParticle";

  auto pr = p->proto();
  if (pr == nullptr)
    return "NullProtoParticle";

  auto muPID = pr->muonPID();

  // Now actually doing the checks
  std::string pname = "";
  if (p->charge() > 0) 
  {
    pname = "pi+";
    if  (pr->info(ProbNNk, -9999) > 0.3 
         && pr->info(ProbNNk, -9999) > pr->info(ProbNNp, -9999)
         && pr->info(ProbNNk, -9999) > pr->info(ProbNNe, -9999) 
         && pr->info(ProbNNk, -9999) > pr->info(ProbNNmu, -9999))
      pname = "K+";
    
        
    if  (pr->info(ProbNNp, -9999) > 0.3 
         && pr->info(ProbNNp, -9999) > pr->info(ProbNNk, -9999) 
         && pr->info(ProbNNp, -9999) > pr->info(ProbNNe, -9999) 
         && pr->info(ProbNNp, -9999) > pr->info(ProbNNmu, -9999))
      pname = "p+";
    
    if  (pr->info(ProbNNe, -9999) > 0.3 
         && pr->info(ProbNNe, -9999) > pr->info(ProbNNk, -9999) 
         && pr->info(ProbNNe, -9999) > pr->info(ProbNNp, -9999) 
         && pr->info(ProbNNe, -9999) > pr->info(ProbNNmu, -9999))
      pname = "e+";
    
    if  (pr->info(ProbNNmu, -9999) > 0.1 && muPID && muPID->IsMuon())
      pname = "mu+";
    
  }
  
  if (p->charge() < 0) 
  {
    
    pname = "pi-";
    if  (pr->info(ProbNNk, -9999) > 0.3 
         && pr->info(ProbNNk, -9999) > pr->info(ProbNNp, -9999) 
         && pr->info(ProbNNk, -9999) > pr->info(ProbNNe, -9999) 
         && pr->info(ProbNNk, -9999) > pr->info(ProbNNmu, -9999))
      pname = "K-";
  
    if  (pr->info(ProbNNp, -9999) > 0.3 
         && pr->info(ProbNNp, -9999) > pr->info(ProbNNk, -9999) 
         && pr->info(ProbNNp, -9999) > pr->info(ProbNNe, -9999) 
         && pr->info(ProbNNp, -9999) > pr->info(ProbNNmu, -9999))
      pname = "p-";
  
    if  (pr->info(ProbNNe, -9999) > 0.3 
         && pr->info(ProbNNe, -9999) > pr->info(ProbNNk, -9999) 
         && pr->info(ProbNNe, -9999) > pr->info(ProbNNp, -9999) 
         && pr->info(ProbNNe, -9999) > pr->info(ProbNNmu, -9999))
      pname = "e-";
  
    if  (pr->info(ProbNNmu, -9999) > 0.1 && muPID && muPID->IsMuon())
      pname = "mu-";
  }

  return pname;

}

const LHCb::VertexBase* JsonConverter::findBestVertex(LHCb::Particle *p) 
{
  if (nullptr == p) 
  {
    if(msgLevel(MSG::DEBUG)) debug()<< " Null pointer passed to findBestVertex"<<endmsg;    
    return nullptr;
  }
  
  const LHCb::VertexBase* bestPV = this->bestPV(p);
  return bestPV;
}

void processTrack(ITrackExtrapolator *extrapolator,
                  const LHCb::Track* track, 
                  LHCb::ParticleID pid,
                  double zmin,
                  double zmax,
                  Stream& json) 
{

  const int npoints = 10;
  LHCb::State s;
  double z;

  // A few checks just in case...
  if (nullptr == track || nullptr == extrapolator) 
    return;

  for(int i=0; i < npoints+1; i++) 
  {
    z = zmin + i*(zmax-zmin)/npoints;
    extrapolator->propagate(*track, z, s, pid);
    Stream p(Container::LIST);
    p << s.x() << s.y() << s.z();
    json << p;
  }  
}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode JsonConverter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DaVinciAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
