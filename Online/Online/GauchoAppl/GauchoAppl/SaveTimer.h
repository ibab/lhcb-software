#ifndef SAVETIMER_H
#define SAVETIMER_H
#include "Gaucho/GenTimer.h"
#include "GauchoAppl/MonAdder.h"
#include "GauchoAppl/HistAdder.h"
class MonSubSys;
class DimService;
class SaveTimer : public GenTimer
{
public:
  MonAdder *m_Adder;
  void *m_buffadd;
  int m_bsiz;
  std::string m_filename;
  std::string m_rootdir;
  std::string m_fnpatt;
  std::string m_partname;
  std::string m_taskname;
  std::string m_infoFileStatus;
  bool m_EOR;
public:
  SaveTimer(MonAdder *tis, int period = 900);
  virtual ~SaveTimer(void);
  void timerHandler ( void );
//  void Stop();
  void SavetoFile(void *buff);
  void setRootDir(std::string &p){m_rootdir = p;return;};
  void setPartName(std::string &p){ m_partname= p;return;};
  void setTaskName(std::string &p){m_taskname = p;return;};
  void setEOR(bool flg){m_EOR = flg;return;};
  DimService *getSaveSetService(){return m_filenamesvc;};
  void setSaveSetService();
  void setSaveSetService(DimService *svc){m_filenamesvc = svc;return;};
  DimService *m_filenamesvc;
};
#endif
