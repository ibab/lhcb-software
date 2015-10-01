// $Id: OMAMessage.cpp,v 1.23 2010-10-21 10:16:46 ggiacomo Exp $
#include <time.h>
#include <sstream>
#include <cstring>
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
  m_anaid(anaID), m_ananame(AnalysisName), m_time(0), m_lastTime(0), m_active(true), 
  m_noccur(1), m_nsolved(0), m_nretrig(0),
  m_anaComment(""), m_isAbort(false), m_confirmed(true), m_dbsync(false), m_hassysname(false)
{
  m_histo_null = m_histo.empty() ? isnull : isnotnull;
  m_taskName_null = m_taskName.empty() ? isnull : isnotnull;
  m_ananame_null = m_ananame.empty() ? isnull : isnotnull; 
  if (anaID) {
    // add specific analysis message (if defined) to message text
    if ( getAnaComment(anaID) ) {
      setText(Text);
    }
  }
  m_sysName="";
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
  m_anaid(0), m_ananame(AnalysisName), m_time(0), m_lastTime(0), m_active(true),  
  m_noccur(1), m_nsolved(0), m_nretrig(0), m_anaComment(""),
  m_isAbort(false), m_confirmed(true), m_dbsync(false), m_hassysname(false)
{
  m_histo_null = m_histo.empty() ? isnull : isnotnull;
  m_taskName_null = m_taskName.empty() ? isnull : isnotnull;
  m_ananame_null = m_ananame.empty() ? isnull : isnotnull;
}

// constructor from DB
OMAMessage::OMAMessage( int ID,
                        OnlineHistDB &env) : 
  OnlineHistDBEnv(env), m_dbsession(&env), m_ID(ID), m_active(true), m_anaComment(""), m_isAbort(true),
  m_confirmed(true), m_dbsync(false), m_hassysname(false)
{
  load();
}

OMAMessage::~OMAMessage() {} 


bool OMAMessage::getAnaComment(int& anaID) {
  // get specific analysis message (if defined) to message text
  bool out=false;
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OMAMessage::getAnaComment";
  text anaMess[VSIZE_ANAMSG]="";
  if ( OCI_SUCCESS == prepareOCIStatement(stmt, "SELECT ANAMESSAGE FROM ANALYSIS WHERE AID=:1 and ANAMESSAGE is not NULL") ) {
    myOCIBindInt(stmt,":1", anaID);
    myOCIDefineString(stmt, 1, anaMess , VSIZE_ANAMSG);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
      m_anaComment = (char *) anaMess;
      out=true;
    }
    releaseOCIStatement(stmt);
  }
  return out;
}


void OMAMessage::load() {
  if( NULL == m_envhp) return; // no HistDB
  if( 0 == m_ID) return; // not present in HistDB
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OMAMessage::load";
  m_isAbort=true;
  std::string command =
    "BEGIN ONLINEHISTDB.GETMESSAGE(:id, theHName => :hn, theSaveSet => :ss";
  command += "  ,theTask => :tk, theAnalysisTask => :atk";
  command += "  ,theLevel => :lev, theMessage => :msg, theAName => :anm, theAid => :aid, theUXTime => :tim";
  command += "  ,theUXLastTime => :lti, theNoccur => :noc, theNsolv => :nso, theNretrig => :nre";
  command += "  ,theActive => :isa); end;";
  if (OCI_SUCCESS == prepareOCITaggedStatement(stmt, command.c_str(), "MSGLOAD") ) {
    text HNAME[VSIZE_NAME]="";
    text SAVESET[VSIZE_SAVESET]="";
    text TASK[VSIZE_HSTASK]="";
    text ANATASK[VSIZE_HSTASK]="";
    text LEVEL[VSIZE_ALEVEL]="";
    text MESSAGE[VSIZE_MESSAGE]="";
    text ANANAME[VSIZE_ANANAME]="";
    int isactive;

    myOCIBindInt   (stmt,":id", m_ID); 
    myOCIBindString(stmt,":hn", HNAME, VSIZE_NAME, &m_histo_null);
    myOCIBindString(stmt,":ss", SAVESET, VSIZE_SAVESET, &m_saveset_null);
    myOCIBindString(stmt,":tk", TASK, VSIZE_HSTASK, &m_taskName_null);
    myOCIBindString(stmt,":atk", ANATASK, VSIZE_HSTASK, &m_anaTaskName_null);
    myOCIBindString(stmt,":lev", LEVEL, VSIZE_ALEVEL);
    myOCIBindString(stmt,":msg", MESSAGE, VSIZE_MESSAGE, &m_msgtext_null);
    myOCIBindString(stmt,":anm", ANANAME, VSIZE_ANANAME, &m_ananame_null);
    myOCIBindInt   (stmt,":aid", m_anaid, &m_anaid_null);
    myOCIBindInt   (stmt,":tim", m_time);
    myOCIBindInt   (stmt,":lti", m_lastTime);
    myOCIBindInt   (stmt,":noc", m_noccur);
    myOCIBindInt   (stmt,":nso", m_nsolved);
    myOCIBindInt   (stmt,":nre", m_nretrig);
    myOCIBindInt   (stmt,":isa", isactive);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt) ) {
      if (m_anaid >= 0) { 
        m_isAbort=false;
        m_histo = m_histo_null ? "" : std::string((const char *) HNAME);
        m_saveSet= m_saveset_null ? "" : std::string((const char *) SAVESET);
        m_taskName = m_taskName_null ? "" : std::string((const char *) TASK);
        m_anaTaskName = m_anaTaskName_null ? "" : std::string((const char *) ANATASK);
        m_msgtext= m_msgtext_null ? "" : std::string((const char *) MESSAGE);
        setLevelFromString((const char *) LEVEL);
        m_ananame = std::string(m_ananame_null!=0 ? "" : (const char*)ANANAME);
        m_active = (isactive != 0);
        m_dbsync=true;

        if (!m_anaid_null)
          getAnaComment(m_anaid);
        else
          m_anaid=0;
      }
    }
    releaseOCITaggedStatement(stmt, "MSGLOAD");
  }
}



