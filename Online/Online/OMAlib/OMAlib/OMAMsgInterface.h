// $Id: OMAMsgInterface.h,v 1.3 2008-08-19 22:45:32 ggiacomo Exp $
#ifndef OMALIB_OMAMSGINTERFACE_H 
#define OMALIB_OMAMSGINTERFACE_H 1

/** @class OMAMsgInterface OMAMsgInterface.h OMAlib/OMAMsgInterface.h
 *  Output interface for Online Monitoring Analysis messages 
 *
 *  @author Giacomo Graziani
 *  @date   2008-02-29
 */

#include <string>
#include <vector>
#include "OMAlib/OMAMessage.h"
class TH1;
class IOMAMsgTool;

class OMAMsgInterface {
public: 
  OMAMsgInterface( ); 
  virtual ~OMAMsgInterface( ); 

  /// unconfirm all messages (to be called before starting new analysis) 
  void resetMessages(std::string& TaskName);
  /// lower and delete all unconfirmed messages  (to be called at the end of analysis)
  void refreshMessageList(std::string& TaskName);
  /// declare a message, that is sent to output only if new or if level is higher
  virtual void raiseMessage(unsigned int Id,
                            OMAMessage::OMAMsgLevel level,
                            std::string& message,
                            std::string& histogramName,
                            std::string& TaskName); 
protected:
  IOMAMsgTool* m_msgTool;
private:
  std::vector<OMAMessage*> m_MessageStore;
};
#endif // OMALIB_OMAMSGINTERFACE_H
