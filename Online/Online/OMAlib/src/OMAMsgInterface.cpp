// $Id: OMAMsgInterface.cpp,v 1.19 2009-06-16 17:39:49 ggiacomo Exp $
#include <cstring>
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAMsgInterface.h"
#include "GaudiKernel/MsgStream.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OMAMsgInterface
// 2008-02-29 : Giacomo Graziani
//-----------------------------------------------------------------------------

OMAMsgInterface::OMAMsgInterface( OnlineHistDB* HistDB , 
                                  std::string Name) : 
  OMAEnv(HistDB, Name), m_savesetName("") , m_taskname(""), 
  m_anaName(""), m_anaid(0), m_msgInit(false), m_outs(NULL)
{
  checkWritePermissions();
  m_MessageStore.clear();
  if(HistDB) 
    loadMessages();
}

OMAMsgInterface::~OMAMsgInterface() 
{
  std::vector<OMAMessage*>::iterator iM;
  for (iM=m_MessageStore.begin(); iM != m_MessageStore.end(); iM++) {
    delete (*iM);
  }
} 



void OMAMsgInterface::checkWritePermissions() {
  if (NULL != m_histDB && NULL != m_outs) {
    if(false == m_histDB->canwrite()) {
      (*m_outs) << MSG::WARNING << "You don't have write permissions on HistDB" <<endmsg;
      (*m_outs) << MSG::WARNING << "No persistency available for analysis messages" <<endmsg;
    }
  }
}


void OMAMsgInterface::loadMessages() {
  if(m_histDB && "noMessage" != m_anaTaskname) {
    // clean up old alarms
    if (m_histDB->canwrite())
      m_histDB->deleteOldMessages(OMAconstants::AlarmExpTime, m_anaTaskname);
    // load known alarms from DB
    std::vector<int> messID;
    m_histDB->getMessages(messID, m_anaTaskname);
    for (std::vector<int>::iterator im= messID.begin() ; im !=  messID.end() ; im++) {
      OMAMessage* lmsg = new OMAMessage(*im, *m_histDB);
      if(false == lmsg->isAbort() ) {
        m_MessageStore.push_back( lmsg );
      }
      else {
        delete lmsg;
      }
    }
    if (NULL != m_outs) {
      (*m_outs) << MSG::INFO << "Loaded "<<m_MessageStore.size() <<
        " existing messages for analysis " <<m_anaTaskname << endmsg;
    }
    m_msgInit = true;
  }
}

void OMAMsgInterface::updateMessages() {
  if(m_histDB && "noMessage" != m_anaTaskname) {
    std::vector<OMAMessage*>::iterator iM;
    for (iM=m_MessageStore.begin(); iM != m_MessageStore.end(); iM++) {
      (*iM)->updateEnv(m_histDB);
    }
  }
}


void OMAMsgInterface::resetMessages(std::string& TaskName) {
  std::vector<OMAMessage*>::iterator iM;
  for (iM=m_MessageStore.begin(); iM != m_MessageStore.end(); iM++) {
    if(TaskName == (*iM)->taskName()  || TaskName == "any")
      (*iM)->unconfirm();
  }
}

void OMAMsgInterface::refreshMessageList(std::string& TaskName) {
  std::vector<OMAMessage*>::iterator iM = m_MessageStore.begin();
  while( iM != m_MessageStore.end() ) {
    bool kept=true;
    if(TaskName == (*iM)->taskName() || TaskName == "any") {
      if( false == (*iM)->confirmed()) {
        kept=false;
        lowerAlarm( (**iM) );
        if (m_histDB->canwrite())
          (*iM)->remove();
        delete (*iM);
        iM = m_MessageStore.erase(iM);
      }
    }
    if (kept) iM++;
  }
  if(m_histDB)
    m_histDB->commit();
}


void OMAMsgInterface::raiseMessage(OMAMessage::OMAMsgLevel level,
                                   std::string& message,
                                   std::string& histogramName) {
  bool newMsg = true;
  bool send = true;
  OMAMessage* msg=NULL;

  std::string histIdentifier =m_taskname+"/"+histogramName;

  std::vector<OMAMessage*>::iterator iM;
  for (iM=m_MessageStore.begin(); iM != m_MessageStore.end(); iM++) {
    if( (*iM)->matchsAnalysis(m_anaTaskname, m_anaName, 
			      histIdentifier, m_anaid)) {
      // message is already known
      msg = (*iM);
      newMsg = false;
      msg->confirm();
      if ( level >=  msg->level() ) {
        msg->setText(message);
      }
      // send to output only if level grew
      if ( level >  msg->level() ) {
        msg->setLevel(level);
      }
      else {
        send = false;
      }
      break;
    }
  }
  if (newMsg && level > OMAMessage::INFO ) {
    if(m_histDB)
      msg = new OMAMessage(histIdentifier,  m_taskname, m_anaTaskname, 
                           m_savesetName, m_anaName,
                           message, level, *m_histDB, m_anaid);
    else 
      msg = new OMAMessage(histIdentifier,  m_taskname, m_anaTaskname, 
                           m_savesetName, m_anaName,
                           message, level);
    m_MessageStore.push_back( msg );
  }
  if (msg) {
    if (m_histDB) {
      if(m_histDB->canwrite())
        msg->store();
      m_histDB->commit();
    }
  }
  if (msg && send ) {
    raiseAlarm( (*msg) );
  }
}



bool OMAMsgInterface::raiseAlarm(OMAMessage& message) {
  (*m_outs) << (MSG::Level) message.level() << "==================================================================="<<endmsg;
  char* time = message.humanTime();
#ifndef _WIN32
  std::remove(time, time+strlen(time)+1,'\n');
#endif
  (*m_outs) << (MSG::Level) message.level() << "********     "<<time <<endmsg;
  (*m_outs) << (MSG::Level) message.level() << message.levelString() <<" from Analysis Task "<<
    m_anaTaskname <<"  in analysis "<<    message.ananame() <<endmsg;
  if(!message.hIdentifier().empty())
    (*m_outs) << (MSG::Level) message.level() << "      on histogram " 
              << message.hIdentifier()<<endmsg;
  (*m_outs) << (MSG::Level) message.level() <<   "      from saveset "<<message.saveSet()<<endmsg;
  (*m_outs) << (MSG::Level) message.level() <<  message.msgtext() <<endmsg;
  (*m_outs) << (MSG::Level) message.level() << "==================================================================="<<endmsg;
  return true;
}




bool OMAMsgInterface::lowerAlarm(OMAMessage& message) {
  (*m_outs) << MSG::INFO <<  "===================== Analysis Task "<<
    m_anaTaskname <<"  analysis "<<  message.ananame() <<endmsg;
  if(!message.hIdentifier().empty())
    (*m_outs) << (MSG::Level) message.level() << "   on histogram "<<message.hIdentifier()<<endmsg;
  (*m_outs) << MSG::INFO << message.levelString() << " has gone" <<endmsg;
  (*m_outs)  <<  MSG::INFO <<"==================================================================="<<endmsg;
  return true;
}
