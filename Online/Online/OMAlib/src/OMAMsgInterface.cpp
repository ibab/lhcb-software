// $Id: OMAMsgInterface.cpp,v 1.40 2010-10-22 10:54:28 ggiacomo Exp $
#include <cstring>
#include <sstream>
#include <time.h>
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAMsgInterface.h"
#include "GaudiKernel/MsgStream.h"
#include <dis.hxx>
#include <dic.hxx>
//-----------------------------------------------------------------------------
// Implementation file for class : OMAMsgInterface
// 2008-02-29 : Giacomo Graziani
//-----------------------------------------------------------------------------

OMAMsgInterface::OMAMsgInterface( OnlineHistDB* HistDB , 
                                  std::string Name) : 
  OMAEnv(HistDB, Name), m_savesetName("") , m_taskname(""), 
  m_anaName(""), m_anaid(0), m_padcolors(false), m_msgInit(false), m_textLog(false),
  m_doPublish(true), m_logToHistDB(true), m_textLogName(""), m_outs(NULL), 
  m_dimSvc(NULL)
{
  checkWritePermissions();
  m_MessageStore.clear();
}

OMAMsgInterface::~OMAMsgInterface() 
{
  std::vector<OMAMessage*>::iterator iM;
  for (iM = m_MessageStore.begin(); iM != m_MessageStore.end(); iM++) {
    delete (*iM);
  }
  if(m_dimSvc) delete m_dimSvc;
} 



void OMAMsgInterface::checkWritePermissions() {
  if (NULL != m_histDB && NULL != m_outs && m_logToHistDB) {
    if(false == m_histDB->canwrite()) {
      (*m_outs) << MSG::WARNING << "You don't have write permissions on HistDB" <<endmsg;
      (*m_outs) << MSG::WARNING << "No persistency available for analysis messages" <<endmsg;
      m_logToHistDB = false;
    }
  }
}

