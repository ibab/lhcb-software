//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistTask.cpp,v 1.3 2007-11-19 17:26:45 ggiacomo Exp $
#include "OnlineHistDB/OnlineHistTask.h"
using namespace OnlineHistDBEnv_constants;

OnlineHistTask::OnlineHistTask(OnlineHistDBEnv &env,
			       std::string Name, 
			       std::string SubDet1, 
			       std::string SubDet2, 
			       std::string SubDet3,
			       bool RunsOnPhysics, 
			       bool RunsOnCalib, 
			       bool RunsOnEmpty,
			       float SavingFrequency) :
  OnlineHistDBEnv(env),
  m_name(Name), m_RunsOnPhysics(RunsOnPhysics), m_RunsOnCalib(RunsOnCalib),
  m_RunsOnEmpty(RunsOnEmpty), m_SavingFrequency(SavingFrequency), m_abort(false) 
{
  declareSubDetectors(SubDet1,SubDet2,SubDet3);
  save();
}

OnlineHistTask::OnlineHistTask(OnlineHistDBEnv &env,
			       std::string Name) :
  OnlineHistDBEnv(env) , m_abort(false)
{

  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistTask::OnlineHistTask";
  if ( OCI_SUCCESS == prepareOCIStatement
       (stmt, "SELECT SUBSYS1,SUBSYS2,SUBSYS3,RUNONPHYSICS,RUNONCALIB,RUNONEMPTY,SAVEFREQUENCY,REFERENCE FROM TASK where TASKNAME=:1") ) {
    text SUBSYS[3][VSIZE_SSNAME];
    SUBSYS[0][0]=SUBSYS[1][0]=SUBSYS[2][0]='\0';
    int RUNONPHYSICS,RUNONCALIB,RUNONEMPTY;
    text REFERENCE[VSIZE_REFERENCE]="";
    myOCIBindString(stmt,":1", Name);
    myOCIDefineString(stmt, 1, SUBSYS[0], VSIZE_SSNAME, &m_sd_null[0]);
    myOCIDefineString(stmt, 2, SUBSYS[1], VSIZE_SSNAME, &m_sd_null[1]);
    myOCIDefineString(stmt, 3, SUBSYS[2], VSIZE_SSNAME, &m_sd_null[2]);
    myOCIDefineInt   (stmt, 4, RUNONPHYSICS, &m_RunsOnPhysics_null);
    myOCIDefineInt   (stmt, 5, RUNONCALIB, &m_RunsOnCalib_null);
    myOCIDefineInt   (stmt, 6, RUNONEMPTY, &m_RunsOnEmpty_null);
    myOCIDefineFloat (stmt, 7, m_SavingFrequency, &m_SavingFrequency_null);
    myOCIDefineString(stmt, 8, REFERENCE, VSIZE_REFERENCE, &m_Reference_null);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
      m_name = Name;      
      m_ndet=0;
      for ( int id=0; id<3; id++) {
	m_sd[id]="NULL";
	if(! m_sd_null[id]) 
	  m_sd[m_ndet++] = std::string((const char*) SUBSYS[id]);
      }
      m_RunsOnPhysics = m_RunsOnPhysics_null ? false : (RUNONPHYSICS > 0);      
      m_RunsOnCalib = m_RunsOnCalib_null ? false : (RUNONCALIB > 0);
      m_RunsOnEmpty = m_RunsOnEmpty_null ? false : (RUNONEMPTY > 0);
      m_Reference = m_Reference_null ? "" : std::string((const char*) REFERENCE);
    }
    else {// task does not exist
      m_abort=true;
    }
  }
}


bool OnlineHistTask::setSubDetectors(std::string SubDet1,
				     std::string SubDet2, 
				     std::string SubDet3) {

  declareSubDetectors(SubDet1,SubDet2,SubDet3);
  return save();
}


void OnlineHistTask::declareSubDetectors(std::string SubDet1,
					 std::string SubDet2, 
					 std::string SubDet3) {
  m_ndet=0;
  m_sd[0]=  SubDet1;
  m_sd[1]=  SubDet2;
  m_sd[2]=  SubDet3;
  
  while (m_sd[m_ndet] != "NULL") {
    if (++m_ndet == 3) break;
  }
}

bool OnlineHistTask::addSubDetector(std::string SubDet) {
  bool out=true;
  if(m_ndet == 3) {
    out = false;
  }
  else {
    m_sd[m_ndet++] = SubDet;
    out= save();
  }
  return out;
}


