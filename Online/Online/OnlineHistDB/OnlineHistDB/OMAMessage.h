// $Id: OMAMessage.h,v 1.1 2009-02-16 10:37:42 ggiacomo Exp $
#ifndef OMALIB_OMAMESSAGE_H 
#define OMALIB_OMAMESSAGE_H 1

#include <string>
#include <ostream>
#include "OnlineHistDB/OnlineHistDBEnv.h"

/** @class OMAMessage OMAMessage.h OMAlib/OMAMessage.h
 *  
 *
 *  @author Giacomo GRAZIANI
 *  @date   2008-08-14
 */
class OMAMessage : public OnlineHistDBEnv {
public: 
  typedef enum { INFO=3, WARNING, ALARM} OMAMsgLevel;
   // constructor from OMAlib (with HistDB)
  OMAMessage( std::string& HistName,
              std::string& TaskName,
              std::string& AnaTaskName,
              std::string& SaveSet,
              std::string& AnalysisName,
              std::string& Text,
              OMAMsgLevel Level,
              OnlineHistDBEnv &env,
              int anaID =0);
  // constructor from OMAlib (no HistDB)
  OMAMessage( std::string& HistName,
              std::string& TaskName,
              std::string& AnaTaskName,
              std::string& SaveSet,
              std::string& AnalysisName,
              std::string& Text,
              OMAMsgLevel Level);
  // constructor from DB
  OMAMessage( int ID,
              OnlineHistDBEnv &env);

  virtual ~OMAMessage();

  // load from HistDB
  void load();
  // store to HistDB
  void store();
  // delete from DB
  void remove();

  unsigned int id() {return m_ID; }
  inline std::string& hIdentifier() {return m_histo; }
  inline std::string& taskName() { return m_taskName; }
  inline std::string& saveSet() {return m_saveSet; }
  inline std::string& ananame() {return m_ananame;}
  inline OMAMsgLevel level() { return m_level; }
  inline void setLevel(OMAMsgLevel Level) { 
    if (Level != m_level) { m_level = Level; m_dbsync=false;}
  }
  inline const char* msgtext() { return m_msgtext.data(); }
  inline void setText(std::string& Text) { m_msgtext = Text; m_dbsync=false;}
  
  const char* levelString();
  void setLevelFromString(const char* slevel);

  inline bool confirmed() { return m_confirmed; }
  inline void confirm() { m_confirmed = true;}
  inline void unconfirm() { m_confirmed = false;}
  
  void dump(std::ostream *out);
  char * humanTime();

  inline bool matchsAnalysis(std::string& AnaTaskName, 
                             std::string& AnaName, int anaID=0) {
    if(AnaTaskName != m_anaTaskName) return false;
    if (anaID) 
      return (anaID == m_anaid);
    else 
      return (AnaName == m_ananame);
  }

private:
  int m_ID;
  std::string m_histo;
  sb2 m_histo_null;
  std::string m_saveSet;
  std::string m_taskName;
  sb2 m_taskName_null;
  std::string m_anaTaskName;
  sb2 m_anaTaskName_null;
  std::string m_msgtext;
  sb2 m_msgtext_null;
  OMAMsgLevel m_level;
  int m_anaid;
  std::string m_ananame;
  sb2 m_ananame_null;
  int m_time;

  bool m_isAbort;
  bool m_confirmed;
  bool m_dbsync;
};
#endif // OMALIB_OMAMESSAGE_H
