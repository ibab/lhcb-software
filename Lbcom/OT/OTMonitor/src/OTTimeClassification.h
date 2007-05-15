// $Id $
#ifndef OTMONITOR_OTTIMECLASSIFICATION_H
#define OTMONITOR_OTTIMECLASSIFICATION_H 1

// STD
#include <map>
#include <string>

// Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// Linkers
#include "Linker/LinkerTool.h"

// Forward declarations
namespace LHCb{
 class OTTime;
 class MCHit;
};

/** @class OTTimeClassification OTTimeClassification.h
 *
 *  Class for checking STNoiseClusters
 *
 *  @author M.Needham
 *  @date   21/4/2001
 */

class OTTimeClassification : public GaudiHistoAlg {

public:
  
  /// constructer
  OTTimeClassification(const std::string& name, ISvcLocator *svcloc );

  /// destructer
  virtual ~OTTimeClassification();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

  /// finalize
  StatusCode finalize();

private:
  ///Some handy typedefs
  typedef std::map<std::string, unsigned int>::const_iterator InfoIter;
  typedef LinkerTool<LHCb::OTTime, LHCb::MCHit> AsctTool;
  typedef AsctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  void fillInfo(const LHCb::MCHit* aHit) const;
  std::string  findSpill(const LHCb::MCHit* aHit) const;
  
  unsigned int tCount() const;

  std::vector<std::string> m_spillVector;  // short names of spills
  std::vector<std::string> m_spillNames; // full name of spills
     
  std::string m_asctLocation;
  std::string m_timesLocation;
  std::string m_hitLocation; 

  mutable std::map<std::string, unsigned int> m_infoMap;
};

#endif // OTMONITOR_OTTIMECLASSIFICATION_H












