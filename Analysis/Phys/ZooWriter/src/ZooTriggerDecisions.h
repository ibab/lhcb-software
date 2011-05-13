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



//HLT1 Lines


MakeTriggerDec(Hlt1BeamGasBeam1                        ,1,0x00000001,"Hlt1BeamGasBeam1Decision");
MakeTriggerDec(Hlt1BeamGasCrossingLonelyBeam1          ,1,0x00000002,"Hlt1BeamGasCrossingLonelyBeam1Decision");
MakeTriggerDec(Hlt1BeamGasBeam2                        ,1,0x00000004,"Hlt1BeamGasBeam2Decision");
MakeTriggerDec(Hlt1BeamGasCrossingLonelyBeam2          ,1,0x00000008,"Hlt1BeamGasCrossingLonelyBeam2Decision");
MakeTriggerDec(Hlt1BeamGasCrossingForcedRZReco         ,1,0x00000010,"Hlt1BeamGasCrossingForcedRZRecoDecision");
MakeTriggerDec(Hlt1ODINTechnical                       ,1,0x00000020,"Hlt1ODINTechnicalDecision");
MakeTriggerDec(Hlt1Tell1Error                          ,1,0x00000040,"Hlt1Tell1ErrorDecision");
MakeTriggerDec(Hlt1SingleElectronWithIP                ,1,0x00000080,"Hlt1SingleElectronWithIPDecision");
MakeTriggerDec(Hlt1SingleElectronNoIP                  ,1,0x00000100,"Hlt1SingleElectronNoIPDecision");
MakeTriggerDec(Hlt1ElectronTrackWithIP                 ,1,0x00000200,"Hlt1ElectronTrackWithIPDecision");
MakeTriggerDec(Hlt1ElectronTrackNoIP                   ,1,0x00000400,"Hlt1ElectronTrackNoIPDecision");
MakeTriggerDec(Hlt1TrackAllL0                          ,1,0x00000800,"Hlt1TrackAllL0Decision");
MakeTriggerDec(Hlt1TrackMuon                           ,1,0x00001000,"Hlt1TrackMuonDecision");
MakeTriggerDec(Hlt1TrackPhoton                         ,1,0x00002000,"Hlt1TrackPhotonDecision");
MakeTriggerDec(Hlt1Lumi                                ,1,0x00004000,"Hlt1LumiDecision");
MakeTriggerDec(Hlt1LumiLowNoBeam                       ,1,0x00008000,"Hlt1LumiLowNoBeamDecision");
MakeTriggerDec(Hlt1LumiLowBeamCrossing                 ,1,0x00010000,"Hlt1LumiLowBeamCrossingDecision");
MakeTriggerDec(Hlt1LumiLowBeam1                        ,1,0x00020000,"Hlt1LumiLowBeam1Decision");
MakeTriggerDec(Hlt1LumiLowBeam2                        ,1,0x00040000,"Hlt1LumiLowBeam2Decision");
MakeTriggerDec(Hlt1LumiMidBeamCrossing                 ,1,0x00080000,"Hlt1LumiMidBeamCrossingDecision");
MakeTriggerDec(Hlt1PhotonTrack                         ,1,0x00100000,"Hlt1PhotonTrackDecision");
MakeTriggerDec(Hlt1PhotonTrackFromEle                  ,1,0x00200000,"Hlt1PhotonTrackFromEleDecision");
MakeTriggerDec(Hlt1PhotonDiTrack                       ,1,0x00400000,"Hlt1PhotonDiTrackDecision");
MakeTriggerDec(Hlt1PhotonDiTrackFromEle                ,1,0x00800000,"Hlt1PhotonDiTrackFromEleDecision");
MakeTriggerDec(Hlt1MBNoBias                            ,1,0x01000000,"Hlt1MBNoBiasDecision");
MakeTriggerDec(Hlt1MBMicroBiasRZVelo                   ,1,0x02000000,"Hlt1MBMicroBiasRZVeloDecision");
MakeTriggerDec(Hlt1MBMicroBiasRZVeloRateLimited        ,1,0x04000000,"Hlt1MBMicroBiasRZVeloRateLimitedDecision");
MakeTriggerDec(Hlt1MBMicroBiasTStation                 ,1,0x08000000,"Hlt1MBMicroBiasTStationDecision");
MakeTriggerDec(Hlt1MBMicroBiasTStationRateLimited      ,1,0x10000000,"Hlt1MBMicroBiasTStationRateLimitedDecision");
MakeTriggerDec(Hlt1SingleHadron                        ,1,0x20000000,"Hlt1SingleHadronDecision");
MakeTriggerDec(Hlt1DiHadron                            ,1,0x40000000,"Hlt1DiHadronDecision");
MakeTriggerDec(Hlt1L0Any                               ,1,0x80000000,"Hlt1L0AnyDecision");


MakeTriggerDec(Hlt1L0AnyRateLimited                    ,2,0x00000001,"Hlt1L0AnyRateLimitedDecision");
MakeTriggerDec(Hlt1NoPVPassThrough                     ,2,0x00000002,"Hlt1NoPVPassThroughDecision");
MakeTriggerDec(Hlt1SingleMuonNoIPL0                    ,2,0x00000004,"Hlt1SingleMuonNoIPL0Decision");
MakeTriggerDec(Hlt1SingleMuonNoIPL0HighPT              ,2,0x00000008,"Hlt1SingleMuonNoIPL0HighPTDecision");
MakeTriggerDec(Hlt1SingleMuonIPCL0                     ,2,0x00000010,"Hlt1SingleMuonIPCL0Decision");
MakeTriggerDec(Hlt1DiMuonNoIPL0Di                      ,2,0x00000020,"Hlt1DiMuonNoIPL0DiDecision");
MakeTriggerDec(Hlt1SingleMuon4BsMuMu                   ,2,0x00000040,"Hlt1SingleMuon4BsMuMuDecision");
MakeTriggerDec(Hlt1DiMuon4BsMuMu                       ,2,0x00000080,"Hlt1DiMuon4BsMuMuDecision");
MakeTriggerDec(Hlt1DiMuonNoIP2L0                       ,2,0x00000100,"Hlt1DiMuonNoIP2L0Decision");
MakeTriggerDec(Hlt1DiMuonNoIPL0Seg                     ,2,0x00000200,"Hlt1DiMuonNoIPL0SegDecision");
MakeTriggerDec(Hlt1DiMuonIPCL0Di                       ,2,0x00000400,"Hlt1DiMuonIPCL0DiDecision");
MakeTriggerDec(Hlt1DiMuonIPC2L0                        ,2,0x00000800,"Hlt1DiMuonIPC2L0Decision");
MakeTriggerDec(Hlt1DiMuonIPCL0Seg                      ,2,0x00001000,"Hlt1DiMuonIPCL0SegDecision");
MakeTriggerDec(Hlt1DiMuonNoPVL0Di                      ,2,0x00002000,"Hlt1DiMuonNoPVL0DiDecision");
MakeTriggerDec(Hlt1DiMuonNoPV2L0                       ,2,0x00004000,"Hlt1DiMuonNoPV2L0Decision");
MakeTriggerDec(Hlt1DiMuonNoPVL0Seg                     ,2,0x00008000,"Hlt1DiMuonNoPVL0SegDecision");
MakeTriggerDec(Hlt1MuTrack                             ,2,0x00010000,"Hlt1MuTrackDecision");
MakeTriggerDec(Hlt1BeamGasCrossing                     ,2,0x00020000,"Hlt1BeamGasCrossingDecision");
MakeTriggerDec(Hlt1Incident                            ,2,0x00040000,"Hlt1IncidentDecision");
MakeTriggerDec(Hlt1ErrorEvent                          ,2,0x00080000,"Hlt1ErrorEventDecision");
MakeTriggerDec(Hlt1Global                              ,2,0x00100000,"Hlt1Global");
MakeTriggerDec(Hlt1NZSVelo                             ,2,0x00200000,"Hlt1NZSVeloDecision");
MakeTriggerDec(Hlt1VeloASide                           ,2,0x00400000,"Hlt1VeloASideDecision");
MakeTriggerDec(Hlt1VeloCSide                           ,2,0x00800000,"Hlt1VeloCSideDecision");
MakeTriggerDec(Hlt1HadronMonVeloReco                   ,2,0x01000000,"Hlt1HadronMonVeloRecoDecision");
MakeTriggerDec(Hlt1HadronMonConf1                      ,2,0x02000000,"Hlt1HadronMonConf1Decision");
MakeTriggerDec(Hlt1HadronMonConf2                      ,2,0x04000000,"Hlt1HadronMonConf2Decision");
MakeTriggerDec(Hlt1HadronMonComp                       ,2,0x08000000,"Hlt1HadronMonCompDecision");
MakeTriggerDec(Hlt1DiHadronLTUnbiased                  ,2,0x10000000,"Hlt1DiHadronLTUnbiasedDecision");
MakeTriggerDec(Hlt1L0Muon                              ,2,0x20000000,"Hlt1L0MuonDecision");
MakeTriggerDec(Hlt1L0MUONminbias                      ,2,0x40000000,"Hlt1L0MUON,minbiasDecision");
MakeTriggerDec(Hlt1L0PU20                              ,2,0x80000000,"Hlt1L0PU20Decision");


