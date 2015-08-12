
// Gaudi
#include "FlagHitsForPatternReco.h"
#include <algorithm>
#include <cassert>
// from boost
#include "boost/array.hpp"
#include "boost/foreach.hpp"

DECLARE_ALGORITHM_FACTORY( FlagHitsForPatternReco )

FlagHitsForPatternReco::FlagHitsForPatternReco(const std::string& name, ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator),
  m_tHitManager(NULL),
  m_ttHitManager(NULL),
  m_rHitManager(NULL),
  m_phiHitManager(NULL)
{
    declareProperty("EfficiencyCorrections", m_effCorrections );
    declareProperty("MisalignmentCorrections", m_misCorrections );
    

}



FlagHitsForPatternReco::~FlagHitsForPatternReco()
{
    // destructor
}


StatusCode FlagHitsForPatternReco::initialize()
{
    StatusCode sc = GaudiAlgorithm::initialize();

    // randomn number generator
    if ( !m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) ) ) {
	return Error( "Unable to create Random generator" );
    } 

    // randomn number generator
    if ( !m_rndmG.initialize( randSvc(), Rndm::Gauss(0.,1.) ) ) {
	return Error( "Unable to create Random generator" );
    }
    

    m_ttHitManager = tool<Tf::TTStationHitManager<PatTTHit> >("PatTTStationHitManager");
    m_tHitManager = tool<Tf::TStationHitManager<PatForwardHit> >("PatTStationHitManager");
    m_rHitManager = tool<Tf::DefaultVeloRHitManager>("Tf::DefaultVeloRHitManager");
    m_phiHitManager = tool<Tf::DefaultVeloPhiHitManager>("Tf::DefaultVeloPhiHitManager");

    for (unsigned i = 0; i < nTStations; ++i)
	for (unsigned j = 0; j < nTLayers; ++j)
	    for (unsigned k = 0; k < nQuadrants; ++k)
		for (unsigned l = 0; l < nModules; ++l)
		  for (unsigned m = 0; m < nOTOtis; ++m){
			m_OTEfficiency[i][j][k][l][m] = 1.;
			m_OTSmearing[i][j][k][l][m] = 0.0;
		  }
   
    for (unsigned i = 0; i < nTStations; ++i)
	for (unsigned j = 0; j < nTLayers; ++j)
	  for (unsigned k = 0; k < nITRegions; ++k){
		m_ITEfficiency[i][j][k] = 1.;
		m_ITSmearing[i][j][k] = 0.0;
		
	  }
    
    for (unsigned i = 0; i < nTTStations; ++i)
	for (unsigned j = 0; j < nTTLayers; ++j)
	  for (unsigned k = 0; k < nTTRegions; ++k){
		m_TTEfficiency[i][j][k] = 1.;
		m_TTSmearing[i][j][k] = 0.;
	  }
   

    for (unsigned i = 0; i < nVeloSensors; ++i)
	for (unsigned j = 0; j < nVeloHalves; ++j)
	    for (unsigned k = 0; k < nVeloTypes; ++k)
	      for (unsigned l = 0; l < nVeloRegions; ++l){
		    m_VeloEfficiency[i][j][k][l] = 1.;
		    m_VeloSmearing[i][j][k][l]=0.0;
	      }

    for (std::vector<std::string>::const_iterator iter = m_effCorrections.begin();
	    iter != m_effCorrections.end(); ++iter) {
	const char* oldstring = iter->c_str();

	const char* newstring = beginsWith(oldstring, "Velo");
	if (newstring != oldstring) {
	    readVeloString(newstring);
	    continue;
	}
	newstring = beginsWith(oldstring, "TT");
	if (newstring != oldstring) {
	    readTTString(newstring);
	    continue;
	}
	newstring = beginsWith(oldstring, "OT");
	if (newstring != oldstring) {
	    readOTString(newstring);
	    continue;
	}
	newstring = beginsWith(oldstring, "IT");
	if (newstring != oldstring) {
	    readITString(newstring);
	    continue;
	}
	return Error("WRONG STRING, SKIP IT");
    } 


    for (std::vector<std::string>::const_iterator iter = m_misCorrections.begin();
	    iter != m_misCorrections.end(); ++iter) {
	const char* oldstring = iter->c_str();

	const char* newstring = beginsWith(oldstring, "Velo");
	if (newstring != oldstring) {
	    readVeloStringSmearing(newstring);
	    continue;
	}
	newstring = beginsWith(oldstring, "TT");
	if (newstring != oldstring) {
	    readTTStringSmearing(newstring);
	    continue;
	}
	newstring = beginsWith(oldstring, "OT");
	if (newstring != oldstring) {
	    readOTStringSmearing(newstring);
	    continue;
	}
	newstring = beginsWith(oldstring, "IT");
	if (newstring != oldstring) {
	    readITStringSmearing(newstring);
	    continue;
	}
	return Error("WRONG STRING, SKIP IT");
    }

    return StatusCode::SUCCESS;
}

