// $Id: OMAMsgInterface.cpp,v 1.4 2008-08-19 22:45:32 ggiacomo Exp $
#include <iostream>
#include "OMAlib/OMAMsgInterface.h"
#include "OMAlib/IOMAMsgTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OMAMsgInterface
//
// 2008-02-29 : Giacomo Graziani
//-----------------------------------------------------------------------------

OMAMsgInterface::OMAMsgInterface(  ) : m_msgTool(NULL)  {
  m_MessageStore.clear();
}

OMAMsgInterface::~OMAMsgInterface() {
  std::vector<OMAMessage*>::iterator iM;
  for (iM=m_MessageStore.begin(); iM != m_MessageStore.end(); iM++) {
    delete (*iM);
  }
} 

void OMAMsgInterface::resetMessages(std::string& TaskName) {
  std::vector<OMAMessage*>::iterator iM;
  for (iM=m_MessageStore.begin(); iM != m_MessageStore.end(); iM++) {
    if(TaskName == (*iM)->taskName() )
      (*iM)->unconfirm();
  }
}

void OMAMsgInterface::refreshMessageList(std::string& TaskName) {
  OMAMessage* tbdMessage;
  std::vector<OMAMessage*>::iterator iM;
  for (iM=m_MessageStore.begin(); iM != m_MessageStore.end(); iM++) {
    if(TaskName == (*iM)->taskName() ) {
      if( false == (*iM)->confirmed()) {
        if(m_msgTool) m_msgTool->lowerAlarm( (**iM) );
        tbdMessage = (*iM);
        iM = m_MessageStore.erase(iM);
        delete tbdMessage;
      }
    }
  }
}


void OMAMsgInterface::raiseMessage(unsigned int Id,
                                   OMAMessage::OMAMsgLevel level,
                                   std::string& message,
                                   std::string& histogramName,
                                   std::string& taskName) {
  bool newMsg = true;
  bool send = true;
  OMAMessage* msg=NULL;
  std::vector<OMAMessage*>::iterator iM;
  for (iM=m_MessageStore.begin(); iM != m_MessageStore.end(); iM++) {
    if( (*iM)->id() == Id && 
        (*iM)->histogramName() == histogramName &&
        (*iM)->taskName() == taskName) {
      // message is already known
      msg = (*iM);
      newMsg = false;
      msg->confirm();
      // send to output only if level grew
      if ( level >  msg->level() ) {
        msg->setLevel(level);
	msg->setText(message);
      }
      else {
        send = false;
      }
      break;
    }
  }
  if (newMsg) {
    msg = new OMAMessage(Id, histogramName, taskName, message, level);
    m_MessageStore.push_back( msg);
  }
  if (msg && send && m_msgTool) {
    m_msgTool->raiseAlarm( (*msg) );
  }
}
