#ifndef _FlagHitsForPatternReco_H_
#define _FlagHitsForPatternReco_H_

/** @class FlagHitsForPatternReco FlagHitsForPatternReco.h
 *
 *
 *  @author S. Hansmann-Menzemer
 *  @date   17.01.2010
 */

#include "GaudiAlg/GaudiAlgorithm.h"
#include "TfKernel/TTStationHitManager.h"
#include "PatKernel/PatTTHit.h"
#include "TfKernel/TStationHitManager.h"
#include "PatKernel/PatForwardHit.h"
#include "TfKernel/DefaultVeloRHitManager.h"
#include "TfKernel/DefaultVeloPhiHitManager.h"
#include "GaudiKernel/RndmGenerators.h"


#include <string>
#include <vector>
#include <map>


// ALL NUMBERS START FROM 0 (e.g. modules go from 0,8)
// OTS0L2Q3M2E0.7 : OT: Station=0, Layer=2, Quater=3, Module=2 has efficiency 0.7
// VeloS4H0T0Z2E0.6: Velo: Sensor=4, Half=0, Type=0 (phi) [Type=1, R], zone=2 has efficiency 0.6
// ITS2L0R3E0.65: IT: Station=2, Layer=0, Region=3, has efficiency 0.65
// TTS1L0R4E0.7: TT: Station=1, Layer=0, Region=4 has efficiency 0.7
// If you skip one granularity this means ALL, e.g:
// OTS0L2M2E0.7:OT: Station=0, Layer=2, all Quarters, Module=2 has efficiency 0.7
// OTS0L2Q3E0.7 : OT: Station=0, Layer=2, Quater=3, all modules have efficiency 0.7

static const int nTStations = 3;
static const int nTLayers = 4;
static const int nModules = 9;
static const int nQuadrants = 4;
static const int nITRegions = 4;
static const int nVeloHalves = 2;
static const int nVeloTypes = 2;
static const int nVeloSensors = 23;
static const int nVeloRegions = 3;
static const int nTTStations = 2;
static const int nTTLayers = 2;
static const int nTTRegions = 12;




class FlagHitsForPatternReco: public GaudiAlgorithm {

public:
  

  // Constructors and destructor
  FlagHitsForPatternReco(const std::string& name,
		      ISvcLocator* pSvcLocator);
  virtual ~FlagHitsForPatternReco();

  virtual StatusCode initialize(); 

  virtual StatusCode execute();

private:

  double m_OTEfficiency[nTStations][nTLayers][nQuadrants][nModules];
  double m_ITEfficiency[nTStations][nTLayers][nITRegions];
  double m_VeloEfficiency[nVeloSensors][nVeloHalves][nVeloTypes][nVeloRegions];
  double m_TTEfficiency[nTTStations][nTTLayers][nTTRegions];
 

  std::vector<std::string> m_effCorrections;

  char* beginsWith (char* input, char* comp);
  
  void readVeloString(char* input);
  void readOTString(char* input);
  void readITString(char* input);
  void readTTString(char* input); 

  Tf::TStationHitManager<PatForwardHit> *  m_tHitManager; 
  Tf::TTStationHitManager<PatTTHit> * m_ttHitManager;
  Tf::DefaultVeloRHitManager* m_rHitManager;
  Tf::DefaultVeloPhiHitManager* m_phiHitManager;

  mutable Rndm::Numbers m_rndm;

};

#endif

