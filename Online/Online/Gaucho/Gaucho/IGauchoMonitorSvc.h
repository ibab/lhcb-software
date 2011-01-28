// $Id: IGauchoMonitorSvc.h,v 1.19 2010/01/20 16:06:03 evh Exp $
#ifndef KERNEL_IGAUCHOMONITORSVC_H 
#define KERNEL_IGAUCHOMONITORSVC_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IMonitorSvc.h"

static const InterfaceID IID_IGauchoMonitorSvc ( "IGauchoMonitorSvc", 1, 0 );

/** @class IGauchoMonitorSvc IGauchoMonitorSvc.h Kernel/IGauchoMonitorSvc.h
 *  
 *
 *  @author Marco CLEMENCIC
 *  @date   2008-05-22
 */

class MonObject;

class IGauchoMonitorSvc : virtual public IMonitorSvc {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IGauchoMonitorSvc; }
  
  virtual void declareInfo(const std::string& name, const bool&  var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const int&  var, const  std::string& desc,  const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const long&  var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const double& var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const std::string& var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const std::pair<double,double>& var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const AIDA::IBaseHistogram* var, const  std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const std::string& format, const void * var, int size, const std::string& desc, const IInterface* owner) = 0;
  
//  virtual void declareInfo(const std::string& name, const MonObject* var, const  std::string& desc, const IInterface* owner) = 0;
//  virtual void declareInfo(const std::string& name, const float&  var, const  std::string& desc,  const IInterface* owner) = 0;
//  virtual void declareInfo(const std::string& name, const std::pair<int,int>& var, const std::string& desc, const IInterface* owner) = 0;
//  virtual void declareInfo(const std::string& name, const std::pair<double,int>& var, const std::string& desc, const IInterface* owner) = 0;
  
  virtual void updateSvc( const std::string& name, int runno, const IInterface* owner = 0 ) = 0;
  virtual void updateAll( bool endOfRun=false , const IInterface* owner = 0) = 0;
  //virtual void resetHistos( const IInterface* owner = 0 ) = 0; 
  //virtual void resetHistos(bool saveHistos) = 0;

//   virtual void setTimerElapsed(bool timerelapsed) = 0;
//   virtual bool getTimerElapsed() const = 0;

  virtual void declareMonRateComplement( int& runNumber, unsigned int& triggerConfigurationKey, int& cycleNumber, double& deltaT, double& offsetTimeFirstEvInRun, double& offsetTimeLastEvInCycle, double& offsetGpsTimeLastEvInCycle) = 0;

  virtual void enableMonObjectsForBool() = 0;
  virtual void enableMonObjectsForInt() = 0;
  virtual void enableMonObjectsForLong() = 0;
  virtual void enableMonObjectsForDouble() = 0;
  virtual void enableMonObjectsForString() = 0;
  virtual void enableMonObjectsForPairs() = 0;
  virtual void enableMonObjectsForHistos() = 0;
  virtual void Lock(void)=0;
  virtual void UnLock(void)=0;
  virtual void resetHistos( const IInterface*  )=0;
  virtual void setRunNo(int runno) = 0;

  
  
};
#endif // KERNEL_IGAUCHOMONITORSVC_H

