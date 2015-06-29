#ifndef GAUDIKERNEL_MONITORSVC_H
#define GAUDIKERNEL_MONITORSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IMonitorSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IUpdateable.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "GaudiKernel/StatEntity.h"
#include <typeinfo>
#include <string>
#include <map>
#include <set>
#include "Gaucho/TaskSaveTimer.h"

// Forward declarations
class ISvcLocator;
class IIncidentSvc;
class MonObject;
class MonSys;
class MonSubSys;
class CntrMgr;
class RateMgr;
class StatusService;

namespace AIDA { class IBaseHistogram; }

/** @class MonitorSvc MonitorSvc.h Gaucho/MonitorSvc.h

This class implements the IMonitorSvc interface, and publishes Gaudi variables
to outside monitoring processes with Dim.

A DimPropServer is started which takes string commands in the format
Algorithm.Property and returns the value of the property.

@author Philippe Vannerem
@author Jose Helder Lopes Jan. 2005
@author Jose Helder Lopes 2006/12/26: Modified to publish general c-structures and AIDA::IProfile1D
@author Juan Otalora Goicochea 2007/11/20: MonObjects
*/

class MonitorSvc
: virtual public IGauchoMonitorSvc,
  virtual public IIncidentListener,
  virtual public IUpdateableIF,
  virtual public Service
{
  StatusCode i_start();
  StatusCode i_stop();

  void i_unsupported(const std::string& name, const std::type_info& typ, const IInterface* owner);
  template<class T> void i_declareCounter(const std::string& name, const T&  var,
            const std::string& desc, const IInterface* owner);

public:
  MonitorSvc(const std::string& name, ISvcLocator* sl);
  virtual ~MonitorSvc();
  //IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  // Service pure virtual member functions
  virtual StatusCode initialize();
  virtual StatusCode start();
  virtual StatusCode finalize();
  void Lock(void);
  void UnLock(void);

  /// Incident handler implemenentation: Inform that a new incident has occured
  virtual void handle(const Incident& inc);

  /** Declare monitoring information
      @param name Monitoring information name knwon to the external system
      @param var  Monitoring Listener address
      @param desc Textual description
      @param owner Owner identifier of the monitoring information
      (needed to peform clean up
  */
  void declareInfo(const std::string& name, const bool&  var,
                   const std::string& desc, const IInterface* owner) ;
  void declareInfo(const std::string& name, const int&  var,
                   const std::string& desc, const IInterface* owner) ;
  void declareInfo(const std::string& name, const long&  var,
                   const std::string& desc, const IInterface* owner) ;
  void declareInfo(const std::string& name, const double& var,
                   const std::string& desc, const IInterface* owner) ;
  void declareInfo(const std::string& name, const std::string& var,
                   const std::string& desc, const IInterface* owner) ;
  void declareInfo(const std::string& name, const std::pair<double,double>&var,
                   const std::string& desc, const IInterface* owner) ;
  void declareInfo(const std::string& name, const StatEntity& var,
                   const std::string& desc, const IInterface* owner) ;
  void declareInfo(const std::string& name, const AIDA::IBaseHistogram* var,
                   const std::string& desc, const IInterface* owner) ;
  // We can not modify IMonitorSvc then we use this method to declare MonObjecs...
  void declareInfo(const std::string& name, const std::string& format, const void * var,
                   int size, const std::string& desc, const IInterface* owner) ;

  //Eric: To be implemented?
  //virtual void declareInfo(const std::string&, const StatEntity&, const std::string&, const IInterface*) {}

  /** Undeclare monitoring information
      @param name Monitoring information name knwon to the external system
      @param owner Owner identifier of the monitoring information
  */
  void undeclareInfo( const std::string& , const IInterface*  );

  /** Undeclare monitoring information
      @param owner Owner identifier of the monitoring information
  */
  void undeclareAll( const IInterface*  );

  /** Update monitoring information
      @param name Monitoring information name knwon to the external system
      @param owner Owner identifier of the monitoring information
  */

  /** Get the names for all declared monitoring informations for a given
      owner. If the owner is NULL, then it returns for all owners
  */
  std::set<std::string> * getInfos(const IInterface*  = 0){return 0;};



  void updateSvc( const std::string& name, int runno , const IInterface* owner) ;
  void updatePerSvc(unsigned long ref);
//  void updateSvc( const std::string& name, bool runno , const IInterface* owner) {};

  /** Update all monitoring information
      @param owner Owner identifier of the monitoring information
  */
  void updateAll( bool  , const IInterface*  = 0) {};
  void resetHistos( const IInterface* owner = 0 ) ;
  void setRunNo(int runno);
  StatusCode update(int id){updateSvc("",id,0);return StatusCode::SUCCESS;};
  //void resetHistos(bool saveHistos);

private:
  StatusService *m_StatusService;
  bool m_expandCounterServices;
  bool m_saveInter;
  MonSys *m_MonSys;
  MonSubSys *m_HistSubSys;
  MonSubSys *m_CntrSubSys;
  CntrMgr *m_CntrMgr;
  RateMgr *m_RateMgr;
  std::string m_utgid;
  std::string m_expandInfix;
  std::string m_partname;
  std::string m_ProcName;
  std::string m_ProgName;
  std::map<std::string,void*> m_InfoMap;
  typedef std::map<const IInterface*, std::set<std::string> > InfoNamesMap;
  typedef InfoNamesMap::iterator InfoNamesMapIt;
  InfoNamesMap  m_InfoNamesMap;
  InfoNamesMapIt m_InfoNamesMapIt;
  /// Reference to the IncidentSvc instance
  IIncidentSvc             *m_incidentSvc;

  std::pair<std::string, std::string> registerDimSvc(const std::string& name, const std::string& dimPrefix, const IInterface* owner, bool isComment);
  void undeclService(std::string infoName);
  void updateService(std::string infoName, bool endOfRun);

  std::string infoOwnerName( const IInterface* owner );
  std::string extract(const std::string mascara, std::string value);

  int m_maxNumCountersMonRate;
  bool m_monRateDeclared;
  void addRate(MonSubSys*, CntrMgr*);
  void StopUpdate();

public:
   void declareMonRateComplement( int& runNumber, unsigned int& triggerConfigurationKey, int& cycleNumber,
       double& deltaT, double& offsetTimeFirstEvInRun,
       double& offsetTimeLastEvInCycle, double& offsetGpsTimeLastEvInCycle);
   int m_CounterInterval;
  int m_uniqueServiceNames;
  int m_disableMonRate;
  int m_disableDimPropServer;
  int m_disableDimCmdServer;
  bool m_started;
  int m_monsysrecover;
  //int m_disableDimRcpGaucho;

  int m_disableMonObjectsForBool;
  int m_disableMonObjectsForInt;
  int m_disableMonObjectsForLong;
  int m_disableMonObjectsForDouble;
  int m_disableMonObjectsForString;
  int m_disableMonObjectsForPairs;
  int m_disableMonObjectsForHistos;

  int m_disableDeclareInfoBool;
  int m_disableDeclareInfoInt;
  int m_disableDeclareInfoLong;
  int m_disableDeclareInfoDouble;
  int m_disableDeclareInfoString;
  int m_disableDeclareInfoPair;
  int m_disableDeclareInfoFormat;
  int m_disableDeclareInfoHistos;
  bool m_DontResetCountersonRunChange;
  TaskSaveTimer *m_savetimer;

  void enableMonObjectsForBool() {m_disableMonObjectsForBool = 0;}
  void enableMonObjectsForInt() {m_disableMonObjectsForInt = 0;}
  void enableMonObjectsForLong() {m_disableMonObjectsForLong = 0;}
  void enableMonObjectsForDouble() {m_disableMonObjectsForDouble = 0;}
  void enableMonObjectsForString() {m_disableMonObjectsForString = 0;}
  void enableMonObjectsForPairs() {m_disableMonObjectsForPairs = 0;}
  void enableMonObjectsForHistos() {m_disableMonObjectsForHistos = 0;}


  void disableMonObjectsForBool() {m_disableMonObjectsForBool = 1;}
  void disableMonObjectsForInt() {m_disableMonObjectsForInt = 1;}
  void disableMonObjectsForLong() {m_disableMonObjectsForLong = 1;}
  void disableMonObjectsForDouble() {m_disableMonObjectsForDouble = 1;}
  void disableMonObjectsForString() {m_disableMonObjectsForString = 1;}
  void disableMonObjectsForPairs() {m_disableMonObjectsForPairs = 1;}
  void disableMonObjectsForHistos() {m_disableMonObjectsForHistos = 1;}
  virtual void StartSaving(std::string &dir, std::string &part, std::string &task, int period, DimService *sav=0);
  virtual void StopSaving();

  int m_updateInterval;
  int m_runno;
  bool m_i_startState;
  int m_MonSSysSize;

};

#endif // GAUDIKERNEL_MONITORSVC_H
