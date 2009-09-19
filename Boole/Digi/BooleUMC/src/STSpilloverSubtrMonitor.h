// $Id: STSpilloverSubtrMonitor.h,v 1.1.1.1 2009-09-19 23:12:04 tskwarni Exp $
#ifndef STSPILLOVERSUBTRMONITOR_H 
#define STSPILLOVERSUBTRMONITOR_H 1

// Include files
// from Gaudi
#include "Kernel/STHistoAlgBase.h"
#include <string>



/** @class STSpilloverSubtrMonitor STSpilloverSubtrMonitor.h
 *  
 * Monitor SubtrMonitor of Spillover in Upgarde Simulations
 *
 *  @author Tomasz Skwarnicki
 *  @date   2009-09-11
 */
class STSpilloverSubtrMonitor :public ST::HistoAlgBase {

public:
  
  // Constructors and destructor
  STSpilloverSubtrMonitor(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~STSpilloverSubtrMonitor();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();

private:

  std::string m_inputLocation;
  std::vector<std::string> m_spillNames;
  std::vector<std::string> m_spillPaths;

};
#endif // STSPILLOVERSUBTRMONITOR_H
