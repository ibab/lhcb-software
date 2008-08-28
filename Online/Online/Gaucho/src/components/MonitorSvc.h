#ifndef GAUDIKERNEL_MONITORSVC_H
#define GAUDIKERNEL_MONITORSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IMonitorSvc.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include <string>
#include <map>
#include <set>

// Forward declarations
class ISvcLocator;
class DimPropServer;
class DimCmdServer;
class MonObject;
class MonRate;
class DimServiceMonObject;

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

class MonitorSvc : public Service, virtual public IMonitorSvc, virtual public IGauchoMonitorSvc {
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
  void declareInfo(const std::string& name, const std::string& var, 
                   const std::string& desc, const IInterface* owner) ;
  void declareInfo(const std::string& name, const std::pair<double,double>&var,
                   const std::string& desc, const IInterface* owner) ;
  void declareInfo(const std::string& name, const AIDA::IBaseHistogram* var, 
                   const std::string& desc, const IInterface* owner) ;
  // We can not modify IMonitorSvc then we use this method to declare MonObjecs...
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
  
  /** Get the names for all declared monitoring informations for a given
      owner. If the owner is NULL, then it returns for all owners
  */
  std::set<std::string> * getInfos(const IInterface* owner = 0);
  
  
  
  void updateSvc( const std::string& name,  bool endOfRun , const IInterface* owner) ;

  /** Update all monitoring information
      @param owner Owner identifier of the monitoring information
  */
  void updateAll( bool endOfRun , const IInterface* owner = 0) ;
  void resetHistos( const IInterface* owner = 0 ) ;

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

//  typedef std::map<std::string, DimServiceMonObject*, std::less<std::string> > DimServiceMonObjectMap;
//  typedef DimServiceMonObjectMap::iterator DimServiceMonObjectMapIt;
//  DimServiceMonObjectMap    m_dimMonObjects;
//  DimServiceMonObjectMapIt  m_dimMonObjectsIt;

  typedef std::map<std::string, DimService*, std::less<std::string> > DimServiceMap;
  typedef DimServiceMap::iterator DimServiceMapIt;
  DimServiceMap    m_dimSrv;
  DimServiceMapIt  m_dimSrvIt;

  bool registerName( const std::string& name, const IInterface* owner);
  std::pair<std::string, std::string> registerDimSvc(const std::string& name, const std::string& dimPrefix, const IInterface* owner, bool isComment);
  void undeclService(std::string infoName);
  void updateService(std::string infoName, bool endOfRun);

  std::string infoOwnerName( const IInterface* owner );
  std::string extract(const std::string mascara, std::string value);

  // MonObjetc to convert conters in rates  
  MonRate  *m_monRate;
  bool m_monRateDeclared;
  
public:    
   void declareMonRateComplement( int& runNumber, int& cycleNumber, double& deltaT, ulonglong& timeFirstEvInRun, ulonglong& timeLastEvInCycle, ulonglong& gpsTimeLastEvInCycle);  

  int m_disableMonRate;
  int m_disableDimPropServer;
  int m_disableDimCmdServer;
  
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
  
  std::string m_teste;
};

#endif // GAUDIKERNEL_MONITORSVC_H
