// $Id: SavesetFinder.cpp,v 1.10 2009-08-25 10:25:38 ggiacomo Exp $

#include "OMAlib/AnalysisTask.h"
#include "OMAlib/SavesetFinder.h"

SavesetFinder::SavesetFinder(AnalysisTask* Ana,
                             std::string &TaskName,
                             std::string &PartitionName) :
  DimInfo(std::string(PartitionName + "/" + TaskName + OMAconstants::SavesetServicePath).c_str(),
          (char*) "SAVESETLOCATION" ),
  m_analysis(Ana), m_taskname(TaskName)
{
  m_lastSaveset = "SAVESETLOCATION";
}

void SavesetFinder::infoHandler() {
  std::string saveset( getString() );
  if( saveset.find("SAVESETLOCATION") == std::string::npos &&
      saveset.find("Zombie") == std::string::npos &&
      saveset.size() >0  &&
      saveset != m_lastSaveset) {
    // new saveset to be analyzed
    m_analysis->openDBSession();
    m_analysis->info() << "calling analyze for task "<<m_taskname <<
      " on saveset "<<saveset << endreq;
    m_analysis->resetMessages(m_taskname);
    m_analysis->analyze( saveset, m_taskname );
    m_analysis->refreshMessageList(m_taskname);
    m_lastSaveset = saveset;
    m_analysis->closeDBSession();
  }
}

SavesetFinder::~SavesetFinder() {
}


