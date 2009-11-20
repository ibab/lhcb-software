// $Id: OMAMsgInterface.h,v 1.19 2009-11-20 10:12:04 ggiacomo Exp $
#ifndef OMALIB_OMAMSGINTERFACE_H 
#define OMALIB_OMAMSGINTERFACE_H 1

/** @class OMAMsgInterface OMAMsgInterface.h OMAlib/OMAMsgInterface.h
 *  Output interface for Online Monitoring Analysis messages.
 *  Supports MsgStream when available (analysis Gaudi jobs),
 *  uses HistDB (if available) to log alarms   
 *
 *  @author Giacomo Graziani
 *  @date   2008-02-29
 */

#include <string>
#include <vector>
#include <fstream>
#include <map>
#include "OMAlib/OMAEnv.h"
#include "OnlineHistDB/OMAMessage.h"
class MsgStream;
class TH1;
class OnlineHistDB;
class DimService;

class OMAMsgInterface : public OMAEnv 
{
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
  /// set current analysis id for messages
  inline void setAnaId(int id) {m_anaid = id;}
  /// set current analysis name for messages
  inline void setAnaName(std::string &name) {m_anaName = name;}
protected:
  void checkWritePermissions();
  void openLog();
  void closeLog();
  void startMessagePublishing();
  void loadMessages();
  void updateMessages();
  void setMsgStream(MsgStream* ms) { m_outs=ms;}  
  std::string m_savesetName;
  std::string m_taskname;
  std::string m_anaName;
  int m_anaid;  

  bool m_msgInit;
  bool m_textLog;
  bool m_doPublish;
  std::string m_textLogName;
  std::ofstream m_logOut;
private:
  void sendLine(std::string line, 
                OMAMessage::OMAMsgLevel level);
  bool raiseAlarm(OMAMessage& message);
  bool lowerAlarm(OMAMessage& message);
  void publishMessage(OMAMessage* &msg);
  void unpublishMessage(OMAMessage* &msg);
  void updateDIMservice(std::string svcString);
  MsgStream* m_outs;
  std::vector<OMAMessage*> m_MessageStore;
  DimService* m_dimSvc;
  char m_lastMessage[OnlineHistDBEnv_constants::VSIZE_MESSAGE];
};
#endif // OMALIB_OMAMSGINTERFACE_H
