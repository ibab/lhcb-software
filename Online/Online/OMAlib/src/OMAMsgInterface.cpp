// $Id: OMAMsgInterface.cpp,v 1.23 2009-09-17 17:54:16 ggiacomo Exp $
#include <cstring>
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAMsgInterface.h"
#include "GaudiKernel/MsgStream.h"
#include <dim/dis.hxx>
//-----------------------------------------------------------------------------
// Implementation file for class : OMAMsgInterface
// 2008-02-29 : Giacomo Graziani
//-----------------------------------------------------------------------------

OMAMsgInterface::OMAMsgInterface( OnlineHistDB* HistDB , 
                                  std::string Name) : 
  OMAEnv(HistDB, Name), m_savesetName("") , m_taskname(""), 
  m_anaName(""), m_anaid(0), m_msgInit(false), m_textLog(false),
  m_doPublish(true), m_textLogName(""), m_outs(NULL), m_iMsg(0)
{
  checkWritePermissions();
  m_MessageStore.clear();
  if(HistDB) 
    loadMessages();
}

OMAMsgInterface::~OMAMsgInterface() 
{
  std::vector<OMAMessage*>::iterator iM;
  for (iM = m_MessageStore.begin(); iM != m_MessageStore.end(); iM++) {
    delete (*iM);
  }
  std::map<OMAMessage*, DimService*>::iterator iS;
  for (iS = m_dimMessages.begin(); iS != m_dimMessages.end(); iS++) {
    delete (*iS).second ;
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
      publishMessage(lmsg);
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

void OMAMsgInterface::openLog() {
  if (m_textLog) {
    m_logOut.open(m_textLogName.c_str(), std::ios::app);
  }
}

void OMAMsgInterface::closeLog() {
  if (m_textLog) {
    m_logOut.close();
  }
}

void OMAMsgInterface:: startMessagePublishing() {
  if(m_doPublish) {
    std::string ServerName="/OMA/"+m_anaTaskname;
    DimServer::start(ServerName.c_str());
    DimServer::autoStartOn();
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
        unpublishMessage(*iM);
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
    if (!newMsg) {
      unpublishMessage(msg);
    }
    publishMessage(msg);
  }
}


void OMAMsgInterface::sendLine(std::string line, 
                               OMAMessage::OMAMsgLevel level) {
  // send alarm messages to MsgService
  (*m_outs) << (MSG::Level) level << line << endmsg;
  // if required, send also to ascii log file
  if (m_textLog) {
    m_logOut << line << std::endl;
  }
}


bool OMAMsgInterface::raiseAlarm(OMAMessage& message) {
  sendLine( "===================================================================", message.level());
  char* time = message.humanTime();
#ifndef _WIN32
  std::remove(time, time+strlen(time)+1,'\n');
#endif
  sendLine( "********     "+std::string(time) , message.level() );
  sendLine( std::string(message.levelString()) + " from Analysis Task "+
            m_anaTaskname, message.level() );
  if (! message.ananame().empty()) 
    sendLine("    in analysis " +  message.ananame() , message.level() ); 
  if (! message.hIdentifier().empty())
    sendLine( "      on histogram " + message.hIdentifier(), message.level() );
  sendLine(   "      from saveset " + message.saveSet(), message.level() );
  sendLine(  message.msgtext() , message.level() );
  sendLine( "===================================================================", message.level() );

  return true;
}



bool OMAMsgInterface::lowerAlarm(OMAMessage& message) {
  sendLine( "===================================================================", OMAMessage::INFO);
  char* time = message.humanTime();
#ifndef _WIN32
  std::remove(time, time+strlen(time)+1,'\n');
#endif
  sendLine( "********     "+std::string(time) , OMAMessage::INFO );
  sendLine( "===================== Analysis Task " +
            m_anaTaskname, OMAMessage::INFO);
  if (! message.ananame().empty()) 
    sendLine("    in analysis " +  message.ananame() , OMAMessage::INFO ); 
  if(!message.hIdentifier().empty())
    sendLine("   on histogram " + message.hIdentifier(),  OMAMessage::INFO);
  sendLine( std::string(message.levelString()) + " has gone" , OMAMessage::INFO);
  sendLine("===================================================================", OMAMessage::INFO );
  return true;
}

void OMAMsgInterface::publishMessage(OMAMessage* &msg) {
  if (m_doPublish) {
    char* time = msg->humanTime();
#ifndef _WIN32
    std::remove(time, time+strlen(time)+1,'\n');
#endif
    std::stringstream svcName, svcContent;
    svcName << "/OMA/" << m_anaTaskname << "/" << msg->levelString() <<
      "/Message" << m_iMsg;
    svcContent << time << " " << msg->levelString() << " from Analysis Task "
               << m_anaTaskname;
    if (! msg->ananame().empty()) 
      svcContent << " analysis " << msg->ananame().data();
    if (! msg->hIdentifier().empty())
      svcContent << " on histogram " << msg->hIdentifier().data();
    svcContent << ":\n";
    svcContent << msg->msgtext();
    m_msgLinks.push_back( svcContent.str() );
    char * ptr = const_cast<char*>(m_msgLinks[m_iMsg].c_str());
    DimService* dimSvc = new DimService(svcName.str().c_str(),
                                        ptr );
    m_dimMessages[msg] = dimSvc;
    m_iMsg++;
  }
}

void OMAMsgInterface::unpublishMessage(OMAMessage* &msg) {
  if (m_doPublish) {
    DimService* svc = m_dimMessages[msg];
    if (svc) {
      m_dimMessages.erase(msg);
      delete svc;
    }
  }
}