MakeTriggerDec(Hlt1L0B2gas                             ,3,0x00000001,"Hlt1L0B2gasDecision");
MakeTriggerDec(Hlt1L0PU                                ,3,0x00000002,"Hlt1L0PUDecision");
MakeTriggerDec(Hlt1L0DiMuon                            ,3,0x00000004,"Hlt1L0DiMuonDecision");
MakeTriggerDec(Hlt1L0Photon                            ,3,0x00000008,"Hlt1L0PhotonDecision");
MakeTriggerDec(Hlt1L0SPD40                             ,3,0x00000010,"Hlt1L0SPD40Decision");
MakeTriggerDec(Hlt1L0Electron                          ,3,0x00000020,"Hlt1L0ElectronDecision");
MakeTriggerDec(Hlt1L0B1gas                             ,3,0x00000040,"Hlt1L0B1gasDecision");
MakeTriggerDec(Hlt1L0DiMuonlowMult                    ,3,0x00000080,"Hlt1L0DiMuon,lowMultDecision");
MakeTriggerDec(Hlt1L0CALO                              ,3,0x00000100,"Hlt1L0CALODecision");
MakeTriggerDec(Hlt1L0MuonlowMult                      ,3,0x00000200,"Hlt1L0Muon,lowMultDecision");
MakeTriggerDec(Hlt1L0SPD                               ,3,0x00000400,"Hlt1L0SPDDecision");
MakeTriggerDec(Hlt1L0Hadron                            ,3,0x00000800,"Hlt1L0HadronDecision");
MakeTriggerDec(Hlt1MuTrack4JPsi                        ,3,0x00001000,"Hlt1MuTrack4JPsiDecision");

//HLT2 Lines


MakeTriggerDec(Hlt2PileUp                              ,4,0x00000001,"Hlt2PileUpDecision");
MakeTriggerDec(Hlt2BeamGasNonBBCrossing                ,4,0x00000002,"Hlt2BeamGasNonBBCrossingDecision");
MakeTriggerDec(Hlt2BeamGasBBCrossing                   ,4,0x00000004,"Hlt2BeamGasBBCrossingDecision");
MakeTriggerDec(Hlt2SingleTFElectron                    ,4,0x00000008,"Hlt2SingleTFElectronDecision");
MakeTriggerDec(Hlt2TFElectronPlusTrack                 ,4,0x00000010,"Hlt2TFElectronPlusTrackDecision");
MakeTriggerDec(Hlt2UnbiasedTFDiElectron                ,4,0x00000020,"Hlt2UnbiasedTFDiElectronDecision");
MakeTriggerDec(Hlt2UnbiasedTFDiElectronLowMass         ,4,0x00000040,"Hlt2UnbiasedTFDiElectronLowMassDecision");
MakeTriggerDec(Hlt2UnbiasedTFDiElectronLowPID          ,4,0x00000080,"Hlt2UnbiasedTFDiElectronLowPIDDecision");
MakeTriggerDec(Hlt2UnbiasedTFJpsi2ee                   ,4,0x00000100,"Hlt2UnbiasedTFJpsi2eeDecision");
MakeTriggerDec(Hlt2UnbiasedTFPsi2ee                    ,4,0x00000200,"Hlt2UnbiasedTFPsi2eeDecision");
MakeTriggerDec(Hlt2UnbiasedTFB2ee                      ,4,0x00000400,"Hlt2UnbiasedTFB2eeDecision");
MakeTriggerDec(Hlt2BiasedTFDiElectron                  ,4,0x00000800,"Hlt2BiasedTFDiElectronDecision");
MakeTriggerDec(Hlt2BiasedTFDiElectronLowMass           ,4,0x00001000,"Hlt2BiasedTFDiElectronLowMassDecision");
MakeTriggerDec(Hlt2BiasedTFDiElectronLowPID            ,4,0x00002000,"Hlt2BiasedTFDiElectronLowPIDDecision");
MakeTriggerDec(Hlt2BiasedTFJpsi2ee                     ,4,0x00004000,"Hlt2BiasedTFJpsi2eeDecision");
MakeTriggerDec(Hlt2BiasedTFPsi2ee                      ,4,0x00008000,"Hlt2BiasedTFPsi2eeDecision");
MakeTriggerDec(Hlt2BiasedTFB2ee                        ,4,0x00010000,"Hlt2BiasedTFB2eeDecision");
MakeTriggerDec(Hlt2DiMuonSameSign                      ,4,0x00020000,"Hlt2DiMuonSameSignDecision");
MakeTriggerDec(Hlt2UnbiasedDiMuon                      ,4,0x00040000,"Hlt2UnbiasedDiMuonDecision");
MakeTriggerDec(Hlt2PromptJPsi                          ,4,0x00080000,"Hlt2PromptJPsiDecision");
MakeTriggerDec(Hlt2PromptJPsiHighPT                    ,4,0x00100000,"Hlt2PromptJPsiHighPTDecision");
MakeTriggerDec(Hlt2UnbiasedDiMuonLowMass               ,4,0x00200000,"Hlt2UnbiasedDiMuonLowMassDecision");
MakeTriggerDec(Hlt2DiMuonUnbiasedJPsi                  ,4,0x00400000,"Hlt2DiMuonUnbiasedJPsiDecision");
MakeTriggerDec(Hlt2DiMuonUnbiasedPsi2S                 ,4,0x00800000,"Hlt2DiMuonUnbiasedPsi2SDecision");
MakeTriggerDec(Hlt2DiMuonUnbiasedBmm                   ,4,0x01000000,"Hlt2DiMuonUnbiasedBmmDecision");
MakeTriggerDec(Hlt2DiMuonUnbiasedZmm                   ,4,0x02000000,"Hlt2DiMuonUnbiasedZmmDecision");
MakeTriggerDec(Hlt2DiMuonDY1                           ,4,0x04000000,"Hlt2DiMuonDY1Decision");
MakeTriggerDec(Hlt2DiMuonDY2                           ,4,0x08000000,"Hlt2DiMuonDY2Decision");
MakeTriggerDec(Hlt2DiMuonDY3                           ,4,0x10000000,"Hlt2DiMuonDY3Decision");
MakeTriggerDec(Hlt2DiMuonDY4                           ,4,0x20000000,"Hlt2DiMuonDY4Decision");
MakeTriggerDec(Hlt2BiasedDiMuonSimple                  ,4,0x40000000,"Hlt2BiasedDiMuonSimpleDecision");
MakeTriggerDec(Hlt2BiasedDiMuonRefined                 ,4,0x80000000,"Hlt2BiasedDiMuonRefinedDecision");


