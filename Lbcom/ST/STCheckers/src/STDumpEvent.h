// $Id: STDumpEvent.h,v 1.1 2009-01-06 13:47:56 mneedham Exp $
#ifndef STDumpEvent_H
#define STDumpEvent_H 1

#include "Kernel/STAlgBase.h"
#include <string>


/** @class STDumpEvent STDumpEvent.h
 *
 *  Class for printing out the ST Event
 *
 *  @author M.Needham
 *  @date   05/01/2009
 */

class STDumpEvent : public ST::AlgBase {

public:
 
  /// constructer
  STDumpEvent(const std::string& name, ISvcLocator *svcloc );

  /// destructer
  virtual ~STDumpEvent();

  /// execute
  StatusCode execute();

private:

  // job options
  bool m_fullDetail;       ///< Print out the details of each DetectorElement
  std::string m_clusterLocation;
  std::string m_summaryLocation; 

};

#endif // STDumpEvent_H
