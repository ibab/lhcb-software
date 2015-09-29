#ifndef HCDIGITTUPLE_H
#define HCDIGITTUPLE_H 1

// Local
#include "HCMonitorBase.h"

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
  /// TAE slot
  int m_tag;
  /// TES location of HC digits.
  std::string m_digitLocation;
  void addQuadrants(const std::vector<unsigned int>& channels,
                    const std::string& station, const bool bwd);
};
#endif
