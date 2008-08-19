// $Id: SavesetFinder.cpp,v 1.3 2008-08-19 22:45:32 ggiacomo Exp $

#include "OMAlib/SavesetFinder.h"
#include "OMAlib/AnalysisTask.h"

SavesetFinder::SavesetFinder(AnalysisTask* Ana, 
                             std::string TaskName  ) :
  DimInfo(std::string(OMAconstants::SavesetServicePath + std::string(TaskName)).c_str(), -1 ),
  m_analysis(Ana), m_taskname(TaskName)
{}

void SavesetFinder::infoHandler() {
  std::string saveset( getString() );
  m_analysis->resetMessages(m_taskname);
  m_analysis->analyze( saveset, m_taskname );
  m_analysis->refreshMessageList(m_taskname);
}

SavesetFinder::~SavesetFinder() {} 


