#ifndef TASKSAVETIMER_H
#define TASKSAVETIMER_H
#include "Gaucho/GenTimer.h"
#include "MonSubSys.h"

class TaskSaveTimer : public GenTimer
{
  MonSubSys *m_subsys;
  void *m_buffadd;
  int m_bsiz;
  std::string m_filename;
  std::string m_rootdir;
//  std::string m_fnpatt;
  std::string m_partname;
  std::string m_taskname;
  bool m_EOR;
public:
  TaskSaveTimer(MonSubSys *tis, int period = 900);
  virtual ~TaskSaveTimer(void);
  void timerHandler ( void );
  void Stop();
  void SavetoFile(void *buff);
  void setRootDir(std::string &p){m_rootdir = p;return;};
  void setPartName(std::string &p){ m_partname= p;return;};
  void setTaskName(std::string &p){m_taskname = p;return;};
  void setEOR(bool flg){m_EOR = flg;return;};
};
#endif
