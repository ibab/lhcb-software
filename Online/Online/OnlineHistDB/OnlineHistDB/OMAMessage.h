// $Id: OMAMessage.h,v 1.14 2010-10-25 14:09:28 ggiacomo Exp $
#ifndef OMALIB_OMAMESSAGE_H 
#define OMALIB_OMAMESSAGE_H 1

#include <string>
#include <ostream>
#include "OnlineHistDB/OnlineHistDB.h"

/** @class OMAMessage OMAMessage.h OMAlib/OMAMessage.h
 *  
 *  warning/alarm message from automatic analysis 
 *  on online monitoring histograms
 *
 *  @author Giacomo GRAZIANI
 *  @date   2008-08-14
 */

namespace OnlineHistDBEnv_constants {
  static const int nmsglevel=6;
  static std::string msglevelName[nmsglevel] = {"", "", "NOSTAT", "INFO", "WARNING", "ALARM"};
}

class OMAMessage : public OnlineHistDBEnv {
public: 
  typedef enum { NOSTAT=2, INFO=3, WARNING, ALARM} OMAMsgLevel;
  typedef enum { ALARMCOLOR=2, WARNINGCOLOR=6 } OMAMsgColor;
   /// constructor from OMAlib (with HistDB)
  OMAMessage( std::string& HistName,
              std::string& TaskName,
              std::string& AnaTaskName,
              std::string& SaveSet,
              std::string& AnalysisName,
              std::string& Text,
              OMAMsgLevel Level,
              OnlineHistDB &env,
              int anaID =0);
  /// constructor from OMAlib (no HistDB)
  OMAMessage( std::string& HistName,
              std::string& TaskName,
              std::string& AnaTaskName,
              std::string& SaveSet,
              std::string& AnalysisName,
              std::string& Text,
              OMAMsgLevel Level);
  /// constructor from DB
  OMAMessage( int ID,
              OnlineHistDB &env);

  virtual ~OMAMessage();

  /// load from HistDB
  void load();
  /// store to HistDB
  void store(bool changePadColor=false);
  /// delete from DB
  void remove();
  /// disable (alarm condition is gone, but keep it as archived message)
  void disable();
  /// enable (alarm condition was gone, but alrm is back)
  void enable();
  

  unsigned int id() { return m_ID; }
  unsigned int anaId() { return m_anaid; }
  inline std::string& hIdentifier() {return m_histo; }
  inline std::string& taskName() { return m_taskName; }
  inline std::string& saveSet() {return m_saveSet; }
  inline std::string& ananame() {return m_ananame;}
  inline OMAMsgLevel level() { return m_level; }
  const char* concernedSystem(int level=1);
  inline void setLevel(OMAMsgLevel Level) { 
    if (Level != m_level) { m_level = Level; m_dbsync=false;}
  }
  inline const char* msgtext() { return m_msgtext.data(); }
  inline void setText(std::string& Text) { 
    std::string newMsg = m_anaComment + " " + Text; 
    if( newMsg != m_msgtext) {
      m_msgtext = newMsg;
      m_dbsync=false;
    }
  }
  inline void setSaveset(std::string& newSvs) {
    if (newSvs != m_saveSet) {
      m_saveSet = newSvs;
      m_dbsync=false;
    }
  }
  const char* levelString();
  void setLevelFromString(const char* slevel);

  inline bool confirmed() { return m_confirmed; }
  inline void confirm() { m_confirmed = true;}
  inline void unconfirm() { m_confirmed = false;}
  
  inline bool isactive() {return m_active;}
  inline int noccur() {return m_noccur;}
  inline int nsolved() {return m_nsolved;}
  inline int nretrig() {return m_nretrig;}

  void dump(std::ostream *out);
  char * humanTime();
  char * humanlastTime();

  inline bool matchsAnalysis(std::string& AnaTaskName, 
                             std::string& AnaName, 
                             std::string& HistoName,
                             int anaID=0) {
    if(AnaTaskName != m_anaTaskName) return false;
    if (anaID) 
      return (anaID == m_anaid &&
	      HistoName == m_histo);
    else 
      return (AnaName == m_ananame);
  }
  inline bool isAbort() {return m_isAbort;}
  inline bool isSyncWithDB() {return m_dbsync;}
  void updateEnv(OnlineHistDB* newSession);

private:
  char* time2human(int time);
  bool getAnaComment(int& anaID);
  void setPadColor();
  void unsetPadColor();
  OnlineHistDB* m_dbsession;
  int m_ID;
  std::string m_histo;
  sb2 m_histo_null;
  std::string m_saveSet;
  sb2 m_saveset_null;
  std::string m_taskName;
  sb2 m_taskName_null;
  std::string m_anaTaskName;
  sb2 m_anaTaskName_null;
  std::string m_msgtext;
  sb2 m_msgtext_null;
  OMAMsgLevel m_level;
  int m_anaid;
  sb2 m_anaid_null;
  std::string m_ananame;
  sb2 m_ananame_null;
  int m_time;
  int m_lastTime;
  bool m_active;
  int m_noccur;
  int m_nsolved;
  int m_nretrig;
  std::string m_anaComment;

  bool m_isAbort;
  bool m_confirmed;
  bool m_dbsync;
  bool m_hassysname;
  std::string m_sysName;
};
#endif // OMALIB_OMAMESSAGE_H
