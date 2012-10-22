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


MakeTriggerDec(Hlt1DiMuonHighMass                      ,1,0x00000001,"Hlt1DiMuonHighMassDecision");
MakeTriggerDec(Hlt1DiMuonLowMass                       ,1,0x00000002,"Hlt1DiMuonLowMassDecision");
MakeTriggerDec(Hlt1SingleMuonNoIP                      ,1,0x00000004,"Hlt1SingleMuonNoIPDecision");
MakeTriggerDec(Hlt1SingleMuonHighPT                    ,1,0x00000008,"Hlt1SingleMuonHighPTDecision");
MakeTriggerDec(Hlt1TrackAllL0                          ,1,0x00000010,"Hlt1TrackAllL0Decision");
MakeTriggerDec(Hlt1TrackMuon                           ,1,0x00000020,"Hlt1TrackMuonDecision");
MakeTriggerDec(Hlt1TrackPhoton                         ,1,0x00000040,"Hlt1TrackPhotonDecision");
MakeTriggerDec(Hlt1Lumi                                ,1,0x00000080,"Hlt1LumiDecision");
MakeTriggerDec(Hlt1LumiMidBeamCrossing                 ,1,0x00000100,"Hlt1LumiMidBeamCrossingDecision");
MakeTriggerDec(Hlt1MBNoBias                            ,1,0x00000200,"Hlt1MBNoBiasDecision");
MakeTriggerDec(Hlt1L0Any                               ,1,0x00000400,"Hlt1L0AnyDecision");
MakeTriggerDec(Hlt1L0AnyRateLimited                    ,1,0x00000800,"Hlt1L0AnyRateLimitedDecision");
MakeTriggerDec(Hlt1L0AnyNoSPD                          ,1,0x00001000,"Hlt1L0AnyNoSPDDecision");
MakeTriggerDec(Hlt1L0AnyNoSPDRateLimited               ,1,0x00002000,"Hlt1L0AnyNoSPDRateLimitedDecision");
MakeTriggerDec(Hlt1NoPVPassThrough                     ,1,0x00004000,"Hlt1NoPVPassThroughDecision");
MakeTriggerDec(Hlt1DiProtonLowMult                     ,1,0x00008000,"Hlt1DiProtonLowMultDecision");
MakeTriggerDec(Hlt1BeamGasNoBeamBeam1                  ,1,0x00010000,"Hlt1BeamGasNoBeamBeam1Decision");
MakeTriggerDec(Hlt1BeamGasNoBeamBeam2                  ,1,0x00020000,"Hlt1BeamGasNoBeamBeam2Decision");
MakeTriggerDec(Hlt1BeamGasBeam1                        ,1,0x00040000,"Hlt1BeamGasBeam1Decision");
MakeTriggerDec(Hlt1BeamGasBeam2                        ,1,0x00080000,"Hlt1BeamGasBeam2Decision");
MakeTriggerDec(Hlt1BeamGasCrossingEnhancedBeam1        ,1,0x00100000,"Hlt1BeamGasCrossingEnhancedBeam1Decision");
MakeTriggerDec(Hlt1BeamGasCrossingEnhancedBeam2        ,1,0x00200000,"Hlt1BeamGasCrossingEnhancedBeam2Decision");
MakeTriggerDec(Hlt1BeamGasCrossingForcedReco           ,1,0x00400000,"Hlt1BeamGasCrossingForcedRecoDecision");
MakeTriggerDec(Hlt1ODINTechnical                       ,1,0x00800000,"Hlt1ODINTechnicalDecision");
MakeTriggerDec(Hlt1Tell1Error                          ,1,0x01000000,"Hlt1Tell1ErrorDecision");
MakeTriggerDec(Hlt1BeamGasCrossingParasitic            ,1,0x02000000,"Hlt1BeamGasCrossingParasiticDecision");
MakeTriggerDec(Hlt1ErrorEvent                          ,1,0x04000000,"Hlt1ErrorEventDecision");
MakeTriggerDec(Hlt1Global                              ,1,0x08000000,"Hlt1Global");
MakeTriggerDec(Hlt1MBMicroBiasVelo                     ,1,0x10000000,"Hlt1MBMicroBiasVeloDecision");
MakeTriggerDec(Hlt1MBMicroBiasVeloRateLimited          ,1,0x20000000,"Hlt1MBMicroBiasVeloRateLimitedDecision");
MakeTriggerDec(Hlt1MBMicroBiasTStation                 ,1,0x40000000,"Hlt1MBMicroBiasTStationDecision");
MakeTriggerDec(Hlt1MBMicroBiasTStationRateLimited      ,1,0x80000000,"Hlt1MBMicroBiasTStationRateLimitedDecision");


