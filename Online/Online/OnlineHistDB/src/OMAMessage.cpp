// $Id: OMAMessage.cpp,v 1.10 2009-05-12 07:44:25 ggiacomo Exp $
#include <time.h>
#include "OnlineHistDB/OMAMessage.h"
using namespace std;
using namespace OnlineHistDBEnv_constants;
//-----------------------------------------------------------------------------
// Implementation file for class : OMAMessage
//
// 2009-02-12 : Giacomo GRAZIANI
//-----------------------------------------------------------------------------


// constructor from OMAlib (with HistDB)
OMAMessage::OMAMessage( std::string& HistName,
                        std::string& TaskName,
                        std::string& AnaTaskName,
                        std::string& SaveSet,
                        std::string& AnalysisName,
                        std::string& Text,
                        OMAMsgLevel Level,
                        OnlineHistDB &env,
                        int anaID) :
  OnlineHistDBEnv(env),  m_dbsession(&env), m_ID(0), m_histo(HistName), m_saveSet(SaveSet),
  m_taskName(TaskName), m_anaTaskName(AnaTaskName), m_anaTaskName_null(0),
  m_msgtext(Text), m_msgtext_null(0), m_level(Level),
  m_anaid(anaID), m_ananame(AnalysisName), m_time(0),
  m_isAbort(false), m_confirmed(true), m_dbsync(false)
{
  m_histo_null = m_histo.empty() ? 1 : 0;
  m_taskName_null = m_taskName.empty() ? 1 : 0;
  m_ananame_null = m_ananame.empty() ? 1 : 0; 
  
}

// constructor from OMAlib (no HistDB)
OMAMessage::OMAMessage( std::string& HistName,
                        std::string& TaskName,
                        std::string& AnaTaskName,
                        std::string& SaveSet,
                        std::string& AnalysisName,
                        std::string& Text,
                        OMAMsgLevel Level) :
  OnlineHistDBEnv(), m_dbsession(NULL), m_ID(0), m_histo(HistName), m_saveSet(SaveSet),
  m_taskName(TaskName),  m_anaTaskName(AnaTaskName), m_anaTaskName_null(0),
  m_msgtext(Text), m_msgtext_null(0), m_level(Level),
  m_anaid(0), m_ananame(AnalysisName), m_time(0),
  m_isAbort(false), m_confirmed(true), m_dbsync(false)
{
  m_histo_null = m_histo.empty() ? 1 : 0;
  m_taskName_null = m_taskName.empty() ? 1 : 0;
  m_ananame_null = m_ananame.empty() ? 1 : 0;
}

// constructor from DB
OMAMessage::OMAMessage( int ID,
                        OnlineHistDB &env) : 
  OnlineHistDBEnv(env), m_dbsession(&env), m_ID(ID), m_isAbort(true),
  m_confirmed(true), m_dbsync(false)
{
  load();
}

OMAMessage::~OMAMessage() {} 

void OMAMessage::load() {
  if( NULL == m_envhp) return; // no HistDB
  if( 0 == m_ID) return; // not present in HistDB
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OMAMessage::load";
  std::string command =
    "BEGIN ONLINEHISTDB.GETMESSAGE(:id, theHName => :hn, theSaveSet => :ss";
  command += "  ,theTask => :tk, theAnalysisTask => :atk";
  command += "  ,theLevel => :lev, theMessage => :msg, theAName => :anm, theAid => :aid, theUXTime => :tim); end;";
  if (OCI_SUCCESS == prepareOCITaggedStatement(stmt, command.c_str(), "MSGLOAD") ) {
    text HNAME[VSIZE_NAME]="";
    text SAVESET[VSIZE_SAVESET]="";
    text TASK[VSIZE_HSTASK]="";
    text ANATASK[VSIZE_HSTASK]="";
    text LEVEL[VSIZE_ALEVEL]="";
    text MESSAGE[VSIZE_MESSAGE]="";
    text ANANAME[VSIZE_ANANAME]="";
    
    myOCIBindInt   (stmt,":id", m_ID); 
    myOCIBindString(stmt,":hn", HNAME, VSIZE_NAME, &m_histo_null);
    myOCIBindString(stmt,":ss", SAVESET, VSIZE_SAVESET);
    myOCIBindString(stmt,":tk", TASK, VSIZE_HSTASK, &m_taskName_null);
    myOCIBindString(stmt,":atk", ANATASK, VSIZE_HSTASK, &m_anaTaskName_null);
    myOCIBindString(stmt,":lev", LEVEL, VSIZE_ALEVEL);
    myOCIBindString(stmt,":msg", MESSAGE, VSIZE_MESSAGE, &m_msgtext_null);
    myOCIBindString(stmt,":anm", ANANAME, VSIZE_ANANAME, &m_ananame_null);
    myOCIBindInt   (stmt,":aid", m_anaid);
    myOCIBindInt   (stmt,":tim", m_time);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt) ) {
      if (m_anaid < 0) {
        m_isAbort=true;
      }
      else {
        m_histo = m_histo_null ? "" : std::string((const char *) HNAME);
        m_saveSet= std::string((const char *) SAVESET);
        m_taskName = m_taskName_null ? "" : std::string((const char *) TASK);
        m_anaTaskName = m_anaTaskName_null ? "" : std::string((const char *) ANATASK);
        m_msgtext= m_msgtext_null ? "" : std::string((const char *) MESSAGE);
        setLevelFromString((const char *) LEVEL);
        m_ananame = m_ananame_null ? "" : std::string((const char *) ANANAME);
        m_dbsync=true;
        m_isAbort=false;
      }
    }
    releaseOCITaggedStatement(stmt, "MSGLOAD");
  }
}