MakeTriggerDec(Hlt2BiasedDiMuonMass                    ,5,0x00000001,"Hlt2BiasedDiMuonMassDecision");
MakeTriggerDec(Hlt2BiasedDiMuonIP                      ,5,0x00000002,"Hlt2BiasedDiMuonIPDecision");
MakeTriggerDec(Hlt2DiMuonBiasedJPsi                    ,5,0x00000004,"Hlt2DiMuonBiasedJPsiDecision");
MakeTriggerDec(Hlt2CharmOSTF2Body                      ,5,0x00000008,"Hlt2CharmOSTF2BodyDecision");
MakeTriggerDec(Hlt2CharmOSTF2BodyWideMass              ,5,0x00000010,"Hlt2CharmOSTF2BodyWideMassDecision");
MakeTriggerDec(Hlt2CharmOSTF3Body                      ,5,0x00000020,"Hlt2CharmOSTF3BodyDecision");
MakeTriggerDec(Hlt2CharmOSTF3BodyWideMass              ,5,0x00000040,"Hlt2CharmOSTF3BodyWideMassDecision");
MakeTriggerDec(Hlt2CharmD02MuMu                        ,5,0x00000080,"Hlt2CharmD02MuMuDecision");
MakeTriggerDec(Hlt2CharmD02PiPiForD02MuMu              ,5,0x00000100,"Hlt2CharmD02PiPiForD02MuMuDecision");
MakeTriggerDec(Hlt2CharmD02KPiForD02MuMu               ,5,0x00000200,"Hlt2CharmD02KPiForD02MuMuDecision");
MakeTriggerDec(Hlt2CharmD02KKForD02MuMu                ,5,0x00000400,"Hlt2CharmD02KKForD02MuMuDecision");
MakeTriggerDec(Hlt2CharmD2KS0PiLL                      ,5,0x00000800,"Hlt2CharmD2KS0PiLLDecision");
MakeTriggerDec(Hlt2TopoOSTF2Body                       ,5,0x00001000,"Hlt2TopoOSTF2BodyDecision");
MakeTriggerDec(Hlt2TopoOSTF3Body                       ,5,0x00002000,"Hlt2TopoOSTF3BodyDecision");
MakeTriggerDec(Hlt2TopoOSTF4Body                       ,5,0x00004000,"Hlt2TopoOSTF4BodyDecision");
MakeTriggerDec(Hlt2IncPhi                              ,5,0x00008000,"Hlt2IncPhiDecision");
MakeTriggerDec(Hlt2IncPhiRobust                        ,5,0x00010000,"Hlt2IncPhiRobustDecision");
MakeTriggerDec(Hlt2IncPhiTrackFit                      ,5,0x00020000,"Hlt2IncPhiTrackFitDecision");
MakeTriggerDec(Hlt2IncPhiSidebands                     ,5,0x00040000,"Hlt2IncPhiSidebandsDecision");
MakeTriggerDec(Hlt2B2KstareeFittedLargeBMassWindow     ,5,0x00080000,"Hlt2B2KstareeFittedLargeBMassWindowDecision");
MakeTriggerDec(Hlt2B2KstareeFittedLargeBMassWindowSignal,5,0x00100000,"Hlt2B2KstareeFittedLargeBMassWindowSignalDecision");
MakeTriggerDec(Hlt2Bs2PhiPhi                           ,5,0x00200000,"Hlt2Bs2PhiPhiDecision");
MakeTriggerDec(Hlt2Dst2PiD02PiPi                       ,5,0x00400000,"Hlt2Dst2PiD02PiPiDecision");
MakeTriggerDec(Hlt2Dst2PiD02MuMu                       ,5,0x00800000,"Hlt2Dst2PiD02MuMuDecision");
MakeTriggerDec(Hlt2Dst2PiD02PiMu                       ,5,0x01000000,"Hlt2Dst2PiD02PiMuDecision");
MakeTriggerDec(Hlt2Dst2PiD02KMu                        ,5,0x02000000,"Hlt2Dst2PiD02KMuDecision");
MakeTriggerDec(Hlt2Dst2PiD02KPi                        ,5,0x04000000,"Hlt2Dst2PiD02KPiDecision");
MakeTriggerDec(Hlt2Dst2PiD02EMu                        ,5,0x08000000,"Hlt2Dst2PiD02EMuDecision");
MakeTriggerDec(Hlt2PassThrough                         ,5,0x10000000,"Hlt2PassThroughDecision");
MakeTriggerDec(Hlt2Transparent                         ,5,0x20000000,"Hlt2TransparentDecision");
MakeTriggerDec(Hlt2Forward                             ,5,0x40000000,"Hlt2ForwardDecision");
MakeTriggerDec(Hlt2DebugEvent                          ,5,0x80000000,"Hlt2DebugEventDecision");


