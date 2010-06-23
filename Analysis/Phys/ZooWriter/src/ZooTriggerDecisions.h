/** @file ZooTriggerDecisions.h
 *
 * @author Manuel Schiller
 * @data 2009-12-22
 */
#ifndef ZOOTRIGGERDECISIONS_H
#define ZOOTRIGGERDECISIONS_H

#include <vector>
#include <utility>
#ifndef __CINT__
#include <boost/static_assert.hpp>
#else
#define BOOST_STATIC_ASSERT(x)
#endif

class ZooWriter;

/// namespace to hold all trigger decisions
namespace TriggerDecisions {
    /// size of trigger decision words array
    const unsigned NTriggerDecisionWords = 10;
    /// basic struct to hold all trigger decision bits for writing them out in batch
    class AllTriggerDecisions {
	protected:
	    /// a list of trigger decisions
	    typedef std::vector<std::pair<
		const char*, std::pair<unsigned, unsigned> > > DecisionList;
	    /// class variable holds all known trigger decisions
	    static DecisionList alldecisions;

	    // ZooWriter is a friend because it is interested in our decision
	    // list
	    friend class ::ZooWriter;
	    /// forbid instantiations by others
	    AllTriggerDecisions() { }
    };

    /// basic class for HLT trigger decisions
    template<unsigned WORD, unsigned MASK> class TriggerDec :
	private AllTriggerDecisions
    {
	public:
	    /// word in trigger decision array
	    static const unsigned word = WORD;
	    /// bitmask in trigger decision array
	    static const unsigned mask = MASK;
	    /// string associated with trigger decision on DaVinci side
	    static const char* str;
	private:
	    /// uncallable constructor (on purpose)
	    TriggerDec()
	    {
		// make sure compiler complains before our array gets too small
		BOOST_STATIC_ASSERT(WORD < TriggerDecisions::NTriggerDecisionWords);
		// register new trigger decision with AllTriggerDecisions
		AllTriggerDecisions::alldecisions.push_back(
			std::make_pair(str, std::make_pair(word, mask)));
	    }
	    /// single instance of object to register with AllTriggerDecisions
	    static const TriggerDec<WORD, MASK> m_self;
    };
}

// define static variable in ZooTriggerDecisions.cpp (and nowhere else)
#ifdef ZOOTRIGGERDECISIONS_CPP
namespace TriggerDecisions {
    AllTriggerDecisions::DecisionList AllTriggerDecisions::alldecisions;
}
#endif


/// define a trigger decision
/** define a trigger decition named NAME, to be saved in word WORD with
 * mask MASK and a HLT decision string HLTDECSTR
 *
 * example:
 * 
 * MakeTriggerDec(Hlt7UselessDecision, 3, 0x80000000, "DaVinciSucksDecision")
 *
 * This would generate the type Hlt7UselessDecision, to be saved in trigger
 * decision word 3, bit mask 0x80000000, and on DaVinci level, the
 * "DaVinciSucksDecision" trigger decision would be queried.
 *
 * Note that the type name (1st macro argument) and the string (last argument)
 * need not match. This makes it easy to accomodate name changes in HLT1 while
 * keeping the same name on Zoo level (and vice versa).
 *
 * If the last string is empty, the ZooWriter will ignore the Trigger decision
 * and not query the HLT decision report. This is useful to combine several
 * trigger decisions within a single word with or, as in
 *
 * MakeTriggerDec(Hlt9CombinedDecision, 4, 0x0c007732, "")
 */
#ifdef ZOOTRIGGERDECISIONS_CPP
#define MakeTriggerDec(NAME, WORD, MASK, HLTDECSTR) \
    namespace TriggerDecisions { \
	typedef TriggerDec<WORD, MASK> NAME; \
	template<> const char* NAME::str = HLTDECSTR; \
	template<> const NAME NAME::m_self = NAME(); \
    }
#else
#define MakeTriggerDec(NAME, WORD, MASK, HLTDECSTR) \
    namespace TriggerDecisions { \
	typedef TriggerDec<WORD, MASK> NAME; \
    }
#endif