void OMAMessage::store(bool changePadColor) {
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
  
  command << ",theActive => :nia, outTime => :newt, outAname => :newa, lastTime => :nlt";
  command << ", outNoccur => :nno, outNsolv => :nns, outNretrig => :nnr";
  command << "); end;";

  if ( OCI_SUCCESS == prepareOCIStatement (stmt, command.str().c_str() ) ) {
    text ANANAME[VSIZE_ANANAME]="";
    int isactive= (int) m_active;
    myOCIBindInt   (stmt,":id", m_ID); 
    myOCIBindInt   (stmt,":nia", isactive);
    myOCIBindString(stmt,":newa", ANANAME, VSIZE_ANANAME);
    myOCIBindInt   (stmt,":newt", m_time);
    myOCIBindInt   (stmt,":nlt", m_lastTime);
    myOCIBindInt   (stmt,":nno", m_noccur);
    myOCIBindInt   (stmt,":nns", m_nsolved);
    myOCIBindInt   (stmt,":nnr", m_nretrig);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt) ) {
      m_ananame = std::string((const char *) ANANAME);
      m_dbsync=true;
    }
    releaseOCIStatement(stmt);
  }

  // flag histograms with alarms using pad color
  if(!m_histo_null && m_dbsession) {
    if (m_active && m_level > INFO) {
      if(changePadColor) setPadColor();
    }
    else {
      unsetPadColor();
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
      m_active=false;
      m_dbsync=false;
      m_isAbort=true;
    }
    releaseOCIStatement(stmt);
  }
  unsetPadColor();
}

void OMAMessage::setPadColor() {
  if(!m_histo_null && m_dbsession) {
    OnlineHistogram* histo = m_dbsession->getHistogram(m_histo);
    if (histo) {
      OMAMsgColor color = (m_level == ALARM) ?  ALARMCOLOR : WARNINGCOLOR;
      int icolor = (int) color;
      histo->setDisplayOption("PADCOLOR", &icolor);
      histo->saveHistDisplayOptions();
    }
  }
}

void OMAMessage::unsetPadColor() {
  if(!m_histo_null && m_dbsession) {
    OnlineHistogram* histo = m_dbsession->getHistogram(m_histo);
    if (histo) {
      histo->unsetDisplayOption("PADCOLOR");
      histo->saveHistDisplayOptions();
    }
  }
}

const char* OMAMessage::levelString() {
  int ilevel=(int) m_level;
  if (ilevel  <0 || ilevel >= nmsglevel) ilevel=0;
  return (msglevelName[ilevel]).c_str();
}

void OMAMessage::setLevelFromString(const char* slevel) {
  std::string st(slevel);
  for (int il=0; il<nmsglevel ; il++) {
    if (st == msglevelName[il]) {
      m_level = (OMAMsgLevel) il;
      return;
    }
  }
  m_level = INFO; // default
}

char * OMAMessage::humanTime() {
  return time2human(m_time);
}
char * OMAMessage::humanlastTime() {
  return time2human(m_lastTime);
}


char * OMAMessage::time2human(int abstime) {
  time_t rawtime = abstime;
  struct tm * timeinfo = gmtime ( &rawtime );
  if (abstime == 0) {
    time( &rawtime);
    timeinfo = localtime( &rawtime );
  }
  return asctime (timeinfo);
}

void OMAMessage::disable() {
  if (m_active) {
    m_active=false;
    m_dbsync=false;
  }
}

void OMAMessage::enable() {
  if (!m_active) {
    m_active=true;
    m_dbsync=false;
  }
}


void OMAMessage::dump(std::ostream *out) {
  if (m_anaid)
    *out << "----- ANALYSIS ID "<<m_anaid<< "  MESSAGE ID "<<m_ID;
  else
    *out << "----- MESSAGE ID "<<m_ID<< " FROM CUSTOM ANALYSIS";

  *out << " ---- STATUS="<< (m_active ? "ACTIVE" : "NOT ACTIVE") << std::endl;
  *out << "********  created on   "<<humanTime();
  *out << levelString() <<" from analysis Task "<<m_anaTaskName<< "  in analysis "<< m_ananame <<std::endl;
  *out << " related to system "<<concernedSystem()<<std::endl;
  if(!m_histo.empty())
    *out << "      on histogram " << m_histo <<std::endl;
  *out <<   "      from saveset "<< m_saveSet<<std::endl;
  *out << m_msgtext<<std::endl;
  *out << "        --------         "<<std::endl;
  *out << "Occurred "<<m_noccur<<" times, solved "<<m_nsolved<<" times, retriggered "<<m_nretrig<<" times"<<std::endl;
  *out << (m_active ? "Last seen on " : "Solved on ") <<humanlastTime();
  *out << "----------------------------------------------------------------------"<<std::endl<<std::endl;

}

void OMAMessage::updateEnv(OnlineHistDB* newSession) {
  if (newSession) {
    m_dbsession = newSession;
    copyEnv(*newSession);
  }
}

const char*  OMAMessage::concernedSystem(int level) {
  if (m_dbsession && !m_hassysname) {
    m_sysName = "";
    if (! m_taskName.empty()) {
      OnlineHistTask* task = m_dbsession->getTask(m_taskName);
      if (task) m_sysName=task->det(level-1);
    }
  }
  return m_sysName.c_str();
}