// analysis init: load all existing messages related to this analysis
void OMAMsgInterface::loadMessages() {
  if(m_histDB && "noMessage" != m_anaTaskname) {
    if (m_outs) {
     (*m_outs) << MSG::INFO << "Loading existing messages for analysis Task " <<
       m_anaTaskname << endmsg;
    }
    // clean up old alarms
    if (m_histDB->canwrite() && m_logToHistDB) {
      m_histDB->deleteOldMessages(OMAconstants::AlarmExpTime, m_anaTaskname);
      m_histDB->commit();
    }
    // load known alarms from DB
    std::vector<int> messID;
    m_histDB->getMessages(messID, m_anaTaskname);
    for (std::vector<int>::iterator im= messID.begin() ; im !=  messID.end() ; im++) {
      OMAMessage* lmsg = new OMAMessage(*im, *m_histDB);
      if(false == lmsg->isAbort() ) {
        //publishMessage(lmsg);
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

bool OMAMsgInterface:: startMessagePublishing() {
  if(m_doPublish) {
    std::string ServerName="OMA/"+m_anaTaskname;
    std::string svcName=ServerName+"/MESSAGES";
    char initVal[OnlineHistDBEnv_constants::VSIZE_MESSAGE]="";

    DimBrowser dbr;
    if(dbr.getServices(svcName.c_str())) {
      // service is already running, stop here
      return false;
    }
    m_dimSvc = new DimService(svcName.c_str(),initVal);    
    DimServer::start(ServerName.c_str());
    sleep(1);
  }
  return true;
}

// connect messages to current HistDB session and resync (alarms could have been modified by Presenter/user code)
void OMAMsgInterface::updateMessages() {
  if(m_histDB && "noMessage" != m_anaTaskname) {
    std::vector<OMAMessage*>::iterator iM;
    for (iM=m_MessageStore.begin(); iM != m_MessageStore.end(); iM++) {
      (*iM)->updateEnv(m_histDB);
      bool wasactive=(*iM)->isactive();
      (*iM)->load();
      if ((*iM)->isAbort()) { // message has been removed outside this program: unpublish and erase it from cache
        if(wasactive) {
          lowerAlarm( (**iM), true, true );
          unpublishMessage(*iM);
        }
        iM = m_MessageStore.erase(iM);
      }
      else if(wasactive && false == (*iM)->isactive()) {
        // message has been disabled outside this program: unpublish
        lowerAlarm( (**iM), true, false );
        unpublishMessage(*iM);
      }
    }
  }
}

// before performing the analysis, unconfirm all active messages related to it
void OMAMsgInterface::resetMessages(std::string& TaskName) {
  std::vector<OMAMessage*>::iterator iM;
  for (iM=m_MessageStore.begin(); iM != m_MessageStore.end(); iM++) {
    if ((*iM)->isactive()) {
      if(TaskName == "any" || 
         TaskName == (*iM)->taskName()  || VirtualTaskName(TaskName)  == (*iM)->taskName() )
        (*iM)->unconfirm();
    }
  }
}

// at the end of analysis, disable all active unconfirmed messages
void OMAMsgInterface::refreshMessageList(std::string& TaskName) {
  std::vector<OMAMessage*>::iterator iM = m_MessageStore.begin();
  while( iM != m_MessageStore.end() ) {
    if ((*iM)->isactive()) {
      if(TaskName == "any" || 
         TaskName == (*iM)->taskName()  || VirtualTaskName(TaskName)  == (*iM)->taskName() ) {
        if( false == (*iM)->confirmed()) {
          lowerAlarm( (**iM) );
          unpublishMessage(*iM);
          (*iM)->disable();
          if (m_histDB) {
            if (m_histDB->canwrite() && m_logToHistDB) {
              (*iM)->store(m_padcolors);
              m_histDB->commit();
            }
          }
        }
      }
    }
    iM++;
  }
}

// called from analysis algorithms
void OMAMsgInterface::raiseMessage(OMAMessage::OMAMsgLevel level,
                                   std::string& message,
                                   std::string& histogramName) {
  bool newMsg = true;
  bool send = false;
  OMAMessage* msg=NULL;

  std::string histIdentifier =m_taskname+"/"+histogramName;

  std::vector<OMAMessage*>::iterator iM;
  for (iM=m_MessageStore.begin(); iM != m_MessageStore.end(); iM++) {
    if( (*iM)->matchsAnalysis(m_anaTaskname, m_anaName, 
			      histIdentifier, m_anaid)) {
      // message is already known
      msg = (*iM);
      newMsg = false;
      if (msg->isactive()) { // was already there: confirm
        msg->confirm();
      }
      else {
        if(level > OMAMessage::INFO ) {
          send = true; // was disabled: retrigger alarm
          msg->confirm();
          msg->enable();
        }
      }
      // resend to output if level grew
      if ( level >  msg->level() ) {
        send = true;
        unpublishMessage(msg); // remove alarm with lower level from alarm logger (will resend with higher level)
      }
      if (level > OMAMessage::INFO ) { // update message
        msg->setLevel(level);
        msg->setText(message);
        msg->setSaveset(m_savesetName);
      }
      break;
    }
  }
  if (newMsg && level > OMAMessage::INFO ) {
    send = true;
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
      if(m_histDB->canwrite() && m_logToHistDB) {
        if( !(msg->isSyncWithDB()) ) {
          msg->store(m_padcolors);
          m_histDB->commit();
        }
      }
    }
  }
  if (msg && send ) {
    raiseAlarm( (*msg) );
    publishMessage(msg);
  }
}


void OMAMsgInterface::sendLine(std::string line, 
                               OMAMessage::OMAMsgLevel level) {
  // send alarm messages to MsgService
  if (m_outs) {
    (*m_outs) << (MSG::Level) level << line << endmsg;
  }
  // if required, send also to ascii log file
  if (m_textLog) {
    m_logOut << line << std::endl;
  }
}

void OMAMsgInterface::senderror( const char* wmessage) {
  if (m_outs) {
    (*m_outs) << MSG::ERROR << wmessage  << endmsg;
  }
  else {
    std::cout << "ERROR: "<< wmessage  << std::endl;
  }
}
void OMAMsgInterface::sendwarning( const char* wmessage) {
  if (m_outs) {
    (*m_outs) << MSG::WARNING << wmessage  << endmsg;
  }
  else {
    std::cout << "WARNING: "<< wmessage  << std::endl;
  }
}
void OMAMsgInterface::sendinfo( const char* wmessage) {
  if (m_outs) {
    (*m_outs) << MSG::INFO << wmessage  << endmsg;
  }
  else {
    std::cout << "INFO: "<< wmessage  << std::endl;
  }
}


bool OMAMsgInterface::raiseAlarm(OMAMessage& message) {
  std::stringstream msgId;
  std::stringstream anaId;
  msgId << message.id();
  anaId << message.anaId();
  sendLine( "========== ANALYSIS ID "+ anaId.str() + "   MESSAGE ID " + msgId.str() +
            "=============================", message.level());
  char* time = message.humanTime();
  char* lasttime = message.humanlastTime();
#ifndef _WIN32
  std::remove(time, time+strlen(time)+1,'\n');
  std::remove(lasttime, lasttime+strlen(lasttime)+1,'\n');
#endif
  sendLine( "********   created on  "+std::string(time) , message.level() );
  sendLine( std::string(message.levelString()) + " from Analysis Task "+
            m_anaTaskname, message.level() );
  sendLine( " related to system " + std::string( message.concernedSystem() ), message.level() );
  if (! message.ananame().empty()) 
    sendLine("    in analysis " +  message.ananame() , message.level() ); 
  if (! message.hIdentifier().empty())
    sendLine( "      on histogram " + message.hIdentifier(), message.level() );
  sendLine(   "      from saveset " + message.saveSet(), message.level() );
  sendLine(  message.msgtext() , message.level() );
  sendLine( "     --------         ", message.level() );
  std::stringstream octext;
  octext << "Occurred "<<message.noccur()<<" times, solved "<<message.nsolved()<<" times, retriggered "<<message.nretrig()
         <<" times";
  sendLine(octext.str(), message.level() );
  sendLine("Last seen on " +std::string(lasttime) , message.level() );
  sendLine( "===================================================================", message.level() );

  return true;
}



bool OMAMsgInterface::lowerAlarm(OMAMessage& message, bool exte, bool removed) {
  std::stringstream msgId;
  std::stringstream anaId;
  msgId << message.id();
  anaId << message.anaId();
  sendLine( "========== ANALYSIS ID "+ anaId.str() + "   MESSAGE ID " + msgId.str() +
            "=============================", OMAMessage::INFO);
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  char* time = asctime (timeinfo);

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
  if (exte) {
    std::string operation = removed ? "removed" : "disabled";
    sendLine( std::string(message.levelString()) + " has been " + operation + " manually by operator" , OMAMessage::INFO);
    sendLine("===================================================================", OMAMessage::INFO );
  }
  else {
    sendLine(   "   after analyzing  saveset " + m_savesetName, OMAMessage::INFO);
    sendLine( std::string(message.levelString()) + " has gone" , OMAMessage::INFO);
  }
  sendLine("===================================================================", OMAMessage::INFO );
  return true;
}



// publishing alarms through DIM:

void OMAMsgInterface::publishMessage(OMAMessage* &msg) {
  if (m_doPublish) {
    char* time = msg->humanTime();
#ifndef _WIN32
    std::remove(time, time+strlen(time)+1,'\n');
#endif
    std::stringstream svcContent;
    svcContent << msg->levelString() <<  "/" << msg->id() << "/"
               << time << " " << msg->msgtext() << "---" << msg->levelString() << " from Analysis Task "
               << m_anaTaskname << " on Saveset "<< msg->saveSet();
    if (! msg->ananame().empty()) 
      svcContent << " analysis " << msg->ananame().data();
    if (! msg->hIdentifier().empty())
      svcContent << " on histogram " << msg->hIdentifier().data();    
    updateDIMservice(svcContent.str());
  }
}

void OMAMsgInterface::unpublishMessage(OMAMessage* &msg) {
  if (m_doPublish) {
    std::stringstream svcContent;
    svcContent << msg->levelString() <<  "/CLEAR/" << msg->id();
    updateDIMservice(svcContent.str());
  }
}

void OMAMsgInterface::updateDIMservice(std::string svcString) {
  if(m_dimSvc) {
    if (svcString.size() > OnlineHistDBEnv_constants::VSIZE_MESSAGE-1) {
      svcString.resize(OnlineHistDBEnv_constants::VSIZE_MESSAGE-1);
    }
    strcpy ( m_lastMessage, svcString.c_str());
    m_dimSvc->updateService( m_lastMessage );
  }
}