// word 0 for L0 Decision bits
MakeTriggerDec(L0B1gas,					0, 0x00000001, "B1gas")
MakeTriggerDec(L0B2gas,					0, 0x00000002, "B2gas")
MakeTriggerDec(L0Calo,					0, 0x00000004, "CALO")
MakeTriggerDec(L0DiMuon,				0, 0x00000008, "DiMuon")
MakeTriggerDec(L0DiMuonlowMult,				0, 0x00000010, "DiMuon,lowMult")
MakeTriggerDec(L0Electron,				0, 0x00000020, "Electron")
MakeTriggerDec(L0GlobalPi0,				0, 0x00000040, "GlobalPi0")
MakeTriggerDec(L0HCAL,					0, 0x00000080, "HCAL")
MakeTriggerDec(L0Hadron,				0, 0x00000100, "Hadron")
MakeTriggerDec(L0LocalPi0,				0, 0x00000200, "LocalPi0")
MakeTriggerDec(L0MuonMinbias,				0, 0x00000400, "Muon,minbias")
MakeTriggerDec(L0Muon,					0, 0x00000800, "Muon")
MakeTriggerDec(L0MuonLowMult,				0, 0x00001000, "Muon,lowMult")
MakeTriggerDec(L0PU,					0, 0x00002000, "PU")
MakeTriggerDec(L0PU20,					0, 0x00004000, "PU20")
MakeTriggerDec(L0Photon,				0, 0x00008000, "Photon")
MakeTriggerDec(L0SPD,					0, 0x00010000, "SPD")
MakeTriggerDec(L0SPD40,					0, 0x00020000, "SPD40")

// Word 1 - HLT1 Global and Muon decisions
MakeTriggerDec(Hlt1Global,				1,0x00000001,"Hlt1Global")
MakeTriggerDec(Hlt1DiMuonIPC2L0,			1,0x00000002,"Hlt1DiMuonIPC2L0")
MakeTriggerDec(Hlt1DiMuonIPCGECSeg,			1,0x00000004,"Hlt1DiMuonIPCGECSeg")
MakeTriggerDec(Hlt1DiMuonIPCL0Di,			1,0x00000008,"Hlt1DiMuonIPCL0Di")
MakeTriggerDec(Hlt1DiMuonIPCL0GEC,			1,0x00000010,"Hlt1DiMuonIPCL0GEC")
MakeTriggerDec(Hlt1DiMuonIPCL0Seg,			1,0x00000020,"Hlt1DiMuonIPCL0Seg")
MakeTriggerDec(Hlt1DiMuonNoIP2L0,			1,0x00000040,"Hlt1DiMuonNoIP2L0")
MakeTriggerDec(Hlt1DiMuonNoIPGECSeg,			1,0x00000080,"Hlt1DiMuonNoIPGECSeg")
MakeTriggerDec(Hlt1DiMuonNoIPL0Di,			1,0x00000100,"Hlt1DiMuonNoIPL0Di")
MakeTriggerDec(Hlt1DiMuonNoIPL0GEC,			1,0x00000200,"Hlt1DiMuonNoIPL0GEC")
MakeTriggerDec(Hlt1DiMuonNoIPL0Seg,			1,0x00000400,"Hlt1DiMuonNoIPL0Seg")
MakeTriggerDec(Hlt1DiMuonNoPV2L0,			1,0x00000800,"Hlt1DiMuonNoPV2L0")
MakeTriggerDec(Hlt1DiMuonNoPVL0Di,			1,0x00001000,"Hlt1DiMuonNoPVL0Di")
MakeTriggerDec(Hlt1DiMuonNoPVL0Seg,			1,0x00002000,"Hlt1DiMuonNoPVL0Seg")
MakeTriggerDec(Hlt1DiMuonNoVeloL0,			1,0x00004000,"Hlt1DiMuonNoVeloL0")
MakeTriggerDec(Hlt1SingleMuonIPCGEC,			1,0x00008000,"Hlt1SingleMuonIPCGEC")
MakeTriggerDec(Hlt1SingleMuonIPCL0,			1,0x00010000,"Hlt1SingleMuonIPCL0")
MakeTriggerDec(Hlt1SingleMuonNoIPGEC,			1,0x00020000,"Hlt1SingleMuonNoIPGEC")
MakeTriggerDec(Hlt1SingleMuonNoIPL0,			1,0x00040000,"Hlt1SingleMuonNoIPL0")
MakeTriggerDec(Hlt1SingleMuonNoVeloL0,			1,0x00080000,"Hlt1SingleMuonNoVeloL0")
MakeTriggerDec(Hlt1MuTrack,				1,0x00100000,"Hlt1MuTrack")
MakeTriggerDec(Hlt1MuTrack4JPsi,			1,0x00200000,"Hlt1MuTrack4JPsi")

