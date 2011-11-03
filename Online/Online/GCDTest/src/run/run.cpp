#include "Framework/DataflowMgr.h"
#include "Test/Algorithm.h"
#include "Test/Bits.h"
#include "RTL/rtl.h"

using namespace std;
using namespace Framework;
using namespace FrameworkTest;

struct input {};
struct output {};

int main(int, char**) {
#define AEF AlgorithmExecutorFactory

  DataflowMgr mgr(0,0);
  const char* raw_mask[]   = {
    "MCParticle",
    "TPCLowPtCollection",
    "TPCCollection",
    "TPCSpacePointCollection",
    "ETDCollection",
    "SETCollection",
    "SITCollection",
    "VXDCollection",
    "FTDCollection",
    "EcalBarrelCollection",
    "HcalBarrelRegCollection",
    "MuonPlugCollection",
    // Needed by Pandora
    "LCalCaloHitCollections", "LHCalCaloHitCollections",
    "ProngVertexCollections","SplitVertexCollections","V0Vertices",
    0};
  ContextFactory* ctxt = new ContextFactory(mgr.ioMask(raw_mask));
  mgr.adoptContextFactory(ctxt);
  
  typedef FrameworkTest::AlgorithmConfig CFG;
#if 0
  const char* null_mask[]  = {0};
  const char* i_mask[]     = {"VXD_HITS","TRACKER_HITS","ECAL_HITS","MUON_HITS",0};
  const char* a_in_mask[]  = {"VXD_HITS",0};
  const char* a_out_mask[] = {"VXD_DEPOSITS",0};
  const char* b_in_mask[]  = {"TRACKER_HITS","ECAL_HITS","MUON_HITS",0};
  const char* b_out_mask[] = {"TRACKER_DEPOSITS","ECAL_DEPOSITS","MUON_DEPOSITS",0};
  const char* c_in_mask[]  = {"VXD_DEPOSITS","TRACKER_DEPOSITS",0};
  const char* c_out_mask[] = {"VXD_TRACKS","TRACKER_TRACKS",0};
  const char* out_mask[]   = {"VXD_TRACKS","TRACKER_TRACKS",0};
  
  ExecutorFactory* in  = new AEF<CFG>(10,CFG("IN", &mgr, raw_mask, i_mask, 10,5));
  ExecutorFactory* f0  = new AEF<CFG>(10,CFG("A",  &mgr, a_in_mask, a_out_mask, 50,10));
  ExecutorFactory* f1  = new AEF<CFG>(10,CFG("B",  &mgr, b_in_mask, b_out_mask, 200,10));
  ExecutorFactory* f2  = new AEF<CFG>(10,CFG("C",  &mgr, c_in_mask, c_out_mask, 50,10));
  ExecutorFactory* f3  = new AEF<CFG>(10,CFG("D",  &mgr, c_in_mask, c_out_mask, 500,10));
  ExecutorFactory* out = new AEF<CFG>(10,CFG("OUT",&mgr, out_mask,  null_mask, 50,10));

  out->initialize();
  f3->initialize();
  f2->initialize();
  f1->initialize();
  f0->initialize();
  in->initialize();
#else
  {
    const char* inputs[] = {"PandoraPFANewClusters", "MCParticle", "PandoraPFANewPFOs",
			    "RelationCaloHit", "LDCTracksMCP", "LDCTracks",0};
    const char* outputs[] = {"CalohitMCTruthLink", "ClusterMCTruthLink", "MCParticlesSkimmed",
			     "RecoMCTruthLink", "TrackMCTruthLink",0};
    ExecutorFactory* f = new AEF<CFG>(10,CFG("MyRecoMCTruthLinker",&mgr,inputs, outputs, 500,10)); f->initialize();
  }
  {
    const char* inputs[] = {"TPCLowPtCollection", "TPCCollection", "TPCSpacePointCollection",0};
    const char* outputs[] = {"AllTPCTrackerHits",0};
    ExecutorFactory* f = new AEF<CFG>(10,CFG("MyTPCDigiProcessor",&mgr,inputs, outputs, 500,10)); f->initialize();
  }
  {
    const char* inputs[] = {"ETDCollection",0};
    const char* outputs[] = {"ETDTrackerHits",0};
    ExecutorFactory* f = new AEF<CFG>(10,CFG("MyETDDigiProcessor",&mgr,inputs, outputs, 500,10)); f->initialize();
  }
  {
    const char* inputs[] = {"FTDTrackerHits", "SITTrackerHits", "VTXTrackerHits",0};
    const char* outputs[] = {"SiTracks", "SiTracksMCP",0};
    ExecutorFactory* f = new AEF<CFG>(10,CFG("MySiliconTrackingCLIC",&mgr,inputs, outputs, 500,10)); f->initialize();
  }
  {
    const char* inputs[] = {"EcalBarrelCollection", "HcalBarrelRegCollection",0};
    const char* outputs[] = {"ECALBarrel", "ECALEndcap", "ECALOther",
			     "HCALBarrel", "HCALEndcap", "HCALOther", "RelationCaloHit",0};
    ExecutorFactory* f = new AEF<CFG>(10,CFG("MyILDCaloDigi",&mgr,inputs, outputs, 500,10)); f->initialize();
  }
  {
    const char* inputs[] = {"SETCollection", "SITCollection", "VXDCollection",0};
    const char* outputs[] = {"SETTrackerHits", "SITTrackerHits", "VTXTrackerHits",0};
    ExecutorFactory* f = new AEF<CFG>(10,CFG("MyVTXDigiProcessor",&mgr,inputs, outputs, 500,10)); f->initialize();
  }
  {
    const char* inputs[] = {"LDCTracks",0};
    const char* outputs[] = {"KinkRecoParticles", "KinkVertices", "ProngRecoParticles",
			     "ProngVertices", "SplitRecoParticles", "SplitVertices",0};
    ExecutorFactory* f = new AEF<CFG>(10,CFG("MyKinkFinder",&mgr,inputs, outputs, 500,10)); f->initialize();
  }
  {
    const char* inputs[] = {"ECALBarrel", "HCALBarrel", "KinkVertices",
			    "LCalCaloHitCollections", "LHCalCaloHitCollections", "MCParticle", "MUON",
			    "ProngVertexCollections", "RelationCaloHit", "LDCTracksMCP", "SplitVertexCollections",
			    "LDCTracks", "V0Vertices",0};
    const char* outputs[] = {"PandoraPFANewClusters", "PandoraPFANewPFOs", "PandoraPFANewReclusterMonitoring",
			     "PandoraPFANewReclusterRelations",0};
    ExecutorFactory* f = new AEF<CFG>(10,CFG("MyMarlinPandora",&mgr,inputs, outputs, 500,10)); f->initialize();
  }
  {
    const char* inputs[] = {"AllTPCTrackerHits",0};
    const char* outputs[] = {"DroppedTPCTrackeHits", "TPCTracksMCP", "TPCTracks",
			     "UsedTPCTrackerHits",0};
    ExecutorFactory* f = new AEF<CFG>(10,CFG("MyLEPTrackingProcessor",&mgr,inputs, outputs, 500,10)); f->initialize();
  }
  {
    const char* inputs[] = {"LDCTracks",0};
    const char* outputs[] = {"V0RecoParticles", "V0Vertices",0};
    ExecutorFactory* f = new AEF<CFG>(10,CFG("MyV0Finder",&mgr,inputs, outputs, 500,10)); f->initialize();
  }
  {
    const char* inputs[] = {"FTDCollection",0};
    const char* outputs[] = {"FTDTrackerHits",0};
    ExecutorFactory* f = new AEF<CFG>(10,CFG("MyNewFTDDigiProcessor",&mgr,inputs, outputs, 500,10)); f->initialize();
  }
  {
    const char* inputs[] = {"ETDTrackerHits", "FTDTrackerHits", "SETTrackerHits",
			    "SITTrackerHits", "SiTracks", "SiTracksMCP", "AllTPCTrackerHits",
			    "TPCTracks", "TPCTracksMCP", "VTXTrackerHits",0};
    const char* outputs[] = {"LDCTracks", "LDCTracksMCP", "RefittedTPCTracks",
			     "RefittedSiTracks", "RefittedSiTracksMCP", "RefittedTPCTracksMCP",0};
    ExecutorFactory* f = new AEF<CFG>(10,CFG("MyFullLDCTracking",&mgr,inputs, outputs, 500,10)); f->initialize();
  }
  {
    const char* inputs[] = {"MuonPlugCollection",0};
    const char* outputs[] = {"MUON", "RelationMuonHit",0};
    ExecutorFactory* f = new AEF<CFG>(10,CFG("MySimpleMuonDigi",&mgr,inputs, outputs, 500,10)); f->initialize();
  }
#endif
  const char* line = "====================================================";
  ::lib_rtl_output(LIB_RTL_INFO,"++ Executing test: %s   run   %s\n",line,line);
  mgr.resolveInputs();
  for(int j=3, i=0; i<2; ++i)  {
    ::lib_rtl_output(LIB_RTL_INFO,"%s  %d  %s\n",line,i,line);
    mgr.createThreads(32);
    while(j>0)  {
      ::lib_rtl_output(LIB_RTL_INFO,"%s  START %3d  %s\n",line,j,line);
      mgr.start(100,10);
      ::lib_rtl_sleep(5000);
      mgr.resume();
      --j;
    }
    mgr.stop();
  }
  ::lib_rtl_output(LIB_RTL_INFO,"%s  DONE  %s\n",line,line);
  ::lib_rtl_sleep(100000);
}
