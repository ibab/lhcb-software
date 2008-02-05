#ifndef ANALYSISTASK_H
#define ANALYSISTASK_H

class OMAlib;
class OnlineHistDB;

// this is just an empty skeleton..
class AnalysisTask
{
 public:
  AnalysisTask();
  virtual ~AnalysisTask ();
  virtual bool exec(std::string SaveSet) = 0;
  
 protected:
  OMAlib* m_omalib;
  OnlineHistDB* m_histDB;
  
};


#endif
