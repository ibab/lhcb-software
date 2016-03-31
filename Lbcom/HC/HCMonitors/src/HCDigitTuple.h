#ifndef HCDIGITTUPLE_H
#define HCDIGITTUPLE_H 1

// Local
#include "HCMonitorBase.h"

class Condition;

/** @class HCDigitTuple HCDigitTuple.h
 *
 *
 *  @author Victor Coco
 *  @date   2015-05-08
 */

class HCDigitTuple : public HCMonitorBase {
 public:
  /// Standard constructor
  HCDigitTuple(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~HCDigitTuple();

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

 private:
  Condition* m_condLHC = nullptr;
  int m_fillNumber = 0;

  /// TAE slot
  int m_tag = 0;
  /// TES location of HC digits.
  std::string m_digitLocation;
  std::string m_correctedDigitLocation;
  bool m_corrected = false;

  /// Retrieve the fill number from the conditions database.
  StatusCode cacheFill();
};
#endif
