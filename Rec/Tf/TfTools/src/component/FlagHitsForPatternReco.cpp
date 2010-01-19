
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "FlagHitsForPatternReco.h"
// from boost
#include "boost/array.hpp"
#include "boost/foreach.hpp"



DECLARE_ALGORITHM_FACTORY( FlagHitsForPatternReco );

FlagHitsForPatternReco::FlagHitsForPatternReco(const std::string& name,
					       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty("EfficiencyCorrections", m_effCorrections );  
 
 

}



FlagHitsForPatternReco::~FlagHitsForPatternReco()
{
  // destructor
}


StatusCode FlagHitsForPatternReco::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); 

  // randomn number generator
   if ( !m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) ) ) {
     return Error( "Unable to create Random generator" );
   }


  m_ttHitManager = tool<Tf::TTStationHitManager<PatTTHit> >("PatTTStationHitManager"); 
  m_tHitManager  = tool<Tf::TStationHitManager<PatForwardHit> >("PatTStationHitManager");
  m_rHitManager = tool<Tf::DefaultVeloRHitManager>("Tf::DefaultVeloRHitManager");
  m_phiHitManager = tool<Tf::DefaultVeloPhiHitManager>("Tf::DefaultVeloPhiHitManager");
 
 
  for (int s=0; s<nTStations; s++)
    for (int l=0; l<nTLayers; l++){
    
      for (int q=0; q<nQuadrants; q++)
	for (int m=0; m<nModules; m++)
	  m_OTEfficiency[s][l][q][m] = 1.0;

      for (int r=0; r<nITRegions; r++)
	m_ITEfficiency[s][l][r] = 1.0;
    }

  for (int s=0; s<nVeloSensors; s++)
    for (int h=0; h<nVeloHalves; h++)
      for (int t=0; t<nVeloTypes; t++)
	for (int r=0; r<nVeloRegions; r++)
	  m_VeloEfficiency[s][h][t][r] = 1.0;
  
  for (int s=0; s<nTTStations; s++)
    for (int l=0; l<nTTLayers; l++)
      for (int r=0; r<nTTRegions; r++)
	m_TTEfficiency[s][l][r] = 1.0;
      
  for ( std::vector<std::string>::const_iterator iter = m_effCorrections.begin() ;
	iter!=m_effCorrections.end() ; ++iter) {
   
    char* oldstring = new char[(*iter).length()+1];
    strcpy(oldstring,(*iter).c_str());

    char* newstring = beginsWith(oldstring, "Velo");
    
    if (newstring != oldstring) {
      readVeloString(newstring);
    }
    else {
      newstring = beginsWith(oldstring, "TT");
      if (newstring != oldstring){
	readTTString(newstring);
      }
      else {
	newstring = beginsWith(oldstring, "OT");
	if (newstring != oldstring){
	  readOTString(newstring);
	}
	else{
	  newstring = beginsWith(oldstring, "IT");
	  if (newstring!=oldstring) {
	    readITString(newstring);
	  }else
	    return Error("WRONG STRING, SKIP IT");
	}
      }
    }

    delete oldstring;
  }

  return StatusCode::SUCCESS;
}
  
