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

#include <cstdlib>
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
// OTS0L2Q3OTIS2E0.7 : OT: Station=0, Layer=2, Quater=3, all modules OTIS 2 have efficiency 0.7

namespace LHCb
{
    class OTChannelID;
}


class FlagHitsForPatternReco: public GaudiAlgorithm
{

        static const unsigned nTStations = 3;
        static const unsigned nTLayers = 4;
        static const unsigned nModules = 9;
        static const unsigned nQuadrants = 4;
        static const unsigned nITRegions = 4;
        static const unsigned nVeloHalves = 2;
        static const unsigned nVeloTypes = 2;
        static const unsigned nVeloSensors = 23;
        static const unsigned nVeloRegions = 3;
        static const unsigned nTTStations = 2;
        static const unsigned nTTLayers = 2;
        static const unsigned nTTRegions = 12;
        static const unsigned nOTOtis = 4;

    public:


        // Constructors and destructor
        FlagHitsForPatternReco(const std::string& name,
                               ISvcLocator* pSvcLocator);
        virtual ~FlagHitsForPatternReco();

        virtual StatusCode initialize();

        virtual StatusCode execute();

    private:

        double m_OTEfficiency[nTStations][nTLayers][nQuadrants][nModules][nOTOtis];
        double m_ITEfficiency[nTStations][nTLayers][nITRegions];
        double m_VeloEfficiency[nVeloSensors][nVeloHalves][nVeloTypes][nVeloRegions];
        double m_TTEfficiency[nTTStations][nTTLayers][nTTRegions];

	double m_OTSmearing[nTStations][nTLayers][nQuadrants][nModules][nOTOtis];
        double m_ITSmearing[nTStations][nTLayers][nITRegions];
        double m_VeloSmearing[nVeloSensors][nVeloHalves][nVeloTypes][nVeloRegions];
        double m_TTSmearing[nTTStations][nTTLayers][nTTRegions];


        std::vector<std::string> m_effCorrections; 
	std::vector<std::string> m_misCorrections;

        const char* beginsWith (const char* input, const char* comp);

        void readVeloString(const char* input);
        void readOTString(const char* input);
        void readITString(const char* input);
        void readTTString(const char* input);

	void readVeloStringSmearing(const char* input);
        void readOTStringSmearing(const char* input);
        void readITStringSmearing(const char* input);
        void readTTStringSmearing(const char* input);

        static int otis(const LHCb::OTChannelID& otid);

        Tf::TStationHitManager<PatForwardHit> *  m_tHitManager;
        Tf::TTStationHitManager<PatTTHit> * m_ttHitManager;
        Tf::DefaultVeloRHitManager* m_rHitManager;
        Tf::DefaultVeloPhiHitManager* m_phiHitManager;
	static inline unsigned long mystrtoul(const char* str, const char*& endptr, int base)
	{
	    char *myend;
	    unsigned long tmp = std::strtoul(str, &myend, base);
	    endptr = myend;
	    return tmp;
	}
	static inline double mystrtod(const char* str, const char*& endptr)
	{
	    char *myend;
	    double tmp = std::strtod(str, &myend);
	    endptr = myend;
	    return tmp;
	}


        mutable Rndm::Numbers m_rndm;
	mutable Rndm::Numbers m_rndmG;

};

#endif

