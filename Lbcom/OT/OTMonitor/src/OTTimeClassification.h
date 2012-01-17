#ifndef OTMONITOR_OTTIMECLASSIFICATION_H
#define OTMONITOR_OTTIMECLASSIFICATION_H 1

// STD
#include <string>
#include <vector>
#include <map>

// Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

#include "GaudiKernel/ToolHandle.h"
#include "OTDAQ/IOTRawBankDecoder.h"

// Forward declarations
namespace LHCb
{
 class MCHit;
}

class DeOTDetector;

/** @class OTTimeClassification OTTimeClassification.h
 *
 *  Class for checking STNoiseClusters
 *
 *  @author M.Needham
 *  @date   21/4/2001
 */

class OTTimeClassification: public GaudiHistoAlg
{
public:
  /// constructor
  OTTimeClassification(const std::string& name, ISvcLocator *pSvcLocator);

  /// destructor
  virtual ~OTTimeClassification();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

  /// finalize
  StatusCode finalize();
private:
  void fillInfo(const std::vector<LHCb::MCHit*>& hits);
  std::string findSpill(const LHCb::MCHit* hit) const;

  ToolHandle<IOTRawBankDecoder> m_decoder;
  DeOTDetector* m_tracker;

  std::vector<std::string> m_spillVector; // short names of spills
  std::vector<std::string> m_spillNames; // full name of spills

  std::map<std::string, unsigned int> m_infoMap;
  typedef std::map<std::string, unsigned int>::const_iterator InfoIter;
};

#endif // OTMONITOR_OTTIMECLASSIFICATION_H
