// $Id: RunInfo.cpp,v 1.1 2010-06-11 13:00:11 ggiacomo Exp $
// Include files



// local
#include "OMAlib/AnalysisTask.h"
#include "OMAlib/RunInfo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RunInfo
//
// 2010-06-09 : Giacomo GRAZIANI
//-----------------------------------------------------------------------------


RunInfo::RunInfo( AnalysisTask* Ana ) : DimInfo("LHCbStatus/RunNumber", (int) -1),
                                        m_analysis(Ana)
{}

void RunInfo::infoHandler() {
  int run = getInt();
  if (run > -1) m_analysis->checkRunStatus(run);
}