StatusCode FlagHitsForPatternReco::execute(){

  m_tHitManager->prepareHits();
  Tf::TStationHitManager<PatForwardHit>::HitRange trange = m_tHitManager->hits();
  BOOST_FOREACH( PatForwardHit* hit, trange ) {
    const Tf::OTHit* otHit = hit->hit()->othit();
    if ( otHit ) {   
      double eff = m_OTEfficiency[hit->hit()->station()][hit->hit()->layer()][otHit->module().quarterID()][otHit->module().moduleID()-1];

      if (eff < 1.0E-5) hit->hit()->setIgnore(true);
      else
	if (eff > 0.99999) hit->hit()->setIgnore(false);
	else
	  if ( m_rndm() > eff) hit->hit()->setIgnore(true);

    }
    else {
      double eff = m_ITEfficiency[hit->hit()->station()][hit->hit()->layer()][hit->hit()->region()];
      if (eff < 1.0E-5) hit->hit()->setIgnore(true);
      else
	if (eff > 0.99999) hit->hit()->setIgnore(false);
	else
	  if (m_rndm() > eff) hit->hit()->setIgnore(true);           
    }
  }
  
  m_ttHitManager->prepareHits();
  Tf::TTStationHitManager<PatTTHit>::HitRange ttrange = m_ttHitManager->hits();
  BOOST_FOREACH( PatTTHit* hit, ttrange ) {
   double eff = m_TTEfficiency[hit->hit()->station()][hit->hit()->layer()][hit->hit()->region()];
   if (eff < 1.0E-5) hit->hit()->setIgnore(true);
   else
     if (eff > 0.99999) hit->hit()->setIgnore(false);
     else
       if (m_rndm() > eff) hit->hit()->setIgnore(true); 
  }

  for (unsigned int half=0; half < 2; ++half) { 
    Tf::DefaultVeloRHitManager::StationIterator si   =
      m_rHitManager->stationsHalfBegin(half);
    Tf::DefaultVeloRHitManager::StationIterator send =
      m_rHitManager->stationsHalfEnd(half);
    // station number
    int stNum = -1;
    for ( ; si != send; ++si) {
      stNum++;
      for (unsigned int zone=0; zone<4; ++zone) { 
        Tf::VeloRHitRange hits = (*si)->hits(zone);
	Tf::VeloRHitRange::const_iterator ihit = hits.begin();
        for ( ; ihit!=hits.end(); ++ihit) {
          Tf::VeloRHit *vrh = *ihit;
	  double eff = m_VeloEfficiency[stNum][half][1][zone];
	  if (eff < 1.0E-5) 
	    vrh->setIgnore(true);
	  else
	    if (eff > 0.99999) vrh->setIgnore(false);
	    else 
	      if (m_rndm() > eff) vrh->setIgnore(true);
	}
      }
    }
  }
  
 for (unsigned int half=0; half < 2; ++half) { 
    Tf::DefaultVeloPhiHitManager::StationIterator si   =
      m_phiHitManager->stationsHalfBegin(half);
    Tf::DefaultVeloPhiHitManager::StationIterator send =
      m_phiHitManager->stationsHalfEnd(half);
    // station number
    int stNum = -1;
    for ( ; si != send; ++si) {
      stNum++;
      for (unsigned int zone=0; zone<2; ++zone) { 
        Tf::VeloPhiHitRange hits = (*si)->hits(zone);
	Tf::VeloPhiHitRange::const_iterator ihit = hits.begin();
	for ( ; ihit!=hits.end(); ++ihit) {
          Tf::VeloPhiHit *vphih = *ihit;
	  double eff = m_VeloEfficiency[stNum][half][0][zone];
	  if (eff < 1.0E-5) vphih->setIgnore(true);
	  else
	    if (eff > 0.99999) vphih->setIgnore(false);
	    else
	      if (m_rndm() > eff) vphih->setIgnore(true);
	}
      }
    }
  } 


    return StatusCode::SUCCESS;
};


 void FlagHitsForPatternReco::readOTString(char* oldstring){

   unsigned long station    = 999;
   unsigned long layer      = 999;
   unsigned long quarter    = 999;
   unsigned long module     = 999;
   double efficiency = 1.0;

   
   char* newstring = beginsWith(oldstring,"S");
   
   if (newstring!=oldstring){
     oldstring = newstring;
     station = strtoul(oldstring, &oldstring, 0);
   }
   
   newstring = beginsWith(oldstring,"L");
   
   if (newstring!=oldstring){
     oldstring = newstring;
     layer = strtoul(oldstring, &oldstring, 0);
   }
      
   newstring = beginsWith(oldstring,"Q");
   
   if (newstring!=oldstring){
     oldstring = newstring;
     quarter = strtoul(oldstring, &oldstring, 0);
   }
   
   newstring = beginsWith(oldstring,"M");
   
   if (newstring!=oldstring){
     oldstring = newstring;
     module = strtoul(oldstring, &oldstring, 0);
   }

   newstring = beginsWith(oldstring,"E");
   
   if (newstring!=oldstring){
     efficiency = strtod(newstring, &newstring);
   }
 
   int station_start = 0;
   int station_end = nTStations;

   if (station!=999){
     station_start = station;
     station_end = station+1;
   }

   int layer_start = 0;
   int layer_end = nTLayers;

   if (layer!=999){
     layer_start = layer;
     layer_end = layer+1;
   }
   
   int quarter_start = 0;
   int quarter_end = nQuadrants;

   if (quarter!=999){
     quarter_start = quarter;
     quarter_end = quarter+1;
   }

   int module_start = 0;
   int module_end = nModules;

   if (module!=999){
     module_start = module;
     module_end = module+1;
   }

   for (int s=station_start; s<station_end; s++)
     for (int l=layer_start; l<layer_end; l++)
       for (int q=quarter_start; q<quarter_end; q++)
	 for (int m=module_start; m<module_end; m++)
	   m_OTEfficiency[s][l][q][m]=efficiency;
 }
 

 void FlagHitsForPatternReco::readITString(char* oldstring){

   unsigned long station    = 999;
   unsigned long layer      = 999;
   unsigned long region     = 999;
   double efficiency        = 1.0;
   
   char* newstring = beginsWith(oldstring,"S");
   
   if (newstring!=oldstring){
     oldstring = newstring;
     station = strtoul(oldstring, &oldstring, 0);
   }
   
   newstring = beginsWith(oldstring, "L");
   
   if (newstring!=oldstring){
     oldstring = newstring;
     layer = strtoul(oldstring, &oldstring, 0);
   }
      
   newstring = beginsWith(oldstring, "R");
   
   if (newstring!=oldstring){
     oldstring = newstring;
     region = strtoul(oldstring, &oldstring, 0);
   }
   
   newstring = beginsWith(oldstring, "E");
   
   if (newstring!=oldstring){
     efficiency = strtod(newstring, &newstring);
   }
 
   int station_start = 0;
   int station_end = nTStations;

   if (station!=999){
     station_start = station;
     station_end = station+1;
   }

   int layer_start = 0;
   int layer_end = nTLayers;

   if (layer!=999){
     layer_start = layer;
     layer_end = layer+1;
   }
   
   int region_start = 0;
   int region_end = nITRegions;

   if (region!=999){
     region_start = region;
     region_end = region+1;
   }

   for (int s=station_start; s<station_end; s++)
     for (int l=layer_start; l<layer_end; l++)
       for (int r=region_start; r<region_end; r++)
	   m_ITEfficiency[s][l][r]=efficiency;
 }


 void FlagHitsForPatternReco::readTTString(char* oldstring){

   unsigned long station    = 999;
   unsigned long layer      = 999;
   unsigned long region     = 999;
   double efficiency        = 1.0;
   
   char* newstring = beginsWith(oldstring,"S");
   
   if (newstring!=oldstring){
     oldstring = newstring;
     station = strtoul(oldstring, &oldstring, 0);
   }
   
   newstring = beginsWith(oldstring,"L");
   
   if (newstring!=oldstring){
     oldstring=newstring;
     layer = strtoul(oldstring, &oldstring, 0);
   }
      
   newstring = beginsWith(oldstring, "R");
   
   if (newstring!=oldstring){
     oldstring = newstring;
     region = strtoul(oldstring, &oldstring, 0);
   }
   
   newstring = beginsWith(oldstring, "E");
   
   if (newstring!=oldstring){
     efficiency = strtod(newstring, &newstring);
   }
 
   int station_start = 0;
   int station_end = nTTStations;

   if (station!=999){
     station_start = station;
     station_end = station+1;
   }

   int layer_start = 0;
   int layer_end = nTTLayers;

   if (layer!=999){
     layer_start = layer;
     layer_end = layer+1;
   }
   
   int region_start = 0;
   int region_end = nTTRegions;

   if (region!=999){
     region_start = region;
     region_end = region+1;
   }

   for (int s=station_start; s<station_end; s++)
     for (int l=layer_start; l<layer_end; l++)
       for (int r=region_start; r<region_end; r++)
	   m_TTEfficiency[s][l][r]=efficiency;
 }

 
 void FlagHitsForPatternReco::readVeloString(char* oldstring){

   unsigned long sensor     = 999;
   unsigned long type       = 999;
   unsigned long halves     = 999;
   unsigned long region     = 999;
   double efficiency        = 1.0;

   
   char* newstring = beginsWith(oldstring, "S");
   
   if (newstring!=oldstring){
     oldstring = newstring;
     sensor = strtoul(oldstring, &oldstring, 0);
   }
   
   newstring = beginsWith(oldstring, "T");
   
   if (newstring!=oldstring){
     oldstring = newstring;
     type = strtoul(oldstring, &oldstring, 0);
   }
      
   newstring = beginsWith(oldstring, "H");
   
   if (newstring!=oldstring){
     oldstring = newstring;
     halves = strtoul(oldstring, &oldstring, 0);
   }
   
   newstring = beginsWith(oldstring, "R");
   
   if (newstring!=oldstring){
     oldstring = newstring;
     region = strtoul(oldstring, &oldstring, 0);
   }

   newstring = beginsWith(oldstring, "E");
   
   if (newstring!=oldstring){
     efficiency = strtod(newstring, &newstring);
   }
 
   int sensor_start = 0;
   int sensor_end = nVeloSensors;

   if (sensor!=999){
     sensor_start = sensor;
     sensor_end = sensor+1;
   }

   int type_start = 0;
   int type_end = nVeloTypes;

   if (type!=999){
     type_start = type;
     type_end = type+1;
   }
   
   int halves_start = 0;
   int halves_end = nVeloHalves;

   if (halves!=999){
     halves_start = halves;
     halves_end = halves+1;
   }

   int region_start = 0;
   int region_end = nVeloRegions;

   if (region!=999){
     region_start = region;
     region_end = region+1;
   }

   for (int s=sensor_start; s<sensor_end; s++)
     for (int t=type_start; t<type_end; t++)
       for (int h=halves_start; h<halves_end; h++)
	 for (int r=region_start; r<region_end; r++)
	   m_VeloEfficiency[s][t][h][r] = efficiency;
 }
 


char* FlagHitsForPatternReco::beginsWith (char* input, char* comp){
  
  if (std::strlen(input) < std::strlen(comp))
    return NULL;

  char* inputsave = input;
  while (*comp) {
    if ( std::tolower(*input) == std::tolower(*comp)){
      ++input;
      ++comp;
    } else
      return inputsave;
  }

  return input;
}