//Word 2 - HLT1 Hadron and electromagnetic decisions
MakeTriggerDec(Hlt1DiHadron,				2,0x00000001,"Hlt1DiHadron")
MakeTriggerDec(Hlt1DiHadronLTUnbiased,			2,0x00000002,"Hlt1DiHadronLTUnbiased")
MakeTriggerDec(Hlt1DiHadronViaT,			2,0x00000004,"Hlt1DiHadronViaT")
MakeTriggerDec(Hlt1HadronMonComp,			2,0x00000008,"Hlt1HadronMonComp")
MakeTriggerDec(Hlt1HadronMonConf1,			2,0x00000010,"Hlt1HadronMonConf1")
MakeTriggerDec(Hlt1HadronMonConf2,			2,0x00000020,"Hlt1HadronMonConf2")
MakeTriggerDec(Hlt1SingleHadron,			2,0x00000040,"Hlt1SingleHadron")
MakeTriggerDec(Hlt1SingleHadronViaT,			2,0x00000080,"Hlt1SingleHadronViaT")
MakeTriggerDec(Hlt1HadronMonVeloReco,			2,0x00000100,"Hlt1HadronMonVeloReco")
MakeTriggerDec(Hlt1PhotonDiTrack,			2,0x00000200,"Hlt1PhotonDiTrack")
MakeTriggerDec(Hlt1PhotonDiTrackFromEle,		2,0x00000400,"Hlt1PhotonDiTrackFromEle")
MakeTriggerDec(Hlt1PhotonTrack,				2,0x00000800,"Hlt1PhotonTrack")
MakeTriggerDec(Hlt1PhotonTrackFromEle,			2,0x00001000,"Hlt1PhotonTrackFromEle")
MakeTriggerDec(Hlt1SingleElectronNoIP,			2,0x00002000,"Hlt1SingleElectronNoIP")
MakeTriggerDec(Hlt1SingleElectronWithIP,		2,0x00004000,"Hlt1SingleElectronWithIP")
MakeTriggerDec(Hlt1ElectronTrackNoIP,			2,0x00008000,"Hlt1ElectronTrackNoIP")
MakeTriggerDec(Hlt1ElectronTrackWithIP,			2,0x00010000,"Hlt1ElectronTrackWithIP")

//Word 3 - HLT1 Minbias, Lumi and Benchmark decisions
MakeTriggerDec(Hlt1BeamGasBeam1,			3,0x00000001,"Hlt1BeamGasBeam1")
MakeTriggerDec(Hlt1BeamGasBeam2,			3,0x00000002,"Hlt1BeamGasBeam2")
MakeTriggerDec(Hlt1BeamGasCrossing,			3,0x00000004,"Hlt1BeamGasCrossing")
MakeTriggerDec(Hlt1BeamGasCrossingForcedRZReco,		3,0x00000008,"Hlt1BeamGasCrossingForcedRZReco")
MakeTriggerDec(Hlt1CosmicOT,				3,0x00000010,"Hlt1CosmicOT")
MakeTriggerDec(Hlt1Incident,				3,0x00000020,"Hlt1Incident")
MakeTriggerDec(Hlt1Lumi,				3,0x00000040,"Hlt1Lumi")
MakeTriggerDec(Hlt1LumiLowBeam1,			3,0x00000080,"Hlt1LumiLowBeam1")
MakeTriggerDec(Hlt1LumiLowBeam1RateLimited,		3,0x00000100,"Hlt1LumiLowBeam1RateLimited")
MakeTriggerDec(Hlt1LumiLowBeam2,			3,0x00000200,"Hlt1LumiLowBeam2")
MakeTriggerDec(Hlt1LumiLowBeam2RateLimited,		3,0x00000400,"Hlt1LumiLowBeam2RateLimited")
MakeTriggerDec(Hlt1LumiLowBeamCrossing,			3,0x00000800,"Hlt1LumiLowBeamCrossing")
MakeTriggerDec(Hlt1LumiLowBeamCrossingRateLimited,	3,0x00001000,"Hlt1LumiLowBeamCrossingRateLimited")
MakeTriggerDec(Hlt1LumiLowNoBeam,			3,0x00002000,"Hlt1LumiLowNoBeam")
MakeTriggerDec(Hlt1LumiLowNoBeamRateLimited,		3,0x00004000,"Hlt1LumiLowNoBeamRateLimited")
MakeTriggerDec(Hlt1LumiMidBeamCrossing,			3,0x00008000,"Hlt1LumiMidBeamCrossing")
MakeTriggerDec(Hlt1MBMicroBiasRZVelo,			3,0x00010000,"Hlt1MBMicroBiasRZVelo")
MakeTriggerDec(Hlt1MBMicroBiasRZVeloRateLimited,	3,0x00020000,"Hlt1MBMicroBiasRZVeloRateLimited")
MakeTriggerDec(Hlt1MBMicroBiasTStation,			3,0x00040000,"Hlt1MBMicroBiasTStation")
MakeTriggerDec(Hlt1MBMicroBiasTStationRateLimited,	3,0x00080000,"Hlt1MBMicroBiasTStationRateLimited")
MakeTriggerDec(Hlt1MBMiniBias,				3,0x00100000,"Hlt1MBMiniBias")
MakeTriggerDec(Hlt1MBNoBias,				3,0x00200000,"Hlt1MBNoBias")
MakeTriggerDec(Hlt1ODINPhysics,				3,0x00400000,"Hlt1ODINPhysics")
MakeTriggerDec(Hlt1ODINTechnical,			3,0x00800000,"Hlt1ODINTechnical")
MakeTriggerDec(Hlt1Tell1Error,				3,0x01000000,"Hlt1Tell1Error")
MakeTriggerDec(Hlt1VeloASide,				3,0x02000000,"Hlt1VeloASide")
MakeTriggerDec(Hlt1VeloCSide,				3,0x04000000,"Hlt1VeloCSide")

