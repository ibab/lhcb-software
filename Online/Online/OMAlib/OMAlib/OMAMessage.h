// $Id: OMAMessage.h,v 1.2 2008-09-26 17:24:55 ggiacomo Exp $
#ifndef OMALIB_OMAMESSAGE_H 
#define OMALIB_OMAMESSAGE_H 1

#include <string>

/** @class OMAMessage OMAMessage.h OMAlib/OMAMessage.h
 *  
 *
 *  @author Giacomo GRAZIANI
 *  @date   2008-08-14
 */
class OMAMessage {
public: 
  typedef enum { INFO=3, WARNING, ALARM} OMAMsgLevel;
  OMAMessage( int Id,
              std::string& HistName,
              std::string& TaskName,
              std::string& Text,
              OMAMsgLevel Level) :
    m_ID(Id), m_histo(HistName), m_task(TaskName),
    m_text(Text), m_level(Level),
    m_confirmed(true) {}
  virtual ~OMAMessage() {} 
  unsigned int id() {return m_ID; }
  inline std::string& histogramName() {return m_histo; }
  inline std::string& taskName() {return m_task; }
  inline OMAMsgLevel level() { return m_level; }
  inline void setLevel(OMAMsgLevel Level) { m_level = Level; }
  inline const char* text() { return m_text.data(); }
  inline void setText(std::string& Text) { m_text = Text; }
  inline const char* levelString() {
    switch(m_level) {
    case ALARM:
      return "ALARM";
      break;
    case WARNING:
      return "WARNING";
      break;
    case INFO:
      return "INFO";
      break;
    }
    return "INFO";
  }
  inline bool confirmed() { return m_confirmed; }
  inline void confirm() { m_confirmed = true;}
  inline void unconfirm() { m_confirmed = false;}

private:
  unsigned int m_ID;
  std::string m_histo;
  std::string m_task;
  std::string m_text;
  OMAMsgLevel m_level;
  bool m_confirmed;
};
#endif // OMALIB_OMAMESSAGE_H