MakeTriggerDec(Hlt2ExpressJPsi                         ,6,0x00000001,"Hlt2ExpressJPsiDecision");
MakeTriggerDec(Hlt2ExpressJPsiTagProbe                 ,6,0x00000002,"Hlt2ExpressJPsiTagProbeDecision");
MakeTriggerDec(Hlt2ExpressLambda                       ,6,0x00000004,"Hlt2ExpressLambdaDecision");
MakeTriggerDec(Hlt2ExpressKS                           ,6,0x00000008,"Hlt2ExpressKSDecision");
MakeTriggerDec(Hlt2ExpressDs2PhiPi                     ,6,0x00000010,"Hlt2ExpressDs2PhiPiDecision");
MakeTriggerDec(Hlt2ExpressBeamHalo                     ,6,0x00000020,"Hlt2ExpressBeamHaloDecision");
MakeTriggerDec(Hlt2ExpressDStar2D0Pi                   ,6,0x00000040,"Hlt2ExpressDStar2D0PiDecision");
MakeTriggerDec(Hlt2ExpressHLT1Physics                  ,6,0x00000080,"Hlt2ExpressHLT1PhysicsDecision");
MakeTriggerDec(Hlt2Bs2PhiGamma                         ,6,0x00000100,"Hlt2Bs2PhiGammaDecision");
MakeTriggerDec(Hlt2Bs2PhiGammaNoCutsK                  ,6,0x00000200,"Hlt2Bs2PhiGammaNoCutsKDecision");
MakeTriggerDec(Hlt2Bs2PhiGammaWideBMass                ,6,0x00000400,"Hlt2Bs2PhiGammaWideBMassDecision");
MakeTriggerDec(Hlt2Bs2PhiGammaLooseDira                ,6,0x00000800,"Hlt2Bs2PhiGammaLooseDiraDecision");
MakeTriggerDec(Hlt2Bd2KstGamma                         ,6,0x00001000,"Hlt2Bd2KstGammaDecision");
MakeTriggerDec(Hlt2Bd2KstGammaNoCutsKPi                ,6,0x00002000,"Hlt2Bd2KstGammaNoCutsKPiDecision");
MakeTriggerDec(Hlt2Bd2KstGammaWideKMass                ,6,0x00004000,"Hlt2Bd2KstGammaWideKMassDecision");
MakeTriggerDec(Hlt2Bd2KstGammaWideBMass                ,6,0x00008000,"Hlt2Bd2KstGammaWideBMassDecision");
MakeTriggerDec(Hlt2Bd2KstGammaLooseDira                ,6,0x00010000,"Hlt2Bd2KstGammaLooseDiraDecision");
MakeTriggerDec(Hlt2B2HH                                ,6,0x00020000,"Hlt2B2HHDecision");
MakeTriggerDec(Hlt2MuonFromHLT1                        ,6,0x00040000,"Hlt2MuonFromHLT1Decision");
MakeTriggerDec(Hlt2SingleMuon                          ,6,0x00080000,"Hlt2SingleMuonDecision");
MakeTriggerDec(Hlt2SingleHighPTMuon                    ,6,0x00100000,"Hlt2SingleHighPTMuonDecision");
MakeTriggerDec(Hlt2MuTrack                             ,6,0x00200000,"Hlt2MuTrackDecision");
MakeTriggerDec(Hlt2MuTrackNoIP                         ,6,0x00400000,"Hlt2MuTrackNoIPDecision");
MakeTriggerDec(Hlt2IncDiProton                         ,6,0x00800000,"Hlt2IncDiProtonDecision");
MakeTriggerDec(Hlt2IncDiProtonPresel                   ,6,0x01000000,"Hlt2IncDiProtonPreselDecision");
MakeTriggerDec(Hlt2IncDiProtonTrackFitted              ,6,0x02000000,"Hlt2IncDiProtonTrackFittedDecision");
MakeTriggerDec(Hlt2TFBs2JpsieePhiUnbiasedPT            ,6,0x04000000,"Hlt2TFBs2JpsieePhiUnbiasedPTDecision");
MakeTriggerDec(Hlt2TFBs2JpsieePhiSignal                ,6,0x08000000,"Hlt2TFBs2JpsieePhiSignalDecision");
MakeTriggerDec(Hlt2TFBc2JpsiMuX                        ,6,0x10000000,"Hlt2TFBc2JpsiMuXDecision");
MakeTriggerDec(Hlt2TFBc2JpsiMuXSignal                  ,6,0x20000000,"Hlt2TFBc2JpsiMuXSignalDecision");
MakeTriggerDec(Hlt2TFBc2JpsiH                          ,6,0x40000000,"Hlt2TFBc2JpsiHDecision");
MakeTriggerDec(Hlt2TFBc2JpsiHPrescaled                 ,6,0x80000000,"Hlt2TFBc2JpsiHPrescaledDecision");


MakeTriggerDec(Hlt2TFBc2JpsiHDetached                  ,7,0x00000001,"Hlt2TFBc2JpsiHDetachedDecision");
MakeTriggerDec(Hlt2diphotonDiMuon                      ,7,0x00000002,"Hlt2diphotonDiMuonDecision");
MakeTriggerDec(Hlt2DisplVerticesSingle                 ,7,0x00000004,"Hlt2DisplVerticesSingleDecision");
MakeTriggerDec(Hlt2DisplVerticesSinglePostScaled       ,7,0x00000008,"Hlt2DisplVerticesSinglePostScaledDecision");
MakeTriggerDec(Hlt2DisplVerticesDouble                 ,7,0x00000010,"Hlt2DisplVerticesDoubleDecision");
MakeTriggerDec(Hlt2B2HHPi0_Merged                      ,7,0x00000020,"Hlt2B2HHPi0_MergedDecision");
MakeTriggerDec(Hlt2B2HHPi0_Resolved                    ,7,0x00000040,"Hlt2B2HHPi0_ResolvedDecision");
MakeTriggerDec(Hlt2ErrorEvent                          ,7,0x00000080,"Hlt2ErrorEventDecision");
MakeTriggerDec(Hlt2Global                              ,7,0x00000100,"Hlt2Global");
MakeTriggerDec(Hlt2Dst2PiD02PiPiD0SB                   ,7,0x00000200,"Hlt2Dst2PiD02PiPiD0SBDecision");
MakeTriggerDec(Hlt2Dst2PiD02PiPiDMSB                   ,7,0x00000400,"Hlt2Dst2PiD02PiPiDMSBDecision");
MakeTriggerDec(Hlt2Dst2PiD02PiPiSignal                 ,7,0x00000800,"Hlt2Dst2PiD02PiPiSignalDecision");
MakeTriggerDec(Hlt2Dst2PiD02MuMuD0SB                   ,7,0x00001000,"Hlt2Dst2PiD02MuMuD0SBDecision");
MakeTriggerDec(Hlt2Dst2PiD02MuMuDMSB                   ,7,0x00002000,"Hlt2Dst2PiD02MuMuDMSBDecision");
MakeTriggerDec(Hlt2Dst2PiD02MuMuSignal                 ,7,0x00004000,"Hlt2Dst2PiD02MuMuSignalDecision");
MakeTriggerDec(Hlt2Dst2PiD02KPiD0SB                    ,7,0x00008000,"Hlt2Dst2PiD02KPiD0SBDecision");
MakeTriggerDec(Hlt2Dst2PiD02KPiDMSB                    ,7,0x00010000,"Hlt2Dst2PiD02KPiDMSBDecision");
MakeTriggerDec(Hlt2Dst2PiD02KPiSignal                  ,7,0x00020000,"Hlt2Dst2PiD02KPiSignalDecision");
MakeTriggerDec(Hlt2Charm2BodySA                        ,7,0x00040000,"Hlt2Charm2BodySADecision");
MakeTriggerDec(Hlt2CharmTF2BodySA                      ,7,0x00080000,"Hlt2CharmTF2BodySADecision");
MakeTriggerDec(Hlt2CharmTF2BodySignal                  ,7,0x00100000,"Hlt2CharmTF2BodySignalDecision");
MakeTriggerDec(Hlt2Charm3BodySA                        ,7,0x00200000,"Hlt2Charm3BodySADecision");
MakeTriggerDec(Hlt2CharmTF3BodySA                      ,7,0x00400000,"Hlt2CharmTF3BodySADecision");
MakeTriggerDec(Hlt2CharmTF3BodySignal                  ,7,0x00800000,"Hlt2CharmTF3BodySignalDecision");
MakeTriggerDec(Hlt2CharmTF2BodyWideMass                ,7,0x01000000,"Hlt2CharmTF2BodyWideMassDecision");
MakeTriggerDec(Hlt2CharmTF2BodyWideMassSA              ,7,0x02000000,"Hlt2CharmTF2BodyWideMassSADecision");
MakeTriggerDec(Hlt2CharmTF3BodyWideMass                ,7,0x04000000,"Hlt2CharmTF3BodyWideMassDecision");
MakeTriggerDec(Hlt2CharmTF3BodyWideMassSA              ,7,0x08000000,"Hlt2CharmTF3BodyWideMassSADecision");
MakeTriggerDec(Hlt2B2HHLTUnbiased                      ,7,0x10000000,"Hlt2B2HHLTUnbiasedDecision");
MakeTriggerDec(Hlt2Topo2BodySA                         ,7,0x20000000,"Hlt2Topo2BodySADecision");
MakeTriggerDec(Hlt2Topo3BodySA                         ,7,0x40000000,"Hlt2Topo3BodySADecision");
MakeTriggerDec(Hlt2TopoTF3BodySA                       ,7,0x80000000,"Hlt2TopoTF3BodySADecision");