//Word 4 - HLT1 L0 Decisions
MakeTriggerDec(Hlt1L0Any,				4,0x00000001,"Hlt1L0Any")
MakeTriggerDec(Hlt1L0B1gas,				4,0x00000002,"Hlt1L0B1gas")
MakeTriggerDec(Hlt1L0B2gas,				4,0x00000004,"Hlt1L0B2gas")
MakeTriggerDec(Hlt1L0CALO,				4,0x00000008,"Hlt1L0CALO")
MakeTriggerDec(Hlt1L0DiMuon,				4,0x00000010,"Hlt1L0DiMuon")
MakeTriggerDec(Hlt1L0DiMuonlowMult,			4,0x00000020,"Hlt1L0DiMuon,lowMult")
MakeTriggerDec(Hlt1L0Electron,				4,0x00000040,"Hlt1L0Electron")
MakeTriggerDec(Hlt1L0GlobalPi0,				4,0x00000080,"Hlt1L0GlobalPi0")
MakeTriggerDec(Hlt1L0HCAL,				4,0x00000100,"Hlt1L0HCAL")
MakeTriggerDec(Hlt1L0Hadron,				4,0x00000200,"Hlt1L0Hadron")
MakeTriggerDec(Hlt1L0LocalPi0,				4,0x00000400,"Hlt1L0LocalPi0")
MakeTriggerDec(Hlt1L0MUONminbias,			4,0x00000800,"Hlt1L0MUON,minbias")
MakeTriggerDec(Hlt1L0Muon,				4,0x00001000,"Hlt1L0Muon")
MakeTriggerDec(Hlt1L0MuonlowMult,			4,0x00002000,"Hlt1L0Muon,lowMult")
MakeTriggerDec(Hlt1L0PU,				4,0x00004000,"Hlt1L0PU")
MakeTriggerDec(Hlt1L0PU20,				4,0x00008000,"Hlt1L0PU20")
MakeTriggerDec(Hlt1L0Photon,				4,0x00010000,"Hlt1L0Photon")
MakeTriggerDec(Hlt1L0SPD,				4,0x00020000,"Hlt1L0SPD")
MakeTriggerDec(Hlt1L0SPD40,				4,0x00400000,"Hlt1L0SPD40")
MakeTriggerDec(Hlt1L0AnyRateLimited,			4,0x00800000,"Hlt1L0AnyRateLimited")

//Words 5-9 - HLT2 Decisions
MakeTriggerDec(Hlt2Global,				5,0x00000001,"Hlt2Global")
// Add additional decisions as follows
// MakeTriggerDec(MyUselessHLTDecision,					5, 0x00000001, "HLTOnDaVinciHeLLSuCksDecision")

#endif // ZOOTRIGGERDECISIONS_H

// vim:sw=4:tw=78:ft=cpp
