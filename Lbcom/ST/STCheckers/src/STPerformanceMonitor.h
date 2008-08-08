// $Id: STPerformanceMonitor.h,v 1.2 2008-08-08 07:31:27 mneedham Exp $
#ifndef STPerformanceMonitor_H
#define STPerformanceMonitor_H 1

#include "GaudiAlg/GaudiHistoAlg.h"

#include <string>

class DeSTDetector;
class DeSTSector;
class ISTReadoutTool;

/** @class STPerformanceMonitor STPerformanceMonitor.h
 *
 *  Class for monitoring STPerformances
 *
 *  @author M.Needham
 *  @date   28/7/2008
 */

class STPerformanceMonitor : public GaudiHistoAlg{

public:
 
  /// constructer
  STPerformanceMonitor( const std::string& name, 
                  ISvcLocator *svcloc );

  /// destructer
  virtual ~STPerformanceMonitor();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

 void getDeadSectors(std::vector<DeSTSector*>& deadSectors, 
		     const std::vector<unsigned int>& banks) const;

 DeSTDetector* m_tracker;
 std::string m_detType;
 std::string m_summaryLocation; 
 std::string m_readoutToolName;
 std::string m_clusterLocation; 
 ISTReadoutTool* m_readoutTool;  
 unsigned int m_event;
 unsigned int m_expectedEvents;

};

#endif // STPerformanceMonitor_H
