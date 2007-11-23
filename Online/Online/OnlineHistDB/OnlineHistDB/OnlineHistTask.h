// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistTask.h,v 1.6 2007-11-23 17:58:55 ggiacomo Exp $
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
  /// task name
  std::string name() {return m_name;}
  /// number of associated subdetector/subsystems (up to 3)
  int ndet() {return m_ndet;}
  /// name of associated subdetector/subsystems (-1 < i < ndet() )
  std::string det(int i) { return (i>=0 && i<3) ?  m_sd[i] : "NULL";}
  /// true if task is configured to run for physics events
  bool runsOnPhysics() {return m_RunsOnPhysics;}
  /// true if task is configured to run for calibration events
  bool runsOnCalib() {return m_RunsOnCalib;}
  /// true if task is configured to run for empty events
  bool runsOnEmpty() {return m_RunsOnEmpty;}
  /// task saving frequency
  float savingFrequency() { return m_SavingFrequency;}
  /// location of latest reference file
  std::string reference() { return m_Reference;}
  
  /// sets the associated subdetector/subsystems. "NULL" unsets the value 
  bool setSubDetectors(std::string SubDet1="NULL", 
			std::string SubDet2="NULL", 
			std::string SubDet3="NULL");
  /// adds an associated subdetector/subsystems, returning true on success
  bool addSubDetector(std::string SubDet);
  /// sets run configuration bits
  bool setRunConfig(bool RunsOnPhysics, 
		    bool RunsOnCalib, 
		    bool RunsOnEmpty) {
    m_RunsOnPhysics = RunsOnPhysics;
    m_RunsOnCalib = RunsOnCalib;
    m_RunsOnEmpty = RunsOnEmpty;
    return save();}
  /// sets task saving frequency
  bool setSavingFrequency(float SavingFrequency) {m_SavingFrequency = SavingFrequency; return save();}
  /// sets the location of latest reference file
  bool setReference(std::string Reference) {m_Reference = Reference; m_Reference_null=0;return save();}
  bool isAbort() {return m_abort;}
  /// dumps task options
  void dump();
  // load task from DB
  void load();

 private:
  std::string m_name;
  int m_ndet;
  std::string m_sd[3];
  sb2 m_sd_null[3];
  bool m_RunsOnPhysics;
  sb2 m_RunsOnPhysics_null;
  bool m_RunsOnCalib;
  sb2 m_RunsOnCalib_null;
  bool m_RunsOnEmpty;
  sb2 m_RunsOnEmpty_null;
  float m_SavingFrequency;
  sb2 m_SavingFrequency_null;
  std::string m_Reference;
  sb2 m_Reference_null;
  bool m_abort;
  bool save();
  void declareSubDetectors(std::string SubDet1="NULL", 
			   std::string SubDet2="NULL", 
			   std::string SubDet3="NULL");
};


class OnlineTaskStorage
{
 public:
  OnlineTaskStorage() {}
  virtual ~OnlineTaskStorage();
  /// declares a new task to the DB, or updates its configuration 
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
 protected:
  void setTaskEnv(OnlineHistDBEnv* Env) {m_Taskenv = Env;}
  void reloadTasks();
 private: 
  OnlineHistDBEnv* m_Taskenv;
  std::vector<OnlineHistTask*> m_myTask;
};


#endif // ONLINEHISTTASK_H