MakeTriggerDec(Hlt1DiProton                            ,2,0x00000001,"Hlt1DiProtonDecision");
MakeTriggerDec(Hlt1VeloClosingMicroBias                ,2,0x00000002,"Hlt1VeloClosingMicroBiasDecision");
MakeTriggerDec(Hlt1SingleElectronNoIP                  ,2,0x00000004,"Hlt1SingleElectronNoIPDecision");
MakeTriggerDec(Hlt1TrackForwardPassThrough             ,2,0x00000008,"Hlt1TrackForwardPassThroughDecision");
MakeTriggerDec(Hlt1TrackForwardPassThroughLoose        ,2,0x00000010,"Hlt1TrackForwardPassThroughLooseDecision");
MakeTriggerDec(Hlt1CharmCalibrationNoBias              ,2,0x00000020,"Hlt1CharmCalibrationNoBiasDecision");
MakeTriggerDec(Hlt1L0HighSumETJet                      ,2,0x00000040,"Hlt1L0HighSumETJetDecision");
MakeTriggerDec(Hlt1BeamGasCrossingForcedRecoFullZ      ,2,0x00000080,"Hlt1BeamGasCrossingForcedRecoFullZDecision");
MakeTriggerDec(Hlt1BeamGasHighRhoVertices              ,2,0x00000100,"Hlt1BeamGasHighRhoVerticesDecision");
MakeTriggerDec(Hlt1VertexDisplVertex                   ,2,0x00000200,"Hlt1VertexDisplVertexDecision");
MakeTriggerDec(Hlt1TrackAllL0Tight                     ,2,0x00000400,"Hlt1TrackAllL0TightDecision");

//HLT2 Lines


