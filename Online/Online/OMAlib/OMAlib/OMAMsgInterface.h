// $Id: OMAMsgInterface.h,v 1.12 2009-06-09 14:32:20 ggiacomo Exp $
#ifndef OMALIB_OMAMSGINTERFACE_H 
#define OMALIB_OMAMSGINTERFACE_H 1

/** @class OMAMsgInterface OMAMsgInterface.h OMAlib/OMAMsgInterface.h
 *  Output interface for Online Monitoring Analysis messages 
 *  supports MsgStream when available (analysis tasks),
 *  uses HistDB to log alarms   
 *
 *  @author Giacomo Graziani
 *  @date   2008-02-29
 */

#include <string>
#include <vector>
#include "OnlineHistDB/OMAMessage.h"
class MsgStream;
class TH1;
class OnlineHistDB;

namespace OMAconstants {
  static const int AlgListID = 11;
  static const std::string version = "v2r6";
  
  static const int AlarmExpTime = 28800; // one shift
  static const double epsilon = 1.e-10;
}

class OMAMsgInterface {
public: 
  OMAMsgInterface(OnlineHistDB* HistDB = NULL, std::string Name=""); 
  virtual ~OMAMsgInterface( ); 

  /// unconfirm all messages (to be called before starting new analysis) 
  void resetMessages(std::string& TaskName);
  /// lower and delete all unconfirmed messages  (to be called at the end of analysis)
  void refreshMessageList(std::string& TaskName);
  /// declare a message, that is sent to output only if new or if level is higher
  /// (this can be used only id analysis name has been already declared through setAnaName)
  void raiseMessage(OMAMessage::OMAMsgLevel level,
                    std::string& message,
                    std::string& histogramName);   
  /// declare a message, that is sent to output only if new or if level is higher
  void raiseMessage(OMAMessage::OMAMsgLevel level,
                    std::string& message,
                    std::string& histogramName,
                    std::string& AnalysisName);  
  inline void setAnaId(int id) {m_anaid = id;}
  inline void setAnaName(std::string &name) {m_anaName = name;}
protected:
  void checkWritePermissions();
  void loadMessages();
  void setMsgStream(MsgStream* ms) { m_outs=ms;}  
  std::string m_anaTaskname;
  std::string m_savesetName;
  std::string m_taskname;
  std::string m_anaName;
  int m_anaid;
  OnlineHistDB* m_histDB;

private:
  bool raiseAlarm(OMAMessage& message);
  bool lowerAlarm(OMAMessage& message);
  MsgStream* m_outs;
  std::vector<OMAMessage*> m_MessageStore;
};
#endif // OMALIB_OMAMSGINTERFACE_H