void OMAMessage::store() {
  if( NULL == m_envhp) return; // no HistDB
  if( m_isAbort ) return; 
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OMAMessage::store";
  std::stringstream command;
  command << "BEGIN :id := ONLINEHISTDB.STOREMESSAGE(theLevel => '"
          << levelString() <<"', theSaveSet =>'"
          << m_saveSet <<"'";
  if(!m_histo_null)
    command << ", theHName => '" << m_histo <<"'";
  if(!m_taskName_null)
    command << ", theTask => '" << m_taskName <<"'";
  if(!m_anaTaskName_null)
    command << ", theAnalysisTask => '" << m_anaTaskName <<"'";
  if(!m_msgtext_null)
    command << ", theMessage => '"<< m_msgtext <<"'";
  if (m_anaid)
    command << ", theAID => "<< m_anaid;
  else 
    command << ",theAName => '" << m_ananame <<"'";
  if(m_ID)
    command << ",theID => "<< m_ID;
  command << ",outTime => :newt, outAname => :newa); end;";


  if ( OCI_SUCCESS == prepareOCIStatement (stmt, command.str().c_str() ) ) {
    text ANANAME[VSIZE_ANANAME]="";
    myOCIBindInt   (stmt,":id", m_ID); 
    myOCIBindString(stmt,":newa", ANANAME, VSIZE_ANANAME);
    myOCIBindInt   (stmt,":newt", m_time);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt) ) {
      m_ananame = std::string((const char *) ANANAME);
      m_dbsync=true;
    }
    releaseOCIStatement(stmt);
  }

  // flag histograms with alarms using pad color
  if(!m_histo_null && (m_level > INFO) && m_dbsession) {
    OnlineHistogram* histo = m_dbsession->getHistogram(m_histo);
    if (histo) {
      OMAMsgColor color = (m_level == ALARM) ?  ALARMCOLOR : WARNINGCOLOR;
      int icolor = (int) color;
      histo->setDisplayOption("PADCOLOR", &icolor);
      histo->saveHistDisplayOptions();
    }
  }

}

void OMAMessage::remove() {
  if( NULL == m_envhp) return; // no HistDB
  if( 0 == m_ID) return; // already not present in HistDB
  m_StmtMethod = "OMAMessage::remove";
  OCIStmt *stmt=NULL;
  if ( OCI_SUCCESS == prepareOCIStatement 
       (stmt,"DELETE FROM ANAMESSAGE WHERE ID=:id") ) {
    myOCIBindInt   (stmt,":id", m_ID);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt) ) {
      m_ID=0;
      m_dbsync=false;
    }
    releaseOCIStatement(stmt);
  }
  if(!m_histo_null && (m_level > INFO) && m_dbsession) {
    OnlineHistogram* histo = m_dbsession->getHistogram(m_histo);
    if (histo) {
      histo->unsetDisplayOption("PADCOLOR");
    }
  }

}

const char* OMAMessage::levelString() {
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

void OMAMessage::setLevelFromString(const char* slevel) {
  std::string st(slevel);
  if (st == "ALARM")
    m_level = ALARM;
  else if(st == "WARNING")
    m_level = WARNING;
  else
    m_level = INFO;
}

char * OMAMessage::humanTime() {
  time_t rawtime = m_time;
  struct tm * timeinfo = gmtime ( &rawtime );
  if (m_time == 0) {
    time( &rawtime);
    timeinfo = localtime( &rawtime );
  }
  return asctime (timeinfo);
}

void OMAMessage::dump(std::ostream *out) {
  *out << "----------------------------------------------------------------------"<<std::endl;
  *out << "********     "<<humanTime();
  *out << levelString() <<" from analysis Task "<<m_anaTaskName<<"  in analysis "<< m_ananame <<std::endl;
  if(!m_histo.empty())
    *out << "      on histogram " << m_histo <<std::endl;
  *out <<   "      from saveset "<< m_saveSet<<std::endl;
  *out << m_msgtext<<std::endl;
  *out << "----------------------------------------------------------------------"<<std::endl<<std::endl;

}