MakeTriggerDec(Hlt2SingleElectronTFLowPt               ,3,0x00000001,"Hlt2SingleElectronTFLowPtDecision");
MakeTriggerDec(Hlt2SingleElectronTFHighPt              ,3,0x00000002,"Hlt2SingleElectronTFHighPtDecision");
MakeTriggerDec(Hlt2DiElectronHighMass                  ,3,0x00000004,"Hlt2DiElectronHighMassDecision");
MakeTriggerDec(Hlt2DiElectronB                         ,3,0x00000008,"Hlt2DiElectronBDecision");
MakeTriggerDec(Hlt2Topo2BodySimple                     ,3,0x00000010,"Hlt2Topo2BodySimpleDecision");
MakeTriggerDec(Hlt2Topo3BodySimple                     ,3,0x00000020,"Hlt2Topo3BodySimpleDecision");
MakeTriggerDec(Hlt2Topo4BodySimple                     ,3,0x00000040,"Hlt2Topo4BodySimpleDecision");
MakeTriggerDec(Hlt2Topo2BodyBBDT                       ,3,0x00000080,"Hlt2Topo2BodyBBDTDecision");
MakeTriggerDec(Hlt2Topo3BodyBBDT                       ,3,0x00000100,"Hlt2Topo3BodyBBDTDecision");
MakeTriggerDec(Hlt2Topo4BodyBBDT                       ,3,0x00000200,"Hlt2Topo4BodyBBDTDecision");
MakeTriggerDec(Hlt2TopoMu2BodyBBDT                     ,3,0x00000400,"Hlt2TopoMu2BodyBBDTDecision");
MakeTriggerDec(Hlt2TopoMu3BodyBBDT                     ,3,0x00000800,"Hlt2TopoMu3BodyBBDTDecision");
MakeTriggerDec(Hlt2TopoMu4BodyBBDT                     ,3,0x00001000,"Hlt2TopoMu4BodyBBDTDecision");
MakeTriggerDec(Hlt2TopoE2BodyBBDT                      ,3,0x00002000,"Hlt2TopoE2BodyBBDTDecision");
MakeTriggerDec(Hlt2TopoE3BodyBBDT                      ,3,0x00004000,"Hlt2TopoE3BodyBBDTDecision");
MakeTriggerDec(Hlt2TopoE4BodyBBDT                      ,3,0x00008000,"Hlt2TopoE4BodyBBDTDecision");
MakeTriggerDec(Hlt2IncPhi                              ,3,0x00010000,"Hlt2IncPhiDecision");
MakeTriggerDec(Hlt2IncPhiSidebands                     ,3,0x00020000,"Hlt2IncPhiSidebandsDecision");
MakeTriggerDec(Hlt2CharmHadD02HHKsLL                   ,3,0x00040000,"Hlt2CharmHadD02HHKsLLDecision");
MakeTriggerDec(Hlt2Dst2PiD02PiPi                       ,3,0x00080000,"Hlt2Dst2PiD02PiPiDecision");
MakeTriggerDec(Hlt2Dst2PiD02MuMu                       ,3,0x00100000,"Hlt2Dst2PiD02MuMuDecision");
MakeTriggerDec(Hlt2Dst2PiD02KMu                        ,3,0x00200000,"Hlt2Dst2PiD02KMuDecision");
MakeTriggerDec(Hlt2Dst2PiD02KPi                        ,3,0x00400000,"Hlt2Dst2PiD02KPiDecision");
MakeTriggerDec(Hlt2PassThrough                         ,3,0x00800000,"Hlt2PassThroughDecision");
MakeTriggerDec(Hlt2Transparent                         ,3,0x01000000,"Hlt2TransparentDecision");
MakeTriggerDec(Hlt2Forward                             ,3,0x02000000,"Hlt2ForwardDecision");
MakeTriggerDec(Hlt2DebugEvent                          ,3,0x04000000,"Hlt2DebugEventDecision");
MakeTriggerDec(Hlt2CharmHadD02KK                       ,3,0x08000000,"Hlt2CharmHadD02KKDecision");
MakeTriggerDec(Hlt2CharmHadD02KKWideMass               ,3,0x10000000,"Hlt2CharmHadD02KKWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02KPi                      ,3,0x20000000,"Hlt2CharmHadD02KPiDecision");
MakeTriggerDec(Hlt2CharmHadD02KPiWideMass              ,3,0x40000000,"Hlt2CharmHadD02KPiWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02PiPi                     ,3,0x80000000,"Hlt2CharmHadD02PiPiDecision");


MakeTriggerDec(Hlt2CharmHadD02PiPiWideMass             ,4,0x00000001,"Hlt2CharmHadD02PiPiWideMassDecision");
MakeTriggerDec(Hlt2ExpressJPsi                         ,4,0x00000002,"Hlt2ExpressJPsiDecision");
MakeTriggerDec(Hlt2ExpressJPsiTagProbe                 ,4,0x00000004,"Hlt2ExpressJPsiTagProbeDecision");
MakeTriggerDec(Hlt2ExpressLambda                       ,4,0x00000008,"Hlt2ExpressLambdaDecision");
MakeTriggerDec(Hlt2ExpressKS                           ,4,0x00000010,"Hlt2ExpressKSDecision");
MakeTriggerDec(Hlt2ExpressDs2PhiPi                     ,4,0x00000020,"Hlt2ExpressDs2PhiPiDecision");
MakeTriggerDec(Hlt2ExpressBeamHalo                     ,4,0x00000040,"Hlt2ExpressBeamHaloDecision");
MakeTriggerDec(Hlt2ExpressDStar2D0Pi                   ,4,0x00000080,"Hlt2ExpressDStar2D0PiDecision");
MakeTriggerDec(Hlt2ExpressHLT1Physics                  ,4,0x00000100,"Hlt2ExpressHLT1PhysicsDecision");
MakeTriggerDec(Hlt2Bs2PhiGamma                         ,4,0x00000200,"Hlt2Bs2PhiGammaDecision");
MakeTriggerDec(Hlt2Bs2PhiGammaWideBMass                ,4,0x00000400,"Hlt2Bs2PhiGammaWideBMassDecision");
MakeTriggerDec(Hlt2Bd2KstGamma                         ,4,0x00000800,"Hlt2Bd2KstGammaDecision");
MakeTriggerDec(Hlt2Bd2KstGammaWideKMass                ,4,0x00001000,"Hlt2Bd2KstGammaWideKMassDecision");
MakeTriggerDec(Hlt2Bd2KstGammaWideBMass                ,4,0x00002000,"Hlt2Bd2KstGammaWideBMassDecision");
MakeTriggerDec(Hlt2CharmHadD2KS0Pi                     ,4,0x00004000,"Hlt2CharmHadD2KS0PiDecision");
MakeTriggerDec(Hlt2CharmHadD2KS0K                      ,4,0x00008000,"Hlt2CharmHadD2KS0KDecision");
MakeTriggerDec(Hlt2CharmRareDecayD02MuMu               ,4,0x00010000,"Hlt2CharmRareDecayD02MuMuDecision");
MakeTriggerDec(Hlt2B2HH                                ,4,0x00020000,"Hlt2B2HHDecision");
MakeTriggerDec(Hlt2MuonFromHLT1                        ,4,0x00040000,"Hlt2MuonFromHLT1Decision");
MakeTriggerDec(Hlt2SingleMuon                          ,4,0x00080000,"Hlt2SingleMuonDecision");
MakeTriggerDec(Hlt2SingleMuonHighPT                    ,4,0x00100000,"Hlt2SingleMuonHighPTDecision");
MakeTriggerDec(Hlt2SingleMuonLowPT                     ,4,0x00200000,"Hlt2SingleMuonLowPTDecision");
MakeTriggerDec(Hlt2TFBc2JpsiMuX                        ,4,0x00400000,"Hlt2TFBc2JpsiMuXDecision");
MakeTriggerDec(Hlt2TFBc2JpsiMuXSignal                  ,4,0x00800000,"Hlt2TFBc2JpsiMuXSignalDecision");
MakeTriggerDec(Hlt2DisplVerticesLowMassSingle          ,4,0x01000000,"Hlt2DisplVerticesLowMassSingleDecision");
MakeTriggerDec(Hlt2DisplVerticesHighMassSingle         ,4,0x02000000,"Hlt2DisplVerticesHighMassSingleDecision");
MakeTriggerDec(Hlt2DisplVerticesDouble                 ,4,0x04000000,"Hlt2DisplVerticesDoubleDecision");
MakeTriggerDec(Hlt2DisplVerticesSinglePostScaled       ,4,0x08000000,"Hlt2DisplVerticesSinglePostScaledDecision");
MakeTriggerDec(Hlt2DisplVerticesHighFDSingle           ,4,0x10000000,"Hlt2DisplVerticesHighFDSingleDecision");
MakeTriggerDec(Hlt2DisplVerticesSingleDown             ,4,0x20000000,"Hlt2DisplVerticesSingleDownDecision");
MakeTriggerDec(Hlt2B2HHPi0_Merged                      ,4,0x40000000,"Hlt2B2HHPi0_MergedDecision");
MakeTriggerDec(Hlt2CharmHadD2HHH                       ,4,0x80000000,"Hlt2CharmHadD2HHHDecision");


MakeTriggerDec(Hlt2CharmHadD2HHHWideMass               ,5,0x00000001,"Hlt2CharmHadD2HHHWideMassDecision");
MakeTriggerDec(Hlt2DiMuon                              ,5,0x00000002,"Hlt2DiMuonDecision");
MakeTriggerDec(Hlt2DiMuonLowMass                       ,5,0x00000004,"Hlt2DiMuonLowMassDecision");
MakeTriggerDec(Hlt2DiMuonJPsi                          ,5,0x00000008,"Hlt2DiMuonJPsiDecision");
MakeTriggerDec(Hlt2DiMuonJPsiHighPT                    ,5,0x00000010,"Hlt2DiMuonJPsiHighPTDecision");
MakeTriggerDec(Hlt2DiMuonPsi2S                         ,5,0x00000020,"Hlt2DiMuonPsi2SDecision");
MakeTriggerDec(Hlt2DiMuonB                             ,5,0x00000040,"Hlt2DiMuonBDecision");
MakeTriggerDec(Hlt2DiMuonZ                             ,5,0x00000080,"Hlt2DiMuonZDecision");
MakeTriggerDec(Hlt2DiMuonDY1                           ,5,0x00000100,"Hlt2DiMuonDY1Decision");
MakeTriggerDec(Hlt2DiMuonDY2                           ,5,0x00000200,"Hlt2DiMuonDY2Decision");
MakeTriggerDec(Hlt2DiMuonDY3                           ,5,0x00000400,"Hlt2DiMuonDY3Decision");
MakeTriggerDec(Hlt2DiMuonDY4                           ,5,0x00000800,"Hlt2DiMuonDY4Decision");
MakeTriggerDec(Hlt2DiMuonDetached                      ,5,0x00001000,"Hlt2DiMuonDetachedDecision");
MakeTriggerDec(Hlt2DiMuonDetachedHeavy                 ,5,0x00002000,"Hlt2DiMuonDetachedHeavyDecision");
MakeTriggerDec(Hlt2DiMuonDetachedJPsi                  ,5,0x00004000,"Hlt2DiMuonDetachedJPsiDecision");
MakeTriggerDec(Hlt2DiMuonNoPV                          ,5,0x00008000,"Hlt2DiMuonNoPVDecision");
MakeTriggerDec(Hlt2TriMuonDetached                     ,5,0x00010000,"Hlt2TriMuonDetachedDecision");
MakeTriggerDec(Hlt2TriMuonTau                          ,5,0x00020000,"Hlt2TriMuonTauDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHH                     ,5,0x00040000,"Hlt2CharmHadD02HHHHDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHWideMass             ,5,0x00080000,"Hlt2CharmHadD02HHHHWideMassDecision");
MakeTriggerDec(Hlt2ErrorEvent                          ,5,0x00100000,"Hlt2ErrorEventDecision");
MakeTriggerDec(Hlt2Global                              ,5,0x00200000,"Hlt2Global");
MakeTriggerDec(Hlt2B2HHLTUnbiased                      ,5,0x00400000,"Hlt2B2HHLTUnbiasedDecision");
MakeTriggerDec(Hlt2CharmHadD02HH_D02PiPi               ,5,0x00800000,"Hlt2CharmHadD02HH_D02PiPiDecision");
MakeTriggerDec(Hlt2CharmHadD02HH_D02PiPiWideMass       ,5,0x01000000,"Hlt2CharmHadD02HH_D02PiPiWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HH_D02KK                 ,5,0x02000000,"Hlt2CharmHadD02HH_D02KKDecision");
MakeTriggerDec(Hlt2CharmHadD02HH_D02KKWideMass         ,5,0x04000000,"Hlt2CharmHadD02HH_D02KKWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HH_D02KPi                ,5,0x08000000,"Hlt2CharmHadD02HH_D02KPiDecision");
MakeTriggerDec(Hlt2CharmHadD02HH_D02KPiWideMass        ,5,0x10000000,"Hlt2CharmHadD02HH_D02KPiWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD2KS0H_D2KS0Pi              ,5,0x20000000,"Hlt2CharmHadD2KS0H_D2KS0PiDecision");
MakeTriggerDec(Hlt2CharmHadD2KS0H_D2KS0K               ,5,0x40000000,"Hlt2CharmHadD2KS0H_D2KS0KDecision");
MakeTriggerDec(Hlt2DiProton                            ,5,0x80000000,"Hlt2DiProtonDecision");


MakeTriggerDec(Hlt2DiProtonTF                          ,6,0x00000001,"Hlt2DiProtonTFDecision");
MakeTriggerDec(Hlt2DiProtonLowMult                     ,6,0x00000002,"Hlt2DiProtonLowMultDecision");
MakeTriggerDec(Hlt2DiProtonLowMultTF                   ,6,0x00000004,"Hlt2DiProtonLowMultTFDecision");
MakeTriggerDec(Hlt2CharmSemilepD02HMuNu_D02KMuNuWS     ,6,0x00000008,"Hlt2CharmSemilepD02HMuNu_D02KMuNuWSDecision");
MakeTriggerDec(Hlt2CharmSemilepD02HMuNu_D02PiMuNuWS    ,6,0x00000010,"Hlt2CharmSemilepD02HMuNu_D02PiMuNuWSDecision");
MakeTriggerDec(Hlt2CharmSemilepD02HMuNu_D02KMuNu       ,6,0x00000020,"Hlt2CharmSemilepD02HMuNu_D02KMuNuDecision");
MakeTriggerDec(Hlt2CharmSemilepD02HMuNu_D02PiMuNu      ,6,0x00000040,"Hlt2CharmSemilepD02HMuNu_D02PiMuNuDecision");
MakeTriggerDec(Hlt2CharmSemilepD2HMuMu                 ,6,0x00000080,"Hlt2CharmSemilepD2HMuMuDecision");
MakeTriggerDec(Hlt2CharmSemilepD2HMuMuWideMass         ,6,0x00000100,"Hlt2CharmSemilepD2HMuMuWideMassDecision");
MakeTriggerDec(Hlt2CharmSemilepD02HHMuMu               ,6,0x00000200,"Hlt2CharmSemilepD02HHMuMuDecision");
MakeTriggerDec(Hlt2CharmSemilepD02HHMuMuWideMass       ,6,0x00000400,"Hlt2CharmSemilepD02HHMuMuWideMassDecision");
MakeTriggerDec(Hlt2diPhotonDiMuon                      ,6,0x00000800,"Hlt2diPhotonDiMuonDecision");
MakeTriggerDec(Hlt2LowMultMuon                         ,6,0x00001000,"Hlt2LowMultMuonDecision");
MakeTriggerDec(Hlt2LowMultHadron                       ,6,0x00002000,"Hlt2LowMultHadronDecision");
MakeTriggerDec(Hlt2LowMultPhoton                       ,6,0x00004000,"Hlt2LowMultPhotonDecision");
MakeTriggerDec(Hlt2LowMultElectron                     ,6,0x00008000,"Hlt2LowMultElectronDecision");
MakeTriggerDec(Hlt2SingleTFElectron                    ,6,0x00010000,"Hlt2SingleTFElectronDecision");
MakeTriggerDec(Hlt2SingleTFVHighPtElectron             ,6,0x00020000,"Hlt2SingleTFVHighPtElectronDecision");
MakeTriggerDec(Hlt2B2HHLTUnbiasedDetached              ,6,0x00040000,"Hlt2B2HHLTUnbiasedDetachedDecision");
MakeTriggerDec(Hlt2CharmHadLambdaC2KPPi                ,6,0x00080000,"Hlt2CharmHadLambdaC2KPPiDecision");
MakeTriggerDec(Hlt2SingleMuonVHighPT                   ,6,0x00100000,"Hlt2SingleMuonVHighPTDecision");
MakeTriggerDec(Hlt2CharmSemilepD02HMuNu_D02KMuNuTight  ,6,0x00200000,"Hlt2CharmSemilepD02HMuNu_D02KMuNuTightDecision");
MakeTriggerDec(Hlt2CharmHadMinBiasLambdaC2KPPi         ,6,0x00400000,"Hlt2CharmHadMinBiasLambdaC2KPPiDecision");
MakeTriggerDec(Hlt2CharmHadMinBiasD02KPi               ,6,0x00800000,"Hlt2CharmHadMinBiasD02KPiDecision");
MakeTriggerDec(Hlt2CharmHadMinBiasD02KK                ,6,0x01000000,"Hlt2CharmHadMinBiasD02KKDecision");
MakeTriggerDec(Hlt2CharmHadMinBiasDplus2hhh            ,6,0x02000000,"Hlt2CharmHadMinBiasDplus2hhhDecision");
MakeTriggerDec(Hlt2CharmHadMinBiasLambdaC2LambdaPi     ,6,0x04000000,"Hlt2CharmHadMinBiasLambdaC2LambdaPiDecision");
MakeTriggerDec(Hlt2DisplVerticesSingle                 ,6,0x08000000,"Hlt2DisplVerticesSingleDecision");
MakeTriggerDec(Hlt2DisplVerticesDoublePostScaled       ,6,0x10000000,"Hlt2DisplVerticesDoublePostScaledDecision");
MakeTriggerDec(Hlt2DisplVerticesSingleHighMassPostScaled,6,0x20000000,"Hlt2DisplVerticesSingleHighMassPostScaledDecision");
MakeTriggerDec(Hlt2DisplVerticesSingleHighFDPostScaled ,6,0x40000000,"Hlt2DisplVerticesSingleHighFDPostScaledDecision");
MakeTriggerDec(Hlt2DisplVerticesSingleMVPostScaled     ,6,0x80000000,"Hlt2DisplVerticesSingleMVPostScaledDecision");


MakeTriggerDec(Hlt2RadiativeTopoTrackTOS               ,7,0x00000001,"Hlt2RadiativeTopoTrackTOSDecision");
MakeTriggerDec(Hlt2RadiativeTopoPhotonL0               ,7,0x00000002,"Hlt2RadiativeTopoPhotonL0Decision");
MakeTriggerDec(Hlt2DiMuonPsi2SHighPT                   ,7,0x00000004,"Hlt2DiMuonPsi2SHighPTDecision");
MakeTriggerDec(Hlt2DoubleDiMuon                        ,7,0x00000008,"Hlt2DoubleDiMuonDecision");
MakeTriggerDec(Hlt2DiMuonAndMuon                       ,7,0x00000010,"Hlt2DiMuonAndMuonDecision");
MakeTriggerDec(Hlt2DiMuonAndGamma                      ,7,0x00000020,"Hlt2DiMuonAndGammaDecision");
MakeTriggerDec(Hlt2DiMuonAndD0                         ,7,0x00000040,"Hlt2DiMuonAndD0Decision");
MakeTriggerDec(Hlt2DiMuonAndDp                         ,7,0x00000080,"Hlt2DiMuonAndDpDecision");
MakeTriggerDec(Hlt2DiMuonAndDs                         ,7,0x00000100,"Hlt2DiMuonAndDsDecision");
MakeTriggerDec(Hlt2DiMuonAndLc                         ,7,0x00000200,"Hlt2DiMuonAndLcDecision");
MakeTriggerDec(Hlt2CharmSemilepD02HHMuMuHardHadronsSoftMuons,7,0x00000400,"Hlt2CharmSemilepD02HHMuMuHardHadronsSoftMuonsDecision");
MakeTriggerDec(Hlt2CharmSemilepD02HHMuMuHardHadronsSoftMuonsWideMass,7,0x00000800,"Hlt2CharmSemilepD02HHMuMuHardHadronsSoftMuonsWideMassDecision");
MakeTriggerDec(Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuons,7,0x00001000,"Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuonsDecision");
MakeTriggerDec(Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuonsWideMass,7,0x00002000,"Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuonsWideMassDecision");
MakeTriggerDec(Hlt2TopoRad2BodyBBDT                    ,7,0x00004000,"Hlt2TopoRad2BodyBBDTDecision");
MakeTriggerDec(Hlt2TopoRad2plus1BodyBBDT               ,7,0x00008000,"Hlt2TopoRad2plus1BodyBBDTDecision");
MakeTriggerDec(Hlt2Lumi                                ,7,0x00010000,"Hlt2LumiDecision");
MakeTriggerDec(Hlt2LowMultHadron_nofilter              ,7,0x00020000,"Hlt2LowMultHadron_nofilterDecision");
MakeTriggerDec(Hlt2LowMultElectron_nofilter            ,7,0x00040000,"Hlt2LowMultElectron_nofilterDecision");
MakeTriggerDec(Hlt2CharmHadD02HHKsDD                   ,7,0x00080000,"Hlt2CharmHadD02HHKsDDDecision");
MakeTriggerDec(Hlt2CharmHadD2KS0KS0                    ,7,0x00100000,"Hlt2CharmHadD2KS0KS0Decision");
MakeTriggerDec(Hlt2CharmHadD2KS0KS0WideMass            ,7,0x00200000,"Hlt2CharmHadD2KS0KS0WideMassDecision");
MakeTriggerDec(Hlt2ExpressD02KPi                       ,7,0x00400000,"Hlt2ExpressD02KPiDecision");
MakeTriggerDec(Hlt2CharmHadLambdaC2KPPiWideMass        ,7,0x00800000,"Hlt2CharmHadLambdaC2KPPiWideMassDecision");
MakeTriggerDec(Hlt2CharmHadLambdaC2KPK                 ,7,0x01000000,"Hlt2CharmHadLambdaC2KPKDecision");
MakeTriggerDec(Hlt2CharmHadLambdaC2KPKWideMass         ,7,0x02000000,"Hlt2CharmHadLambdaC2KPKWideMassDecision");
MakeTriggerDec(Hlt2CharmHadLambdaC2PiPPi               ,7,0x04000000,"Hlt2CharmHadLambdaC2PiPPiDecision");
MakeTriggerDec(Hlt2CharmHadLambdaC2PiPPiWideMass       ,7,0x08000000,"Hlt2CharmHadLambdaC2PiPPiWideMassDecision");
MakeTriggerDec(Hlt2CharmHadLambdaC2PiPK                ,7,0x10000000,"Hlt2CharmHadLambdaC2PiPKDecision");
MakeTriggerDec(Hlt2CharmHadLambdaC2PiPKWideMass        ,7,0x20000000,"Hlt2CharmHadLambdaC2PiPKWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD2KS0H_D2KS0DDPi            ,7,0x40000000,"Hlt2CharmHadD2KS0H_D2KS0DDPiDecision");
MakeTriggerDec(Hlt2CharmHadD2KS0H_D2KS0DDK             ,7,0x80000000,"Hlt2CharmHadD2KS0H_D2KS0DDKDecision");


MakeTriggerDec(Hlt2DiPhi                               ,8,0x00000001,"Hlt2DiPhiDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDstNoHltOne_4pi      ,8,0x00000002,"Hlt2CharmHadD02HHHHDstNoHltOne_4piDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDstNoHltOne_4piWideMass,8,0x00000004,"Hlt2CharmHadD02HHHHDstNoHltOne_4piWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDstNoHltOne_K3pi     ,8,0x00000008,"Hlt2CharmHadD02HHHHDstNoHltOne_K3piDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDstNoHltOne_K3piWideMass,8,0x00000010,"Hlt2CharmHadD02HHHHDstNoHltOne_K3piWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDstNoHltOne_KKpipi   ,8,0x00000020,"Hlt2CharmHadD02HHHHDstNoHltOne_KKpipiDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDstNoHltOne_KKpipiWideMass,8,0x00000040,"Hlt2CharmHadD02HHHHDstNoHltOne_KKpipiWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDstNoHltOne_2K2pi    ,8,0x00000080,"Hlt2CharmHadD02HHHHDstNoHltOne_2K2piDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDstNoHltOne_2K2piWideMass,8,0x00000100,"Hlt2CharmHadD02HHHHDstNoHltOne_2K2piWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDstNoHltOne_3Kpi     ,8,0x00000200,"Hlt2CharmHadD02HHHHDstNoHltOne_3KpiDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDstNoHltOne_3KpiWideMass,8,0x00000400,"Hlt2CharmHadD02HHHHDstNoHltOne_3KpiWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDstNoHltOne_Ch2      ,8,0x00000800,"Hlt2CharmHadD02HHHHDstNoHltOne_Ch2Decision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDstNoHltOne_Ch2WideMass,8,0x00001000,"Hlt2CharmHadD02HHHHDstNoHltOne_Ch2WideMassDecision");
MakeTriggerDec(Hlt2CharmSemilep3bodyD2PiMuMu           ,8,0x00002000,"Hlt2CharmSemilep3bodyD2PiMuMuDecision");
MakeTriggerDec(Hlt2CharmSemilep3bodyD2PiMuMuSS         ,8,0x00004000,"Hlt2CharmSemilep3bodyD2PiMuMuSSDecision");
MakeTriggerDec(Hlt2CharmSemilep3bodyD2KMuMu            ,8,0x00008000,"Hlt2CharmSemilep3bodyD2KMuMuDecision");
MakeTriggerDec(Hlt2CharmSemilep3bodyD2KMuMuSS          ,8,0x00010000,"Hlt2CharmSemilep3bodyD2KMuMuSSDecision");
MakeTriggerDec(Hlt2CharmSemilep3bodyLambdac2PMuMu      ,8,0x00020000,"Hlt2CharmSemilep3bodyLambdac2PMuMuDecision");
MakeTriggerDec(Hlt2CharmSemilep3bodyLambdac2PMuMuSS    ,8,0x00040000,"Hlt2CharmSemilep3bodyLambdac2PMuMuSSDecision");
MakeTriggerDec(Hlt2LambdaC_LambdaC2Lambda0LLPi         ,8,0x00080000,"Hlt2LambdaC_LambdaC2Lambda0LLPiDecision");
MakeTriggerDec(Hlt2LambdaC_LambdaC2Lambda0LLK          ,8,0x00100000,"Hlt2LambdaC_LambdaC2Lambda0LLKDecision");
MakeTriggerDec(Hlt2LambdaC_LambdaC2Lambda0DDPi         ,8,0x00200000,"Hlt2LambdaC_LambdaC2Lambda0DDPiDecision");
MakeTriggerDec(Hlt2LambdaC_LambdaC2Lambda0DDK          ,8,0x00400000,"Hlt2LambdaC_LambdaC2Lambda0DDKDecision");
MakeTriggerDec(Hlt2RadiativeTopoTrack                  ,8,0x00800000,"Hlt2RadiativeTopoTrackDecision");
MakeTriggerDec(Hlt2RadiativeTopoPhoton                 ,8,0x01000000,"Hlt2RadiativeTopoPhotonDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDst_4pi              ,8,0x02000000,"Hlt2CharmHadD02HHHHDst_4piDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDst_4piWideMass      ,8,0x04000000,"Hlt2CharmHadD02HHHHDst_4piWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDst_K3pi             ,8,0x08000000,"Hlt2CharmHadD02HHHHDst_K3piDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDst_K3piWideMass     ,8,0x10000000,"Hlt2CharmHadD02HHHHDst_K3piWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDst_KKpipi           ,8,0x20000000,"Hlt2CharmHadD02HHHHDst_KKpipiDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDst_KKpipiWideMass   ,8,0x40000000,"Hlt2CharmHadD02HHHHDst_KKpipiWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDst_2K2pi            ,8,0x80000000,"Hlt2CharmHadD02HHHHDst_2K2piDecision");


MakeTriggerDec(Hlt2CharmHadD02HHHHDst_2K2piWideMass    ,9,0x00000001,"Hlt2CharmHadD02HHHHDst_2K2piWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDst_3Kpi             ,9,0x00000002,"Hlt2CharmHadD02HHHHDst_3KpiDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDst_3KpiWideMass     ,9,0x00000004,"Hlt2CharmHadD02HHHHDst_3KpiWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDst_Ch2              ,9,0x00000008,"Hlt2CharmHadD02HHHHDst_Ch2Decision");
MakeTriggerDec(Hlt2CharmHadD02HHHHDst_Ch2WideMass      ,9,0x00000010,"Hlt2CharmHadD02HHHHDst_Ch2WideMassDecision");
MakeTriggerDec(Hlt2CharmSemilepD02PiPiMuMu             ,9,0x00000020,"Hlt2CharmSemilepD02PiPiMuMuDecision");
MakeTriggerDec(Hlt2CharmSemilepD02KKMuMu               ,9,0x00000040,"Hlt2CharmSemilepD02KKMuMuDecision");
MakeTriggerDec(Hlt2CharmSemilepD02KPiMuMu              ,9,0x00000080,"Hlt2CharmSemilepD02KPiMuMuDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHH_4pi                 ,9,0x00000100,"Hlt2CharmHadD02HHHH_4piDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHH_4piWideMass         ,9,0x00000200,"Hlt2CharmHadD02HHHH_4piWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHH_K3pi                ,9,0x00000400,"Hlt2CharmHadD02HHHH_K3piDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHH_K3piWideMass        ,9,0x00000800,"Hlt2CharmHadD02HHHH_K3piWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHH_KKpipi              ,9,0x00001000,"Hlt2CharmHadD02HHHH_KKpipiDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHH_KKpipiWideMass      ,9,0x00002000,"Hlt2CharmHadD02HHHH_KKpipiWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHH_2K2pi               ,9,0x00004000,"Hlt2CharmHadD02HHHH_2K2piDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHH_2K2piWideMass       ,9,0x00008000,"Hlt2CharmHadD02HHHH_2K2piWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHH_3Kpi                ,9,0x00010000,"Hlt2CharmHadD02HHHH_3KpiDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHH_3KpiWideMass        ,9,0x00020000,"Hlt2CharmHadD02HHHH_3KpiWideMassDecision");
MakeTriggerDec(Hlt2CharmHadD02HHHH_Ch2                 ,9,0x00040000,"Hlt2CharmHadD02HHHH_Ch2Decision");
MakeTriggerDec(Hlt2CharmHadD02HHHH_Ch2WideMass         ,9,0x00080000,"Hlt2CharmHadD02HHHH_Ch2WideMassDecision");
MakeTriggerDec(Hlt2DiMuonDetachedPsi2S                 ,9,0x00100000,"Hlt2DiMuonDetachedPsi2SDecision");
MakeTriggerDec(Hlt2CharmHadD02HHXDst_hhX               ,9,0x00200000,"Hlt2CharmHadD02HHXDst_hhXDecision");
MakeTriggerDec(Hlt2CharmHadD02HHXDst_hhXWideMass       ,9,0x00400000,"Hlt2CharmHadD02HHXDst_hhXWideMassDecision");
MakeTriggerDec(Hlt2LowMultD2KPi                        ,9,0x00800000,"Hlt2LowMultD2KPiDecision");
MakeTriggerDec(Hlt2LowMultD2KPiPi                      ,9,0x01000000,"Hlt2LowMultD2KPiPiDecision");
MakeTriggerDec(Hlt2LowMultD2K3Pi                       ,9,0x02000000,"Hlt2LowMultD2K3PiDecision");
MakeTriggerDec(Hlt2LowMultChiC2HH                      ,9,0x04000000,"Hlt2LowMultChiC2HHDecision");
MakeTriggerDec(Hlt2LowMultChiC2HHHH                    ,9,0x08000000,"Hlt2LowMultChiC2HHHHDecision");
MakeTriggerDec(Hlt2LowMultD2KPiWS                      ,9,0x10000000,"Hlt2LowMultD2KPiWSDecision");
MakeTriggerDec(Hlt2LowMultD2KPiPiWS                    ,9,0x20000000,"Hlt2LowMultD2KPiPiWSDecision");
MakeTriggerDec(Hlt2LowMultD2K3PiWS                     ,9,0x40000000,"Hlt2LowMultD2K3PiWSDecision");
MakeTriggerDec(Hlt2LowMultChiC2HHWS                    ,9,0x80000000,"Hlt2LowMultChiC2HHWSDecision");


MakeTriggerDec(Hlt2LowMultChiC2HHHHWS                  ,10,0x00000001,"Hlt2LowMultChiC2HHHHWSDecision");
MakeTriggerDec(Hlt2LowMultDDInc                        ,10,0x00000002,"Hlt2LowMultDDIncDecision");
MakeTriggerDec(Hlt2DisplVerticesSingleLoosePS          ,10,0x00000004,"Hlt2DisplVerticesSingleLoosePSDecision");
MakeTriggerDec(Hlt2DisplVerticesSingleHighFD           ,10,0x00000008,"Hlt2DisplVerticesSingleHighFDDecision");
MakeTriggerDec(Hlt2DisplVerticesSingleVeryHighFD       ,10,0x00000010,"Hlt2DisplVerticesSingleVeryHighFDDecision");
MakeTriggerDec(Hlt2DisplVerticesSingleHighMass         ,10,0x00000020,"Hlt2DisplVerticesSingleHighMassDecision");
MakeTriggerDec(Hlt2DisplVerticesSinglePS               ,10,0x00000040,"Hlt2DisplVerticesSinglePSDecision");
MakeTriggerDec(Hlt2DisplVerticesDoublePS               ,10,0x00000080,"Hlt2DisplVerticesDoublePSDecision");

#endif // ZOOTRIGGERDECISIONS_H
