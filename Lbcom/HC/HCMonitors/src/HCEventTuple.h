#ifndef HCEVENTTUPLE_H
#define HCEVENTTUPLE_H 1

// Local
#include "HCMonitorBase.h"
#include <DetDesc/Condition.h>


/** @class HCEventTuple HCEventTuple.h
 *
 *
 *  @author Paolo Gandini
 *  @date   2015-05-08
 */

class HCEventTuple : public HCMonitorBase {
 public:
  /// Standard constructor
  HCEventTuple(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~HCEventTuple();

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution
  StatusCode myCallBackFunction();
  


 private:
  /// TAE slot
  int m_tag;
  /// TES location of HC digits.
  std::string m_digitLocation;
  std::string m_correctedDigitLocation;
  bool m_corrected;

  // condition database                                                                                                                                                                 
  Condition* m_FillNumberCondition;
  std::string m_condition_path;  
  int m_FillNumber;
  int m_saved;

};
#endif