StatusCode FlagHitsForPatternReco::execute()
{
    m_tHitManager->prepareHits();
    Tf::TStationHitManager<PatForwardHit>::HitRange trange = m_tHitManager->hits();
    BOOST_FOREACH(PatForwardHit* hit, trange) {
	const Tf::OTHit* otHit = hit->hit()->othit();
	if (otHit) {
	    double eff = m_OTEfficiency[hit->hit()->station()][hit->hit()->layer()][
		otHit->module().quarterID()][otHit->module().moduleID() - 1][
		otis(otHit->rawhit().channel())];
	    if (eff < 1.0E-5) {
		hit->hit()->setIgnore(true);
	    } else {
		if (eff > 0.99999) {
		    hit->hit()->setIgnore(false);
		} else {
		    if (m_rndm() > eff)
			hit->hit()->setIgnore(true);
		}
	    }
	    


	    hit->hit()->setXAtYEq0(hit->hit()->xAtYEq0()+m_OTSmearing[hit->hit()->station()]
				   [hit->hit()->layer()][otHit->module().quarterID()]
				   [otHit->module().moduleID() - 1][otis(otHit->rawhit().channel())]);
	    
	      
	} else {
	    double eff = m_ITEfficiency[hit->hit()->station()][hit->hit()->layer()][hit->hit()->region()];
	    if (eff < 1.0E-5) {
		hit->hit()->setIgnore(true);
	    } else {
		if (eff > 0.99999) {
		    hit->hit()->setIgnore(false);
		} else {
		    if (m_rndm() > eff)
			hit->hit()->setIgnore(true);
		}
	    }
	    
	 	    
	    hit->hit()->setXAtYEq0(hit->hit()->xAtYEq0()+m_ITSmearing[hit->hit()->station()]
				   [hit->hit()->layer()][hit->hit()->region()]); 

	}
    }

    m_ttHitManager->prepareHits();
    Tf::TTStationHitManager<PatTTHit>::HitRange ttrange = m_ttHitManager->hits();
    BOOST_FOREACH(PatTTHit* hit, ttrange) {
	double eff = m_TTEfficiency[hit->hit()->station()][hit->hit()->layer()][hit->hit()->region()];
	if (eff < 1.0E-5) {
	    hit->hit()->setIgnore(true);
	} else {
	    if (eff > 0.99999) {
		hit->hit()->setIgnore(false);
	    } else {
		if (m_rndm() > eff)
		    hit->hit()->setIgnore(true);
	    }
	}

	hit->hit()->setXAtYEq0(hit->hit()->xAtYEq0()+
			       m_TTSmearing[hit->hit()->station()]
			       [hit->hit()->layer()][hit->hit()->region()]); 
    }


    for (unsigned half = 0; half < 2; ++half) {
	Tf::DefaultVeloRHitManager::StationIterator si   =
	    m_rHitManager->stationsHalfBegin(half);
	Tf::DefaultVeloRHitManager::StationIterator send =
	    m_rHitManager->stationsHalfEnd(half);
	// station number
	int stNum = -1;
	for ( ; si != send; ++si) {
	    stNum++;
	    for (unsigned int zone = 0; zone < 4; ++zone) {
		Tf::VeloRHitRange hits = (*si)->hits(zone);
		Tf::VeloRHitRange::const_iterator ihit = hits.begin();
		for ( ; ihit != hits.end(); ++ihit) {
		    Tf::VeloRHit *vrh = *ihit;
		    double eff = m_VeloEfficiency[stNum][half][1][zone];
		    if (eff < 1.0E-5) {
			vrh->setIgnore(true);
		    } else {
			if (eff > 0.99999) {
			    vrh->setIgnore(false);
			} else {
			    if (m_rndm() > eff)
				vrh->setIgnore(true);
			}
		    }
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
	    for (unsigned zone = 0; zone < 2; ++zone) {
		Tf::VeloPhiHitRange hits = (*si)->hits(zone);
		Tf::VeloPhiHitRange::const_iterator ihit = hits.begin();
		for ( ; ihit != hits.end(); ++ihit) {
		    Tf::VeloPhiHit *vphih = *ihit;
		    double eff = m_VeloEfficiency[stNum][half][0][zone];
		    if (eff < 1.0E-5) {
			vphih->setIgnore(true);
		    } else {
			if (eff > 0.99999) {
			    vphih->setIgnore(false);
			} else {
			    if (m_rndm() > eff)
				vphih->setIgnore(true);
			}
		    }
		}
	    }
	}
    }

    return StatusCode::SUCCESS;
}

void FlagHitsForPatternReco::readOTString(const char* oldstring)
{

    unsigned long station    = 999;
    unsigned long layer      = 999;
    unsigned long quarter    = 999;
    unsigned long module     = 999;
    unsigned long otis	    = 999;
    double efficiency = 1.0;

    const char* newstring = beginsWith(oldstring,"S");

    if (newstring != oldstring) {
	oldstring = newstring;
	station = mystrtoul(oldstring, oldstring, 0);
	assert(station < nTStations);
    }

    newstring = beginsWith(oldstring,"L");

    if (newstring != oldstring) {
	oldstring = newstring;
	layer = mystrtoul(oldstring, oldstring, 0);
	assert(layer < nTLayers);
    }

    newstring = beginsWith(oldstring,"Q");

    if (newstring != oldstring) {
	oldstring = newstring;
	quarter = mystrtoul(oldstring, oldstring, 0);
	assert(quarter < nQuadrants);
    }

    newstring = beginsWith(oldstring,"M");

    if (newstring != oldstring) {
	oldstring = newstring;
	module = mystrtoul(oldstring, oldstring, 0);
	assert(module < nModules);
    }
    newstring = beginsWith(oldstring,"OTIS");

    if (newstring != oldstring) {
	oldstring = newstring;
	otis = mystrtoul(oldstring, oldstring, 0);
	assert(otis < nOTOtis);
    }


    newstring = beginsWith(oldstring,"E");

    if (newstring != oldstring) {
	efficiency = mystrtod(newstring, newstring);
	assert(0. <= efficiency && 1. >= efficiency);
    }

    int station_start = 0;
    int station_end = nTStations;

    if (station != 999) {
	station_start = station;
	station_end = station + 1;
    }

    int layer_start = 0;
    int layer_end = nTLayers;

    if (layer != 999) {
	layer_start = layer;
	layer_end = layer + 1;
    }

    int quarter_start = 0;
    int quarter_end = nQuadrants;

    if (quarter != 999) {
	quarter_start = quarter;
	quarter_end = quarter + 1;
    }

    int module_start = 0;
    int module_end = nModules;

    if (module != 999) {
	module_start = module;
	module_end = module + 1;
    }

    int otis_start = 0;
    int otis_end = nOTOtis;
    if (999 != otis) {
	otis_start = otis;
	otis_end = otis + 1;
    }

    for (int s = station_start; s < station_end; s++) {
	for (int l = layer_start; l < layer_end; l++) {
	    for (int q = quarter_start; q < quarter_end; q++) {
		for (int m = module_start; m < module_end; m++) {
		    for (int o = otis_start; o < otis_end; ++o) {
			m_OTEfficiency[s][l][q][m][o]=efficiency;
		    }
		}
	    }
	}
    }
}


void FlagHitsForPatternReco::readITString(const char* oldstring)
{

    unsigned long station    = 999;
    unsigned long layer      = 999;
    unsigned long region     = 999;
    double efficiency        = 1.0;

    const char* newstring = beginsWith(oldstring,"S");

    if (newstring != oldstring) {
	oldstring = newstring;
	station = mystrtoul(oldstring, oldstring, 0);
	assert(station < nTStations);
    }

    newstring = beginsWith(oldstring, "L");

    if (newstring != oldstring) {
	oldstring = newstring;
	layer = mystrtoul(oldstring, oldstring, 0);
	assert(layer < nTLayers);
    }

    newstring = beginsWith(oldstring, "R");

    if (newstring != oldstring) {
	oldstring = newstring;
	region = mystrtoul(oldstring, oldstring, 0);
	assert(region < nITRegions);
    }

    newstring = beginsWith(oldstring, "E");

    if (newstring != oldstring) {
	efficiency = mystrtod(newstring, newstring);
	assert(0. <= efficiency && 1. >= efficiency);
    }

    int station_start = 0;
    int station_end = nTStations;

    if (station != 999) {
	station_start = station;
	station_end = station + 1;
    }

    int layer_start = 0;
    int layer_end = nTLayers;

    if (layer != 999) {
	layer_start = layer;
	layer_end = layer + 1;
    }

    int region_start = 0;
    int region_end = nITRegions;

    if (region != 999) {
	region_start = region;
	region_end = region + 1;
    }

    for (int s = station_start; s < station_end; s++) {
	for (int l = layer_start; l < layer_end; l++) {
	    for (int r = region_start; r < region_end; r++) {
		m_ITEfficiency[s][l][r]=efficiency;
	    }
	}
    }
}


void FlagHitsForPatternReco::readTTString(const char* oldstring)
{

    unsigned long station    = 999;
    unsigned long layer      = 999;
    unsigned long region     = 999;
    double efficiency        = 1.0;

    const char* newstring = beginsWith(oldstring,"S");

    if (newstring != oldstring) {
	oldstring = newstring;
	station = mystrtoul(oldstring, oldstring, 0);
	assert(station < nTTStations);
    }

    newstring = beginsWith(oldstring,"L");

    if (newstring != oldstring) {
	oldstring=newstring;
	layer = mystrtoul(oldstring, oldstring, 0);
	assert(layer < nTTLayers);
    }

    newstring = beginsWith(oldstring, "R");

    if (newstring != oldstring) {
	oldstring = newstring;
	region = mystrtoul(oldstring, oldstring, 0);
	assert(region < nTTRegions);
    }

    newstring = beginsWith(oldstring, "E");

    if (newstring != oldstring) {
	efficiency = mystrtod(newstring, newstring);
	assert(0. <= efficiency && 1. >= efficiency);
    }

    int station_start = 0;
    int station_end = nTTStations;

    if (station != 999) {
	station_start = station;
	station_end = station + 1;
    }

    int layer_start = 0;
    int layer_end = nTTLayers;

    if (layer != 999) {
	layer_start = layer;
	layer_end = layer + 1;
    }

    int region_start = 0;
    int region_end = nTTRegions;

    if (region != 999) {
	region_start = region;
	region_end = region + 1;
    }

    for (int s = station_start; s < station_end; s++) {
	for (int l = layer_start; l < layer_end; l++) {
	    for (int r = region_start; r < region_end; r++) {
		m_TTEfficiency[s][l][r]=efficiency;
	    }
	}
    }
}


void FlagHitsForPatternReco::readVeloString(const char* oldstring)
{

    unsigned long sensor     = 999;
    unsigned long type       = 999;
    unsigned long halves     = 999;
    unsigned long region     = 999;
    double efficiency        = 1.0;


    const char* newstring = beginsWith(oldstring, "S");

    if (newstring != oldstring) {
	oldstring = newstring;
	sensor = mystrtoul(oldstring, oldstring, 0);
	assert(sensor < nVeloSensors);
    }

    newstring = beginsWith(oldstring, "T");

    if (newstring != oldstring) {
	oldstring = newstring;
	type = mystrtoul(oldstring, oldstring, 0);
	assert(type < nVeloTypes);
    }

    newstring = beginsWith(oldstring, "H");

    if (newstring != oldstring) {
	oldstring = newstring;
	halves = mystrtoul(oldstring, oldstring, 0);
	assert(halves < nVeloHalves);
    }

    newstring = beginsWith(oldstring, "R");

    if (newstring != oldstring) {
	oldstring = newstring;
	region = mystrtoul(oldstring, oldstring, 0);
	assert(region < nVeloRegions);
    }

    newstring = beginsWith(oldstring, "E");

    if (newstring != oldstring) {
	efficiency = mystrtod(newstring, newstring);
	assert(0. <= efficiency && 1. >= efficiency);
    }

    int sensor_start = 0;
    int sensor_end = nVeloSensors;

    if (sensor != 999) {
	sensor_start = sensor;
	sensor_end = sensor + 1;
    }

    int type_start = 0;
    int type_end = nVeloTypes;

    if (type != 999) {
	type_start = type;
	type_end = type + 1;
    }

    int halves_start = 0;
    int halves_end = nVeloHalves;

    if (halves != 999) {
	halves_start = halves;
	halves_end = halves + 1;
    }

    int region_start = 0;
    int region_end = nVeloRegions;

    if (region != 999) {
	region_start = region;
	region_end = region + 1;
    }

    for (int s = sensor_start; s < sensor_end; s++) {
	for (int t = type_start; t < type_end; t++) {
	    for (int h = halves_start; h < halves_end; h++) {
		for (int r = region_start; r < region_end; r++) {
		    m_VeloEfficiency[s][t][h][r] = efficiency;
		}
	    }
	}
    }
}


void FlagHitsForPatternReco::readOTStringSmearing(const char* oldstring)
{

    unsigned long station    = 999;
    unsigned long layer      = 999;
    unsigned long quarter    = 999;
    unsigned long module     = 999;
    unsigned long otis	    = 999;
    double smearing = 0.0;

    const char* newstring = beginsWith(oldstring,"S");

    if (newstring != oldstring) {
	oldstring = newstring;
	station = mystrtoul(oldstring, oldstring, 0);
	assert(station < nTStations);
    }

    newstring = beginsWith(oldstring,"L");

    if (newstring != oldstring) {
	oldstring = newstring;
	layer = mystrtoul(oldstring, oldstring, 0);
	assert(layer < nTLayers);
    }

    newstring = beginsWith(oldstring,"Q");

    if (newstring != oldstring) {
	oldstring = newstring;
	quarter = mystrtoul(oldstring, oldstring, 0);
	assert(quarter < nQuadrants);
    }

    newstring = beginsWith(oldstring,"M");

    if (newstring != oldstring) {
	oldstring = newstring;
	module = mystrtoul(oldstring, oldstring, 0);
	assert(module < nModules);
    }
    newstring = beginsWith(oldstring,"OTIS");

    if (newstring != oldstring) {
	oldstring = newstring;
	otis = mystrtoul(oldstring, oldstring, 0);
	assert(otis < nOTOtis);
    }


    newstring = beginsWith(oldstring,"S");

    if (newstring != oldstring) {
	smearing = mystrtod(newstring, newstring);
    }

    int station_start = 0;
    int station_end = nTStations;

    if (station != 999) {
	station_start = station;
	station_end = station + 1;
    }

    int layer_start = 0;
    int layer_end = nTLayers;

    if (layer != 999) {
	layer_start = layer;
	layer_end = layer + 1;
    }

    int quarter_start = 0;
    int quarter_end = nQuadrants;

    if (quarter != 999) {
	quarter_start = quarter;
	quarter_end = quarter + 1;
    }

    int module_start = 0;
    int module_end = nModules;

    if (module != 999) {
	module_start = module;
	module_end = module + 1;
    }

    int otis_start = 0;
    int otis_end = nOTOtis;
    if (999 != otis) {
	otis_start = otis;
	otis_end = otis + 1;
    }

    for (int s = station_start; s < station_end; s++) {
	for (int l = layer_start; l < layer_end; l++) {
	    for (int q = quarter_start; q < quarter_end; q++) {
		for (int m = module_start; m < module_end; m++) {
		    for (int o = otis_start; o < otis_end; ++o) {
			m_OTSmearing[s][l][q][m][o]=smearing;
		    }
		}
	    }
	}
    }
}


void FlagHitsForPatternReco::readITStringSmearing(const char* oldstring)
{

    unsigned long station    = 999;
    unsigned long layer      = 999;
    unsigned long region     = 999;
    double smearing          = 0.0;

    const char* newstring = beginsWith(oldstring,"S");

    if (newstring != oldstring) {
	oldstring = newstring;
	station = mystrtoul(oldstring, oldstring, 0);
	assert(station < nTStations);
    }

    newstring = beginsWith(oldstring, "L");

    if (newstring != oldstring) {
	oldstring = newstring;
	layer = mystrtoul(oldstring, oldstring, 0);
	assert(layer < nTLayers);
    }

    newstring = beginsWith(oldstring, "R");

    if (newstring != oldstring) {
	oldstring = newstring;
	region = mystrtoul(oldstring, oldstring, 0);
	assert(region < nITRegions);
    }

    newstring = beginsWith(oldstring, "S");

    if (newstring != oldstring) {
	smearing = mystrtod(newstring, newstring);
    }

    int station_start = 0;
    int station_end = nTStations;

    if (station != 999) {
	station_start = station;
	station_end = station + 1;
    }

    int layer_start = 0;
    int layer_end = nTLayers;

    if (layer != 999) {
	layer_start = layer;
	layer_end = layer + 1;
    }

    int region_start = 0;
    int region_end = nITRegions;

    if (region != 999) {
	region_start = region;
	region_end = region + 1;
    }

    for (int s = station_start; s < station_end; s++) {
	for (int l = layer_start; l < layer_end; l++) {
	    for (int r = region_start; r < region_end; r++) {
		m_ITSmearing[s][l][r]=smearing;
	    }
	}
    }
}


void FlagHitsForPatternReco::readTTStringSmearing(const char* oldstring)
{

    unsigned long station    = 999;
    unsigned long layer      = 999;
    unsigned long region     = 999;
    double smearing        = 1.0;

    const char* newstring = beginsWith(oldstring,"S");

    if (newstring != oldstring) {
	oldstring = newstring;
	station = mystrtoul(oldstring, oldstring, 0);
	assert(station < nTTStations);
    }

    newstring = beginsWith(oldstring,"L");

    if (newstring != oldstring) {
	oldstring=newstring;
	layer = mystrtoul(oldstring, oldstring, 0);
	assert(layer < nTTLayers);
    }

    newstring = beginsWith(oldstring, "R");

    if (newstring != oldstring) {
	oldstring = newstring;
	region = mystrtoul(oldstring, oldstring, 0);
	assert(region < nTTRegions);
    }

    newstring = beginsWith(oldstring, "S");

    if (newstring != oldstring) {
      smearing = mystrtod(newstring, newstring);
    }

    int station_start = 0;
    int station_end = nTTStations;

    if (station != 999) {
	station_start = station;
	station_end = station + 1;
    }

    int layer_start = 0;
    int layer_end = nTTLayers;

    if (layer != 999) {
	layer_start = layer;
	layer_end = layer + 1;
    }

    int region_start = 0;
    int region_end = nTTRegions;

    if (region != 999) {
	region_start = region;
	region_end = region + 1;
    }

    for (int s = station_start; s < station_end; s++) {
	for (int l = layer_start; l < layer_end; l++) {
	    for (int r = region_start; r < region_end; r++) {
		m_TTSmearing[s][l][r]=smearing;
	    }
	}
    }
}


void FlagHitsForPatternReco::readVeloStringSmearing(const char* oldstring)
{

    unsigned long sensor     = 999;
    unsigned long type       = 999;
    unsigned long halves     = 999;
    unsigned long region     = 999;
    double smearing          = 0.0;


    const char* newstring = beginsWith(oldstring, "S");

    if (newstring != oldstring) {
	oldstring = newstring;
	sensor = mystrtoul(oldstring, oldstring, 0);
	assert(sensor < nVeloSensors);
    }

    newstring = beginsWith(oldstring, "T");

    if (newstring != oldstring) {
	oldstring = newstring;
	type = mystrtoul(oldstring, oldstring, 0);
	assert(type < nVeloTypes);
    }

    newstring = beginsWith(oldstring, "H");

    if (newstring != oldstring) {
	oldstring = newstring;
	halves = mystrtoul(oldstring, oldstring, 0);
	assert(halves < nVeloHalves);
    }

    newstring = beginsWith(oldstring, "R");

    if (newstring != oldstring) {
	oldstring = newstring;
	region = mystrtoul(oldstring, oldstring, 0);
	assert(region < nVeloRegions);
    }

    newstring = beginsWith(oldstring, "S");

    if (newstring != oldstring) {
	smearing = mystrtod(newstring, newstring);
    }

    int sensor_start = 0;
    int sensor_end = nVeloSensors;

    if (sensor != 999) {
	sensor_start = sensor;
	sensor_end = sensor + 1;
    }

    int type_start = 0;
    int type_end = nVeloTypes;

    if (type != 999) {
	type_start = type;
	type_end = type + 1;
    }

    int halves_start = 0;
    int halves_end = nVeloHalves;

    if (halves != 999) {
	halves_start = halves;
	halves_end = halves + 1;
    }

    int region_start = 0;
    int region_end = nVeloRegions;

    if (region != 999) {
	region_start = region;
	region_end = region + 1;
    }

    for (int s = sensor_start; s < sensor_end; s++) {
	for (int t = type_start; t < type_end; t++) {
	    for (int h = halves_start; h < halves_end; h++) {
		for (int r = region_start; r < region_end; r++) {
		    m_VeloSmearing[s][t][h][r] = smearing;
		}
	    }
	}
    }
}

const char* FlagHitsForPatternReco::beginsWith(const char* input, const char* comp)
{

    if (std::strlen(input) < std::strlen(comp))
	return NULL;

    const char* inputsave = input;
    while (*comp) {
	if (std::tolower(*input) == std::tolower(*comp)) {
	    ++input;
	    ++comp;
	} else {
	    return inputsave;
	}
    }

    return input;
}

int FlagHitsForPatternReco::otis(const LHCb::OTChannelID& otid)
{
    static const int otis[2][2][4] = {
	{ { 0, 1, 3, 2 }, { 2, 3, 1, 0 } },
	{ { 1, 0, 2, 3 }, { 3, 2, 0, 1 } }
    };

    // straws are numbered from 1 to 128... oh well...
    return otis[otid.quarter() >> 1][otid.layer() & 1][(otid.straw() - 1) >> 5];
}