bool OnlineHistTask::save() {
  bool out=false;
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistTask::save";
  if ( OCI_SUCCESS == prepareOCIStatement
       (stmt, "BEGIN ONLINEHISTDB.DECLARETASK(:x1,:x2,:x3,:x4,:x5,:x6,:x7,:x8,:x9); END;") ) {
    int RUNONPHYSICS= (int)m_RunsOnPhysics,
      RUNONCALIB= (int)m_RunsOnCalib,
      RUNONEMPTY= (int)m_RunsOnEmpty;
    myOCIBindString(stmt,":x1", m_name);
    myOCIBindString(stmt,":x2", m_sd[0], &m_sd_null[0]);
    myOCIBindString(stmt,":x3", m_sd[1], &m_sd_null[1]);
    myOCIBindString(stmt,":x4", m_sd[2], &m_sd_null[2]);
    myOCIBindInt   (stmt,":x5", RUNONPHYSICS, &m_RunsOnPhysics_null);
    myOCIBindInt   (stmt,":x6", RUNONCALIB, &m_RunsOnCalib_null);
    myOCIBindInt   (stmt,":x7", RUNONEMPTY, &m_RunsOnEmpty_null);
    myOCIBindFloat (stmt,":x8", m_SavingFrequency, &m_SavingFrequency_null);
    myOCIBindString(stmt,":x9", m_Reference, &m_Reference_null);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
      out=true;
    }
  }
  return out;
}

void OnlineHistTask::dump() {
    cout << "Task "<< name() << "  related to "<<ndet() << " detectors:"<<endl <<"             ";
    for (int i=0 ; i< ndet(); i++) 
      cout << det(i) << "  ";
  cout << endl << " Run Conf: Ph=" << runsOnPhysics() <<
    "  Calib=" << runsOnCalib() << "  Empty=" << runsOnEmpty() <<endl ;
  if (m_SavingFrequency_null == 0)
    cout << " Frequency : " << savingFrequency() <<endl;
  if (m_Reference_null == 0)
    cout << " Reference : " << reference() <<endl;
  cout<< "-----------------------------------------------------------------------"<<endl;
}




OnlineTaskStorage::~OnlineTaskStorage() {
  if (m_Taskenv->debug() > 2) cout << "Deleting "<<
    m_myTask.size() << " OnlineHistTask objects"<<endl;
  std::vector<OnlineHistTask*>::iterator it;
  for (it = m_myTask.begin();it != m_myTask.end(); ++it) 
    delete *it; 
}

bool OnlineTaskStorage::declareTask(std::string Name, 
			       std::string SubDet1, 
			       std::string SubDet2, 
			       std::string SubDet3,
			       bool RunsOnPhysics, 
			       bool RunsOnCalib, 
			       bool RunsOnEmpty,
			       float SavingFrequency)
{  
  bool out=true;
  OnlineHistTask* tk=0;
  std::vector<OnlineHistTask*>::iterator it;
  for (it = m_myTask.begin();it != m_myTask.end(); ++it) {
    if((*it)->name() ==  Name) { 
      tk=(*it);
      out &= tk->setSubDetectors(SubDet1,SubDet2,SubDet3);
      out &= tk->setRunConfig(RunsOnPhysics,RunsOnCalib,RunsOnEmpty);
      out &= tk->setSavingFrequency(SavingFrequency);
      break;
    }
  }
  if (tk == 0) { 
    tk= new OnlineHistTask(*m_Taskenv,  
			   Name,SubDet1,SubDet2,SubDet3,
			   RunsOnPhysics,RunsOnCalib,RunsOnEmpty,
			   SavingFrequency);
    if(tk->isAbort()) {
	cout<<"Error from OnlineHistDB::declareTask : error creating Task "
	    << Name <<endl;
	delete tk;
	tk=0;
	out=false;
    }
    else 
      m_myTask.push_back(tk);
  }
  return out;
  
}

OnlineHistTask* OnlineTaskStorage::getTask(std::string Name) {
  OnlineHistTask* tk=0;
  std::vector<OnlineHistTask*>::iterator it;
  for (it = m_myTask.begin();it != m_myTask.end(); ++it) {
    if((*it)->name() ==  Name) { 
      tk=*it;
      break;
    }
  }
  if (tk == 0) { 
    tk= new OnlineHistTask(*m_Taskenv, Name);
    if(tk->isAbort()) {
	cout<<"Error from OnlineHistDB::getTask : not existing Task " 
	    << Name <<endl;
	delete tk;
	tk=0;
    }
    else 
      m_myTask.push_back(tk);
  }
  return tk;
}