MakeTriggerDec(Hlt2TopoTF2BodySA                       ,8,0x00000001,"Hlt2TopoTF2BodySADecision");
MakeTriggerDec(Hlt2TopoTF3BodyReq3Yes                  ,8,0x00000002,"Hlt2TopoTF3BodyReq3YesDecision");
MakeTriggerDec(Hlt2TopoTF3BodyReq2Yes                  ,8,0x00000004,"Hlt2TopoTF3BodyReq2YesDecision");
MakeTriggerDec(Hlt2TopoTF2BodyReq3Yes                  ,8,0x00000008,"Hlt2TopoTF2BodyReq3YesDecision");
MakeTriggerDec(Hlt2TopoTF2BodyReq2Yes                  ,8,0x00000010,"Hlt2TopoTF2BodyReq2YesDecision");
MakeTriggerDec(Hlt2TopoRobTF2Body                      ,8,0x00000020,"Hlt2TopoRobTF2BodyDecision");
MakeTriggerDec(Hlt2TopoRobTF3Body                      ,8,0x00000040,"Hlt2TopoRobTF3BodyDecision");
MakeTriggerDec(Hlt2B2D2hhhBachelorWithKsSignal         ,8,0x00000080,"Hlt2B2D2hhhBachelorWithKsSignalDecision");
MakeTriggerDec(Hlt2B2D2hhBachelorSignal                ,8,0x00000100,"Hlt2B2D2hhBachelorSignalDecision");
MakeTriggerDec(Hlt2B2D2hhhBachelorWithKsBroadMW        ,8,0x00000200,"Hlt2B2D2hhhBachelorWithKsBroadMWDecision");
MakeTriggerDec(Hlt2B2D2hhBachelorBroadMW               ,8,0x00000400,"Hlt2B2D2hhBachelorBroadMWDecision");
MakeTriggerDec(Hlt2B2D2XRobust3BodyMonitor             ,8,0x00000800,"Hlt2B2D2XRobust3BodyMonitorDecision");
MakeTriggerDec(Hlt2B2D2hhBachelorSignalMonitor         ,8,0x00001000,"Hlt2B2D2hhBachelorSignalMonitorDecision");
MakeTriggerDec(Hlt2B2D2hhhBachelorWithKsBroadMWMonitor ,8,0x00002000,"Hlt2B2D2hhhBachelorWithKsBroadMWMonitorDecision");
MakeTriggerDec(Hlt2B2D2hhBachelorBroadMWMonitor        ,8,0x00004000,"Hlt2B2D2hhBachelorBroadMWMonitorDecision");


