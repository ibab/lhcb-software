// $Id: SavesetFinder.cpp,v 1.2 2008-04-30 13:28:54 ggiacomo Exp $

#include "OMAlib/SavesetFinder.h"
#include "OMAlib/AnalysisTask.h"

SavesetFinder::SavesetFinder(AnalysisTask* Ana, 
                             std::string TaskName  ) :
  DimInfo(std::string(OMAconstants::SavesetServicePath + std::string(TaskName)).c_str(), -1 ),
  m_analysis(Ana), m_taskname(TaskName)
{}

void SavesetFinder::infoHandler() {
  std::string saveset( getString() );
  m_analysis->analyze( saveset, m_taskname );
}

SavesetFinder::~SavesetFinder() {} 


