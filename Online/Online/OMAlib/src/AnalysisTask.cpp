
#include "OMAlib/OMAlib.h"
#include "OMAlib/AnalysisTask.h"

AnalysisTask::AnalysisTask()
{
  m_omalib = new OMAlib();
  m_histDB = m_omalib->dbSession();
}

AnalysisTask::~AnalysisTask()
{
  delete m_omalib;
}
