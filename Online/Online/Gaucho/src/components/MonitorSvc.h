#ifndef GAUDIKERNEL_MONITORSVC_H
#define GAUDIKERNEL_MONITORSVC_H

#include "GaudiKernel/Service.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include <string>
#include <map>
#include <set>

// Forward declarations
class ISvcLocator;
class DimPropServer;
class DimCmdServer;
class MonObject;
class MonTimer;
class DimServiceMonObject;

namespace AIDA { class IBaseHistogram; }
//class MonObjects;
/** @class MonitorSvc MonitorSvc.h GaudiKernel/MonitorSvc.h
    
This class implements the IGauchoMonitorSvc interface, and publishes Gaudi variables
to outside monitoring processes with Dim.

An internal DimEngine is used for this purpose. A DimPropServer is started 
which takes string commands in the format Algorithm.Property and returns 
the value of the property.

@author Philippe Vannerem
@author Jose Helder Lopes Jan. 2005
@author Jose Helder Lopes 2006/12/26: Modified to publish general c-structures and AIDA::IProfile1D  
@author Juan Otalora Goicochea 2007/11/20: MonObjects
*/

class MonitorSvc : public Service, virtual public IMonitorSvc, public IGauchoMonitorSvc {
public:
  MonitorSvc(const std::string& name, ISvcLocator* sl);
  virtual ~MonitorSvc();
  //IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  // Service pure virtual member functions
  StatusCode initialize();
  StatusCode finalize();
  
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
  void declareInfo(const std::string& name, const float& var, 
                   const std::string& desc, const IInterface* owner) ;
  void declareInfo(const std::string& name, const std::string& var, 
                   const std::string& desc, const IInterface* owner) ;
  void declareInfo(const std::string& name, const std::pair<double,double>&var,
                   const std::string& desc, const IInterface* owner) ;
  void declareInfo(const std::string& name, const std::pair<int,int>&var,
                   const std::string& desc, const IInterface* owner) ;
  void declareInfo(const std::string& name, const std::pair<double,int>&var,
                   const std::string& desc, const IInterface* owner) ;
  void declareInfo(const std::string& name, const AIDA::IBaseHistogram* var, 
                   const std::string& desc, const IInterface* owner) ;
  void declareInfo(const std::string& name, const MonObject* var, 
                   const std::string& desc, const IInterface* owner) ;
  
  //Obsolete because the using of MonObjects
  void declareInfo(const std::string& name, const std::string& format, const void * var, 
                   int size, const std::string& desc, const IInterface* owner) ;

			    
  /** Undeclare monitoring information
      @param name Monitoring information name knwon to the external system
      @param owner Owner identifier of the monitoring information
  */
  void undeclareInfo( const std::string& name, const IInterface* owner ) ;
  
  /** Undeclare monitoring information
      @param owner Owner identifier of the monitoring information
  */
  void undeclareAll( const IInterface* owner ) ;

  /** Update monitoring information
      @param name Monitoring information name knwon to the external system
      @param owner Owner identifier of the monitoring information
  */
  void updateSvc( const std::string& name,  bool endOfRun , const IInterface* owner) ;

  /** Update all monitoring information
      @param owner Owner identifier of the monitoring information
  */
  void updateAll( bool endOfRun , const IInterface* owner = 0) ;
  void resetHistos( const IInterface* owner = 0 ) ;

  void setTimerElapsed(bool timerelapsed);
  void setTimeFirstEvElapsed(time_t time);
  bool getTimerElapsed() const;
  
  bool m_TimerElapsed;

  /** Get the names for all declared monitoring informations for a given
      owner. If the owner is NULL, then it returns for all owners
  */
  std::set<std::string> * getInfos(const IInterface* owner = 0);

private:

  std::string m_utgid; 
  DimPropServer* m_dimpropsvr;
  DimCmdServer* m_dimcmdsvr;
  
  // Map associating to each algorithm name a set with the info 
  // names from this algorithm 
  typedef std::map<const IInterface*, std::set<std::string> > InfoNamesMap;
  typedef InfoNamesMap::iterator InfoNamesMapIt;
  InfoNamesMap  m_InfoNamesMap;
  InfoNamesMapIt m_InfoNamesMapIt;

  typedef std::map<std::string, DimServiceMonObject*, std::less<std::string> > DimServiceMonObjectMap;
  typedef DimServiceMonObjectMap::iterator DimServiceMonObjectMapIt;
  DimServiceMonObjectMap    m_dimMonObjects;
  DimServiceMonObjectMapIt  m_dimMonObjectsIt;
  
/*  std::map<std::string, DimServiceMonObject*, std::less<std::string> > m_dimMonObjects;
  std::map<std::string, DimServiceMonObject*, std::less<std::string> >::iterator m_dimMonObjectsIt;*/
  
  //All declareInfo Members call declareInfoMonObject 
  void declareInfoMonObject(const std::string& name, MonObject* var, const std::string& desc, const IInterface* owner) ;
  
  void declServiceMonObject(std::string infoName, const MonObject* monObject);
  void undeclServiceMonObject(std::string infoName);
  void updateServiceMonObject(std::string infoName, bool endOfRun);

  
  //MonTimerAsync* m_monTimerAsync;
  MonTimer* m_monTimer;
 // IGauchoMonitorSvc *m_gauchoMonitorSvc;
  int m_refreshTime;
  std::string infoOwnerName( const IInterface* owner );
  
  // MonRate information
  int    *m_runNumber;       // Maybe we have to use double
  int    *m_cycleNumber;
  longlong *m_timeFirstEvInRun;
  longlong *m_timeLastEvInCycle; // Maybe we have to use double, have to check serialization
  
  void declareMonRateComplement( int& runNumber, int& cycleNumber, longlong& timeFirstEvInRun, longlong& timeLastEvInCycle);  
};

#endif // GAUDIKERNEL_MONITORSVC_H
