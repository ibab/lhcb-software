// $Id: SavesetFinder.cpp,v 1.4 2008-11-11 13:39:08 ggiacomo Exp $

#include "OMAlib/SavesetFinder.h"
#include "OMAlib/AnalysisTask.h"

SavesetFinder::SavesetFinder(AnalysisTask* Ana, 
                             std::string &TaskName,
                             std::string &PartitionName) :
  DimInfo(std::string(PartitionName + "/" + TaskName + OMAconstants::SavesetServicePath).c_str(), -1, "not found" ),
  m_analysis(Ana), m_taskname(TaskName), m_lastSaveset("none")
{
}

void SavesetFinder::infoHandler() {
  if( std::string(getString()) != "not found") {
    std::string saveset( getString() );
    if (saveset.size() >0  && saveset != m_lastSaveset) {
      m_analysis->info() << "calling analyze for task "<<m_taskname <<
        " on saveset "<<saveset << endreq;
      m_analysis->resetMessages(m_taskname);
      m_analysis->analyze( saveset, m_taskname );
      m_analysis->refreshMessageList(m_taskname);
      m_lastSaveset = saveset;
    }
  }
}

SavesetFinder::~SavesetFinder() {} 


