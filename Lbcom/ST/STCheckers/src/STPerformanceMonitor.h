// $Id: STPerformanceMonitor.h,v 1.3 2008-10-16 13:10:34 mneedham Exp $
#ifndef STPerformanceMonitor_H
#define STPerformanceMonitor_H 1

#include "Kernel/STHistoAlgBase.h"

#include <string>

class DeSTSector;


/** @class STPerformanceMonitor STPerformanceMonitor.h
 *
 *  Class for monitoring STPerformances
 *
 *  @author M.Needham
 *  @date   28/7/2008
 */

class STPerformanceMonitor : public ST::HistoAlgBase{

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



 std::string m_summaryLocation; 
 std::string m_readoutToolName;
 std::string m_clusterLocation; 

 unsigned int m_event;
 unsigned int m_expectedEvents;

};

#endif // STPerformanceMonitor_H
