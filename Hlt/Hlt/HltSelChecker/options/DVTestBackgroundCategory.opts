/** @file 
 *  Test file for HLT
 *
 *  @author P. Koppenburg
 *  @date 2006-02-01
 */
//--------------------------------------------------------------
#include "$HLTSELECTIONSROOT/options/DVTestHlt.opts"

ApplicationMgr.TopAlg += { "HltBackgroundCategory" } ; 
HltBackgroundCategory.PrintTree = true ;


// ToolSvc.RelatedPVFinder.OutputLevel =1 ;
// Bu2LLKTuple.OutputLevel = 2 ;

//MessageSvc.OutputLevel = 2 ;
//MessageSvc.Format = "% F%80W%S%7W%R%T %0W%M";
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M";

//ChargedProtoPAlg.OutputLevel = 2 ;


//#include "$HLTSELECTIONSROOT/options/Bs2JpsiPhiDigi.opts"
//#include "$HLTSELECTIONSROOT/options/Bs2PhiPhiDigi.opts"
//#include "$HLTSELECTIONSROOT/options/B2DstarX.opts"
#include "$HLTSELECTIONSROOT/options/MinimumBiasPFN.opts"
//#include "$HLTSELECTIONSROOT/options/MinimumBiasRaw.opts"
//#include "$HLTSELECTIONSROOT/options/Bd2MuMuKstarPFN.opts"
ApplicationMgr.EvtMax = 10000 ;
EventSelector.PrintFreq = 100 ;


