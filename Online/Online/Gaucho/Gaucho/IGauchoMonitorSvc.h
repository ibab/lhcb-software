// $Id: IGauchoMonitorSvc.h,v 1.6 2008-07-18 07:55:58 evh Exp $
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
  
/*  virtual void declareInfo(const std::string& name, const bool&  var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const int&  var, const  std::string& desc,  const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const long&  var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const double& var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const std::string& var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const std::pair<double,double>& var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const AIDA::IBaseHistogram* var, const  std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const std::string& format, const void * var, int size, const std::string& desc, const IInterface* owner) = 0;*/
  
/*  virtual void declareInfo(const std::string& name, const MonObject* var, const  std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const float&  var, const  std::string& desc,  const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const std::pair<int,int>& var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const std::pair<double,int>& var, const std::string& desc, const IInterface* owner) = 0;*/
  
  virtual void updateSvc( const std::string& name, bool endOfRun=false, const IInterface* owner = 0 ) = 0;
  virtual void updateAll( bool endOfRun=false , const IInterface* owner = 0) = 0;
  virtual void resetHistos( const IInterface* owner = 0 ) = 0; 

//   virtual void setTimerElapsed(bool timerelapsed) = 0;
//   virtual bool getTimerElapsed() const = 0;

  virtual void declareMonRateComplement( int& runNumber, int& cycleNumber, double& deltaT, ulonglong& timeFirstEvInRun, ulonglong& timeLastEvInCycle, ulonglong& gpsTimeLastEvInCycle) = 0;
  
};
#endif // KERNEL_IGAUCHOMONITORSVC_H