/////2010 Data
/*
// Word 1 - HLT1 Global and Muon decisions
MakeTriggerDec(Hlt1Global,						1,0x00000001,"Hlt1Global");
MakeTriggerDec(Hlt1SingleMuonNoIPL0,					1,0x00000002,"Hlt1SingleMuonNoIPL0Decision");
MakeTriggerDec(Hlt1SingleMuonIPCL0,					1,0x00000004,"Hlt1SingleMuonIPCL0Decision");
MakeTriggerDec(Hlt1DiMuonNoIPL0Di,					1,0x00000008,"Hlt1DiMuonNoIPL0DiDecision");
MakeTriggerDec(Hlt1DiMuonNoIP2L0,					1,0x00000010,"Hlt1DiMuonNoIP2L0Decision");
MakeTriggerDec(Hlt1DiMuonNoIPL0Seg,					1,0x00000020,"Hlt1DiMuonNoIPL0SegDecision");
MakeTriggerDec(Hlt1DiMuonIPCL0Di,					1,0x00000040,"Hlt1DiMuonIPCL0DiDecision");
MakeTriggerDec(Hlt1DiMuonIPC2L0,					1,0x00000080,"Hlt1DiMuonIPC2L0Decision");
MakeTriggerDec(Hlt1DiMuonIPCL0Seg,					1,0x00000100,"Hlt1DiMuonIPCL0SegDecision");
MakeTriggerDec(Hlt1DiMuonNoPVL0Di,					1,0x00000200,"Hlt1DiMuonNoPVL0DiDecision");
MakeTriggerDec(Hlt1DiMuonNoPV2L0,					1,0x00000400,"Hlt1DiMuonNoPV2L0Decision");
MakeTriggerDec(Hlt1DiMuonNoPVL0Seg,					1,0x00000800,"Hlt1DiMuonNoPVL0SegDecision");
MakeTriggerDec(Hlt1MuTrack,						1,0x00001000,"Hlt1MuTrackDecision");
MakeTriggerDec(Hlt1MuTrack4JPsi,					1,0x00002000,"Hlt1MuTrack4JPsiDecision");

//Word 2 - HLT1 Hadron and electromagnetic decisions
MakeTriggerDec(Hlt1SingleElectronWithIP,				2,0x00000001,"Hlt1SingleElectronWithIPDecision");
MakeTriggerDec(Hlt1SingleElectronNoIP,					2,0x00000002,"Hlt1SingleElectronNoIPDecision");
MakeTriggerDec(Hlt1ElectronTrackWithIP,					2,0x00000004,"Hlt1ElectronTrackWithIPDecision");
MakeTriggerDec(Hlt1ElectronTrackNoIP,					2,0x00000008,"Hlt1ElectronTrackNoIPDecision");
MakeTriggerDec(Hlt1PhotonTrack,						2,0x00000010,"Hlt1PhotonTrackDecision");
MakeTriggerDec(Hlt1PhotonTrackFromEle,					2,0x00000020,"Hlt1PhotonTrackFromEleDecision");
MakeTriggerDec(Hlt1PhotonDiTrack,					2,0x00000040,"Hlt1PhotonDiTrackDecision");
MakeTriggerDec(Hlt1PhotonDiTrackFromEle,				2,0x00000080,"Hlt1PhotonDiTrackFromEleDecision");
MakeTriggerDec(Hlt1SingleHadron,					2,0x00000100,"Hlt1SingleHadronDecision");
MakeTriggerDec(Hlt1DiHadron,						2,0x00000200,"Hlt1DiHadronDecision");
MakeTriggerDec(Hlt1HadronMonVeloReco,					2,0x00000400,"Hlt1HadronMonVeloRecoDecision");
MakeTriggerDec(Hlt1HadronMonConf1,					2,0x00000800,"Hlt1HadronMonConf1Decision");
MakeTriggerDec(Hlt1HadronMonConf2,					2,0x00001000,"Hlt1HadronMonConf2Decision");
MakeTriggerDec(Hlt1HadronMonComp,					2,0x00002000,"Hlt1HadronMonCompDecision");
MakeTriggerDec(Hlt1DiHadronLTUnbiased,					2,0x00004000,"Hlt1DiHadronLTUnbiasedDecision");

//Word 3 - HLT1 Minbias, Lumi and Benchmark decisions
MakeTriggerDec(Hlt1VeloASide,						3,0x00000001,"Hlt1VeloASideDecision");
MakeTriggerDec(Hlt1VeloCSide,						3,0x00000002,"Hlt1VeloCSideDecision");
MakeTriggerDec(Hlt1BeamGasBeam1,					3,0x00000004,"Hlt1BeamGasBeam1Decision");
MakeTriggerDec(Hlt1BeamGasBeam2,					3,0x00000008,"Hlt1BeamGasBeam2Decision");
MakeTriggerDec(Hlt1BeamGasCrossingForcedRZReco,				3,0x00000010,"Hlt1BeamGasCrossingForcedRZRecoDecision");
MakeTriggerDec(Hlt1ODINTechnical,					3,0x00000020,"Hlt1ODINTechnicalDecision");
MakeTriggerDec(Hlt1Tell1Error,						3,0x00000040,"Hlt1Tell1ErrorDecision");
MakeTriggerDec(Hlt1Lumi,						3,0x00000080,"Hlt1LumiDecision");
MakeTriggerDec(Hlt1LumiLowNoBeam,					3,0x00000100,"Hlt1LumiLowNoBeamDecision");
MakeTriggerDec(Hlt1LumiLowBeamCrossing,					3,0x00000200,"Hlt1LumiLowBeamCrossingDecision");
MakeTriggerDec(Hlt1LumiLowBeam1,					3,0x00000400,"Hlt1LumiLowBeam1Decision");
MakeTriggerDec(Hlt1LumiLowBeam2,					3,0x00000800,"Hlt1LumiLowBeam2Decision");
MakeTriggerDec(Hlt1LumiMidBeamCrossing,					3,0x00001000,"Hlt1LumiMidBeamCrossingDecision");
MakeTriggerDec(Hlt1MBNoBias,						3,0x00002000,"Hlt1MBNoBiasDecision");
MakeTriggerDec(Hlt1MBMicroBiasRZVelo,					3,0x00004000,"Hlt1MBMicroBiasRZVeloDecision");
MakeTriggerDec(Hlt1MBMicroBiasRZVeloRateLimited,			3,0x00008000,"Hlt1MBMicroBiasRZVeloRateLimitedDecision");
MakeTriggerDec(Hlt1MBMicroBiasTStation,					3,0x00010000,"Hlt1MBMicroBiasTStationDecision");
MakeTriggerDec(Hlt1MBMicroBiasTStationRateLimited,			3,0x00020000,"Hlt1MBMicroBiasTStationRateLimitedDecision");
MakeTriggerDec(Hlt1BeamGasCrossing,					3,0x00040000,"Hlt1BeamGasCrossingDecision");
MakeTriggerDec(Hlt1Incident,						3,0x00080000,"Hlt1IncidentDecision");
MakeTriggerDec(Hlt1ErrorEvent,						3,0x00100000,"Hlt1ErrorEventDecision");

//Word 4 - HLT1 L0 Decisions
MakeTriggerDec(Hlt1L0Muon,						4,0x00000001,"Hlt1L0MuonDecision");
MakeTriggerDec(Hlt1L0MUONminbias,					4,0x00000002,"Hlt1L0MUON,minbiasDecision");
MakeTriggerDec(Hlt1L0PU20,						4,0x00000004,"Hlt1L0PU20Decision");
MakeTriggerDec(Hlt1L0B2gas,						4,0x00000008,"Hlt1L0B2gasDecision");
MakeTriggerDec(Hlt1L0PU,						4,0x00000010,"Hlt1L0PUDecision");
MakeTriggerDec(Hlt1L0DiMuon,						4,0x00000020,"Hlt1L0DiMuonDecision");
MakeTriggerDec(Hlt1L0Photon,						4,0x00000040,"Hlt1L0PhotonDecision");
MakeTriggerDec(Hlt1L0SPD40,						4,0x00000080,"Hlt1L0SPD40Decision");
MakeTriggerDec(Hlt1L0Electron,						4,0x00000100,"Hlt1L0ElectronDecision");
MakeTriggerDec(Hlt1L0B1gas,						4,0x00000200,"Hlt1L0B1gasDecision");
MakeTriggerDec(Hlt1L0DiMuonlowMult,					4,0x00000400,"Hlt1L0DiMuon,lowMultDecision");
MakeTriggerDec(Hlt1L0CALO,						4,0x00000800,"Hlt1L0CALODecision");
MakeTriggerDec(Hlt1L0MuonlowMult,					4,0x00001000,"Hlt1L0Muon,lowMultDecision");
MakeTriggerDec(Hlt1L0SPD,						4,0x00002000,"Hlt1L0SPDDecision");
MakeTriggerDec(Hlt1L0Hadron,						4,0x00004000,"Hlt1L0HadronDecision");
MakeTriggerDec(Hlt1L0Any,						4,0x00008000,"Hlt1L0AnyDecision");
MakeTriggerDec(Hlt1L0AnyRateLimited,					4,0x00010000,"Hlt1L0AnyRateLimitedDecision");



//HLT2 Lines, no particular order!
MakeTriggerDec(Hlt2Global,						5,0x00000001,"Hlt2Global");
MakeTriggerDec(Hlt2PileUp,						5,0x00000002,"Hlt2PileUpDecision");
MakeTriggerDec(Hlt2SingleTFElectron,					5,0x00000004,"Hlt2SingleTFElectronDecision");
MakeTriggerDec(Hlt2TFElectronPlusTrack,					5,0x00000008,"Hlt2TFElectronPlusTrackDecision");
MakeTriggerDec(Hlt2UnbiasedTFDiElectron,				5,0x00000010,"Hlt2UnbiasedTFDiElectronDecision");
MakeTriggerDec(Hlt2UnbiasedTFDiElectronLowMass,				5,0x00000020,"Hlt2UnbiasedTFDiElectronLowMassDecision");
MakeTriggerDec(Hlt2UnbiasedTFDiElectronLowPID,				5,0x00000040,"Hlt2UnbiasedTFDiElectronLowPIDDecision");
MakeTriggerDec(Hlt2UnbiasedTFJpsi2ee,					5,0x00000080,"Hlt2UnbiasedTFJpsi2eeDecision");
MakeTriggerDec(Hlt2UnbiasedTFPsi2ee,					5,0x00000100,"Hlt2UnbiasedTFPsi2eeDecision");
MakeTriggerDec(Hlt2UnbiasedTFB2ee,					5,0x00000200,"Hlt2UnbiasedTFB2eeDecision");
MakeTriggerDec(Hlt2BiasedTFDiElectron,					5,0x00000400,"Hlt2BiasedTFDiElectronDecision");
MakeTriggerDec(Hlt2BiasedTFDiElectronLowMass,				5,0x00000800,"Hlt2BiasedTFDiElectronLowMassDecision");
MakeTriggerDec(Hlt2BiasedTFDiElectronLowPID,				5,0x00001000,"Hlt2BiasedTFDiElectronLowPIDDecision");
MakeTriggerDec(Hlt2BiasedTFJpsi2ee,					5,0x00002000,"Hlt2BiasedTFJpsi2eeDecision");
MakeTriggerDec(Hlt2BiasedTFPsi2ee,					5,0x00004000,"Hlt2BiasedTFPsi2eeDecision");
MakeTriggerDec(Hlt2BiasedTFB2ee,					5,0x00008000,"Hlt2BiasedTFB2eeDecision");
MakeTriggerDec(Hlt2DiMuonSameSign,					5,0x00010000,"Hlt2DiMuonSameSignDecision");
MakeTriggerDec(Hlt2UnbiasedDiMuon,					5,0x00020000,"Hlt2UnbiasedDiMuonDecision");
MakeTriggerDec(Hlt2UnbiasedDiMuonLowMass,				5,0x00040000,"Hlt2UnbiasedDiMuonLowMassDecision");
MakeTriggerDec(Hlt2DiMuonUnbiasedJPsi,					5,0x00080000,"Hlt2DiMuonUnbiasedJPsiDecision");
MakeTriggerDec(Hlt2DiMuonUnbiasedPsi2S,					5,0x00100000,"Hlt2DiMuonUnbiasedPsi2SDecision");
MakeTriggerDec(Hlt2DiMuonUnbiasedBmm,					5,0x00200000,"Hlt2DiMuonUnbiasedBmmDecision");
MakeTriggerDec(Hlt2DiMuonUnbiasedZmm,					5,0x00400000,"Hlt2DiMuonUnbiasedZmmDecision");
MakeTriggerDec(Hlt2DiMuonDY1,						5,0x00800000,"Hlt2DiMuonDY1Decision");
MakeTriggerDec(Hlt2DiMuonDY2,						5,0x01000000,"Hlt2DiMuonDY2Decision");
MakeTriggerDec(Hlt2DiMuonDY3,						5,0x02000000,"Hlt2DiMuonDY3Decision");
MakeTriggerDec(Hlt2DiMuonDY4,						5,0x04000000,"Hlt2DiMuonDY4Decision");
MakeTriggerDec(Hlt2BiasedDiMuonSimple,					5,0x08000000,"Hlt2BiasedDiMuonSimpleDecision");
MakeTriggerDec(Hlt2BiasedDiMuonRefined,					5,0x10000000,"Hlt2BiasedDiMuonRefinedDecision");
MakeTriggerDec(Hlt2BiasedDiMuonMass,					5,0x20000000,"Hlt2BiasedDiMuonMassDecision");
MakeTriggerDec(Hlt2BiasedDiMuonIP,					5,0x40000000,"Hlt2BiasedDiMuonIPDecision");
MakeTriggerDec(Hlt2Charm2BodySA,					5,0x80000000,"Hlt2Charm2BodySADecision");

MakeTriggerDec(Hlt2CharmTF2BodySA,					6,0x00000001,"Hlt2CharmTF2BodySADecision");
MakeTriggerDec(Hlt2CharmTF2BodySignal,					6,0x00000002,"Hlt2CharmTF2BodySignalDecision");
MakeTriggerDec(Hlt2Charm3BodySA,					6,0x00000004,"Hlt2Charm3BodySADecision");
MakeTriggerDec(Hlt2CharmTF3BodySA,					6,0x00000008,"Hlt2CharmTF3BodySADecision");
MakeTriggerDec(Hlt2CharmTF3BodySignal,					6,0x00000010,"Hlt2CharmTF3BodySignalDecision");
MakeTriggerDec(Hlt2CharmTF2BodyWideMass,				6,0x00000020,"Hlt2CharmTF2BodyWideMassDecision");
MakeTriggerDec(Hlt2CharmTF2BodyWideMassSA,				6,0x00000040,"Hlt2CharmTF2BodyWideMassSADecision");
MakeTriggerDec(Hlt2CharmTF3BodyWideMass,				6,0x00000080,"Hlt2CharmTF3BodyWideMassDecision");
MakeTriggerDec(Hlt2CharmTF3BodyWideMassSA,				6,0x00000100,"Hlt2CharmTF3BodyWideMassSADecision");
MakeTriggerDec(Hlt2CharmD02MuMu,					6,0x00000200,"Hlt2CharmD02MuMuDecision");
MakeTriggerDec(Hlt2CharmD02PiPiForD02MuMu,				6,0x00000400,"Hlt2CharmD02PiPiForD02MuMuDecision");
MakeTriggerDec(Hlt2CharmD02KPiForD02MuMu,				6,0x00000800,"Hlt2CharmD02KPiForD02MuMuDecision");
MakeTriggerDec(Hlt2CharmD02KKForD02MuMu,				6,0x00001000,"Hlt2CharmD02KKForD02MuMuDecision");
MakeTriggerDec(Hlt2B2HHLTUnbiased,					6,0x00002000,"Hlt2B2HHLTUnbiasedDecision");
MakeTriggerDec(Hlt2Topo2BodySA,						6,0x00004000,"Hlt2Topo2BodySADecision");
MakeTriggerDec(Hlt2Topo3BodySA,						6,0x00008000,"Hlt2Topo3BodySADecision");
MakeTriggerDec(Hlt2TopoTF3BodySA,					6,0x00010000,"Hlt2TopoTF3BodySADecision");
MakeTriggerDec(Hlt2TopoTF2BodySA,					6,0x00020000,"Hlt2TopoTF2BodySADecision");
MakeTriggerDec(Hlt2TopoTF3BodyReq3Yes,					6,0x00040000,"Hlt2TopoTF3BodyReq3YesDecision");
MakeTriggerDec(Hlt2TopoTF3BodyReq2Yes,					6,0x00080000,"Hlt2TopoTF3BodyReq2YesDecision");
MakeTriggerDec(Hlt2TopoTF2BodyReq3Yes,					6,0x00100000,"Hlt2TopoTF2BodyReq3YesDecision");
MakeTriggerDec(Hlt2TopoTF2BodyReq2Yes,					6,0x00200000,"Hlt2TopoTF2BodyReq2YesDecision");
MakeTriggerDec(Hlt2TopoOSTF2Body,					6,0x00400000,"Hlt2TopoOSTF2BodyDecision");
MakeTriggerDec(Hlt2TopoOSTF3Body,					6,0x00800000,"Hlt2TopoOSTF3BodyDecision");
MakeTriggerDec(Hlt2TopoRobTF2Body,					6,0x01000000,"Hlt2TopoRobTF2BodyDecision");
MakeTriggerDec(Hlt2TopoRobTF3Body,					6,0x02000000,"Hlt2TopoRobTF3BodyDecision");
MakeTriggerDec(Hlt2IncPhi,						6,0x04000000,"Hlt2IncPhiDecision");
MakeTriggerDec(Hlt2IncPhiRobust,					6,0x08000000,"Hlt2IncPhiRobustDecision");
MakeTriggerDec(Hlt2IncPhiTrackFit,					6,0x10000000,"Hlt2IncPhiTrackFitDecision");
MakeTriggerDec(Hlt2IncPhiSidebands,					6,0x20000000,"Hlt2IncPhiSidebandsDecision");
MakeTriggerDec(Hlt2B2KstareeFittedLargeBMassWindow,			6,0x40000000,"Hlt2B2KstareeFittedLargeBMassWindowDecision");
MakeTriggerDec(Hlt2B2KstareeFittedLargeBMassWindowSignal,		6,0x80000000,"Hlt2B2KstareeFittedLargeBMassWindowSignalDecision");

MakeTriggerDec(Hlt2Bs2PhiPhi,						7,0x00000001,"Hlt2Bs2PhiPhiDecision");
MakeTriggerDec(Hlt2Dst2PiD02PiPi,					7,0x00000002,"Hlt2Dst2PiD02PiPiDecision");
MakeTriggerDec(Hlt2Dst2PiD02PiPiD0SB,					7,0x00000004,"Hlt2Dst2PiD02PiPiD0SBDecision");
MakeTriggerDec(Hlt2Dst2PiD02PiPiDMSB,					7,0x00000008,"Hlt2Dst2PiD02PiPiDMSBDecision");
MakeTriggerDec(Hlt2Dst2PiD02PiPiSignal,					7,0x00000010,"Hlt2Dst2PiD02PiPiSignalDecision");
MakeTriggerDec(Hlt2Dst2PiD02MuMu,					7,0x00000020,"Hlt2Dst2PiD02MuMuDecision");
MakeTriggerDec(Hlt2Dst2PiD02MuMuD0SB,					7,0x00000040,"Hlt2Dst2PiD02MuMuD0SBDecision");
MakeTriggerDec(Hlt2Dst2PiD02MuMuDMSB,					7,0x00000080,"Hlt2Dst2PiD02MuMuDMSBDecision");
MakeTriggerDec(Hlt2Dst2PiD02MuMuSignal,					7,0x00000100,"Hlt2Dst2PiD02MuMuSignalDecision");
MakeTriggerDec(Hlt2Dst2PiD02PiMu,					7,0x00000200,"Hlt2Dst2PiD02PiMuDecision");
MakeTriggerDec(Hlt2Dst2PiD02KMu,					7,0x00000400,"Hlt2Dst2PiD02KMuDecision");
MakeTriggerDec(Hlt2Dst2PiD02KPi,					7,0x00000800,"Hlt2Dst2PiD02KPiDecision");
MakeTriggerDec(Hlt2Dst2PiD02KPiD0SB,					7,0x00001000,"Hlt2Dst2PiD02KPiD0SBDecision");
MakeTriggerDec(Hlt2Dst2PiD02KPiDMSB,					7,0x00002000,"Hlt2Dst2PiD02KPiDMSBDecision");
MakeTriggerDec(Hlt2Dst2PiD02KPiSignal,					7,0x00004000,"Hlt2Dst2PiD02KPiSignalDecision");
MakeTriggerDec(Hlt2PassThrough,						7,0x00008000,"Hlt2PassThroughDecision");
MakeTriggerDec(Hlt2Transparent,						7,0x00010000,"Hlt2TransparentDecision");
MakeTriggerDec(Hlt2Forward,						7,0x00020000,"Hlt2ForwardDecision");
MakeTriggerDec(Hlt2DebugEvent,						7,0x00040000,"Hlt2DebugEventDecision");
MakeTriggerDec(Hlt2B2D2hhhBachelorWithKsSignal,				7,0x00080000,"Hlt2B2D2hhhBachelorWithKsSignalDecision");
MakeTriggerDec(Hlt2B2D2hhBachelorSignal,				7,0x00100000,"Hlt2B2D2hhBachelorSignalDecision");
MakeTriggerDec(Hlt2B2D2hhhBachelorWithKsBroadMW,			7,0x00200000,"Hlt2B2D2hhhBachelorWithKsBroadMWDecision");
MakeTriggerDec(Hlt2B2D2hhBachelorBroadMW,				7,0x00400000,"Hlt2B2D2hhBachelorBroadMWDecision");
MakeTriggerDec(Hlt2B2D2XRobust3BodyMonitor,				7,0x00800000,"Hlt2B2D2XRobust3BodyMonitorDecision");
MakeTriggerDec(Hlt2B2D2hhBachelorSignalMonitor,				7,0x01000000,"Hlt2B2D2hhBachelorSignalMonitorDecision");
MakeTriggerDec(Hlt2B2D2hhhBachelorWithKsBroadMWMonitor,			7,0x02000000,"Hlt2B2D2hhhBachelorWithKsBroadMWMonitorDecision");
MakeTriggerDec(Hlt2B2D2hhBachelorBroadMWMonitor,			7,0x04000000,"Hlt2B2D2hhBachelorBroadMWMonitorDecision");
MakeTriggerDec(Hlt2ExpressJPsi,						7,0x08000000,"Hlt2ExpressJPsiDecision");
MakeTriggerDec(Hlt2ExpressJPsiTagProbe,					7,0x10000000,"Hlt2ExpressJPsiTagProbeDecision");
MakeTriggerDec(Hlt2ExpressLambda,					7,0x20000000,"Hlt2ExpressLambdaDecision");
MakeTriggerDec(Hlt2ExpressKS,						7,0x40000000,"Hlt2ExpressKSDecision");
MakeTriggerDec(Hlt2ExpressDs2PhiPi,					7,0x80000000,"Hlt2ExpressDs2PhiPiDecision");

MakeTriggerDec(Hlt2ExpressBeamHalo,					8,0x00000001,"Hlt2ExpressBeamHaloDecision");
MakeTriggerDec(Hlt2ExpressDStar2D0Pi,					8,0x00000002,"Hlt2ExpressDStar2D0PiDecision");
MakeTriggerDec(Hlt2Bs2PhiGamma,						8,0x00000004,"Hlt2Bs2PhiGammaDecision");
MakeTriggerDec(Hlt2Bs2PhiGammaNoCutsK,					8,0x00000008,"Hlt2Bs2PhiGammaNoCutsKDecision");
MakeTriggerDec(Hlt2Bs2PhiGammaWideBMass,				8,0x00000010,"Hlt2Bs2PhiGammaWideBMassDecision");
MakeTriggerDec(Hlt2Bs2PhiGammaLooseDira,				8,0x00000020,"Hlt2Bs2PhiGammaLooseDiraDecision");
MakeTriggerDec(Hlt2Bd2KstGamma,						8,0x00000040,"Hlt2Bd2KstGammaDecision");
MakeTriggerDec(Hlt2Bd2KstGammaNoCutsKPi,				8,0x00000080,"Hlt2Bd2KstGammaNoCutsKPiDecision");
MakeTriggerDec(Hlt2Bd2KstGammaWideKMass,				8,0x00000100,"Hlt2Bd2KstGammaWideKMassDecision");
MakeTriggerDec(Hlt2Bd2KstGammaWideBMass,				8,0x00000200,"Hlt2Bd2KstGammaWideBMassDecision");
MakeTriggerDec(Hlt2Bd2KstGammaLooseDira,				8,0x00000400,"Hlt2Bd2KstGammaLooseDiraDecision");
MakeTriggerDec(Hlt2B2HH,						8,0x00000800,"Hlt2B2HHDecision");
MakeTriggerDec(Hlt2MuonFromHLT1,					8,0x00001000,"Hlt2MuonFromHLT1Decision");
MakeTriggerDec(Hlt2SingleMuon,						8,0x00002000,"Hlt2SingleMuonDecision");
MakeTriggerDec(Hlt2SingleHighPTMuon,					8,0x00004000,"Hlt2SingleHighPTMuonDecision");
MakeTriggerDec(Hlt2MuTrack,						8,0x00008000,"Hlt2MuTrackDecision");
MakeTriggerDec(Hlt2MuTrackNoIP,						8,0x00010000,"Hlt2MuTrackNoIPDecision");
MakeTriggerDec(Hlt2IncDiProton,						8,0x00020000,"Hlt2IncDiProtonDecision");
MakeTriggerDec(Hlt2IncDiProtonPresel,					8,0x00040000,"Hlt2IncDiProtonPreselDecision");
MakeTriggerDec(Hlt2IncDiProtonTrackFitted,		        	8,0x00080000,"Hlt2IncDiProtonTrackFittedDecision");
MakeTriggerDec(Hlt2TFBs2JpsieePhiUnbiasedPT,		       	       	8,0x00100000,"Hlt2TFBs2JpsieePhiUnbiasedPTDecision");
MakeTriggerDec(Hlt2TFBs2JpsieePhiSignal,       		     		8,0x00200000,"Hlt2TFBs2JpsieePhiSignalDecision");
MakeTriggerDec(Hlt2DisplVerticesSingle,		       			8,0x00400000,"Hlt2DisplVerticesSingleDecision");
MakeTriggerDec(Hlt2DisplVerticesDouble,		       			8,0x00800000,"Hlt2DisplVerticesDoubleDecision");
MakeTriggerDec(Hlt2ErrorEvent,						8,0x01000000,"Hlt2ErrorEventDecision");

*/

#endif // ZOOTRIGGERDECISIONS_H

// vim:sw=4:tw=78:ft=cpp
