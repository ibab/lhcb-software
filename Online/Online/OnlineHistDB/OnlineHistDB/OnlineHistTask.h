// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistTask.h,v 1.1 2007-07-09 10:17:41 ggiacomo Exp $
#ifndef ONLINEHISTTASK_H
#define ONLINEHISTTASK_H 1
/** @class  OnlineHistTask OnlineHistTask.h OnlineHistDB/OnlineHistTask.h
 * 
 *  Task Class header for the C++ interface to the Online Monitoring Histogram DB
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 27/06/2007
 */

#include "OnlineHistDB/OnlineHistDBEnv.h"


class OnlineHistTask : public OnlineHistDBEnv
{
 public:
  OnlineHistTask(OnlineHistDBEnv &env,
		 std::string Name, 
		 std::string SubDet1, 
		 std::string SubDet2, 
		 std::string SubDet3,
		 bool RunsOnPhysics, 
		 bool RunsOnCalib, 
		 bool RunsOnEmpty,
		 float SavingFrequency);
  OnlineHistTask(OnlineHistDBEnv &env,
		 std::string Name);
  virtual ~OnlineHistTask() {}
  std::string name() {return m_name;}
  int ndet() {return m_ndet;}
  std::string det(int i) { return (i>=0 && i<3) ?  m_sd[i] : "NULL";}
  bool runsOnPhysics() {return m_RunsOnPhysics;}
  bool runsOnCalib() {return m_RunsOnCalib;}
  bool runsOnEmpty() {return m_RunsOnEmpty;}
  float savingFrequency() { return m_SavingFrequency;}
  std::string reference() { return m_Reference;}
  
  bool setSubDetectors(std::string SubDet1="NULL", 
			std::string SubDet2="NULL", 
			std::string SubDet3="NULL");
  bool addSubDetector(std::string SubDet);
  bool setRunConfig(bool RunsOnPhysics, 
		    bool RunsOnCalib, 
		    bool RunsOnEmpty) {
    m_RunsOnPhysics = RunsOnPhysics;
    m_RunsOnCalib = RunsOnCalib;
    m_RunsOnEmpty = RunsOnEmpty;
    return save();}
  bool setSavingFrequency(float SavingFrequency) {m_SavingFrequency = SavingFrequency; return save();}
  bool setReference(std::string Reference) {m_Reference = Reference; return save();}
  bool isAbort() {return m_abort;}

 private:
  std::string m_name;
  int m_ndet;
  std::string m_sd[3];
  bool m_RunsOnPhysics;
  bool m_RunsOnCalib;
  bool m_RunsOnEmpty;
  float m_SavingFrequency;
  std::string m_Reference;
  bool m_abort;
  bool save();
  void declareSubDetectors(std::string SubDet1="NULL", 
			   std::string SubDet2="NULL", 
			   std::string SubDet3="NULL");
};


class OnlineTaskStorage
{
 public:
  OnlineTaskStorage(OnlineHistDBEnv* Env);
  virtual ~OnlineTaskStorage();
  bool declareTask(std::string Name, 
		   std::string SubDet1="NULL", 
		   std::string SubDet2="NULL", 
		   std::string SubDet3="NULL",
		   bool RunsOnPhysics=false, 
		   bool RunsOnCalib=false, 
		   bool RunsOnEmpty=false,
		   float SavingFrequency=0);
  /// get an OnlineHistTask object, holding informations of an existing task, that can be used to view/edit a task record
  OnlineHistTask* getTask(std::string Name);
 private: 
  OnlineHistDBEnv* m_Taskenv;
  std::vector<OnlineHistTask*> m_myTask;
};


#endif // ONLINEHISTTASK_H
