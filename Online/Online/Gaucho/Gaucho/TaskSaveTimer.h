#ifndef TASKSAVETIMER_H
#define TASKSAVETIMER_H
#include "Gaucho/GenTimer.h"
#include "Gaucho/MonSubSys.h"
#include "Gaucho/MonSys.h"
typedef std::map<std::string,void*> HISTMAP;
class TaskSaveTimer : public GenTimer
{
  MonSubSys *m_subsys;
  void *m_buffadd;
  int m_bsiz;
  void *m_prevbuff;
  int m_prevsiz;
  std::string m_filename;
  std::string m_rootdir;
//  std::string m_fnpatt;
  std::string m_partname;
  std::string m_taskname;
  bool m_EOR;
//  HISTMAP *m_prevmap;
//  HISTMAP *m_currmap;
  MonSys *m_MonSys;
//  void fillmap(void *buff, HISTMAP *m);
//  void diffblock(void *a, void *b, void *a_b);
public:
  TaskSaveTimer(MonSubSys *tis, int period = 900);
  virtual ~TaskSaveTimer(void);
  void timerHandler ( void );
  void Stop();
  void SavetoFile(void *buff);
  void setRootDir(std::string &p){m_rootdir = p;return;};
  void setPartName(std::string &p){ m_partname= p;return;};
  void setTaskName(std::string &p){m_taskname = p;return;};
  void setMonSys(MonSys *p){m_MonSys = p;return;};
  void setEOR(bool flg){m_EOR = flg;return;};
  DimService *m_filenamesvc;
};
#endif
