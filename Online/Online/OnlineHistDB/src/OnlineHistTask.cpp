//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistTask.cpp,v 1.1 2007-07-09 10:17:42 ggiacomo Exp $
#include "OnlineHistDB/OnlineHistTask.h"


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
  Statement *qst=m_conn->createStatement 
    ("SELECT SUBSYS1,SUBSYS2,SUBSYS3,RUNONPHYSICS,RUNONCALIB,RUNONEMPTY,SAVEFREQUENCY,REFERENCE FROM TASK where TASKNAME=:1");
  try{
    qst->setString(1, Name);
    m_name = Name;
    ResultSet *rset =qst->executeQuery();
    if(rset->next ()) {
      m_ndet=0;
      for ( int id=0; id<3; id++) {
	m_sd[id]="NULL";
	if(rset->getString(id+1) != "") 
	  m_sd[m_ndet++] = rset->getString(id+1) ;
      }
      m_RunsOnPhysics = (rset->getInt(4) > 0);      
      m_RunsOnCalib = (rset->getInt(5) > 0);
      m_RunsOnEmpty = (rset->getInt(6) > 0);
      m_SavingFrequency = rset->getFloat(7);
      m_Reference = rset->getString(8);
    }
    else { // task does not exist
      m_abort=true;
    }
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistTask::OnlineHistTask for "+Name);
      m_abort=true;
    }
  m_conn->terminateStatement (qst);  
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
  bool out=true;
  Statement *tstmt=m_conn->createStatement 
    ("begin OnlineHistDB.DeclareTask(:x1,:x2,:x3,:x4,:x5,:x6,:x7,:x8,:x9); end;");
  try{
    tstmt->setString(1, m_name);
    tstmt->setString(2, m_sd[0]);
    tstmt->setString(3, m_sd[1]);
    tstmt->setString(4, m_sd[2]);
    tstmt->setInt(5, (int)m_RunsOnPhysics);
    tstmt->setInt(6, (int)m_RunsOnCalib);
    tstmt->setInt(7, (int)m_RunsOnEmpty);
    tstmt->setFloat(8, m_SavingFrequency);
    tstmt->setString(9, m_Reference);
    tstmt->executeUpdate ();
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistTask::save for Task "+m_name);
      out=false;
    }
  m_conn->terminateStatement (tstmt);  
  return out;
}


OnlineTaskStorage::OnlineTaskStorage(OnlineHistDBEnv* Env) : m_Taskenv(Env)
{}


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
