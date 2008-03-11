// $Id: SavesetFinder.cpp,v 1.1 2008-03-11 18:23:27 ggiacomo Exp $

#include "OMAlib/SavesetFinder.h"
#include "OMAlib/AnalysisTask.h"

SavesetFinder::SavesetFinder(AnalysisTask* Ana, 
                             std::string TaskName  ) :
  DimInfo(std::string(OMAconstants::SavesetServicePath + std::string(TaskName)).c_str(), -1 ),
  m_analysis(Ana), m_taskname(TaskName)
{}

void SavesetFinder::infoHandler() {
  m_analysis->analyze( std::string( getString() ) , m_taskname );
}

SavesetFinder::~SavesetFinder() {} 


